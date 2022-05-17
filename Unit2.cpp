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
	 myThreadPtr = NULL;

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
	string fs_type;

	BYTE temp[512] = data;
	for (int i=0; i<512; i++) {
		if (temp[i]=='\0' || temp[i]=='\n' || temp[i]=='\t') temp[i]=' ';
	}

	if (strstr((char*)temp, "NTFS")!=NULL) {
		type = NTFS; fs_type = "NTFS";
	} else if (strstr((char*)temp, "FAT32")!=NULL) {
		type = FAT32; fs_type = "FAT32";
	}   else  {
			MessageBox(GetActiveWindow(), L"ФС не опознана", L"Ошибка",
			  MB_ICONERROR); exit(3);
	}

	FS = FS->CreateFS(type, data);

	Label4->Caption =  UnicodeString(fs_type.c_str());
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

	hDisk = CreateFileW(L"\\\\.\\F:", GENERIC_READ, FILE_SHARE_WRITE |
		  FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if (hDisk == INVALID_HANDLE_VALUE) {
		MessageBox(GetActiveWindow(), L"Ошибка чтения диска", L"Ошибка",
		  MB_ICONERROR); exit(1);
	}


	VirtualStringTree1->BeginUpdate();

	for (ClusterNumber=1; ClusterNumber<=FS->GetClustersCount(); ClusterNumber++) {
		  buf = FS->ReadCluster(hDisk, buf);
		  BYTE *signatura = (char*)AnsiString(Edit1->Text).c_str();
		  BYTE *find = FS->FindSignatureInCluster(buf, signatura);
		  if (find != NULL) {
		  //ptrFile = fopen("file.txt", "a");
		  //FS->PrintClusterToFile(ptrFile, buf, ClusterNumber);
		  PVirtualNode entryNode = VirtualStringTree1->AddChild(VirtualStringTree1->RootNode);
		  OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(entryNode);
		  table->ClusterNumber = ClusterNumber;
		  table->Signatura = Edit1->Text;
		  }
	}

	VirtualStringTree1->EndUpdate();
    Label10->Caption = "Проанализировано " + UnicodeString(ClusterNumber) +
			" из " + UnicodeString(FS->GetClustersCount()) + " кластеров";
	delete[] buf;
	CloseHandle(hDisk);
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
