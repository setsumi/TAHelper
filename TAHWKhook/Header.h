//---------------------------------------------------------------------------
// Borland C++Builder
// Copyright (c) 1987, 1999 Inprise Corporation. All Rights Reserved.
//---------------------------------------------------------------------------

#ifdef __DLL__
#define DLL_EXPORT __declspec(dllexport) __stdcall
#else
#define DLL_EXPORT __declspec(dllimport) __stdcall
#endif

extern "C"
{
	int DLL_EXPORT Hook(HWND HandleofTarget, HWND HandleofApp, HWND HandleofEditor);
	void DLL_EXPORT Unhook();
	void DLL_EXPORT PressCtrl(BOOL down);
}

void WINAPI Subclass();
void WINAPI UnSubclass();

