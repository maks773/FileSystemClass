//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit3.h"
#include "Unit2.h"
#include "sqlite3.h"
#include "Classes.h"
#include <iostream>
#include <string>
using namespace std;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VirtualTrees"
#pragma resource "*.dfm"
TForm2 *Form2;   sqlite3 *Database;
FileSystemClass *FS;  string file_category = "image";
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{
	ptrReadCluster = NULL;
	VirtualStringTree1->NodeDataSize = sizeof(OriginTable);

	FS = IdentifyFS();

	Label4->Caption =  FS->GetFileSystemType().c_str();
	Label2->Caption =  FS->GetClusterSize();
	Label6->Caption =  FS->GetClustersCount();
	Label8->Caption =  FS->GetFileSystemSize();
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender)
{
	Button1->Enabled = false;
	Button3->Enabled = false;
	Button4->Enabled = true;
	VirtualStringTree1->Clear();
	ProgressBar1->Step = int(20000.0 / FS->GetClustersCount() * 100);
	ProgressBar1->Position = ProgressBar1->Step;
	ptrReadCluster = new ReadCluster(false, FS, file_category, VirtualStringTree1);
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
void __fastcall TForm2::VirtualStringTree1AddToSelection(TBaseVirtualTree *Sender,
		  PVirtualNode Node)
{
  if (Node == NULL) return;
  if (VirtualStringTree1->FocusedNode)  Button2->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button2Click(TObject *Sender)
{
  DeleteEntry(VirtualStringTree1, Database);
  Button2->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button3Click(TObject *Sender)
{
  DeleteAll(VirtualStringTree1, Database);
  Button2->Enabled = false;
  Button3->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TForm2::RadioButton1Click(TObject *Sender)
{
  file_category = "image";
}
//---------------------------------------------------------------------------

void __fastcall TForm2::RadioButton2Click(TObject *Sender)
{
  file_category = "audio";
}
//---------------------------------------------------------------------------

void __fastcall TForm2::RadioButton3Click(TObject *Sender)
{
  file_category = "video";
}
//---------------------------------------------------------------------------

void __fastcall TForm2::RadioButton4Click(TObject *Sender)
{
  file_category = "document";
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button4Click(TObject *Sender)
{
  ptrReadCluster->Terminate();
  Button4->Enabled = false;
  Button1->Enabled = true;
  Button3->Enabled = true;
}
//---------------------------------------------------------------------------

