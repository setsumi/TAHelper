//---------------------------------------------------------------------------
#ifndef WindowUtilsH
#define WindowUtilsH

//---------------------------------------------------------------------------
#include <Classes.hpp>

//---------------------------------------------------------------------------
int wndGetWindowTextLen(HWND hwnd);
UnicodeString wndGetWindowText(HWND hwnd);
UnicodeString wndGetWindowTitle(HWND hwnd);
UnicodeString wndGetWindowClass(HWND hwnd);
void wndButtonClick(HWND hwnd);
void wndSetWindowText(HWND hwnd, UnicodeString text);

//---------------------------------------------------------------------------
#endif // WindowUtilsH
