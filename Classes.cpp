#include "Unit1.h"
#include "Unit2.h"
#include "Classes.h"
#include <iostream>
#include <string>
#include <math.h>
using namespace std;

//�������� ������� ������ FileSystemClass

FileSystemClass* FileSystemClass::CreateFS(FS_type type, BYTE *data)  //��������� �����
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
			MessageBox(GetActiveWindow(), L"������ ������ �����", L"������",
				MB_ICONERROR); exit(2);
	  }

	  return(buf);
	}

void FileSystemClass::PrintClusterToFile(FILE *ptrFile, BYTE *buf, ULONGLONG ClusterNumber)
	{
	  if (ptrFile != NULL) {
		for(int j=0; j<Cluster_size; j++) {
			if (buf[j]<=15) fprintf(ptrFile, "0%X ", buf[j]); else
			  fprintf(ptrFile, "%X ", buf[j]);
			if ((j+1)%16 ==0 && j!=0) {
			   fprintf(ptrFile, "\t\t");
			   for (int z = j-15; z<=j; z++)
				  if (buf[z]=='\n' || buf[z]=='\r' || buf[z]=='\v' || buf[z]=='\t')
					 fprintf(ptrFile, " "); else fprintf(ptrFile, "%c", buf[z]);
			   fprintf(ptrFile, "\n");
			}
		}

		fprintf(ptrFile, "\n-----------------������� �%d---------------------\n\n",
			  ClusterNumber);

	  } else {
		MessageBox(GetActiveWindow(), L"������ ������ �����", L"������",
			  MB_ICONERROR); exit(2);
	  }

	  fclose (ptrFile);
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


//�������� ������� ������ NTFS_FileSystemClass

NTFS_FileSystemClass::NTFS_FileSystemClass(BYTE *BootRecord)
	{
	  pNTFS_BootRecord = (NTFS_BootRecord*)BootRecord;
	  Sector_size = pNTFS_BootRecord -> sector_size; //������ �������
	  Sectors_count = pNTFS_BootRecord -> sectors_count; //���������� ���������
	  Sectors_count_r = pNTFS_BootRecord -> sectors_count_r; //����� ���������� ��������
	  Cluster_size = Sector_size*Sectors_count;  //������ ��������
	  Clusters_count_r  = Sectors_count_r/Sectors_count; //����� ���������� ���������
	  fs_size = ceil(Sectors_count_r*Sector_size/1024.0/1024.0/1024.0*100)/100; //������ �� � ��
	  First_cluster = 0; //����� ������� �������� ��
	  fs_type = "NTFS";
	}

ULONGLONG NTFS_FileSystemClass::GetFirstFileRecordCluster()
	{
	  return pNTFS_BootRecord -> mft_cluster_number;
	}

//�������� ������� ������ FAT32_FileSystemClass

FAT32_FileSystemClass::FAT32_FileSystemClass(BYTE *BootRecord)
	{
	  pFAT32_BootRecord = (FAT32_BootRecord*)BootRecord;
	  Sector_size = pFAT32_BootRecord -> sector_size; //������ �������
	  Sectors_count = pFAT32_BootRecord -> sectors_count; //���������� ���������
	  if (pFAT32_BootRecord -> sectors_count_r1 != 0)
		Sectors_count_r = pFAT32_BootRecord -> sectors_count_r1;
	  else
		Sectors_count_r = pFAT32_BootRecord -> sectors_count_r2; //����� ���������� ��������
	  Cluster_size = Sector_size*Sectors_count;  //������ ��������
	  SkipSectors = pFAT32_BootRecord->reserv_sectors
	   + pFAT32_BootRecord->fat_size * pFAT32_BootRecord->count_fat_table;
	  Clusters_count_r  = (Sectors_count_r - SkipSectors) / Sectors_count; //����� ���������� ���������
	  fs_size = ceil(Clusters_count_r*Cluster_size/1024.0/1024.0/1024.0*100)/100; //������ �� � ��
	  First_cluster = 2; //����� ������� �������� ��
	  fs_type = "FAT32";
	}

ULONGLONG FAT32_FileSystemClass::GetFirstFileRecordCluster()
	{
	  return pFAT32_BootRecord -> root_dir_cluster_number;
	}


//���������� ������� ������ ClustersIterator

ClustersIterator::ClustersIterator(FileSystemClass* _FS) :
	current(0), FS(_FS)
	{
	   cluster = new BYTE[FS->GetClusterSize()];

	   hDisk = CreateFileW(L"\\\\.\\F:", GENERIC_READ, FILE_SHARE_WRITE |
		  FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	   if (hDisk == INVALID_HANDLE_VALUE) {
		MessageBox(GetActiveWindow(), L"������ ������ �����", L"������",
		  MB_ICONERROR); exit(1);
	   }

	   fs_type = FS->GetFileSystemType();

	   if(fs_type == "FAT32") {
		DWORD dwPtr = SetFilePointer(hDisk, FS->GetSectorSize()*FS->SkipSectors,
		  NULL, FILE_BEGIN);

		if(dwPtr == INVALID_SET_FILE_POINTER) {
		MessageBox(GetActiveWindow(), L"������ ������ �����", L"������",
		  MB_ICONERROR); exit(1);
		}
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
	  if (IsDone()) {
	   MessageBox(GetActiveWindow(), L"��������: ������������ ������", L"������",
		  MB_ICONERROR); exit(4);
	  }
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

//���������� ������� ������ ClustersDecorator

ClustersDecorator::ClustersDecorator(ClustersIterator *_It) :
	ClustersIterator(*_It)
	{

	}

void ClustersDecorator::Next()
	{
	  cluster = CurrentItem(); int i;
	  for (i = 0; i < FS->GetClusterSize(); i++) {
		 if (cluster[i] != '0x00') break;
	  }

	  if (i < FS->GetClusterSize()) current++; else ClustersDecorator::Next();
	}                    //�������� ���� �����������
