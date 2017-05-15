//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop

#include <Registry.hpp>
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
#define EXPORT __declspec(dllexport)
#include "TAPlugin.h"
#include "File1.h"
#include "Utils.h"
#include "WindowUtils.h"
#include "UnitSubst.h"
#include "UnitOptions.h"
//---------------------------------------------------------------------------
USEFORM("UnitSubst.cpp", FormTAHsubst);
//---------------------------------------------------------------------------

#define TAHPLUGIN_WNDCLASS_MESSAGE	L"TAHPluginMessageWindow"
#define TAH_WNDCLASS_MAINFORM				L"TFormTAHelper"		 // in sync with TAHelper

#define TAHPLUGIN_REPLACEMENTS_DIR	L"Replacements\\"
#define TAH_FILE_NAMES							L"names.txt"   // in sync with TAHelper for simplicity

HWND CreateMessageWindow();
bool ValidateWindows();
void LoadConfig();
UnicodeString GenNamesFileName(UnicodeString profile);
void LoadRepl(tActRepl *pRepl);
void ProcessRepl(UnicodeString &str, tActRepl *prepl);
wchar_t * DoSubstitutions(const wchar_t *in);

HINSTANCE		hInstance = 0;
HWND				hMsgWnd = 0;
HWND				hTAHmainWnd = 0;
TStringList	*pProfileList = 0;
TStringList	*pActiveProfileList = 0;
tConfig			Config;
TList				*pActReplList = 0;
tActRepl		*pGlobalRepl = 0;
DWORD				ProcessID = 0;

//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	switch(reason)
	{
	case DLL_PROCESS_ATTACH:
		try {
			hInstance = hinst;
			LoadConfig();
			hMsgWnd = CreateMessageWindow();
			if (!hMsgWnd) throw Exception(L"CreateMessageWindow() failed");
			FormTAHsubst = new TFormTAHsubst(0);
			FormOptions = new TFormOptions(0);
			pProfileList = new TStringList();
			InitTStringList(pProfileList, true);
			pActiveProfileList = new TStringList();
			InitTStringList(pActiveProfileList, true);
			pActReplList = new TList();
			pGlobalRepl = new tActRepl(TAHPLUGIN_GLOBAL_PROFILE);
			GetWindowThreadProcessId(hMsgWnd, &ProcessID);
			// create default subfolders and files...
			if (!DirectoryExists(GetModuleDir() + TAHPLUGIN_REPLACEMENTS_DIR)) { // replacements folder
				CreateDir(GetModuleDir() + TAHPLUGIN_REPLACEMENTS_DIR);
			}
			UnicodeString filename(GenNamesFileName(TAHPLUGIN_GLOBAL_PROFILE)); // global replacements file
			if (!FileExists(filename)) {
				TFileStream *file = new TFileStream(filename, fmCreate);
				file->Write((void*)FormTAHsubst->MemoInplace->Text.w_str() , FormTAHsubst->MemoInplace->Text.Length() * FormTAHsubst->MemoInplace->Text.ElementSize());
				delete file;
			}
		} catch(Exception &e)	{ ShowMessage(UnicodeString(L"TAHplugin => DllEntryPoint(DLL_PROCESS_ATTACH) : ") + e.Message);	}
			catch(...)					{ ShowMessage(L"TAHplugin => DllEntryPoint(DLL_PROCESS_ATTACH) : Unknown exception"); }
		break;
	case DLL_PROCESS_DETACH:
		try {
			SaveConfig();
			DestroyWindow(hMsgWnd);
			delete FormOptions;
			delete FormTAHsubst;
			delete pProfileList;
			delete pActiveProfileList;
			for (int i=0; i < pActReplList->Count; i++)
				delete (tActRepl*)pActReplList->Items[i];
			delete pActReplList;
			delete pGlobalRepl;
		} catch(Exception &e)	{ ShowMessage(UnicodeString(L"TAHplugin => DllEntryPoint(DLL_PROCESS_DETACH) : ") + e.Message);	}
			catch(...)					{ ShowMessage(L"TAHplugin => DllEntryPoint(DLL_PROCESS_DETACH) : Unknown exception"); }
		break;
	}
	return 1;
}

//---------------------------------------------------------------------------
// Init
unsigned int __stdcall TAPluginGetVersion(const void *) {
	return TA_PLUGIN_VERSION;
}
int __stdcall TAPluginInitialize(const TAInfo *taInfo, void **) {
	// At the moment, don't need to check taInfo version.
	return 1;
}

//---------------------------------------------------------------------------
// After substitution hook
wchar_t * __stdcall TAPluginModifyStringPostSubstitution(const wchar_t *in)
{
	// popup TA on translate
	if (Config.TApopupOnTrans) {
		HWND hwta = GetTAWindow();
		if (hwta) {
			BringWindowToFront(hwta);
		}
	}

	wchar_t *out=0;

	try {
		// Substitutions After
		//config
		if (!Config.SubstBeforeTA) {
			out = DoSubstitutions(in);
		}

		// SYSTRAN
		if (ValidateWindows()) {
			wndSetWindowText(hMsgWnd, (out)?out:in);
			SendMessage(hTAHmainWnd, WM_USER+1, 0, 0); // signal to TAHelper
		}
	} catch(Exception &e) { ShowMessage(UnicodeString(L"TAHplugin => TAPluginModifyStringPostSubstitution() : ") + e.Message); }
		catch(...)					{ ShowMessage(L"TAHplugin => TAPluginModifyStringPostSubstitution() : Unknown exception"); }

	return out;
}

//---------------------------------------------------------------------------
// Before substitution hook
wchar_t * __stdcall TAPluginModifyStringPreSubstitution(const wchar_t *in)
{
	wchar_t *out=0;

	try {
		// Substitutions Before
		//config
		if (Config.SubstBeforeTA) {
			out = DoSubstitutions(in);
		}
	} catch(Exception &e) { ShowMessage(UnicodeString(L"TAHplugin => TAPluginModifyStringPreSubstitution() : ") + e.Message); }
		catch(...)					{ ShowMessage(L"TAHplugin => TAPluginModifyStringPreSubstitution() : Unknown exception"); }

	return out;
}

//---------------------------------------------------------------------------
// Frees strings returned by previous function.
void __stdcall TAPluginFree(void *in) {
	free(in);
}

//---------------------------------------------------------------------------
// Update active profiles list
void __stdcall TAPluginActiveProfileList(int numActiveLists, const wchar_t **activeLists) {
	pActiveProfileList->Clear();
	for (int i=0; i<numActiveLists; i++) {
		UnicodeString profile(activeLists[i]);
		profile = strReplaceAll(profile, L"\\", L"+"); // in sync with TA
		if (profile != TAHPLUGIN_GLOBAL_PROFILE)
			pActiveProfileList->Add(profile);
	}
}

//---------------------------------------------------------------------------
// Substitutions work
wchar_t * DoSubstitutions(const wchar_t *in)
{
	//config
	if (!Config.SubstEnable) return 0;

	// delete inactive replacements
	for (int i=0; i < pActReplList->Count; i++) {
		tActRepl* prepl = (tActRepl*)pActReplList->Items[i];
		if (pActiveProfileList->IndexOf(prepl->Profile) == -1) {
			delete prepl;
			pActReplList->Delete(i);
			i--;
		}
	}
	// add new active replacements
	for (int i=0; i < pActiveProfileList->Count; i++) {
		if (FindLoadedProfile(pActiveProfileList->Strings[i]) == -1) {
			tActRepl *prepl = new tActRepl(pActiveProfileList->Strings[i]);
			pActReplList->Add(prepl);
		}
	}
	// load and process all replacements
	UnicodeString str(in);
	for (int i=0; i < pActReplList->Count; i++) {
		tActRepl *prepl = (tActRepl*)pActReplList->Items[i];
		LoadRepl(prepl);
		ProcessRepl(str, prepl);
	}
	LoadRepl(pGlobalRepl);
	ProcessRepl(str, pGlobalRepl);

	//debug
//	TStringList *ss = new TStringList();
//	ss->Text = str;
//	SaveNames(L"test_output", ss);
//	delete ss;
	//debugend

	// allocate and assign output string
	wchar_t *out = (wchar_t*) malloc(str.Length() * sizeof(wchar_t) + sizeof(wchar_t));
	wcscpy(out, str.w_str());
	return out;
}

//---------------------------------------------------------------------------
bool IsTag(UnicodeString str)
{
	return (str == TAHPLUGIN_TAG_TEXT || str == TAHPLUGIN_TAG_REGEX);
}
//---------------------------------------------------------------------------
tTag WhatTag(UnicodeString str)
{
	if (str == TAHPLUGIN_TAG_TEXT)
		return TagText;
	else if (str == TAHPLUGIN_TAG_REGEX)
		return TagRegex;
	return TagInvalid;
}

//---------------------------------------------------------------------------
void ProcessRepl(UnicodeString &str, tActRepl *prepl)
{
	TListEnumerator *i = prepl->pReplLst->GetEnumerator();
	while (i->MoveNext()) {
		TList *block = (TList*)i->GetCurrent();
		if (block->Count) {
			tName *n = (tName*)block->Items[0];
			tTag tag = WhatTag(n->orig);
			int j=1;
			switch (tag) {
			case TagInvalid: // Regex
				j=0;
			case TagRegex: // Regex
				for (; j < block->Count; j++) {
					tName *ni = (tName*)block->Items[j];
					//debug
//					ShowMessage(prepl->Profile+L"(R)"+ni->orig+L"->"+ni->repl);
					str = strRegexReplaceAll(str, ni->orig, ni->repl, L"TAHplugin => ");
				}
				break;
			case TagText: // Text
				for (; j < block->Count; j++) {
					tName *ni = (tName*)block->Items[j];
					//debug
//					ShowMessage(prepl->Profile+L"(T)"+ni->orig+L"->"+ni->repl);
					str = strReplaceAll(str, ni->orig, ni->repl);
				}
				break;
			}
		}
	}
}

//---------------------------------------------------------------------------
LRESULT CALLBACK MessageWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------
HWND CreateMessageWindow()
{
	// register class
	BYTE maskIcon[128]; ZeroMemory((void*)maskIcon, sizeof(BYTE) * 128);
	HICON hicon = CreateIcon(HInstance, 32, 32, 1, 1, maskIcon, maskIcon);
	WNDCLASS wclass;
	wclass.style = CS_DBLCLKS;
	wclass.lpfnWndProc = MessageWindowProc;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = hInstance;
	wclass.hIcon = hicon;
	wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclass.hbrBackground = CreateSolidBrush(RGB(0,0,0));
	wclass.lpszMenuName = NULL;
	wclass.lpszClassName = TAHPLUGIN_WNDCLASS_MESSAGE;
	RegisterClassW(&wclass);
	// create window
	return CreateWindowW(TAHPLUGIN_WNDCLASS_MESSAGE, L"",
											WS_POPUP, 0,0,0,0,
											NULL, NULL, hInstance, 0);
}

//---------------------------------------------------------------------------
bool ValidateWindows()
{
	if (!hMsgWnd || !IsWindow(hMsgWnd))
		return false;

	hTAHmainWnd = FindWindow(TAH_WNDCLASS_MAINFORM, NULL);
	if (!hTAHmainWnd)
		return false;

	return true;
}

//---------------------------------------------------------------------------
//	>0 (positive)   Item1 is less than Item2
//	0   Item1 is equal to Item2
//	<0 (negative)   Item1 is greater than Item2
int __fastcall CompareNames(void *Item1, void *Item2)
{
	int len1 = (reinterpret_cast<tName*>(Item1))->orig.Length();
	int len2 = (reinterpret_cast<tName*>(Item2))->orig.Length();
	return len2 - len1;
}
//---------------------------------------------------------------------------
// load resident replacements profile
void LoadRepl(tActRepl *pRepl)
{
	TBytes buffer;
	TFileStream *file;
	UnicodeString filename(GenNamesFileName(pRepl->Profile)); // file name
	// find file
	if (FileExists(filename)) {
		WIN32_FILE_ATTRIBUTE_DATA fileInfo;
		GetFileAttributesExW(filename.w_str(), GetFileExInfoStandard, &fileInfo);
		tFileSig srcSig;
		srcSig.createTime = *(__int64*)&fileInfo.ftCreationTime;
		srcSig.modTime = *(__int64*)&fileInfo.ftLastWriteTime;
		srcSig.size = fileInfo.nFileSizeLow + (((__int64)fileInfo.nFileSizeHigh)<<32);
		// check for file changes
		if (srcSig.createTime != pRepl->FileSig.createTime ||
				srcSig.modTime != pRepl->FileSig.modTime ||
				srcSig.size != pRepl->FileSig.size) {
    	// update file signature
			pRepl->FileSig.createTime = srcSig.createTime;
			pRepl->FileSig.modTime = srcSig.modTime;
			pRepl->FileSig.size = srcSig.size;
			// loading
			try {
				file = new TFileStream(filename, fmOpenRead);
				buffer.set_length(file->Size+1);
				file->Read(&buffer[0], buffer.Length-1);
				*(wchar_t*)&buffer[buffer.Length-1] = (wchar_t)0;
				delete file;
				// store file contents in temporary strings
				TStringList *strings = new TStringList();
				InitTStringList(strings, false);
				strings->Text = (wchar_t*)&buffer[0];
				// clear replacements list
				pRepl->ClearRepl();
				// process strings and fill replacements list
				tTag mode = TagRegex; // regex by default
				TList *block = new TList();
				UnicodeString si, orig, repl;
				for (int i=0; i < strings->Count; i++) {
					si = FormatSubstEntry(strings->Strings[i]); // some cleanup
					if (!si.Length()) continue; // ignore empty lines
					if (si[1] == L'*') continue; // ignore comments (lines starting with '*')
					// get orig and repl
					int pos = si.Pos(L"\t"); // find Tab-separator
					if (pos > 1 && pos < si.Length()) {
						repl = si.SubString(pos+1, si.Length()-pos); // new text to replace
						orig = si.SubString(1, pos-1); // original text
					} else { // no text to replace
						repl =L"";
						orig = si;
					}
					//
					tTag t = WhatTag(orig);
					if (t != TagInvalid) {
						if (t != mode) {
            	mode = t;
							pRepl->AddRepl(block); // finish with previous block
							block = new TList(); // create next block
							tName *name = new tName(orig, repl);
							block->Add(name); // add block line
						}
					} else {
						tName *name = new tName(orig, repl);
						block->Add(name); // add block line
					}
				}
				pRepl->AddRepl(block); // add last block
				delete strings;
				// sort text blocks
				TListEnumerator *i = pRepl->pReplLst->GetEnumerator();
				while (i->MoveNext()) {
					TList *sublist = (TList*)i->GetCurrent();
					if (sublist->Count) {
						tName *tmp = (tName*)sublist->Items[0];
						if (WhatTag(tmp->orig) == TagText) {
							sublist->Delete(0);
							sublist->Sort(CompareNames);
							sublist->Insert(0, tmp);
						}
					}
        }
			} catch(Exception &e) { ShowMessage(UnicodeString(L"TAHplugin => LoadRepl() : ") + e.Message); }
				catch(...)					{ ShowMessage(L"TAHplugin => LoadRepl() : Unknown exception"); }
		}
	} else { // file not found
  	pRepl->FileSig.Clear();
		pRepl->ClearRepl();
	}
}

//---------------------------------------------------------------------------
UnicodeString GenNamesFileName(UnicodeString profile)
{
	UnicodeString rv;
	if (profile == TAHPLUGIN_GLOBAL_PROFILE) {
		rv = GetModuleDir() + TAHPLUGIN_REPLACEMENTS_DIR + TAH_FILE_NAMES;
	} else {
		rv = GetModuleDir() + TAHPLUGIN_REPLACEMENTS_DIR + profile + L"." + TAH_FILE_NAMES;
	}
	return rv;
}

//---------------------------------------------------------------------------
// fills TStringList with contents of the file
bool LoadNames(UnicodeString profile, TStringList* strings)
{
	bool rv = true;
	strings->Clear(); // clear strings first
	TBytes buffer; // buffer to read file
	TFileStream *file; // file name
	UnicodeString filename(GenNamesFileName(profile));
	// find file
	if (FileExists(filename)) {
		// loading
		try {
			file = new TFileStream(filename, fmOpenRead);
			buffer.set_length(file->Size+1);
			file->Read(&buffer[0], buffer.Length-1);
			*(wchar_t*)&buffer[buffer.Length-1] = (wchar_t)0;
			delete file;
			// store file contents to strings variable
			strings->Text = (wchar_t*)&buffer[0];
		} catch(Exception &e) {
			rv = false; // error occured
			ShowMessage(UnicodeString(L"TAHplugin => LoadNames() : ") + e.Message);
		} catch(...) {
			rv = false; // unknown error occured
			ShowMessage(L"TAHplugin => LoadNames() : Unknown exception");
		}
	} else {
		rv = false; // file not found
	}
	return rv;
}

//---------------------------------------------------------------------------
// saves contents of replacements strings to the file
void SaveNames(UnicodeString profile, TStringList* strings)
{
	try {
		// file name
		UnicodeString filename(GenNamesFileName(profile));
		if (strings->Count) {
			// save strings to file
			TFileStream *file = new TFileStream(filename, fmCreate);
			file->Write((void*)strings->Text.w_str() , strings->Text.Length() * strings->Text.ElementSize());
			delete file;
		} else {
    	// delete file if strings is empty
			DeleteFile(filename);
    }
	} catch(Exception &e)	{ ShowMessage(UnicodeString(L"TAHplugin => SaveNames() : ") + e.Message);	}
		catch(...)					{ ShowMessage(L"TAHplugin => SaveNames() : Unknown exception"); }
}

//---------------------------------------------------------------------------
void LoadConfig()
{
	TIniFile *ini = new TIniFile(ChangeFileExt(GetModuleName(), L".ini"));
	Config.ShowModal = ini->ReadInteger(L"ROOT", L"ShowModal", 1);

	Config.SubstEnable				= ini->ReadInteger(L"SUBST", L"SubstEnable", 1);
	Config.SubstWindowLeft		= ini->ReadInteger(L"SUBST", L"SubstWindowLeft", 50);
	Config.SubstWindowTop			= ini->ReadInteger(L"SUBST", L"SubstWindowTop", 50);
	Config.SubstWindowWidth		= ini->ReadInteger(L"SUBST", L"SubstWindowWidth", 411);
	Config.SubstWindowHeight	= ini->ReadInteger(L"SUBST", L"SubstWindowHeight", 419);
	Config.SubstBeforeTA			= ini->ReadInteger(L"SUBST", L"SubstBeforeTA", 1);

	Config.TApopupOnTrans			= ini->ReadInteger(L"TA", L"TApopupOnTrans", 0);
	delete ini;
}

//---------------------------------------------------------------------------
void SaveConfig()
{
	TIniFile *ini = new TIniFile(ChangeFileExt(GetModuleName(), L".ini"));
	ini->WriteInteger(L"ROOT", L"ShowModal", Config.ShowModal);

	ini->WriteInteger(L"SUBST", L"SubstEnable",				Config.SubstEnable);
	ini->WriteInteger(L"SUBST", L"SubstWindowLeft",		Config.SubstWindowLeft);
	ini->WriteInteger(L"SUBST", L"SubstWindowTop",		Config.SubstWindowTop);
	ini->WriteInteger(L"SUBST", L"SubstWindowWidth",	Config.SubstWindowWidth);
	ini->WriteInteger(L"SUBST", L"SubstWindowHeight",	Config.SubstWindowHeight);
	ini->WriteInteger(L"SUBST", L"SubstBeforeTA",			Config.SubstBeforeTA);

	ini->WriteInteger(L"TA", L"TApopupOnTrans", Config.TApopupOnTrans);
	delete ini;
}

//---------------------------------------------------------------------------
void UpdateProfiles()
{
	pProfileList->Clear();
	// update from TA's game configs folder
	UnicodeString find(GetModuleDir());
	for (int i= 0; i < 2; i++)
		find = find.SubString(1, find.LastDelimiter(L"\\") - 1);
	find += L"\\Game Configs\\*.ini";
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(find.w_str(), &data);
	if (hFind !=INVALID_HANDLE_VALUE) {
		do {
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
			UnicodeString filename(data.cFileName);
			filename = filename.SubString(1, filename.LastDelimiter(L".") - 1);
			pProfileList->Add(filename);  // add profile
		}
		while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	// update from TAHplugin's replacements folder
	UnicodeString term(L".");
	term += TAH_FILE_NAMES;
	find.printf(L"%s%s%s%s", GetModuleDir(), TAHPLUGIN_REPLACEMENTS_DIR, L"*", term);
	hFind = FindFirstFile(find.w_str(), &data);
	if (hFind !=INVALID_HANDLE_VALUE) {
		do {
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
			// cut off last occurence of term
			wchar_t *seek = data.cFileName;
			wchar_t *last = NULL;
			do {
				seek = wcsstr(seek, term.w_str()); // find
				if (seek) {
					last = seek;
					seek += term.Length(); // move past occurence to find next one
				}
			} while (seek);
			*last = 0;
			pProfileList->Add(data.cFileName); // add profile
		}
		while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

//---------------------------------------------------------------------------
UnicodeString FormatSubstEntry(UnicodeString line)
{
	line = strStripChars(line, L"\n\r");
	line = strTrimL(line, L"\t");
	if (line.Length() && line[1] != L'*') {
		line = strTrimR(line, L"\t");
		line = strRemoveDuplicates(line, L"\t");
	}
	return line;
}

//---------------------------------------------------------------------------
int FindLoadedProfile(UnicodeString profile)
{
	for (int i=0; i < pActReplList->Count; i++) {
		if (((tActRepl*)pActReplList->Items[i])->Profile == profile) {
			return i;
		}
	}
	return -1;
}
//---------------------------------------------------------------------------
BOOL CALLBACK EnumWndProcTAwnd(HWND hwnd, LPARAM lParam)
{
	HWND *hwout = (HWND*)lParam;
	DWORD wpid = 0;
	GetWindowThreadProcessId(hwnd, &wpid);
	if (ProcessID && wpid == ProcessID) {
		UnicodeString wcls(wndGetWindowClass(hwnd));
		if (wcls.Pos(TA_WNDCLASS_MAINWINDOW)) {
			*hwout = hwnd;
			return FALSE;
		}
	}
	return TRUE;
}
//---------------------------------------------------------------------------
HWND GetTAWindow()
{
	HWND taw = 0;
	EnumWindows((WNDENUMPROC)EnumWndProcTAwnd, (LPARAM)&taw);
	return taw;
}
//---------------------------------------------------------------------------

