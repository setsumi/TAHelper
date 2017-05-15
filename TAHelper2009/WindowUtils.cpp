//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WindowUtils.h"

//---------------------------------------------------------------------------
int wndGetWindowTextLen(HWND hwnd)
{
	return (int)SendMessage(hwnd, WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);
}
//---------------------------------------------------------------------------
UnicodeString wndGetWindowText(HWND hwnd)
{
	UnicodeString ret;
	LRESULT textlen = SendMessage(hwnd, WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);
	if (textlen) {
		wchar_t *tbuf = new wchar_t[textlen + 1];
		SendMessage(hwnd, WM_GETTEXT, (WPARAM)(textlen + 1), (LPARAM)tbuf);
		ret = tbuf;
		delete []tbuf;
	}
	return ret;
}
//---------------------------------------------------------------------------
UnicodeString wndGetWindowTitle(HWND hwnd)
{
	wchar_t *tbuf = new wchar_t[512]; tbuf[0] = 0;
	GetWindowText(hwnd, tbuf, 512);
	UnicodeString ret(tbuf);
	delete []tbuf;
	return ret;
}
//---------------------------------------------------------------------------
UnicodeString wndGetWindowClass(HWND hwnd)
{
	wchar_t *tbuf = new wchar_t[512]; tbuf[0] = 0;
	GetClassName(hwnd, tbuf, 511);
	UnicodeString ret(tbuf);
	delete []tbuf;
	return ret;
}
//---------------------------------------------------------------------------
void wndButtonClick(HWND hwnd)
{
	PostMessage(hwnd, WM_LBUTTONDOWN, (WPARAM)0, (LPARAM)0);
	PostMessage(hwnd, WM_LBUTTONUP, (WPARAM)0, (LPARAM)0);
}
//---------------------------------------------------------------------------
void wndSetWindowText(HWND hwnd, UnicodeString text) {
	SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)text.w_str());
}
//---------------------------------------------------------------------------

