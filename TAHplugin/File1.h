//---------------------------------------------------------------------------
#ifndef File1H
#define File1H

//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#include "StringUtils.h"

//---------------------------------------------------------------------------
// Global configuration parameters
struct tConfig {
	int ShowModal; // non modal mode is weird (Tab, Arrows...)

	int SubstEnable;
	int SubstWindowLeft;
	int SubstWindowTop;
	int SubstWindowWidth;
	int SubstWindowHeight;
	int SubstBeforeTA;

	int TApopupOnTrans;

	tConfig() {
		ShowModal = 1;

		SubstEnable = 1;
		SubstWindowLeft = 50;
		SubstWindowTop = 50;
		SubstWindowWidth = 411;
		SubstWindowHeight = 419;
		SubstBeforeTA = 1;

		TApopupOnTrans = 0;
	}
};

//---------------------------------------------------------------------------
// File signature for loading check
struct tFileSig {
	__int64 size;
	__int64 modTime;
	__int64 createTime;

	tFileSig() { Clear(); }

	void Clear() {
		size = 0;
		modTime = 0;
		createTime = 0;
	}
};

//-------------------------------------
// Replacement entry
struct tName {
	UnicodeString orig;
	UnicodeString repl;

	tName(UnicodeString Orig, UnicodeString Repl) {
		orig = Orig;
		repl = Repl;
  }
};

//-------------------------------------
// Resident replacements profile
struct tActRepl{
	UnicodeString Profile; // profile name
	tFileSig FileSig; // file signature
	TList *pReplLst; // list of replacement blocks

	tActRepl(UnicodeString profile) {
		Profile = profile;
		pReplLst = new TList();
	}
	~tActRepl() {
		ClearRepl();
		delete pReplLst;
	}

	void ClearRepl() {
		TListEnumerator *i = pReplLst->GetEnumerator();
		while (i->MoveNext()) {
			TList *sublist = (TList*)i->GetCurrent();
			TListEnumerator *j = sublist->GetEnumerator();
			while (j->MoveNext())
				delete (tName*)j->GetCurrent();
			delete sublist;
		}
		pReplLst->Clear();
	}

	void AddRepl(TList *block) {
		pReplLst->Add(block);
	}
};

//---------------------------------------------------------------------------
#define TA_WNDCLASS_MAINWINDOW			L"Translation Aggregator Main Window"	// in sync with TA

#define TAHPLUGIN_GLOBAL_PROFILE	L"*" // in sync with TA
#define TAHPLUGIN_TAG_TEXT				L"[TEXT]"
#define TAHPLUGIN_TAG_REGEX				L"[REGEX]"
enum tTag { TagInvalid=0, TagText, TagRegex };
//---------------------------------------------------------------------------
bool LoadNames(UnicodeString profile, TStringList* strings);
void SaveNames(UnicodeString profile, TStringList* strings);
void UpdateProfiles();
bool IsTag(UnicodeString str);
tTag WhatTag(UnicodeString str);
UnicodeString FormatSubstEntry(UnicodeString line);
int FindLoadedProfile(UnicodeString profile);
void SaveConfig();
HWND GetTAWindow();

//---------------------------------------------------------------------------
extern TStringList *pProfileList;
extern TStringList *pActiveProfileList;
extern tConfig Config;

//---------------------------------------------------------------------------
#endif // File1H
