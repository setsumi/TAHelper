//---------------------------------------------------------------------------
// Borland C++Builder
// Copyright (c) 1987, 1999 Inprise Corporation. All Rights Reserved.
//---------------------------------------------------------------------------

#pragma option -zRSHSEG      // change default data segment name
#pragma option -zTSHCLASS    // change default data class name

#include <windows.h>

// Here is the initialized data that will be shared.
HWND			hTarget = 0;  // target window handle
HWND			hEditor = 0; // targer child control, receiving translation messages
HWND			hApp = 0;
HINSTANCE	hInstance = 0;
HHOOK			hWinHook = 0;
LONG			OldWndProc = 0; // target old WindowProc
LONG			OldWndProcEditor = 0; // target child editor old WindowProc
BOOL			CtrlDown = 0; // flag to tress Ctrl in child editor WindowProc

