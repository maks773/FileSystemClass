#include "Unit3.h"
#include "Unit2.h"
#include "Classes.h"
#include "sqlite3.h"
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
using namespace std;

//описание вспомогательных функций классов

void CheckFilePointer(DWORD dwPtr)       //провекра позиционирования в файле
{
  if(dwPtr == INVALID_SET_FILE_POINTER) {
	  PrintError(1);
  }
}

void beauty_print(BYTE *buf, FILE *ptrFile, int j)  //печать кластера в файл
{                                                   //в формате hex и ASCII
  if (buf[j]<=15)
	  fprintf(ptrFile, "0%X ", buf[j]);
  else
	  fprintf(ptrFile, "%X ", buf[j]);

  if ((j+1)%16 ==0 && j!=0) {
	  fprintf(ptrFile, "\t\t");
	  for (int z = j-15; z<=j; z++)
		if (buf[z]=='\n' || buf[z]=='\r' || buf[z]=='\v' || buf[z]=='\t')
		   fprintf(ptrFile, " ");
		else
		   fprintf(ptrFile, "%c", buf[z]);
	  fprintf(ptrFile, "\n");
  }
}




//описание методов класса FileSystemClass

FileSystemClass* FileSystemClass::CreateFS(FS_type type, BYTE *data)
{
  FileSystemClass *p;
  switch(type)
  {
	case NTFS: p = new NTFS_FileSystemClass(data); break;
	case FAT32: p = new FAT32_FileSystemClass(data);  break;
  }
  return p;
}

UINT16 FileSystemClass::GetClusterSize()
{
  return Cluster_size;
}

ULONGLONG FileSystemClass::GetClustersCount()
{
  return Clusters_count_r;
}

double FileSystemClass::GetFileSystemSize()
{
  return fs_size;
}

UINT16 FileSystemClass::GetSectorSize()
{
  return Sector_size;
}

UINT16 FileSystemClass::GetFirstClusterNumber()
{
  return First_cluster;
}

std::string FileSystemClass::GetFileSystemType()
{
  return fs_type;
}

BYTE* FileSystemClass::ReadCluster(HANDLE hDisk, BYTE *buf)
{
  DWORD bytesRead = 1;
  BOOL readRes = false;
  readRes = ReadFile(hDisk,buf,Cluster_size,&bytesRead,NULL);

  if (readRes == false || bytesRead != Cluster_size) {
	   PrintError(2);
  }

  return(buf);
}

void FileSystemClass::PrintClusterToFile(FILE *ptrFile, BYTE *buf, ULONGLONG ClusterNumber)
{
  if (ptrFile != NULL) {

	for(int j=0; j<Cluster_size; j++)
		beauty_print(buf, ptrFile, j);

	fprintf(ptrFile, "\n-----------------Кластер №%d-----------------\n\n\n\n",
		  ClusterNumber);
  }
  else  PrintError(2);
}

BYTE* FileSystemClass::FindSignatureInCluster(BYTE *buf, BYTE *signatura, bool mode)
{
  BYTE *temp = buf;

  if (mode == true)
  for (int i=0; i<Cluster_size; i++) {
	if (buf[i]=='\0' || buf[i]=='\n' || buf[i]=='\t' || i>20) temp[i]=' ';
  }

  BYTE *p = strstr((char*)temp, (char*)signatura);
  if (p != NULL) {
  if ((mode == false && strlen(p) == strlen(temp)) || mode == true)
	return p;
  else
	return NULL;
  } else return NULL;
}




//описание методов класса NTFS_FileSystemClass

NTFS_FileSystemClass::NTFS_FileSystemClass(BYTE *BootRecord)
{
  pNTFS_BootRecord = (NTFS_BootRecord*)BootRecord;
  Sector_size = pNTFS_BootRecord -> sector_size;
  Sectors_count = pNTFS_BootRecord -> sectors_count;
  Sectors_count_r = pNTFS_BootRecord -> sectors_count_r;
  Cluster_size = Sector_size*Sectors_count;
  Clusters_count_r  = Sectors_count_r/Sectors_count;
  fs_size = ceil(Sectors_count_r*Sector_size/1024.0/1024.0/1024.0*100)/100;
  First_cluster = 0;
  fs_type = "NTFS";
}

ULONGLONG NTFS_FileSystemClass::GetFirstFileRecordCluster()
{
  return pNTFS_BootRecord -> mft_cluster_number;
}




//описание методов класса FAT32_FileSystemClass

FAT32_FileSystemClass::FAT32_FileSystemClass(BYTE *BootRecord)
{
  pFAT32_BootRecord = (FAT32_BootRecord*)BootRecord;
  Sector_size = pFAT32_BootRecord -> sector_size;
  Sectors_count = pFAT32_BootRecord -> sectors_count;
  if (pFAT32_BootRecord -> sectors_count_r1 != 0)
	Sectors_count_r = pFAT32_BootRecord -> sectors_count_r1;
  else
	Sectors_count_r = pFAT32_BootRecord -> sectors_count_r2;
  Cluster_size = Sector_size*Sectors_count;
  SkipSectors = pFAT32_BootRecord->reserv_sectors
   + pFAT32_BootRecord->fat_size * pFAT32_BootRecord->count_fat_table;
  Clusters_count_r  = (Sectors_count_r - SkipSectors) / Sectors_count;
  fs_size = ceil(Clusters_count_r*Cluster_size/1024.0/1024.0/1024.0*100)/100;
  First_cluster = 2;
  fs_type = "FAT32";
}

ULONGLONG FAT32_FileSystemClass::GetFirstFileRecordCluster()
{
  return pFAT32_BootRecord -> root_dir_cluster_number;
}





//описание методов класса ClustersIterator

ClustersIterator::ClustersIterator(FileSystemClass* _FS) :
	current(0), FS(_FS)
{
   cluster = new BYTE[FS->GetClusterSize()];

   hDisk = CreateFileW(L"\\\\.\\C:", GENERIC_READ, FILE_SHARE_WRITE |
	  FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

   if (hDisk == INVALID_HANDLE_VALUE)  PrintError(1);

   fs_type = FS->GetFileSystemType();

   if(fs_type == "FAT32") {
	DWORD dwPtr = SetFilePointer(hDisk, FS->GetSectorSize()*FS->SkipSectors,
	  NULL, FILE_BEGIN);
	CheckFilePointer(dwPtr);
   }
}

void ClustersIterator::First()
{
  current = FS->GetFirstClusterNumber();
}

void ClustersIterator::Next()
{
  current++;
}

bool ClustersIterator::IsDone()
{
  return current >= FS->GetClustersCount()+1;
}

BYTE* ClustersIterator::CurrentItem()
{
  if (IsDone()) PrintError(4);
  cluster = FS->ReadCluster(hDisk, cluster);
  return cluster;
}

ULONGLONG ClustersIterator::GetCurrentNum()
{
  return current;
}

ClustersIterator::~ClustersIterator()
{
  CloseHandle(hDisk);
  delete[] cluster;
}





//реализация методов класса ClustersDecorator

ClustersDecorator::ClustersDecorator(ClustersIterator *_It) :
	ClustersIterator(*_It)
{

}

BYTE* ClustersDecorator::CurrentItem()
{
  if (IsDone()) PrintError(4);
  cluster = FS->ReadCluster(hDisk, cluster);  int i;

  for (i = 0; i < FS->GetClusterSize(); i++) {
	 if (cluster[i] != 0x00) break;
  }

  if (i < FS->GetClusterSize()) return cluster; else return 0;
}





//реализация внеклассовых функций основной программы

string GetImageExt(int j)
{
  if (j>1) return "jpg/jpeg";
  else if (j==0) return  "png";
  else if (j==1) return  "bmp";
}

string FindImage(BYTE *buf, FileSystemClass *FS)
{
  BYTE picture[][100] = {
	{ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, '\0' }, //png
	{ 0x42, 0x4D, '\0' }, //bmp
	{ 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, '\0' }, //jpg, jpeg
	{ 0xFF, 0xD8, 0xFF, 0xEE, '\0' }, //jpg, jpeg
	{ 0xFF, 0xD8, 0xFF, 0xDB, '\0' },  //jpg, jpeg
	{ 0xFF, 0xD8, 0xFF, 0xE1, '\0'}, //jpg, jpeg
	{ 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, '\0'} //jpg, jpeg
  };

  BYTE *find;
  bool mode = false;
  string Extension = "";

  for (int j=0; j<7; j++) {
	find = FS->FindSignatureInCluster(buf, picture[j], mode);
	if (find != NULL) {
		Extension = GetImageExt(j);
		if (j==5) { mode = true; Extension = ""; buf = find; continue; }
		break;
	}
  }

  return Extension;
}


string GetAudioExt(int j)
{
  if (j<4) return "mp3";
  else if (j==4 || j==5) return  "wav";
}

string FindAudio(BYTE *buf, FileSystemClass *FS)
{
  BYTE audio[][100] = {
	{ 0xFF, 0xFB, '\0' }, //mp3
	{ 0xFF, 0xF3, '\0' }, //mp3
	{ 0xFF, 0xF2, '\0' }, //mp3
	{ 0x49, 0x44, 0x33, '\0' }, //mp3
	{ 0x52, 0x49, 0x46, 0x46, '\0' }, //wav
	{ 0x57, 0x41, 0x56, 0x45, '\0' }  //wav
  };

  BYTE *find;
  bool mode = false;
  string Extension = "";

  for (int j=0; j<6; j++) {
	find = FS->FindSignatureInCluster(buf, audio[j], mode);
	if (find != NULL) {
	   Extension = GetAudioExt(j);
	   if (j==4) { mode = true; Extension = ""; buf = find; continue; }
	   break;
	}
  }
  return Extension;
}


string GetVideoExt(int j)
{
  if (j>0) return "avi";
  else if (j==0) return "mp4";
}

string FindVideo(BYTE *buf, FileSystemClass *FS)
{
  BYTE video[][100] = {
	{ 0x66, 0x74, 0x79, 0x70, 0x69, 0x73, 0x6F, 0x6D, '\0' }, //mp4
	{ 0x52, 0x49, 0x46, 0x46, '\0' }, //avi
	{ 0x41, 0x56, 0x49, 0x20, '\0' } //avi
  };

  BYTE *find;
  string Extension = "";

  for (int j=0; j<3; j++) {
	if (j==0)
		find = FS->FindSignatureInCluster(&buf[4], video[j], true);
	else if (j==2)
		find = FS->FindSignatureInCluster(&buf[8], video[j], true);
	else
		find = FS->FindSignatureInCluster(buf, video[j], false);
	if (find != NULL) {
		Extension = GetVideoExt(j);
		if (j==1) { buf = find; Extension = ""; continue; }
		break;

	}
  }

  return Extension;
}

string GetDocExt(int j)
{
  if (j==0) return "MS Offise (docx, xlsx, ...)";
  else if (j==1) return "pdf";
}

string FindDoc(BYTE *buf, FileSystemClass *FS)
{
  BYTE document[][100] = {
	{ 0x50, 0x4B, 0x03, 0x04, 0x14, 0x00, 0x06, 0x00, '\0' }, //MS Offise
	{ 0x25, 0x50, 0x44, 0x46, 0x2D, '\0' } //pdf
  };

  BYTE *find;
  string Extension = "";

  for (int j=0; j<2; j++) {
	find = FS->FindSignatureInCluster(buf, document[j], false);
	if (find != NULL) {
		Extension = GetDocExt(j);
		break;
	}
  }

  return Extension;
}


void DeleteAll(TVirtualStringTree *VirtualStringTree1, sqlite3 *Database)
{
  VirtualStringTree1->Clear();

  char *errmsg;
  int res = sqlite3_open16(L"FindExtensions.db", &Database);
  if (res != 0)  PrintError(5);

  const char *sql = "DELETE FROM FindData";
  res = sqlite3_exec(Database, sql, 0, 0, &errmsg);
  if (res != SQLITE_OK) PrintError(7);

  sqlite3_close(Database);
}

void DeleteEntry(TVirtualStringTree *VirtualStringTree1, sqlite3 *Database)
{
  PVirtualNode selNode = VirtualStringTree1->FocusedNode;
  OriginTable *table = (OriginTable*)selNode->GetData();

  char *errmsg;
  int res = sqlite3_open16(L"FindExtensions.db", &Database);
  if (res != 0) PrintError(5);

  stringstream ss;
  ss << "DELETE FROM FindData WHERE ClusterNumber = " << (ULONGLONG)table->ClusterNumber;
  const char *sql = ss.str().c_str();

  VirtualStringTree1->DeleteNode(VirtualStringTree1->FocusedNode);

  res = sqlite3_exec(Database, sql, 0, 0, &errmsg);
  if (res != SQLITE_OK)  PrintError(7);

  sqlite3_close(Database);
}


FileSystemClass* IdentifyFS()
{
  HANDLE hDisk = CreateFileW(L"\\\\.\\C:", GENERIC_READ, FILE_SHARE_WRITE |
	FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

  if (hDisk == INVALID_HANDLE_VALUE)  PrintError(1);

  BYTE data[512]; DWORD bytesRead;
  BOOL readRes = ReadFile(hDisk,data,512,&bytesRead,NULL);

  if (readRes == false || bytesRead != 512) {
	PrintError(2);
  }

  FS_type type;  //тип ФС

  BYTE temp[512] = data;
  for (int i=0; i<512; i++) {
	if (temp[i]=='\0' || temp[i]=='\n' || temp[i]=='\t') temp[i]=' ';
  }

  if (strstr((char*)temp, "NTFS")!=NULL)
	type = NTFS;
  else if (strstr((char*)temp, "FAT32")!=NULL)
	type = FAT32;
  else  PrintError(3);

  FileSystemClass *FS = FS->CreateFS(type, data);
  return FS;
}


void WriteToDB(sqlite3 *Database, ULONGLONG ClusterNumber, string Extension)
{
  char *errmsg;
  const char *sql = "CREATE TABLE IF NOT EXISTS FindData(\
	id INTEGER PRIMARY KEY, ClusterNumber INTEGER, Extension TEXT)";
  int res = sqlite3_exec(Database, sql, 0, 0, &errmsg);

  if (res != SQLITE_OK) PrintError(6);

  stringstream ss;
  ss << "INSERT INTO FindData(id, ClusterNumber, Extension) VALUES(NULL, "
	 << ClusterNumber << ", '" << Extension << "')";
  sql = ss.str().c_str();

  res = sqlite3_exec(Database, sql, 0, 0, &errmsg);
  if (res != SQLITE_OK) PrintError(7);
}


void PrintError(int ErrorCode)
{
  switch(ErrorCode)
  {
	case 1:
	  MessageBox(GetActiveWindow(), L"Ошибка чтения диска", L"Ошибка",
		MB_ICONERROR); exit(1);

	case 2:
      MessageBox(GetActiveWindow(), L"Ошибка чтения файла", L"Ошибка",
		MB_ICONERROR); exit(2);

	case 3:
      MessageBox(GetActiveWindow(), L"ФС не опознана", L"Ошибка",
		MB_ICONERROR); exit(3);

	case 4:
	  MessageBox(GetActiveWindow(), L"Итератор: Переполнение буфера", L"Ошибка",
		MB_ICONERROR); exit(4);

	case 5:
	  MessageBox(GetActiveWindow(), L"Не удалось открыть файл БД", L"Ошибка",
		MB_ICONERROR); exit(5);

	case 6:
	  MessageBox(GetActiveWindow(), L"Не удалось создать таблицу", L"Ошибка",
		MB_ICONERROR); exit(6);

	case 7:
	  MessageBox(GetActiveWindow(), L"Не удалось выполнить запрос",	L"Ошибка",
		MB_ICONERROR); exit(7);

	default:
	  MessageBox(GetActiveWindow(), L"Неизвестная ошибка",	L"Ошибка",
		MB_ICONERROR); exit(-1);
  }
}
