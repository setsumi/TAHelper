//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Wakan.h"

//---------------------------------------------------------------------------
tWakan::tWakan(TTimer *timer)
{
	hDll = 0;
	bCompact = 0;
	pTimer = timer;
	dllHook = 0;
	dllPressCtrl = 0;

	Init();
}
tWakan::~tWakan()
{
	FreeDll();
	ShowHide(true);
}
//---------------------------------------------------------------------------
void tWakan::Init()
{
	hwRoot = hwEditor = hwToolbar = 0;
}

//---------------------------------------------------------------------------
bool tWakan::DllControl(HWND owner)
{
	bool rv = FindWakan();
	if (rv) {
		if (!hDll) {
			hDll = LoadLibrary(L"TAHWKhook.dll");
			if (hDll) {
				dllHook = (dllhook)GetProcAddress(hDll, "Hook");
				if (dllHook) {
					dllPressCtrl = (dllpressctrl)GetProcAddress(hDll, "PressCtrl");
					if (dllPressCtrl) {
						dllHook(hwRoot, owner, hwEditor); // Hook !!!
					} else {
						FreeDll();
						throw Exception(L"tWakan::DllControl() -> GetProcAddress(2) failed");
					}
				} else {
					FreeDll();
					throw Exception(L"tWakan::DllControl() -> GetProcAddress(1) failed");
				}
			} else {
				throw Exception(L"tWakan::DllControl() -> LoadLibrary() failed");
			}
		}
	} else {
		FreeDll();
	}
	return rv;
}
//---------------------------------------------------------------------------
void tWakan::FreeDll()
{
	if (hDll) {
		FreeLibrary(hDll);
		hDll = 0;
		dllHook = 0;
		dllPressCtrl = 0;
	}
}

//---------------------------------------------------------------------------
void tWakan::CompactControl(int comp)
{
	static int prev = 0;
	if (comp != -1) {
		bCompact = comp;
	}
	if (FindWakan()) {
		if (bCompact == 1 && prev == 0) {
			prev = bCompact;
			Compact();
		}
	} else {
    prev = 0;
  }
}
//---------------------------------------------------------------------------
void tWakan::Compact()
{
	if (FindWakan()) {
		HMENU hm = GetMenu(hwRoot);
		if (hm) {
			SetMenu(hwRoot, NULL);
			DestroyMenu(hm);
		}
		if (hwToolbar) {
			RECT rc;
			GetWindowRect(hwToolbar, &rc);
			if (rc.bottom - rc.top > 5) {
				SetWindowPos(hwToolbar, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE);
			}
		}
	}
}

//---------------------------------------------------------------------------
bool tWakan::FindWakan()
{
	hwRoot = FindWindow(L"TfMenu", L"WaKan 1.67 - Tool for learning Japanese & Chinese");
	if (hwRoot) {
		hwEditor = NULL;
		EnumChildWindows(hwRoot, (WNDENUMPROC)EnumChildProc, (LPARAM)this);
	}
	return ValidateWindows();
}

//---------------------------------------------------------------------------
bool tWakan::OnTimer()
{
	if (!FindWakan() || !IsWindowVisible(hwEditor) || !hDll) { // lost in translation
		return false;
	}

	switch(pTimer->Tag) {
	case 0:
		// select all text - Ctrl+A
		dllPressCtrl(TRUE);
		PostMessage(hwEditor, WM_KEYDOWN, 'A', 1);
		break;
	case 1:
		// delete text - Delete  (Ctrl pressed state is no big deal here)
		PostMessage(hwEditor, WM_KEYDOWN, VK_DELETE, 1);
		pTimer->Interval = 2000; // wait for slowpoke WaKan to update its clipboard
		break;
	case 2:
		// paste from clipboard - Ctrl+V
		PostMessage(hwEditor, WM_KEYDOWN, 'V', 1);
		pTimer->Interval = 25; // now fast
		break;
	case 3:
		// select all text - Ctrl+A
		PostMessage(hwEditor, WM_KEYDOWN, 'A', 1);
		break;
	case 4:
		// translate - Ctrl+F
		PostMessage(hwEditor, WM_KEYDOWN, 'F', 1);
		pTimer->Interval = 200; // average translation time
		break;
	case 5:
		// move cursor on top - Ctrl+PageUp
		PostMessage(hwEditor, WM_KEYDOWN, VK_PRIOR, 1);
		pTimer->Interval = 2000; // keep Ctrl pressed longer
		break;
	case 6:
		{
			// release Ctrl
			dllPressCtrl(FALSE);
			// detach input from Wakan
			DWORD tid = GetWindowThreadProcessId(hwEditor, NULL);
			AttachThreadInput(GetCurrentThreadId(), tid, FALSE);
		}
		break;
	}
	return true;
}
//---------------------------------------------------------------------------
void tWakan::Translate()
{
	pTimer->Enabled = false;

	if (FindWakan() && IsWindowVisible(hwEditor) && hDll) {
		// attach input to Wakan
		DWORD tid = GetWindowThreadProcessId(hwEditor, NULL);
		AttachThreadInput(GetCurrentThreadId(), tid, TRUE);
		SetFocus(hwEditor);
		// begin translation
		pTimer->Tag = 0;
		pTimer->Interval = 25;
		pTimer->Enabled = true;
	}
}

//---------------------------------------------------------------------------
bool tWakan::ValidateWindows()
{ // don't care about toolbar
	bool ret = (hwRoot && IsWindow(hwRoot) &&
							hwEditor && IsWindow(hwEditor));
	if (!ret) Init();
	return ret;
}

//---------------------------------------------------------------------------
BOOL CALLBACK tWakan::EnumChildProc(HWND hwnd, LPARAM lParam) {
	tWakan *caller = (tWakan*)lParam;
	wchar_t *tbuf = new wchar_t[WAKAN_TEXTBUF_LENGTH]; *tbuf = 0;
	UnicodeString s;
	GetClassName(hwnd, tbuf, WAKAN_TEXTBUF_LENGTH-1);
	s = tbuf;
	if (s == L"TPanel") {
		RECT r;
		GetClientRect(hwnd, &r);
		if (r.bottom == 33) {
			caller->hwToolbar = hwnd; // toolbar
		}
	}
	else if (s == L"TListBox") {
		GetWindowText(hwnd, tbuf, WAKAN_TEXTBUF_LENGTH-1);
		s = tbuf;
		if (s == L"") {
			caller->hwEditor = hwnd; // editor
		}
	}
	delete []tbuf;
	return TRUE;
}

//---------------------------------------------------------------------------
bool tWakan::ShowHide(bool exit)
{
	bool rv = false;
	if (FindWakan()) {
		if (exit || !IsWindowVisible(hwRoot)) {
			ShowWindow(hwRoot, SW_SHOWNA);
			rv = true;
		} else {
			ShowWindow(hwRoot, SW_HIDE);
			rv = false;
		}
	}
	return rv;
}

//---------------------------------------------------------------------------

