//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
#include "Classes.h"
#include <iostream>
#include <string>
using namespace std;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VirtualTrees"
#pragma resource "*.dfm"
TForm2 *Form2;
FileSystemClass *FS; BYTE *buf; HANDLE hDisk; ULONGLONG ClusterNumber;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{
	VirtualStringTree1->NodeDataSize = sizeof(OriginTable);

	hDisk = CreateFileW(L"\\\\.\\F:", GENERIC_READ, FILE_SHARE_WRITE |
	 FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if (hDisk == INVALID_HANDLE_VALUE) {
	 MessageBox(GetActiveWindow(), L"Ошибка чтения диска", L"Ошибка",
	  MB_ICONERROR); exit(1);
	}

	BYTE data[512]; DWORD bytesRead;
	BOOL readRes = ReadFile(hDisk,data,512,&bytesRead,NULL);
	if (readRes == false || bytesRead != 512) {
	MessageBox(GetActiveWindow(), L"Ошибка чтения файла", L"Ошибка",
	 MB_ICONERROR); exit(2); }

	FS_type type;  //тип ФС

	BYTE temp[512] = data;
	for (int i=0; i<512; i++) {
		if (temp[i]=='\0' || temp[i]=='\n' || temp[i]=='\t') temp[i]=' ';
	}

	if (strstr((char*)temp, "NTFS")!=NULL)
		type = NTFS;
	else if (strstr((char*)temp, "FAT32")!=NULL)
		type = FAT32;
	else  {
			MessageBox(GetActiveWindow(), L"ФС не опознана", L"Ошибка",
			  MB_ICONERROR); exit(3);
	}

	FS = FS->CreateFS(type, data);

	Label4->Caption =  FS->GetFileSystemType().c_str();
	Label2->Caption =  FS->GetClusterSize();
	Label6->Caption =  FS->GetClustersCount();
	Label8->Caption =  FS->GetFileSystemSize();

	CloseHandle(hDisk);
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender)
{
	VirtualStringTree1->Clear();

	//FILE *ptrFile;
	buf = new BYTE[FS->GetClusterSize()];
	ClusterNumber = FS->GetFirstClusterNumber();
	string Extension;

	BYTE picture[][100] = {
	  { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, '\0' }, //png
	  { 0x42, 0x4D, '\0' }, //bmp
	  { 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, '\0' }, //jpg, jpeg
	  { 0xFF, 0xD8, 0xFF, 0xEE, '\0' }, //jpg, jpeg
	  { 0xFF, 0xD8, 0xFF, 0xDB, '\0' },  //jpg, jpeg
	  { 0xFF, 0xD8, 0xFF, 0xE1, '\0'}, //jpg, jpeg
	  { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, '\0'}, //jpg, jpeg
	};

	BYTE audio[][100] = {
	  { 0xFF, 0xFB, '\0' }, //mp3
	  { 0xFF, 0xF3, '\0' }, //mp3
	  { 0xFF, 0xF2, '\0' }, //mp3
	  { 0x49, 0x44, 0x33, '\0' }, //mp3
	  { 0x52, 0x49, 0x46, 0x46, '\0' }, //wav
	  { 0x57, 0x41, 0x56, 0x45, '\0' }  //wav
	};

	BYTE video[][100] = {
	  { 0x66, 0x74, 0x79, 0x70, 0x69, 0x73, 0x6F, 0x6D, '\0' }, //mp4
	  { 0x52, 0x49, 0x46, 0x46, '\0' }, //avi
	  { 0x41, 0x56, 0x49, 0x20, '\0' } //avi
	};

	BYTE document[][100] = {
	  { 0x50, 0x4B, 0x03, 0x04, 0x14, 0x00, 0x06, 0x00, '\0' }, //MS Offise
	  { 0x25, 0x50, 0x44, 0x46, 0x2D, '\0' } //pdf
	};

	ClustersIterator *it = new ClustersIterator(FS);   //итератор
	ClustersDecorator decor(it);                       //декоратор

	VirtualStringTree1->BeginUpdate();
	//for(decor.First(); !decor.IsDone(); decor.Next()) {  //для декоратора
	for(it->First(); !it->IsDone(); it->Next()) {          //для итератора
		  ClusterNumber = it->GetCurrentNum();    //для итератора
		  //ClusterNumber = decor.GetCurrentNum();    //для декоратора
		  bool mode = false; //режим работы функции поиска сигнатуры
		  BYTE *find;
		  //buf = decor.CurrentItem();             //для декоратора
		  buf = it->CurrentItem();                 //для итератора
		  if (RadioButton1->Checked)
			for (int j=0; j<7; j++) {
			  find = FS->FindSignatureInCluster(buf, picture[j], mode);
			  if (find != NULL) {
				if (j>1) Extension = "jpg/jpeg"; else if (j==0) Extension = "png";
				else if (j==1) Extension = "bmp";
				if (j==5) { mode = true; buf = find; continue; }
				//ptrFile = fopen("file.txt", "a");
				//FS->PrintClusterToFile(ptrFile, buf, ClusterNumber);
				PVirtualNode entryNode = VirtualStringTree1->AddChild(VirtualStringTree1->RootNode);
				OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(entryNode);
				table->ClusterNumber = ClusterNumber;
				table->Signatura = Extension.c_str();
			  }
			} else

			if (RadioButton2->Checked)
			for (int j=0; j<6; j++) {
			  find = FS->FindSignatureInCluster(buf, audio[j], mode);
			  if (find != NULL) {
				if (j<4) Extension = "mp3"; else if (j==4 || j==5) Extension = "wav";
				if (j==4) { mode = true; buf = find; continue; }
				//ptrFile = fopen("file.txt", "a");
				//FS->PrintClusterToFile(ptrFile, buf, ClusterNumber);
				PVirtualNode entryNode = VirtualStringTree1->AddChild(VirtualStringTree1->RootNode);
				OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(entryNode);
				table->ClusterNumber = ClusterNumber;
				table->Signatura = Extension.c_str();
			  }
			} else

			if (RadioButton3->Checked)
			for (int j=0; j<3; j++) {
			  if (j==0)
				find = FS->FindSignatureInCluster(&buf[4], video[j], true);
			  else if (j==2)
				find = FS->FindSignatureInCluster(&buf[8], video[j], true);
			  else
				find = FS->FindSignatureInCluster(buf, video[j], false);
			  if (find != NULL) {
				if (j>0) Extension = "avi"; else if (j==0) Extension = "mp4";
				if (j==1) { buf = find; continue; }
				//ptrFile = fopen("file.txt", "a");
				//FS->PrintClusterToFile(ptrFile, buf, ClusterNumber);
				PVirtualNode entryNode = VirtualStringTree1->AddChild(VirtualStringTree1->RootNode);
				OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(entryNode);
				table->ClusterNumber = ClusterNumber;
				table->Signatura = Extension.c_str();
			  }
			}

			if (RadioButton4->Checked)
			for (int j=0; j<2; j++) {
			  find = FS->FindSignatureInCluster(buf, document[j], mode);
			  if (find != NULL) {
				if (j==0) Extension = "MS Offise (docx, xlsx, ...)";
				else if (j==1) Extension = "pdf";
				//ptrFile = fopen("file.txt", "a");
				//FS->PrintClusterToFile(ptrFile, buf, ClusterNumber);
				PVirtualNode entryNode = VirtualStringTree1->AddChild(VirtualStringTree1->RootNode);
				OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(entryNode);
				table->ClusterNumber = ClusterNumber;
				table->Signatura = Extension.c_str();
			  }
			}

			ClusterNumber++;
	}

	VirtualStringTree1->EndUpdate();

	Label10->Caption = "Проанализировано " + UnicodeString(ClusterNumber -
	   FS->GetFirstClusterNumber() + 1) +	" из " + UnicodeString(FS->GetClustersCount())
		 + " кластеров";

	delete[] buf;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::VirtualStringTree1GetText(TBaseVirtualTree *Sender, PVirtualNode Node,
		  TColumnIndex Column, TVSTTextType TextType, UnicodeString &CellText)

{
   if (Node == NULL) return;
   OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(Node);
   switch(Column)
   {
	   case 0:
	   {
			CellText = UnicodeString(table->ClusterNumber); break;
	   }
	   case 1:
	   {
			CellText = table->Signatura ; break;
	   }
   }
}
//---------------------------------------------------------------------------


