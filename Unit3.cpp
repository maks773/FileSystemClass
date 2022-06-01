//---------------------------------------------------------------------------

#include <System.hpp>
#pragma hdrstop

#include "Unit3.h"
#include "Unit2.h"
#include "sqlite3.h"
#include "Classes.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#pragma package(smart_init)

//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall ReadCluster::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

HANDLE hEvent1, hEvent2, hEvent3, hEvent4;

__fastcall ReadCluster::ReadCluster(bool CreateSuspended, FileSystemClass *_FS,
	string _file_category, TVirtualStringTree *_VirtualStringTree1) : TThread(CreateSuspended)
{

   hEvent1 = CreateEvent(NULL, FALSE, TRUE, NULL);
   hEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
   hEvent3 = CreateEvent(NULL, FALSE, FALSE, NULL);
   hEvent4 = CreateEvent(NULL, FALSE, TRUE, NULL);

   FS = _FS; file_category = _file_category;
   VirtualStringTree1 = _VirtualStringTree1; ptrFindExt = NULL;

   ptrFile = fopen("file.txt", "a");

   int res = sqlite3_open16(L"FindExtensions.db", &Database);
   if (res != 0)  PrintError(5);

   buf = new BYTE[FS->GetClusterSize()];
   ClusterNumber = FS->GetFirstClusterNumber();
   Priority = tpTimeCritical;
}

//---------------------------------------------------------------------------

void __fastcall ReadCluster::Execute()
{
	FreeOnTerminate = true;
	ClustersIterator *it;

	switch (Form2->ComboBox1->ItemIndex) {
	  case 0:  it = new ClustersIterator(FS); break;     //итератор
	  case 1:  it = new ClustersDecorator(new ClustersIterator(FS)); break;   //декоратор
	  default: it = new ClustersIterator(FS);
	}

	ptrFindExt = new FindExt(false, buf, ClusterNumber, VirtualStringTree1,
		   FS, file_category, Database, ptrFile);

	ptrRis = new Otrisovka(false);

	for(it->First(); !it->IsDone(); it->Next()) {

		WaitForSingleObject(hEvent1, INFINITE);
		SetEvent(hEvent3);

		if (Terminated) {
		   ptrFindExt->Terminate();
		   ptrRis->Terminate();
		   break;
		}

		ptrFindExt->ClusterNumber = it->GetCurrentNum();
		ptrFindExt->buf = it->CurrentItem();

		if (ptrFindExt->buf == 0) {
			SetEvent(hEvent1);
			continue;
		}

		SetEvent(hEvent2);
	}

	if (Form2->ProgressBar1->Position < Form2->ProgressBar1->Max)
		Form2->ProgressBar1->Position = Form2->ProgressBar1->Max;

	sqlite3_close(Database);
	fclose(ptrFile);
	delete[] buf;
	delete it;
	Form2->Button1->Enabled = true;
	Form2->Button3->Enabled = true;
	Form2->Button4->Enabled = false;
}
//---------------------------------------------------------------------------

__fastcall FindExt::FindExt(bool CreateSuspended, BYTE *_buf, ULONGLONG _ClusterNumber,
   TVirtualStringTree *_VirtualStringTree1, FileSystemClass *_FS,
	std::string _file_category, sqlite3* _Database, FILE *_ptrFile) : TThread(CreateSuspended)
{
  ClusterNumber = _ClusterNumber; buf = _buf;
  VirtualStringTree1 = _VirtualStringTree1; FS = _FS;
  file_category = _file_category;  Database =_Database;   ptrFile = _ptrFile;
  Priority = tpTimeCritical;
}

void __fastcall FindExt::AddNodeVS()
{
   entryNode = VirtualStringTree1->AddChild(VirtualStringTree1->RootNode);
}

void __fastcall FindExt::Execute()
{
  FreeOnTerminate = true;

  for (;;) {

	  WaitForSingleObject(hEvent2, INFINITE);

	  string Extension = "";

	  if (file_category == "image") {
		Extension = FindImage(buf, FS);
	  } else

	  if (file_category == "audio") {
		Extension = FindAudio(buf, FS);
	  } else

	  if (file_category == "video") {
		Extension = FindVideo(buf, FS);
	  } else

	  if (file_category == "document") {
		Extension = FindDoc(buf, FS);
	  }

	  if (!Extension.empty()) {

		//вывод в VirtualStringTree
		Synchronize(&AddNodeVS);
		OriginTable *table = (OriginTable*)VirtualStringTree1->GetNodeData(entryNode);
		table->ClusterNumber = ClusterNumber;
		table->Signatura = Extension.c_str();

		//вывод в файл
		//FS->PrintClusterToFile(ptrFile, buf, ClusterNumber);

		//вывод в БД
		WriteToDB(Database, ClusterNumber, Extension);
	  }

	  SetEvent(hEvent1);
	  if (Terminated) break;
  }
}


__fastcall Otrisovka::Otrisovka(bool CreateSuspended)
{
  Priority = tpTimeCritical;
}

void __fastcall Otrisovka::BarLabel()
{
  if (Form2->ptrReadCluster->ptrFindExt->ClusterNumber % 20000 == 0)
	Form2->ProgressBar1->StepIt();

  Form2->Label10->Caption = "Проанализировано " + UnicodeString(Form2->
	ptrReadCluster->ptrFindExt->ClusterNumber) + " из " + UnicodeString(Form2->
	ptrReadCluster->ptrFindExt->FS->GetClustersCount()) + " кластеров";
}

void __fastcall Otrisovka::Execute()
{
  FreeOnTerminate = true;
  for (;;) {
	WaitForSingleObject(hEvent3, INFINITE);
	Synchronize(&BarLabel);
	if (Terminated) break;
  }
}



