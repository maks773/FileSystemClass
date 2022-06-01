//---------------------------------------------------------------------------

#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <string>
#include "Unit2.h"
#include "sqlite3.h"
//---------------------------------------------------------------------------
class ReadCluster : public TThread
{
private:
	BYTE *buf;
	ULONGLONG ClusterNumber;
	TVirtualStringTree *VirtualStringTree1;
	class FileSystemClass *FS;
	std::string file_category;
	OriginTable *table;
	sqlite3* Database;
	FILE *ptrFile;
protected:
	void __fastcall Execute();
public:
	__fastcall ReadCluster(bool, FileSystemClass *, std::string, TVirtualStringTree *);
	class FindExt *ptrFindExt;
	class Otrisovka *ptrRis;
};

class FindExt : public TThread
{
private:
	
	TVirtualStringTree *VirtualStringTree1;
	PVirtualNode entryNode;
	sqlite3* Database;
	FILE *ptrFile;
	std::string file_category;
protected:
	void __fastcall Execute();
public:
	__fastcall FindExt(bool, BYTE *, ULONGLONG, TVirtualStringTree *, FileSystemClass *,
	   std::string, sqlite3*, FILE *);
	void __fastcall AddNodeVS();
	//void __fastcall ResumFunc();
	BYTE *buf;
	ULONGLONG ClusterNumber;
	FileSystemClass *FS;
};

class Otrisovka: public TThread
{
protected:
	void __fastcall Execute();
public:
	__fastcall Otrisovka(bool);
	void __fastcall BarLabel();
};
//---------------------------------------------------------------------------
#endif
