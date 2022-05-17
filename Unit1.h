//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class MyThread : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall MyThread(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
