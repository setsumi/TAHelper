//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop

#include "Header.h"
//---------------------------------------------------------------------------
//   Important note about DLL memory management when your DLL uses the
//   static version of the RunTime Library:
//
//   If your DLL exports any functions that pass String objects (or structs/
//   classes containing nested Strings) as parameter or function results,
//   you will need to add the library MEMMGR.LIB to both the DLL project and
//   any other projects that use the DLL.  You will also need to use MEMMGR.LIB
//   if any other projects which use the DLL will be performing new or delete
//   operations on any non-TObject-derived classes which are exported from the
//   DLL. Adding MEMMGR.LIB to your project will change the DLL and its calling
//   EXE's to use the BORLNDMM.DLL as their memory manager.  In these cases,
//   the file BORLNDMM.DLL should be deployed along with your DLL.
//
//   To avoid using BORLNDMM.DLL, pass string information using "char *" or
//   ShortString parameters.
//
//   If your DLL uses the dynamic version of the RTL, you do not need to
//   explicitly add MEMMGR.LIB as this will be done implicitly for you
//---------------------------------------------------------------------------
USEUNIT("File2.cpp");
USEDEF("Shared.def");

//---------------------------------------------------------------------------
extern HWND				hTarget;  // target windows handle
extern HWND				hEditor; // targer child control, receiving translation messages
extern HWND				hApp; // handle of main app
extern HINSTANCE	hInstance;
extern HHOOK			hWinHook;
extern LONG				OldWndProc; // target window old WindowProc
extern LONG				OldWndProcEditor; // target child editor old WindowProc
extern BOOL				CtrlDown; // flag to tress Ctrl in child editor WindowProc
//---------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcEditor(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------


#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	switch(reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			hInstance=(HINSTANCE)hinst;
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			Unhook();
		}
		break;
	}
	return 1;
}

//---------------------------------------------------------------------------
//Set back the old window procedure
void WINAPI UnSubclass()
{
	if (GetWindowLong(hTarget, GWL_WNDPROC) == (LONG)WindowProc) {
		SetWindowLong(hTarget, GWL_WNDPROC, (LONG)OldWndProc);
	}
	if (GetWindowLong(hEditor, GWL_WNDPROC) == (LONG)WindowProcEditor) {
		SetWindowLong(hEditor, GWL_WNDPROC, (LONG)OldWndProcEditor);
	}
}
//---------------------------------------------------------------------------
//Set new window procedure
void WINAPI Subclass()
{
	if (GetWindowLong(hTarget, GWL_WNDPROC) != (LONG)WindowProc) {
		OldWndProc = SetWindowLong(hTarget, GWL_WNDPROC, (LONG)WindowProc);
	}
	if (GetWindowLong(hEditor, GWL_WNDPROC) != (LONG)WindowProcEditor) {
		OldWndProcEditor = SetWindowLong(hEditor, GWL_WNDPROC, (LONG)WindowProcEditor);
	}
}
//---------------------------------------------------------------------------
void DLL_EXPORT Unhook()
{
	UnSubclass();
	if (hWinHook) {
		UnhookWindowsHookEx(hWinHook);
		hWinHook = 0;
	}
}

//---------------------------------------------------------------------------
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0) {
		Subclass();
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

//---------------------------------------------------------------------------
int DLL_EXPORT Hook(HWND HandleofTarget, HWND HandleofApp, HWND HandleofEditor)
{
	hTarget = HandleofTarget;
	hApp = HandleofApp;
	hEditor = HandleofEditor;
	hWinHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)HookProc, hInstance, GetWindowThreadProcessId(hTarget, NULL));
	if (hWinHook)
		SendNotifyMessage(hTarget, WM_APP+1024, 0, 0); // trigger hook and subclassing (since hook is message-based)
	return hWinHook != 0;
}

//---------------------------------------------------------------------------
// target window subclassed Window Procedure
typedef int (CALLBACK* wndproc)();
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_GETMINMAXINFO) {
		MINMAXINFO *pmmi = (MINMAXINFO*)lParam;
		pmmi->ptMinTrackSize.x = 300;
		pmmi->ptMinTrackSize.y = 155;
		return 0;
	}
	else if (uMsg == WM_WINDOWPOSCHANGING) {
		return 0;
	}
	else if (uMsg == WM_DESTROY) {
		// Send the message to the main app to make FreeLibrary() call
		SendNotifyMessage(hApp, WM_APP+1024, (WPARAM)(LOWORD(wParam)), (LPARAM)uMsg);
	}
	else if (CtrlDown) {
		// force Ctrl pressed then receiving translation keyboard input
		BYTE ks[256]; GetKeyboardState(ks);
		ks[VK_CONTROL] = 0x80;
		SetKeyboardState(ks);
	}
	return CallWindowProc((wndproc)OldWndProc, hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------
void DLL_EXPORT PressCtrl(BOOL down)
{
	CtrlDown = down;

	if (!CtrlDown) {
		BYTE ks[256]; GetKeyboardState(ks);
		ks[VK_CONTROL] = 0x00;
		SetKeyboardState(ks);
	}
}
//---------------------------------------------------------------------------
// target child editor subclassed Window Procedure
LRESULT CALLBACK WindowProcEditor(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (CtrlDown) {
		// force Ctrl pressed then receiving translation keyboard input
		BYTE ks[256]; GetKeyboardState(ks);
		ks[VK_CONTROL] = 0x80;
		SetKeyboardState(ks);
	}
	return CallWindowProc((wndproc)OldWndProcEditor, hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

