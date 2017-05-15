//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Utils.h"

//---------------------------------------------------------------------------
UnicodeString GetModuleVersion()
{
	UnicodeString ret;
	DWORD hnd=0, vis=0;
	if(0 != (vis = GetFileVersionInfoSize(GetModuleName(unsigned(HInstance)).w_str(), &hnd))) {
		BYTE *data = new BYTE[vis];
		if(GetFileVersionInfo(GetModuleName(unsigned(HInstance)).w_str(), hnd, vis, data)) {
			UINT ilen = 0;
			struct LANGANDCODEPAGE {
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;
			// Read first language and code page
			if(VerQueryValue(data, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &ilen)) {
				UnicodeString str;
				str.printf(L"\\StringFileInfo\\%04x%04x\\FileVersion", lpTranslate->wLanguage, lpTranslate->wCodePage);
				// Retrieve file version for language and code page
				wchar_t *pver = NULL;
				if(VerQueryValue(data, str.w_str(), (LPVOID*)&pver, &ilen)) {
          ret = pver;
        }
			}
		}
		delete []data;
	}
	return ret;
}

//---------------------------------------------------------------------------
UnicodeString GetModuleName()
{
	return GetModuleName(unsigned(HInstance));
}
//---------------------------------------------------------------------------
UnicodeString GetModuleDir()
{
	UnicodeString name(GetModuleName(unsigned(HInstance)));
	return name.SubString(1, name.LastDelimiter(L"\\"));
}

//---------------------------------------------------------------------------
// This function demonstrates how to remove the
// row specified by the Index parameter... //
void RemoveRow(TStringGrid* StringGrid, int Index)
{
  assert(StringGrid != NULL);
  SNDMSG(StringGrid->Handle, WM_SETREDRAW, false, 0);
  try
  {
    const int row_count = StringGrid->RowCount;

    // (1) shift the contents of the trailing columns
		for (int row = Index; row < row_count - 1; ++row)
    {
			StringGrid->Rows[row] = StringGrid->Rows[row + 1];
		}

		// (2) remove the last row
    StringGrid->RowCount = row_count -1;
  }
  __finally
  {
    SNDMSG(StringGrid->Handle, WM_SETREDRAW, true, 0);
  }

	// update (repaint) the shifted rows
  RECT R = StringGrid->CellRect(0, Index);
  InflateRect(&R, StringGrid->Width, StringGrid->Height);
  InvalidateRect(StringGrid->Handle, &R, false);
}
//---------------------------------------------------------------------------
// This function demonstrates how to insert a new row
// after the row specified by the AfterIndex parameter... //
void InsertRow(TStringGrid* StringGrid, int AfterIndex)
{
  assert(StringGrid != NULL);
  SNDMSG(StringGrid->Handle, WM_SETREDRAW, false, 0);
  try
  {
    const int row_count = StringGrid->RowCount;

    // (1) append a new row to the end
    StringGrid->RowCount = row_count + 1;

    // (2) shift the contents of the trailing rows
    for (int row = row_count; row > AfterIndex + 1; --row)
    {
      StringGrid->Rows[row] = StringGrid->Rows[row - 1];
    }
    StringGrid->Rows[AfterIndex + 1]->Clear();
  }
  __finally
  {
    SNDMSG(StringGrid->Handle, WM_SETREDRAW, true, 0);
  }

  // update (repaint) the shifted rows
  RECT R = StringGrid->CellRect(0, AfterIndex);
  InflateRect(&R, StringGrid->Width, StringGrid->Height);
  InvalidateRect(StringGrid->Handle, &R, false);
}

//---------------------------------------------------------------------------
AnsiString BinToHex(void *data, int size)
{
	char *hex_s = new char[size*2+1]; hex_s[size*2] = '\0';
	BinToHex(data, hex_s, size);
	AnsiString hex_a(hex_s);
	delete []hex_s;
	return hex_a;
}
AnsiString StrToHex(UnicodeString text)
{
	void *data = text.w_str();
	int size = (text.Length()+1)*sizeof(wchar_t); // zero-terminator added too
	return BinToHex(data, size);
}
UnicodeString HexToStr(AnsiString hex)
{
	int len = hex.Length()/2/sizeof(wchar_t);  // zero-terminator already included by _StrToHex()
	wchar_t *str_s = new wchar_t[len];
	HexToBin(hex.c_str(), (void*)str_s, hex.Length()/2);
	return UnicodeString(str_s);
}

//---------------------------------------------------------------------------
BOOL BringWindowToFront(HWND hwnd)
{
	DWORD lForeThreadID = 0;
	DWORD lThisThreadID = 0;
	BOOL lReturn = TRUE;

	// Make a window, specified by its handle (hwnd)
	// the foreground window.

	// If it is already the foreground window, exit.
	if(hwnd != GetForegroundWindow()) {

		// Get the threads for this window and the foreground window.

		lForeThreadID = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
		lThisThreadID = GetWindowThreadProcessId(hwnd, NULL);

		// By sharing input state, threads share their concept of
		// the active window.

		if(lForeThreadID != lThisThreadID) {
			// Attach the foreground thread to this window.
			AttachThreadInput(lForeThreadID, lThisThreadID, TRUE);
			// Make this window the foreground window.
			lReturn = SetForegroundWindow(hwnd);
			// Detach the foreground window's thread from this window.
			AttachThreadInput(lForeThreadID, lThisThreadID, FALSE);
		} else {
			lReturn = SetForegroundWindow(hwnd);
		}

		// Restore this window to its normal size.

		if(IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		} else {
			ShowWindow(hwnd, SW_SHOW);
		}
	}
	return lReturn;
}

//---------------------------------------------------------------------------

