//---------------------------------------------------------------------------

#ifndef WakanH
#define WakanH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//#include <Controls.hpp>
//#include <StdCtrls.hpp>
//#include <Forms.hpp>
//#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
#define WAKAN_TEXTBUF_LENGTH	512

// hook DLL function pointer types
typedef int (CALLBACK* dllhook)(HWND,HWND,HWND); // Hook()
typedef void (CALLBACK* dllpressctrl)(BOOL); // PressCtrl()

class tWakan
{
private:
	HWND hwRoot, hwEditor, hwToolbar;

	HMODULE hDll;
	int bCompact;
	TTimer *pTimer;
	dllhook dllHook;
	dllpressctrl dllPressCtrl;

public:
	tWakan(TTimer *timer);
	~tWakan();

	HWND GetRoot() { return hwRoot; }
	HWND GetEditor() { return hwEditor; }

	void Compact();
	bool FindWakan();
	bool DllControl(HWND owner);
	void FreeDll();
	void CompactControl(int comp=-1);
	void Translate(); // begin translation
	bool OnTimer(); // sent keys one by one
	bool ShowHide(bool exit=false);

private:
	void Init();
	bool ValidateWindows();
	static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
};

//---------------------------------------------------------------------------
#endif // WakanH
