//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("Unit1.cpp", FormTAHelper);
USEFORM("UnitOSD.cpp", FormOSD);
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
int PASCAL wWinMain (HINSTANCE, HINSTANCE, LPWSTR, int)
{
	try
	{
		// do not allow multiple instances
		HWND taw = FindWindow(L"TFormTAHelper", NULL);
		if (taw) {
			BringWindowToFront(taw);
		} else {
			Application->Initialize();
			Application->CreateForm(__classid(TFormTAHelper), &FormTAHelper);
		Application->CreateForm(__classid(TFormOSD), &FormOSD);
		Application->Run();
		}
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
