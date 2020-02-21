//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Registry.hpp>
#include <boost/regex.hpp>
#include <tlhelp32.h>
#include <psapi.h>

#include "Unit1.h"
#include "UnitOSD.h"
#include "StringUtils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------

TFormTAHelper *FormTAHelper;

//---------------------------------------------------------------------------
LRESULT CALLBACK HookKeyboardProc(int nCode,	WPARAM wParam, LPARAM lParam)
{
	static bool wink=false; // Win-key down flag
	if(!(nCode < 0)) { // allowed to process message now
		if(nCode == HC_ACTION) {
			KBDLLHOOKSTRUCT *phs = (KBDLLHOOKSTRUCT*)lParam;
			if(phs->vkCode == 0x5B) { // Left Win-key
				wink = (wParam == WM_KEYDOWN);
				return -1; // eat message
			} else {
				if(wink && (wParam == WM_KEYDOWN)) { // all other keys (while Win pressed)
					char letter = (char)phs->vkCode;
					HWND taw = FormTAHelper->TALFind(); // get TA window

					//ShowMessage(IntToStr((int)letter));
					if (letter == 'J') { // show/hide WaKan
						if (FormTAHelper->m_Wakan->ShowHide())
							if (FormTAHelper->CheckBoxWKenable->Checked)
								FormTAHelper->m_Wakan->Translate();
						return -1; // eat message
					}
					else if (letter == 'M') { // translate clipboard in WaKan
						FormTAHelper->m_Wakan->Translate();
						return -1; // eat message
					}
					else if (letter == -65) { // '?' display current time
						FormOSD->ShowOSD(TDateTime::CurrentTime().TimeString());
						return -1; // eat message
					}
					else if (letter == 'K') { // restore WaKan position
						FormTAHelper->SpeedButtonWKrestorePosClick(NULL);
						return -1; // eat message
					}
					else if (letter == 'H') { // display hotkeys help
						if (FormOSD->IsVisible())
							FormOSD->Hide();
						else
							FormOSD->ShowMsg(FormTAHelper->MemoHKhotkeys->Lines->Text);
						return -1; // eat message
					}
					else if (letter == 'C') { // toggle Clipboard Reviser active
						FormTAHelper->CheckBoxCRenable->Checked = !FormTAHelper->CheckBoxCRenable->Checked;
						FormOSD->ShowOSD(UnicodeString(L"Clipboard Reviser: ") + (FormTAHelper->CheckBoxCRenable->Checked? L"ON": L"OFF"));
						return -1; // eat message
					}
					else if (letter == -67) { // '-' history up + copy
						FormTAHelper->HistoryMove(true, true);
						return -1; // eat message
					}
					else if (letter == -69) { // '+' history down + copy
						FormTAHelper->HistoryMove(false, true);
						return -1; // eat message
					}
					else if(letter == 'Q') { // show/hide TAHelper
						if(IsWindowObscured(FormTAHelper->Handle)) {
							FormTAHelper->TrayIcon1->Visible = false;
							ShowWindow(Application->Handle, SW_RESTORE);
							BringWindowToFront(FormTAHelper->Handle);
						} else {
							FormTAHelper->TrayIcon1->Visible = true;
							ShowWindow(Application->Handle, SW_HIDE);
							ShowWindow(FormTAHelper->Handle, SW_HIDE);
						}
						return -1; // eat message
					}
					else if(letter == 'A') { // translate all
						HWND hwTAdrag = FindWindow(TA_DRAGW_CLASS, NULL);
						if(hwTAdrag != NULL) {
							PostMessage(hwTAdrag, WMA_TRANSLATE_ALL, 0, 0);
						}
						return -1; // eat message
					}
					else if(letter == 'T') { // toggle toolbar
						if(taw) {
							PostMessage(taw, WM_COMMAND, ID_VIEW_HIDETOOLBAR, 0);
							// restore positions of detached windows
							FormTAHelper->TimerTALrefind->Enabled = true;
						}
						return -1; // eat message
					}
					else if(letter == 'W') { // toggle frame
						if(taw) {
							PostMessage(taw, WM_COMMAND, ID_VIEW_HIDEWINDOWFRAME, 0);
							// restore positions of detached windows
							FormTAHelper->TimerTALrefind->Enabled = true;
						}
						return -1; // eat message
					}
					else if(tBETWEEN('0',letter,'9')) { // switch layout
						if(taw) {
							DWORD wpar = letter - '0';
							PostMessage(taw, WM_COMMAND, wpar + ID_LAYOUT_0, 0);
							// restore positions of detached windows
							FormTAHelper->TimerTALrefind->Enabled = true;
						}
						return -1; // eat message
					}
					else if(letter == 'O') { // toggle topmost
						if(taw) {
							// inverse TA state
							bool top = !GetWinTopState(taw);
							SetWinTopState(taw, top);
							// sync detached windows state
							for(int i=0; i<FormTAHelper->m_StrayWinList->Count; i++) {
								if(!SetWinTopState((HWND)FormTAHelper->m_StrayWinList->Items[i], top)) {
									FormTAHelper->m_StrayWinList->Delete(i); // delete invalid windows
									i--;
								}
							}
						}
						return -1; // eat message
					}
					else if(letter == 'I') { // hide/show
						if(taw) {
							UINT showCmd = IsWindowVisible(taw)?SW_HIDE:SW_SHOWNOACTIVATE;
							WINDOWPLACEMENT wp; wp.length = sizeof(WINDOWPLACEMENT);
							// process TA window
							GetWindowPlacement(taw, &wp);
							wp.showCmd = showCmd;
							wp.flags |= WPF_ASYNCWINDOWPLACEMENT;
							SetWindowPlacement(taw, &wp);
							// process detached windows
							for(int i=0; i<FormTAHelper->m_StrayWinList->Count; i++) {
								HWND hw = (HWND)FormTAHelper->m_StrayWinList->Items[i];
								if(GetWindowPlacement(hw, &wp)) {
									wp.showCmd = showCmd;
									wp.flags |= WPF_ASYNCWINDOWPLACEMENT;
									SetWindowPlacement(hw, &wp);
								} else {
									FormTAHelper->m_StrayWinList->Delete(i);
									i--;
								}
							}
						}
						return -1; // eat message
					}
				}
			}
		}
	}
	// Call next hook
	return CallNextHookEx(FormTAHelper->m_hKeyboardHook, nCode, wParam, lParam);
}
//---------------------------------------------------------------------------
LRESULT CALLBACK HookMouseProc(int nCode,	WPARAM wParam, LPARAM lParam)
{
	if(!(nCode < 0)) { // allowed to process message now
		if(wParam == WM_MOUSEWHEEL) {
    	// get info about scroll
			MSLLHOOKSTRUCT *phs = (MSLLHOOKSTRUCT*)lParam;
			int delta = (signed short)(HIWORD(phs->mouseData)) / (signed short)WHEEL_DELTA;
			POINT p; GetCursorPos(&p); // cursor position
			HWND hw = WindowFromPoint(p); 								 // wintow at cursor
			HWND hwanc = hw? GetAncestor(hw, GA_ROOT):NULL; // top window at cursor

			HWND hwtah = GetAncestor(FormTAHelper->Handle, GA_ROOT); // TAHelper top
			HWND hwakan = FormTAHelper->m_Wakan->GetRoot();
			if (hwakan) hwakan = GetAncestor(hwakan, GA_ROOT); // WaKan top

			// in case of mouse pointer over TAHelper or Wakan
			// send WM_MOUSEWHEEL as it is (almost, it works anyway)
			if(hwanc == hwtah || (hwakan && hwanc && hwanc == hwakan)) { // <---- TAHelper or WaKan
				LPARAM lpar = phs->pt.y;
				lpar = lpar<<16; lpar |= phs->pt.x;
				PostMessage(hw, wParam, phs->mouseData, lpar);
				return -1; // eat this message
			}
			else if (hw){ // TA or any other edit boxes (send WM_VSCROLL)
				UnicodeString wclass(wndGetWindowClass(hw));
				if (wclass == L"FURIGANA SUBWINDOW") { // only one message
					PostMessage(hw, WM_VSCROLL, delta>0?SB_LINEUP:SB_LINEDOWN, 0);
					return -1; // eat this message
				} else if (wclass == L"RICHEDIT50W" || wclass == L"Edit") { // number of messages according to settings
					for(int i=0; i<FormTAHelper->UpDownFSstep->Position; i++)
						PostMessage(hw, WM_VSCROLL, delta>0?SB_LINEUP:SB_LINEDOWN, 0);
					return -1; // eat this message
				}
			}
		}
	}
	// Call next hook
	return CallNextHookEx(FormTAHelper->m_hMouseHook, nCode, wParam, lParam);
}

//---------------------------------------------------------------------------
void TerminateProcess(HWND hWnd)
{
	HANDLE hProc = NULL;
	DWORD procID = NULL;

	if (!GetWindowThreadProcessId(hWnd, &procID)) {
		throw Exception(L"TerminateProcess(HWND) Get window process ID failed");
	}

	if (procID)
		hProc = OpenProcess(PROCESS_TERMINATE, FALSE, procID);
	if (!hProc) {
		throw Exception(L"TerminateProcess(HWND) Can not open process");
	}

	if (!TerminateProcess(hProc, 0)) {
		throw Exception(L"TerminateProcess(HWND) Can not terminate process");
	}

	// cleanup
	if (hWnd)  CloseHandle(hWnd);
	if (hProc) CloseHandle(hProc);
}
void TerminateProcess(DWORD procID)
{
	HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, procID);
	if (!hProc) {
		throw Exception(L"TerminateProcess(DWORD) Can not open process");
	}
	if (!TerminateProcess(hProc, 0)) {
		throw Exception(L"TerminateProcess(DWORD) Can not terminate process");
	}
	if (hProc) CloseHandle(hProc);
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
int GetAutohideTaskbar()
{
	APPBARDATA ad; ad.cbSize = sizeof(APPBARDATA);
	ad.hWnd = FindWindow(L"Shell_traywnd", NULL);
	if (ad.hWnd) {
		// get autohide state of the Microsoft Windows taskbar
		ad.lParam = SHAppBarMessage(ABM_GETSTATE, &ad);
		return (ad.lParam & ABS_AUTOHIDE)? 1: 0;
	}
	return -1;
}
void SetAutohideTaskbar(int autohide)
{
	if (autohide == -1) return;
	int cstate = GetAutohideTaskbar();
	if (cstate == -1 || cstate == autohide) return;

	APPBARDATA ad; ad.cbSize = sizeof(APPBARDATA);
	ad.hWnd = FindWindow(L"Shell_traywnd", NULL);
	if (ad.hWnd) {
		// get autohide and always-on-top states of the Microsoft Windows taskbar
		ad.lParam = SHAppBarMessage(ABM_GETSTATE, &ad);
		// set new autohide state
		if (autohide)
			ad.lParam |= ABS_AUTOHIDE;
		else
			ad.lParam &= (~ABS_AUTOHIDE);
		SHAppBarMessage(ABM_SETSTATE, &ad);
	}
}

//---------------------------------------------------------------------------
std::wstring regexReplaceAll(std::wstring source, std::wstring find, std::wstring replace)
{
	std::wstring rv;
	try {
		boost::wregex e(find);  // match regex
		rv = boost::regex_replace(source, e, replace); // result
	} catch (...) {
		UnicodeString RetErr;
		RetErr.printf(L"TAHelper・Error in regex: %s　　%s", find.c_str(), replace.c_str());
		return RetErr.w_str();
//  	rv = source;
//		if (TrayIcon1->Visible) // show program if hided
//			TrayIcon1Click(this);
//		MessageBox(Handle, (UnicodeString(L"Error in regular expression:\n")+find.c_str()+L"\n"+replace.c_str()).w_str(), TAH_APP_TITLE, MB_OK|MB_ICONERROR);
	}
	return rv;
}
//---------------------------------------------------------------------------
tWString strRemoveDuplicates(tWString text, tWString phr)
{	// remove duplicate strings
	int pos=0, lb0=-666666, phrlen=phr.Length();
	while(true)
	{
		int fpos = text.Find((wchar_t*)phr, pos);
		if(fpos >= 0)
		{
			if(fpos-phrlen == lb0)
				text.Delete(fpos, phrlen);
			else
			{
				pos = fpos+phrlen;
        if(pos >= text.Length()) break;
				lb0 = fpos;
			}
		} else
			break;
	}
	return text;
}
//---------------------------------------------------------------------------
tWString strRemoveDuplicateChar(tWString text, int number)
{
	int seek=0;
	while(true) {
		// ignore invisible chars
		while(tWString::tSymbols::IsSpacelike(text[seek])) {
			seek++; if(seek >= text.Length()) goto strRemoveDuplicateChar_End; // OK
		}
		if(seek+number > text.Length())
			return L""; // FAILED
		else {
			for(int i=1; i<number; i++)
				if(text[seek+1] == text[seek])
					text.Delete(seek+1, 1);
				else
					return L""; // FAILED
			seek++; if(seek >= text.Length()) goto strRemoveDuplicateChar_End; // OK
		}
	}
strRemoveDuplicateChar_End:
	return text;
}

//---------------------------------------------------------------------------
void TFormTAHelper::HistoryMove(bool moveup, bool copy)
{
	TPoint mousepos; mousepos.x = 0; mousepos.y = 0;
	bool handled = true;
	m_HistoryScroll.active = true;
	if(moveup)
		FormMouseWheelUp(NULL, (TShiftState)0, mousepos, handled);
	else
		FormMouseWheelDown(NULL, (TShiftState)0, mousepos, handled);
	m_HistoryScroll.active = false;
	if(copy)
		SpeedButtonCRcopyClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::WndProc(Messages::TMessage &Message)
{
	switch(Message.Msg)
	{
	case WM_APP+1024: // signal from WaKan hook DLL (WaKan window is about to be destroyed)
		m_Wakan->FreeDll();
		break;
	case WM_USER+1: // signal from TAHplugin to get text for Systran
		// translate by SYSTRAN
		if (CheckBoxSTRtextFromTA->Checked) {
			SystranTranslate(true);
		}
		// redraw detached windows
		if (m_OptionsGlobal.WDforceRedraw) {
			TimerWDredraw->Enabled = false;
			TimerWDredraw->Enabled = true;
		}
		break;
	case WM_CLOSE:
		break;
	case WM_CREATE:
		break;
	case WM_CHANGECBCHAIN:
		if((HWND)Message.WParam == m_hNextClipboardListener) {
			m_hNextClipboardListener = (HWND)Message.LParam;
		} else {
			SendMessage(m_hNextClipboardListener, WM_CHANGECBCHAIN, Message.WParam, Message.LParam);
		}
		return;
	case WM_DRAWCLIPBOARD:
		{
    	if(Boot) return; // block if program starting - not initialized properly yet
			HWND hWndOwner = GetClipboardOwner();
			HWND hWndAncestor = hWndOwner? GetAncestor(hWndOwner, GA_ROOT): NULL;
			// block text from TA
			if(	hWndAncestor &&
					(hWndAncestor == TALFind() || AmongDetachedWindows(hWndAncestor)) ) {
				m_Text.Clear(); // reset processing-copy block
				return;
			}
			// self-copy handling:
			// in case of "copy-send" was pressed, pass notification along the chain
			// or else, do NOT pass notification
			bool sameAncestor = hWndAncestor && (hWndAncestor == GetAncestor(Handle, GA_ROOT));
			if(sameAncestor) {
				m_Text.Clear(); // reset processing-copy block
				if(SpeedButtonCRcopy->Tag) { // "copy-send pressed" flag
					SpeedButtonCRcopy->Tag = 0;
					// send chain notification
					if(m_hNextClipboardListener) SendMessage(m_hNextClipboardListener, WM_DRAWCLIPBOARD, 0, 0);
					// "copy-send" indication
					CRnotify(clYellow);

					// translate clipboard by SYSTRAN
					if (!CheckBoxSTRtextFromTA->Checked) {
						SystranTranslate(false);
					}

					// translate clipboard in WaKan
					if (CheckBoxWKenable->Checked) {
						m_Wakan->Translate();
					}

					// redraw detached windows
					if (m_OptionsGlobal.WDforceRedraw) {
						TimerWDredraw->Enabled = false;
						TimerWDredraw->Enabled = true;
					}
				}
				return;
			}

			// get clipboard text to process
			tWString s(GetClipboard(Handle));

			//block large text
			if(s.Length() > m_OptionsGlobal.CRtextVolumeLimit) return;

			// block self-triggered clipboard event
			// (text is no raw but was processed and copied by me)
			if(s == m_Text) return;
			// block clipboard flood from external sources
			if(s == m_TextRaw) return;
			m_TextRaw = s; // store raw text to check later

			// add cleaned raw text to history
			s.Trim();
			if(s.Length()) {
				s = strRemoveDuplicates(s, STR_CRLF);
				UnicodeString *pstr = new UnicodeString(s);
				m_CRhistoryRawList->Add(pstr);
				if(m_CRhistoryRawList->Count > TAH_MAX_HISTORY) {
					delete (UnicodeString*)m_CRhistoryRawList->Items[0];
					m_CRhistoryRawList->Delete(0);
				}
			}

			// process text
			ProcessText(m_TextRaw);
		}
		return;
	}
	TForm::WndProc(Message);
}
//---------------------------------------------------------------------------
bool TFormTAHelper::ProcessText(tWString s)
{
	tWString s1, s2;

	// basic clean
	//s.Trim();
	// do not process empty text
	if(!s.Length()) return false;

	// processing indication
	// tag == 0 - normal capture
	// tag == 1 - reprocess
	CRnotify((SpeedButtonCRreprocess->Tag == 0)? clLime: clAqua);
	SpeedButtonCRreprocess->Tag = 0; // preemptive reset

	// option: remove all LNBRs
	if(CheckBoxCRremoveLinebreaks->Checked)
		s.DeleteAll(STR_CRLF);
	else // by default remove only excessive LNBRs
		s = strRemoveDuplicates(s, STR_CRLF);

	// option: remove character repetition
	if(CheckBoxCRrepetitionCharacter->Checked) {
//		s1 = strRemoveDuplicateChar(s, StrToIntW(EditCRrepetitionCharacterNumber->Text.w_str()));
//		if(s1.Length()) s = s1; // only if removing is not failed

  	// initialization
		struct tMach {
			std::wstring str;
			int pos;
			tMach() { pos=0; }
		};
		std::wstring sall;  // all text
		sall = s.w_str();
		std::wstring::const_iterator start, end;
		start = sall.begin();
		end = sall.end();
		boost::match_results<std::wstring::const_iterator> what;
		UnicodeString reg1s;
		reg1s.printf(L"((.)\\2{%d}){%d,9999}", UpDownCRrepetitionCharacterNumber->Position-1, UpDownCRrepetitionCharacterLen->Position);
		boost::wregex reg1(reg1s.w_str()); // regex to find character repetition
		// collect all maches
		TList *maches = new TList();
		while(regex_search(start, end, what, reg1)) {
			tMach *mach = new tMach();
			mach->str = what[0]; // entire mach string
			mach->pos = what.position(); // mach position relative to end of previous mach
			maches->Add(mach);
			// update search position
			start = what[0].second;
		}
		// process and replace all maches
		UnicodeString all(sall.c_str());
		int trackpos = 1;
		for (int i=0; i<maches->Count; i++) {
			tMach *m = (tMach*)maches->Items[i];
			all = all.Delete(trackpos + m->pos, m->str.length()); // remove mach from text
			UnicodeString reg2s;
			reg2s.printf(L"((.)\\2{%d})", UpDownCRrepetitionCharacterNumber->Position-1);
			boost::wregex reg2(reg2s.w_str());  // regex to remove char repetition
			m->str = boost::regex_replace(m->str, reg2, L"\\2"); // process mach
			all = all.Insert(m->str.c_str(), trackpos + m->pos); // insert processed mach to text
			trackpos += m->pos + m->str.length();
			// delete mach object
			delete m;
		}
		// delete maches list
		delete maches;
		// assign processed text
  	s = all.w_str();
	}

	// option: replace LNBRs with dots
	if(CheckBoxCRseparateLinebreaks->Checked) {
		s.Replace(STR_CRLF, L"。");
		s = strRemoveDuplicates(s, L"。");
	}

	// option: process names
	if(CheckBoxCRnames->Checked) {
		// get names one by one to check
		for(int i=0; i<m_NameList->Count; i++) {
			s1 = ((tName*)m_NameList->Items[i])->orig; // original name
			s2 = ((tName*)m_NameList->Items[i])->repl; // replace by

			// option: remove names from beginning/end
			bool namewas = false;
			int pos = -1;
			if(RadioButtonCRremoveNames->Checked || RadioButtonCRseparateNames->Checked) {
				if(UpDownCRnameFrom->Tag) { // remove from beginning
					pos = s.Find(s1);
					while(pos >= 0 && tBETWEEN(0, pos, StrToIntW(EditCRnameSearchDeviation->Text.w_str()))) {
						namewas = true;
						s.Delete(0, s1.Length() + pos); // cut off to the edge of string
						pos = s.Find(s1);
					}
				} else { // remove from end
					pos = s.FindReverse(s1);
					while(pos >= 0 && tBETWEEN(s.Length() - s1.Length() - StrToIntW(EditCRnameSearchDeviation->Text.w_str()), pos, s.Length() - s1.Length())) {
						namewas = true;
						s.Delete(pos, s.Length() - pos); // cut off to the edge of string
						pos = s.FindReverse(s1);
					}
				}
			}
			// option: separate names (insert separated name in the beginning)
			if(RadioButtonCRseparateNames->Checked && namewas) {
				if (!s2.Length()) s2 = s1;
				switch(ComboBoxCRseparateNamesBy->ItemIndex) {
					case 0: // no separators
						break;
					case 1: // dot
						s2 += L"。";
						break;
					case 2: // thin brackets
						s2 = tWString(L"「") + s2 + L"」";
						break;
					case 3: // thick brackets
						s2 = tWString(L"『") + s2 + L"』";
						break;
					case 4: // angle brackets
						s2 = tWString(L"《") + s2 + L"》";
						break;
					case 5: // square brackets
						s2 = tWString(L"【") + s2 + L"】";
						break;
					default:
						throw Exception(L"TFormTAHelper::ProcessText() invalid ComboBoxCRseparateNamesBy->ItemIndex");
				}
				// append deleted name(translated) to beginning
				s = s2 + s;
			}
			// process only first name found
			if(namewas) break;
		}
	}

	// option: replace/remove all names
	if (CheckBoxCRreplaceAllNames->Checked) {
		for (int i=0; i<m_NameList->Count; i++) { // get names one by one to check
			s1 = ((tName*)m_NameList->Items[i])->orig; // original name
			s2 = ((tName*)m_NameList->Items[i])->repl; // replace by
			if (UpDownCRallNames->Tag) {
				// replace all names
				s = regexReplaceAll(s.w_str(), s1.w_str(), s2.w_str()).c_str();
				//if (s2 != s1) s.Replace(s1, s2);
			} else {
				// delete all names
				s = regexReplaceAll(s.w_str(), s1.w_str(), L"").c_str();
				//s.DeleteAll(s1);
			}
		}
	}

	// option: remove strings
	if (CheckBoxCRremoveStrings->Checked) {
		tWString ts, cur;
		ts = EditCRremoveStrings->Text.w_str(); // get solid string
		for (int i=0; i < ts.Length(); i++) {
			if (ts[i]==L';' || ts[i]==L'；') { // separator
				if (cur.Length()) {
					s = regexReplaceAll(s.w_str(), cur.w_str(), L"").c_str();
					//s.DeleteAll(cur);
					cur.Clear();
				}
			} else
				cur += ts[i];
		}
		if (cur.Length()) {
			s = regexReplaceAll(s.w_str(), cur.w_str(), L"").c_str();
			//s.DeleteAll(cur);
    }
	}

	s.Trim(); // final clean

	// show text
	MemoCRcontents->Text = s.w_str();

	if(!s.Length()) return false; // do not process empty text

	// add processed text to history
	UnicodeString *pstr = new UnicodeString(s);
	m_CRhistoryList->Add(pstr);
	if(m_CRhistoryList->Count > TAH_MAX_HISTORY) {
		delete (UnicodeString*)m_CRhistoryList->Items[0];
		m_CRhistoryList->Delete(0);
	}
	m_HistoryScroll.pos = m_CRhistoryList->Count - 1; // index of last element

	// Shift in time
	if (CheckBoxCRtimeShift->Checked) {
		if (m_CRhistoryList->Count <= UpDownCRtimeShift->Position) return false; // nothing to send yet
		s = ((UnicodeString*)m_CRhistoryList->Items[m_CRhistoryList->Count - UpDownCRtimeShift->Position - 1])->w_str();
	}

	// Store text for test later
	m_Text = s;

	// copy to clipboard
	if(CopyToClipboard(s, NULL)) { // owner not me, but external source
		// notify next window about new clipboard contents
		if(m_hNextClipboardListener) SendMessage(m_hNextClipboardListener, WM_DRAWCLIPBOARD, 0, 0);
	} else {
		StatusbarError(L"Copy to clipboard failed");
	}

	// translate clipboard by SYSTRAN
	if (!CheckBoxSTRtextFromTA->Checked) {
		SystranTranslate(false);
	}

	// translate clipboard in WaKan
	if (CheckBoxWKenable->Checked) {
		m_Wakan->Translate();
	}

	// redraw detached windows
	if (m_OptionsGlobal.WDforceRedraw) {
		TimerWDredraw->Enabled = false;
		TimerWDredraw->Enabled = true;
	}
	return true;
}
//---------------------------------------------------------------------------
bool TFormTAHelper::CopyToClipboard(tWString text, HWND owner)
{
	bool ret = false;
	HGLOBAL hLocMem = NULL;
	if (m_OptionsGlobal.CRforceJpnClipbrdLocale) { // Force Japanese Clipboard Locale
		hLocMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, sizeof(LCID));
		PLCID lcid = (PLCID)GlobalLock(hLocMem);
		*lcid = MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_JAPANESE_JAPAN), SORT_JAPANESE_XJIS);
		GlobalUnlock(hLocMem);
	}
	HGLOBAL hGlobal = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, text.BufferSizeByte());
	if(hGlobal) {
		void *data = GlobalLock(hGlobal);
		if(data) {
			memcpy(data, (void*)text.w_str(), text.BufferSizeByte());
			GlobalUnlock(hGlobal);
			if(OpenClipboard(owner)) {
				EmptyClipboard();
				SetClipboardData(CF_UNICODETEXT, hGlobal);
				if (m_OptionsGlobal.CRforceJpnClipbrdLocale) // Force Japanese Clipboard Locale
					SetClipboardData(CF_LOCALE, hLocMem);
				CloseClipboard();
				ret = true; // copy success
			}else
				data = 0;
		}
		if(!data) {
			GlobalFree(hGlobal);
		}
	}
	return ret;
}

//---------------------------------------------------------------------------
bool TFormTAHelper::AmongDetachedWindows(HWND hwnd)
{
	bool ret = false;
	if (IsWindow(hwnd)) {
		for (int i=0; i < m_StrayWinList->Count; i++) {
			HWND hw = (HWND)m_StrayWinList->Items[i];
			if (IsWindow(hw)) {
				if (hw == hwnd) {
					ret = true;
					break;
        }
			} else { //cleanup invalid windows
				m_StrayWinList->Delete(i);
				i--;
			}
		}
	}
	return ret;
}
//---------------------------------------------------------------------------
__fastcall TFormTAHelper::TFormTAHelper(TComponent* Owner)
	: TForm(Owner)
{
	Boot = true;

	m_hNextClipboardListener = NULL;
	m_NameList = new TList;
	m_StrayWinList = new TList;
	m_pGetWinDrag = new tGetWindowDrag();
	m_DragMode = NONE;
	m_CRhistoryList = new TList;
	m_CRhistoryRawList = new TList;
	m_Systran = new tSystran(TimerSTRreceive);
	m_Wakan = new tWakan(TimerWKclass);

	// register class for background cover
	BYTE maskIcon[128]; ZeroMemory((void*)maskIcon, sizeof(BYTE) * 128);
	HICON hicon = CreateIcon(HInstance, 32, 32, 1, 1, maskIcon, maskIcon);
	WNDCLASS wclass;
	wclass.style = CS_DBLCLKS;
	wclass.lpfnWndProc = BackgroundWindowProc;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = HInstance;
	wclass.hIcon = hicon;
	wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wclass.hbrBackground = CreateSolidBrush(RGB(0,0,0));
	wclass.lpszMenuName = NULL;
	wclass.lpszClassName = TAH_CLASS_BACKCOVER;
	RegisterClass(&wclass);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::CheckBoxCRenableClick(TObject *Sender)
{
	if(CheckBoxCRenable->Checked) {
		// set clipboart monitor
		m_hNextClipboardListener = SetClipboardViewer(Handle);
	} else {
		// release clipboard monitor
		ChangeClipboardChain(Handle, m_hNextClipboardListener);
		m_hNextClipboardListener = NULL;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::CheckBoxFSenableClick(TObject *Sender)
{
	if(CheckBoxFSenable->Checked)
	{
		// set mouse hook
		m_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)HookMouseProc, GetModuleHandle(NULL), NULL/*GetCurrentThreadId()*/);
		if(m_hMouseHook == NULL) throw Exception(L"TFormTAHelper::CheckBoxFSenableClick() SetWindowsHookEx() failed");
	}else
	{
		UnhookWindowsHookEx(m_hMouseHook);
		m_hMouseHook = NULL;
	}
}
//---------------------------------------------------------------------------
tWString GetClipboard(HWND handle)
{
	tWString string;
	if(IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(handle))
	{
		HANDLE hGlobal = GetClipboardData(CF_UNICODETEXT);
		if(hGlobal)
		{
			wchar_t *data = (wchar_t*)GlobalLock(hGlobal);
			if(data)
			{
				int w = (int) GlobalSize(data)/2;
				int len = 0;
				while (len < w && data[len]) len++;
				if(len && len != w)
				{
					string = data;
				}
				GlobalUnlock(data);
			}
		}
		CloseClipboard();
	}
	return string;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonTopmostClick(TObject *Sender)
{
	SetWindowPos(Handle, SpeedButtonTopmost->Down?HWND_TOPMOST:HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
}
//---------------------------------------------------------------------------
void TFormTAHelper::LoadNames(bool global)
{
	MemoCRnames->Tag = 1; // prevent spamming OnChange()

	// read names from file to Memo-control
	TMemo *pmemo = global ? MemoCRnamesGlobal : MemoCRnames;
	pmemo->Lines->BeginUpdate();
	pmemo->Lines->Clear();

	TBytes buffer;
	TFileStream *file = NULL;
	UnicodeString filename(GenNamesFileName(global));
	try {
		if(!FileExists(filename)) {
			int h = FileCreate(filename);
			FileClose(h);
		}
		file = new TFileStream(filename, fmOpenRead);
		buffer.set_length(file->Size+1);
		file->Read(&buffer[0], buffer.Length-1);
		*(wchar_t*)&buffer[buffer.Length-1] = (wchar_t)0;
		// assign loaded text to memo-box
		pmemo->Lines->Text = (wchar_t*)&buffer[0];
		delete file;
	} catch(Exception &e) {
		Application->ShowException(&e);
	}
	pmemo->Lines->EndUpdate();

	MemoCRnames->Tag = 0; // restore normal OnChange()
}
//---------------------------------------------------------------------------
void TFormTAHelper::UpdateNames()
{
	MemoCRnames->Tag = 1; // prevent spamming OnChange()
	// clear memory list
	ClearNameList();
	// fill memory list and clean memo-boxes too
	AddNamesFrom(MemoCRnames); // local names first (priority in case of doubles)
	AddNamesFrom(MemoCRnamesGlobal);

	MemoCRnames->Tag = 0; // restore normal OnChange()
}
//---------------------------------------------------------------------------
void TFormTAHelper::AddNamesFrom(TMemo *pTextCtrl)
{
	// add names to memory list and clean memo-box too
	pTextCtrl->Lines->BeginUpdate();

	TStringList *strings = new TStringList(); // temp lines var
	InitTStringList(strings, false);
	strings->Text = pTextCtrl->Lines->Text;

	UnicodeString si, orig, repl;
	for (int i=0; i<strings->Count; i++) {
		strings->Strings[i] = FormatSubstEntry(strings->Strings[i]); // format line
		si = strings->Strings[i]; // current line
		if (!si.Length()) continue; // ignore empty lines
		if (si[1] == L'*') continue; // ignore comments (lines starting with '*')
		// get orig and repl
		int pos = si.Pos(L"\t"); // find Tab-separator
		if (pos > 1 && pos < si.Length()) {
			repl = si.SubString(pos+1, si.Length()-pos); // new text to replace
			orig = si.SubString(1, pos-1); // original text
		} else { // no text to replace
			repl = L"";
			orig = si;
		}
		// add replacement entry to memory list
		tName *pn = new tName();
		pn->repl = repl.w_str();
		pn->orig = orig.w_str();
		m_NameList->Add(pn);
	}
	pTextCtrl->Lines->Text = strings->Text; // assign formatted text back to control
	delete strings;
	pTextCtrl->Lines->EndUpdate();
}
//---------------------------------------------------------------------------
void TFormTAHelper::UpdateOptions(bool store)
{
	if(store) { // save controls state to struct
		m_Options.Left											= Left;
		m_Options.Top												= Top;
		m_Options.Width											= Width;
		m_Options.Height										= Height;
		m_Options.Topmost										= SpeedButtonTopmost->Down?1:0;
		m_Options.NamesSplitterPos					= MemoCRnames->Width;

		m_Options.CRenable									= CheckBoxCRenable->Checked?1:0;
		m_Options.CRremoveLinebreaks				= CheckBoxCRremoveLinebreaks->Checked?1:0;
		m_Options.CRseparateLinebreaks			= CheckBoxCRseparateLinebreaks->Checked?1:0;
		m_Options.CRsuppressCharRepetition	= CheckBoxCRrepetitionCharacter->Checked?1:0;
		m_Options.CRcharRepetitionNumber		= (int)UpDownCRrepetitionCharacterNumber->Position;
		m_Options.CRcharRepetitionLen				= (int)UpDownCRrepetitionCharacterLen->Position;
		m_Options.CRnames										= CheckBoxCRnames->Checked?1:0;
		m_Options.CRnamesFrom								= UpDownCRnameFrom->Tag;
		m_Options.CRnamesRemove							= RadioButtonCRremoveNames->Checked?1:0;
		m_Options.CRnamesSeparate						= RadioButtonCRseparateNames->Checked?1:0;
		m_Options.CRnamesSeparateBy					= ComboBoxCRseparateNamesBy->ItemIndex;
		m_Options.CRnameSearchDeviation			= StrToIntW(EditCRnameSearchDeviation->Text.w_str());
		m_Options.CRreplaceAllNames					= CheckBoxCRreplaceAllNames->Checked?1:0;
		m_Options.CRreplaceAllNamesWhat			= UpDownCRallNames->Tag;
		m_Options.CRremoveStrings						= CheckBoxCRremoveStrings->Checked?1:0;
		m_Options.CRremoveStringsWhat				= EditCRremoveStrings->Text;
		m_Options.CRtimeShift								= CheckBoxCRtimeShift->Checked?1:0;
		m_Options.CRtimeShiftNumber					= (int)UpDownCRtimeShift->Position;

		m_Options.FSenable									= CheckBoxFSenable->Checked?1:0;
		m_Options.FSnumberOfLines						= (int)UpDownFSstep->Position;

		m_Options.HKenable									= CheckBoxHKenable->Checked?1:0;

		m_Options.WDslot										= ComboBoxWDslot->ItemIndex;

		m_Options.MMdisplayBackgroung				= PopupMenuForm->Items->Items[6]->Items[0]->Checked?1:0;
		m_Options.MMtopLeft									= PopupMenuForm->Items->Items[6]->Items[1]->Checked?1:0;
		m_Options.MMtopCenter								= PopupMenuForm->Items->Items[6]->Items[2]->Checked?1:0;
		m_Options.MMsafeMode								= PopupMenuForm->Items->Items[6]->Items[3]->Checked?1:0;
		m_Options.MMautohideTaskbar					= PopupMenuForm->Items->Items[7]->Checked?1:0;
		m_Options.MMviewPanelWK							= PopupMenuForm->Items->Items[9]->Items[0]->Checked?1:0;
		m_Options.MMviewPanelTA							= PopupMenuForm->Items->Items[9]->Items[1]->Checked?1:0;
		m_Options.MMviewPanelHK							= PopupMenuForm->Items->Items[9]->Items[2]->Checked?1:0;
		m_Options.MMviewPanelWD							= PopupMenuForm->Items->Items[9]->Items[3]->Checked?1:0;
		m_Options.MMtaskbarVisible          = ToggleTaskbarVisibility1->Checked?1:0;

		m_Options.WKenable									= CheckBoxWKenable->Checked?1:0;
		m_Options.WKcompact									= CheckBoxWKcompact->Checked?1:0;
		m_Options.WKpatched									= CheckBoxWKpatched->Checked?1:0;

		m_Options.STRtextFromTA							= CheckBoxSTRtextFromTA->Checked?1:0;
		m_Options.TARestartOnHang           = CheckBoxTARestartOnHang->Checked?1:0;

	} else { // restore state from struct
		if(m_Options.Left || m_Options.Top || m_Options.Width || m_Options.Height) {
			Left		= m_Options.Left;
			Top			= m_Options.Top;
			Width		= m_Options.Width;
			Height	= m_Options.Height;
		}
		SpeedButtonTopmost->Down								= (m_Options.Topmost == 1);
		MemoCRnames->Width											= m_Options.NamesSplitterPos;

		CheckBoxCRenable->Checked								= (m_Options.CRenable == 1);
		CheckBoxCRremoveLinebreaks->Checked			= (m_Options.CRremoveLinebreaks == 1);
		CheckBoxCRseparateLinebreaks->Checked		= (m_Options.CRseparateLinebreaks == 1);
		CheckBoxCRrepetitionCharacter->Checked	= (m_Options.CRsuppressCharRepetition == 1);
		UpDownCRrepetitionCharacterNumber->Position	= (short)m_Options.CRcharRepetitionNumber;
		UpDownCRrepetitionCharacterLen->Position		= (short)m_Options.CRcharRepetitionLen;
		CheckBoxCRnames->Checked								= (m_Options.CRnames == 1);
		UpDownCRnameFrom->Tag										= m_Options.CRnamesFrom;
		UpdateCRnamesFrom();
		RadioButtonCRremoveNames->Checked				= (m_Options.CRnamesRemove == 1);
		RadioButtonCRseparateNames->Checked			= (m_Options.CRnamesSeparate == 1);
		ComboBoxCRseparateNamesBy->ItemIndex		= m_Options.CRnamesSeparateBy;
		EditCRnameSearchDeviation->Text					= IntToStr(m_Options.CRnameSearchDeviation);
		CheckBoxCRreplaceAllNames->Checked			= (m_Options.CRreplaceAllNames == 1);
		UpDownCRallNames->Tag										= m_Options.CRreplaceAllNamesWhat;
		UpdateCRallNames();
		CheckBoxCRremoveStrings->Checked				= (m_Options.CRremoveStrings == 1);
		EditCRremoveStrings->Text								= m_Options.CRremoveStringsWhat;
		CheckBoxCRtimeShift->Checked						= (m_Options.CRtimeShift == 1);
		UpDownCRtimeShift->Position							= (short)m_Options.CRtimeShiftNumber;

		CheckBoxFSenable->Checked								= (m_Options.FSenable == 1);
		UpDownFSstep->Position									= (short)m_Options.FSnumberOfLines;

		CheckBoxHKenable->Checked								= (m_Options.HKenable == 1);

		ComboBoxWDslot->ItemIndex								= m_Options.WDslot;

		PopupMenuForm->Items->Items[6]->Items[0]->Checked	= (m_Options.MMdisplayBackgroung == 1);
		PopupMenuForm->Items->Items[6]->Items[1]->Checked	= (m_Options.MMtopLeft == 1);
		PopupMenuForm->Items->Items[6]->Items[2]->Checked	= (m_Options.MMtopCenter == 1);
		PopupMenuForm->Items->Items[6]->Items[3]->Checked	= (m_Options.MMsafeMode == 1);
		PopupMenuForm->Items->Items[7]->Checked						= (m_Options.MMautohideTaskbar == 1);
		AutohideTaskbar1Click(PopupMenuForm->Items->Items[7]);
		ToggleTaskbarVisibility1->Checked                 = (m_Options.MMtaskbarVisible == 1);
		ToggleTaskbarVisibility1Click((TObject*)1);
		PopupMenuForm->Items->Items[9]->Items[0]->Checked	= (m_Options.MMviewPanelWK == 1);
		PopupMenuForm->Items->Items[9]->Items[1]->Checked	= (m_Options.MMviewPanelTA == 1);
		PopupMenuForm->Items->Items[9]->Items[2]->Checked	= (m_Options.MMviewPanelHK == 1);
		PopupMenuForm->Items->Items[9]->Items[3]->Checked	= (m_Options.MMviewPanelWD == 1);
		WaKan1Click(NULL);

		CheckBoxWKenable->Checked		= (m_Options.WKenable == 1);
		CheckBoxWKcompact->Checked	= (m_Options.WKcompact == 1);
		CheckBoxWKpatched->Checked	= (m_Options.WKpatched == 1);

		CheckBoxSTRtextFromTA->Checked = (m_Options.STRtextFromTA == 1);
		CheckBoxTARestartOnHang->Checked = (m_Options.TARestartOnHang == 1);
	}
}
//---------------------------------------------------------------------------
void WriteString(TFileStream *fs, wchar_t *str)
{
	int len = wcslen(str);
	fs->Write((void*)&len, sizeof(len));
	if (len > 0) {
		fs->Write((void*)str, (len+1) * sizeof(wchar_t));
	}
}
//---------------------------------------------------------------------------
UnicodeString ReadString(TFileStream *fs)
{
	UnicodeString ret;
	int len = -1;
	fs->Read((void*)&len, sizeof(len));
	if (len < 0)
		throw Exception(L"ReadString() invalid len");
	else if (len > 0) {
		wchar_t *buf = new wchar_t[len+1];
		fs->Read((void*)buf, (len+1) * sizeof(wchar_t));
		ret = buf;
		delete []buf;
	}
	return ret;
}
//---------------------------------------------------------------------------
void TFormTAHelper::SaveOptions(UnicodeString file)
{
	// store form values to options struct
	UpdateOptions(true);
	// write config file
	try {
		TIniFile *ini = new TIniFile(file);
		ini->WriteInteger(L"ROOT", L"Left", m_Options.Left);
		ini->WriteInteger(L"ROOT", L"Top", m_Options.Top);
		ini->WriteInteger(L"ROOT", L"Width", m_Options.Width);
		ini->WriteInteger(L"ROOT", L"Height", m_Options.Height);
		ini->WriteInteger(L"ROOT", L"Topmost", m_Options.Topmost);
		ini->WriteInteger(L"ROOT", L"NamesSplitterPos", m_Options.NamesSplitterPos);

		ini->WriteInteger(L"ClipboardReviser", L"CRenable", m_Options.CRenable);
		ini->WriteInteger(L"ClipboardReviser", L"CRremoveLinebreaks", m_Options.CRremoveLinebreaks);
		ini->WriteInteger(L"ClipboardReviser", L"CRseparateLinebreaks", m_Options.CRseparateLinebreaks);
		ini->WriteInteger(L"ClipboardReviser", L"CRsuppressCharRepetition", m_Options.CRsuppressCharRepetition);
		ini->WriteInteger(L"ClipboardReviser", L"CRcharRepetitionNumber", m_Options.CRcharRepetitionNumber);
		ini->WriteInteger(L"ClipboardReviser", L"CRcharRepetitionLen", m_Options.CRcharRepetitionLen);
		ini->WriteInteger(L"ClipboardReviser", L"CRnames", m_Options.CRnames);
		ini->WriteInteger(L"ClipboardReviser", L"CRnamesFrom", m_Options.CRnamesFrom);
		ini->WriteInteger(L"ClipboardReviser", L"CRnamesRemove", m_Options.CRnamesRemove);
		ini->WriteInteger(L"ClipboardReviser", L"CRnamesSeparate", m_Options.CRnamesSeparate);
		ini->WriteInteger(L"ClipboardReviser", L"CRnamesSeparateBy", m_Options.CRnamesSeparateBy);
		ini->WriteInteger(L"ClipboardReviser", L"CRnameSearchDeviation", m_Options.CRnameSearchDeviation);
		ini->WriteInteger(L"ClipboardReviser", L"CRreplaceAllNames", m_Options.CRreplaceAllNames);
		ini->WriteInteger(L"ClipboardReviser", L"CRreplaceAllNamesWhat", m_Options.CRreplaceAllNamesWhat);
		ini->WriteInteger(L"ClipboardReviser", L"CRremoveStrings", m_Options.CRremoveStrings);
		ini->WriteString (L"ClipboardReviser", L"CRremoveStringsWhat", StrToHex(m_Options.CRremoveStringsWhat));
		ini->WriteInteger(L"ClipboardReviser", L"CRtimeShift", m_Options.CRtimeShift);
		ini->WriteInteger(L"ClipboardReviser", L"CRtimeShiftNumber", m_Options.CRtimeShiftNumber);

		ini->WriteInteger(L"FloatingScroll", L"FSenable", m_Options.FSenable);
		ini->WriteInteger(L"FloatingScroll", L"FSnumberOfLines", m_Options.FSnumberOfLines);

		ini->WriteInteger(L"HotKeys", L"HKenable", m_Options.HKenable);

		ini->WriteInteger(L"WindowDetacher", L"WDslot", m_Options.WDslot);
		ini->WriteString (L"WindowDetacher", L"WDslots", BinToHex((void*)m_Options.WDslots, sizeof(RECT)*TOPT_WDSLOTS));

		ini->WriteInteger(L"MainMenu", L"MMdisplayBackgroung", m_Options.MMdisplayBackgroung);
		ini->WriteInteger(L"MainMenu", L"MMtopLeft", m_Options.MMtopLeft);
		ini->WriteInteger(L"MainMenu", L"MMtopCenter", m_Options.MMtopCenter);
		ini->WriteInteger(L"MainMenu", L"MMsafeMode", m_Options.MMsafeMode);
		ini->WriteInteger(L"MainMenu", L"MMautohideTaskbar", m_Options.MMautohideTaskbar);
		ini->WriteInteger(L"MainMenu", L"MMviewPanelWK", m_Options.MMviewPanelWK);
		ini->WriteInteger(L"MainMenu", L"MMviewPanelTA", m_Options.MMviewPanelTA);
		ini->WriteInteger(L"MainMenu", L"MMviewPanelHK", m_Options.MMviewPanelHK);
		ini->WriteInteger(L"MainMenu", L"MMviewPanelWD", m_Options.MMviewPanelWD);
		ini->WriteInteger(L"MainMenu", L"MMtaskbarVisible", m_Options.MMtaskbarVisible);

		ini->WriteInteger(L"WaKan", L"WKenable", m_Options.WKenable);
		ini->WriteInteger(L"WaKan", L"WKcompact", m_Options.WKcompact);
		ini->WriteString (L"WaKan", L"WKpos", BinToHex((void*)&(m_Options.WKpos), sizeof(RECT)));
		ini->WriteInteger(L"WaKan", L"WKpatched", m_Options.WKpatched);

		ini->WriteInteger(L"Systran", L"STRtextFromTA", m_Options.STRtextFromTA);
		ini->WriteInteger(L"TA", L"TARestartOnHang", m_Options.TARestartOnHang);
		// close file
		delete ini;
		UpdateConfigFileValue(file);
		StatusbarError(L"Configuration saved", 2);
	} catch(Exception &e) {
		Application->ShowException(&e);
	}

}
//---------------------------------------------------------------------------
void TFormTAHelper::LoadOptions(UnicodeString file)
{
	// import of old configuration
	UnicodeString ext(ExtractFileExt(file));
	if (ext.CompareIC(L".cfg") == 0) {
		LoadOptions0(file);
		UpdateOptions(false); // update form controls
		RenameFile(file, file+L".bak"); // rename old config file to hide it from open dialog
		UnicodeString msg; msg.printf(L"Configuration was stored in new format (*.tah).\nIt will be used from now on.\n\n ※ Backup of old configuration: %s", ExtractFileName(file+L".bak"));
		file = ChangeFileExt(file, L".tah");
		SaveOptions(file);
		MessageBox(Handle, msg.w_str(), TAH_APP_TITLE, MB_ICONINFORMATION);
		return;
	}
	// load new configuration
	if(!FileExists(file)) {
		if(Boot)
    	UpdateOptions(false);
		SaveOptions(file);
	} else {
		try {
			TIniFile *ini = new TIniFile(file);

			m_Options.Left							= ini->ReadInteger(L"ROOT", L"Left", 10);
			m_Options.Top								= ini->ReadInteger(L"ROOT", L"Top", 10);
			m_Options.Width							= ini->ReadInteger(L"ROOT", L"Width", 550);
			m_Options.Height						= ini->ReadInteger(L"ROOT", L"Height", 450);
			m_Options.Topmost						= ini->ReadInteger(L"ROOT", L"Topmost", 1);
			m_Options.NamesSplitterPos	= ini->ReadInteger(L"ROOT", L"NamesSplitterPos", 181);

			m_Options.CRenable									= ini->ReadInteger(L"ClipboardReviser", L"CRenable", 0);
			m_Options.CRremoveLinebreaks				= ini->ReadInteger(L"ClipboardReviser", L"CRremoveLinebreaks", 0);
			m_Options.CRseparateLinebreaks			= ini->ReadInteger(L"ClipboardReviser", L"CRseparateLinebreaks", 0);
			m_Options.CRsuppressCharRepetition	= ini->ReadInteger(L"ClipboardReviser", L"CRsuppressCharRepetition", 0);
			m_Options.CRcharRepetitionNumber		= ini->ReadInteger(L"ClipboardReviser", L"CRcharRepetitionNumber", 2);
			m_Options.CRcharRepetitionLen				= ini->ReadInteger(L"ClipboardReviser", L"CRcharRepetitionLen", 3);
			m_Options.CRnames										= ini->ReadInteger(L"ClipboardReviser", L"CRnames", 0);
			m_Options.CRnamesFrom								= ini->ReadInteger(L"ClipboardReviser", L"CRnamesFrom", 1);
			m_Options.CRnamesRemove							= ini->ReadInteger(L"ClipboardReviser", L"CRnamesRemove", 1);
			m_Options.CRnamesSeparate						= ini->ReadInteger(L"ClipboardReviser", L"CRnamesSeparate", 0);
			m_Options.CRnamesSeparateBy					= ini->ReadInteger(L"ClipboardReviser", L"CRnamesSeparateBy", 0);
			m_Options.CRnameSearchDeviation			= ini->ReadInteger(L"ClipboardReviser", L"CRnameSearchDeviation", 0);
			m_Options.CRreplaceAllNames					= ini->ReadInteger(L"ClipboardReviser", L"CRreplaceAllNames", 0);
			m_Options.CRreplaceAllNamesWhat			= ini->ReadInteger(L"ClipboardReviser", L"CRreplaceAllNamesWhat", 1);
			m_Options.CRremoveStrings						= ini->ReadInteger(L"ClipboardReviser", L"CRremoveStrings", 0);
			m_Options.CRremoveStringsWhat = HexToStr(ini->ReadString(L"ClipboardReviser", L"CRremoveStringsWhat", L""));
			m_Options.CRtimeShift								= ini->ReadInteger(L"ClipboardReviser", L"CRtimeShift", 0);
			m_Options.CRtimeShiftNumber					= ini->ReadInteger(L"ClipboardReviser", L"CRtimeShiftNumber", 1);

			m_Options.FSenable				= ini->ReadInteger(L"FloatingScroll", L"FSenable", 0);
			m_Options.FSnumberOfLines	= ini->ReadInteger(L"FloatingScroll", L"FSnumberOfLines", 1);

			m_Options.HKenable				= ini->ReadInteger(L"HotKeys", L"HKenable", 0);

			m_Options.WDslot					= ini->ReadInteger(L"WindowDetacher", L"WDslot", 0);
			HexToBin(ini->ReadString(L"WindowDetacher", L"WDslots", L"").w_str(), (void*)m_Options.WDslots, sizeof(RECT)*TOPT_WDSLOTS);

			m_Options.MMdisplayBackgroung	= ini->ReadInteger(L"MainMenu", L"MMdisplayBackgroung", 1);
			m_Options.MMtopLeft						= ini->ReadInteger(L"MainMenu", L"MMtopLeft", 0);
			m_Options.MMtopCenter					= ini->ReadInteger(L"MainMenu", L"MMtopCenter", 1);
			m_Options.MMsafeMode					= ini->ReadInteger(L"MainMenu", L"MMsafeMode", 0);
			m_Options.MMautohideTaskbar		= ini->ReadInteger(L"MainMenu", L"MMautohideTaskbar", (m_AutohideTaskbar == 1)?1:0);
			m_Options.MMviewPanelWK				= ini->ReadInteger(L"MainMenu", L"MMviewPanelWK", 1);
			m_Options.MMviewPanelTA				= ini->ReadInteger(L"MainMenu", L"MMviewPanelTA", 1);
			m_Options.MMviewPanelHK				= ini->ReadInteger(L"MainMenu", L"MMviewPanelHK", 1);
			m_Options.MMviewPanelWD				= ini->ReadInteger(L"MainMenu", L"MMviewPanelWD", 1);
			m_Options.MMtaskbarVisible    = ini->ReadInteger(L"MainMenu", L"MMtaskbarVisible", 1);

			m_Options.WKenable	= ini->ReadInteger(L"WaKan", L"WKenable", 0);
			m_Options.WKcompact	= ini->ReadInteger(L"WaKan", L"WKcompact", 0);
			HexToBin(ini->ReadString(L"WaKan", L"WKpos", L"").w_str(), (void*)&(m_Options.WKpos), sizeof(RECT));
			m_Options.WKpatched	= ini->ReadInteger(L"WaKan", L"WKpatched", 0);

			m_Options.STRtextFromTA = ini->ReadInteger(L"Systran", L"STRtextFromTA", 1);
			m_Options.TARestartOnHang = ini->ReadInteger(L"TA", L"TARestartOnHang", 1);
			// close ini file
			delete ini;
			StatusbarError(L"Configuration loaded", 2);
		} catch(Exception &e) {
			Application->ShowException(&e);
		}
	}
	UpdateOptions(false); // update form controls
	UpdateConfigFileValue(file);
}
//---------------------------------------------------------------------------
void TFormTAHelper::LoadOptions0(UnicodeString file)
{
	// load new configuration
	if(FileExists(file)) {
		try {
			tOptions0 options; // old options format
			TFileStream *fs = new TFileStream(file, fmOpenRead);
			// read options struct
			fs->Read((void*)&options, sizeof(tOptions0));
			// read strings to remove
			m_Options.CRremoveStringsWhat = ReadString(fs);
			delete fs; // close file

			// assigh loaded data to current options
			m_Options.Left							= options.Left;
			m_Options.Top								= options.Top;
			m_Options.Width							= options.Width;
			m_Options.Height						= options.Height;
			m_Options.Topmost						= options.Topmost;
			m_Options.NamesSplitterPos	= options.NamesSplitterPos;

			m_Options.CRenable									= options.CRenable;
			m_Options.CRremoveLinebreaks				= options.CRremoveLinebreaks;
			m_Options.CRseparateLinebreaks			= options.CRseparateLinebreaks;
			m_Options.CRsuppressCharRepetition	= options.CRsuppressCharRepetition;
			m_Options.CRcharRepetitionNumber		= options.CRcharRepetitionNumber;
			m_Options.CRnames										= options.CRnames;
			m_Options.CRnamesFrom								= options.CRnamesFrom;
			m_Options.CRnamesRemove							= options.CRnamesRemove;
			m_Options.CRnamesSeparate						= options.CRnamesSeparate;
			m_Options.CRnamesSeparateBy					= options.CRnamesSeparateBy;
			m_Options.CRnameSearchDeviation			= options.CRnameSearchDeviation;
			m_Options.CRreplaceAllNames					= options.CRreplaceAllNames;
			m_Options.CRreplaceAllNamesWhat			= options.CRreplaceAllNamesWhat;
			m_Options.CRremoveStrings						= options.CRremoveStrings;

			m_Options.FSenable				= options.FSenable;
			m_Options.FSnumberOfLines	= options.FSnumberOfLines;

			m_Options.HKenable = options.HKenable;

			m_Options.WDslot = options.WDslot;
			for (int i = 0; i < TOPT_WDSLOTS; i++) {
				if (i >= TOPT_WDSLOTS0)
					break;
				m_Options.WDslots[i] = options.WDslots[i];
			}
		} catch(Exception &e) {
			Application->ShowException(&e);
		}
	}
}
//---------------------------------------------------------------------------
void TFormTAHelper::UpdateConfigFileValue(UnicodeString file)
{
	// update file name
	m_ConfigFile = file;
	// update form
	Caption = FileFromPath(file) + L" - " + TAH_APP_TITLE;
	Application->Title = UnicodeString(TAH_APP_TITLE) + L" - " + FileFromPath(file);
}
//---------------------------------------------------------------------------
UnicodeString TFormTAHelper::FileFromPath(UnicodeString path)
{
	int i = path.LastDelimiter(L"\\");
	return path.SubString(i+1, path.Length()-i);
}
//---------------------------------------------------------------------------
//UnicodeString TFormTAHelper::CurrentDir()
//{
//	wchar_t buff[_MAX_PATH]; buff[0] = 0;
//	GetCurrentDirectory(_MAX_PATH, buff);
//	return (UnicodeString(buff) + L"\\");
//}
//---------------------------------------------------------------------------
UnicodeString TFormTAHelper::GetLastConfig()
{
	UnicodeString ret;
	try {
		TRegistry *reg = new TRegistry();
		if(reg->OpenKey(TAH_REGKEY, false)) {
			ret = reg->ReadString(L"LastConfig");
			reg->CloseKey();
			reg->DeleteKey(TAH_REGKEY); // delete obsolete registry key
			reg->CloseKey();
		} else {
			TIniFile *ini = new TIniFile(ChangeFileExt(GetModuleName(unsigned(HInstance)), L".ini"));
			ret = HexToStr(ini->ReadString(L"ROOT", L"LastConfig", L""));
			delete ini;
		}
		delete reg;
	} catch (...) {}
	if (ret.Length())
		if (!FileExists(ret))
			ret = L"";
	return ret;
}
//---------------------------------------------------------------------------
void TFormTAHelper::SetLastConfig(UnicodeString file)
{
	TIniFile *ini = new TIniFile(ChangeFileExt(GetModuleName(unsigned(HInstance)), L".ini"));
	ini->WriteString(L"ROOT", L"LastConfig", StrToHex(file));
	delete ini;
}
//---------------------------------------------------------------------------
void TFormTAHelper::SaveOptionsGlobal()
{
	TIniFile *ini = new TIniFile(ChangeFileExt(GetModuleName(unsigned(HInstance)), L".ini"));
	ini->WriteInteger(L"WindowDetacher", L"WDforceRedraw", m_OptionsGlobal.WDforceRedraw);
	ini->WriteInteger(L"WindowDetacher", L"WDredrawDelay", m_OptionsGlobal.WDredrawDelay);
	ini->WriteInteger(L"ClipboardReviser", L"CRforceJpnClipbrdLocale", m_OptionsGlobal.CRforceJpnClipbrdLocale);
	ini->WriteInteger(L"ClipboardReviser", L"CRtextVolumeLimit", m_OptionsGlobal.CRtextVolumeLimit);
	delete ini;
}
//---------------------------------------------------------------------------
void TFormTAHelper::LoadOptionsGlobal()
{
	TIniFile *ini = new TIniFile(ChangeFileExt(GetModuleName(unsigned(HInstance)), L".ini"));
	m_OptionsGlobal.WDforceRedraw = ini->ReadInteger(L"WindowDetacher", L"WDforceRedraw", 1);
	m_OptionsGlobal.WDredrawDelay = ini->ReadInteger(L"WindowDetacher", L"WDredrawDelay", 200);
	TimerWDredraw->Interval = m_OptionsGlobal.WDredrawDelay;
	m_OptionsGlobal.CRforceJpnClipbrdLocale = ini->ReadInteger(L"ClipboardReviser", L"CRforceJpnClipbrdLocale", 1);
	m_OptionsGlobal.CRtextVolumeLimit = ini->ReadInteger(L"ClipboardReviser", L"CRtextVolumeLimit", 1024);
	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::FormShow(TObject *Sender)
{
	if(Boot) { // startup init
		LoadOptionsGlobal();

		UnicodeString filename(ParamStr(1)); // get config file from command line
		if (filename.Length() && !filename.Pos(L":"))
			filename = ProgramDir() + filename;
		if (!filename.Length())
			filename = GetLastConfig(); // get last config file
		if (!filename.Length())
			filename = ProgramDir() + L"Default.tah"; // default config file
		LoadOptions(filename);

		// Always on top state
		SpeedButtonTopmostClick(this);
		// load names
		LoadNames(true); // global names
		LoadNames(false); // local names
		UpdateNames(); // update memory list

		// find TA, SYSTRAN, WaKan
		TALFind(true);
		STRFind();
		WKFind();

		Clock();

		Boot = false; // init complete
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::FormClose(TObject *Sender,
			TCloseAction &Action)
{
	// no need to check TA anymore
	TimerTALcheck->Enabled = false;

	SaveOptionsGlobal();

	// save current options
	SaveOptions(m_ConfigFile);
	SaveNames(true); // global names
	SaveNames(false); // local names
	SetLastConfig(m_ConfigFile);

	// remove hooks
	if(CheckBoxCRenable->Checked) {
		CheckBoxCRenable->Checked = false;
		CheckBoxCRenableClick(this);
	}
	if(CheckBoxFSenable->Checked) {
		CheckBoxFSenable->Checked = false;
		CheckBoxFSenableClick(this);
	}
	if(CheckBoxHKenable->Checked) {
		CheckBoxHKenable->Checked = false;
		CheckBoxHKenableClick(this);
	}

	// show TA if was hidden
	HWND taw = TALFind();
	if (taw && !IsWindowVisible(taw))
		ShowWindow(taw, SW_SHOW);

	// restore autohide state of taskbar
	ToggleTaskbarVisibility1Click(NULL);
	SetAutohideTaskbar(m_AutohideTaskbar);

	// clear and delete names list
	ClearNameList();
	delete m_NameList;
  // delete window dragger
	delete m_pGetWinDrag;
	// delete stray windows list
	delete m_StrayWinList;
	// delete history lists
	for(int i=0; i<m_CRhistoryList->Count; i++)
		delete (UnicodeString*)m_CRhistoryList->Items[i];
	delete m_CRhistoryList;
	for(int i=0; i<m_CRhistoryRawList->Count; i++)
		delete (UnicodeString*)m_CRhistoryRawList->Items[i];
	delete m_CRhistoryRawList;
	// delete SYSTRAN
	delete m_Systran;
	// delete WaKan
	delete m_Wakan;
}
//---------------------------------------------------------------------------
void TFormTAHelper::ClearNameList()
{
  // clear names list
  for(int i=0; i<m_NameList->Count; i++)
  	if(m_NameList->Items[i])
    	delete (tName*)m_NameList->Items[i];
	m_NameList->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerDragTimer(TObject *Sender)
{
	TimerDrag->Enabled = false;

	if(m_pGetWinDrag->OnDrag()) {
		HWND hw = m_pGetWinDrag->Handle();
		if(m_DragMode == WD) { // Window Detacher now dragging
			if(ComboBoxWDslot->ItemIndex > 0) {
				SetWDhwnd(hw); // update WD handle container and display
				ComboBoxWDslotChange(NULL); // NULL means "called from drag(here)"
			} else {
				StatusbarError(L"Wrong label selected. No point in detaching TA itself");
			}
		}
	}
	TimerDrag->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::ButtonWDdetachClick(TObject *Sender)
{
	if(ComboBoxWDslot->ItemIndex == 0) {
		StatusbarError(L"Wrong label selected. No point in detaching TA itself");
		return;
	}
	HWND hwta = TALFind();
	if(!hwta) {
		StatusbarError(L"TA not found");
		return;
	}
	// get TA window AlwaysOnTop state
	bool topmost = GetWinTopState(hwta);

	// get picked up window
	HWND hw = (HWND)EditWDhandle->Tag;
	if(!IsWindow(hw)) {
		StatusbarError(L"The window is no more");
		return;
	}
	// store handle in current slot
	m_WDwindows[ComboBoxWDslot->ItemIndex].handle = hw;

	// remove menu if any
	HMENU hm = GetMenu(hw);
	if (hm) {
		::SetMenu(hw, NULL);
		DestroyMenu(hm);
	}
	// detach window
	ShowWindow(hw, SW_HIDE);
	SetWindowLong(hw, GWL_HWNDPARENT, (LONG)0);
	SetWindowLong(hw, GWL_STYLE, WS_CAPTION|WS_SIZEBOX|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX);
	SetWindowLong(hw, GWL_EXSTYLE, WS_EX_TOOLWINDOW|WS_EX_NOPARENTNOTIFY);
	SetWindowText(hw, m_WDwindows[ComboBoxWDslot->ItemIndex].title.w_str());
	// place it according to current slot data
	RECT r = m_Options.WDslots[ComboBoxWDslot->ItemIndex];
	if(!tRECTEMPTY(r))
		SetWindowPos(hw, topmost ? HWND_TOPMOST : HWND_NOTOPMOST,
			r.left, r.top, r.right-r.left+1, r.bottom-r.top+1,
			SWP_DRAWFRAME|SWP_FRAMECHANGED|SWP_SHOWWINDOW);
	else
		SetWindowPos(hw, topmost ? HWND_TOPMOST : HWND_NOTOPMOST,
			0,0,0,0,
			SWP_DRAWFRAME|SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	// store its ancestor handle for further detection of this window
	hw = GetAncestor(hw, GA_ROOT);
	if(m_StrayWinList->IndexOf((void*)hw) == -1) {
		m_StrayWinList->Add((void*)hw);
	}
	// update WD buttons
	ComboBoxWDslotChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::CheckBoxHKenableClick(TObject *Sender)
{
	if(CheckBoxHKenable->Checked) {
		// set keyboard hook
		m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)HookKeyboardProc, GetModuleHandle(NULL), NULL/*GetCurrentThreadId()*/);
		if(m_hKeyboardHook == NULL) throw Exception(L"TFormTAHelper::CheckBoxHKenableClick() SetWindowsHookEx() failed");
	} else {
		UnhookWindowsHookEx(m_hKeyboardHook);
		m_hKeyboardHook = NULL;
	}
}
//---------------------------------------------------------------------------
HWND TFormTAHelper::TALFind(bool forcefind, bool notimer)
{
	if (!notimer) TimerTALcheck->Enabled = false;

	HWND hw0 = (HWND)LabelTALstatus->Tag; // old value
	HWND hw = FindWindow(TA_WCLASS, NULL);
	if (hw != NULL) hw = GetAncestor(hw, GA_ROOT); // new value
	if (hw != NULL && hw != hw0) {
		RefreshClipboardMonitor();
		// store TA exe path
		DWORD processID = 0;
		HANDLE processHandle = NULL;
		TCHAR filename[MAX_PATH];
		if (GetWindowThreadProcessId(hw, &processID)) {
			processHandle = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, processID);
			if (processHandle != NULL) {
				if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH)) {
					m_TAexe = filename;
				}
				CloseHandle(processHandle);
			}
		}
	}
	if (forcefind || hw != hw0) TALResult(hw); // assign and display result

	// monitor TA
	if (CheckBoxTARestartOnHang->Checked) {
		if (hw == NULL) { // restart TA on termination
			if (!m_TAexe.IsEmpty()) {
				// but check for running process first (compiling dictionaries while no window yet)
				bool tarunning = false;
				HANDLE hProcSnap = NULL;
				PROCESSENTRY32 pe32;
				pe32.dwSize = sizeof(pe32);

				hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if (hProcSnap == INVALID_HANDLE_VALUE) {
					throw Exception(L"TFormTAHelper::TALFind() : CreateToolhelp32Snapshot() failed");
				}
				if (!Process32First(hProcSnap, &pe32)) {
					CloseHandle(hProcSnap);
					throw Exception(L"TFormTAHelper::TALFind() : Process32First() failed");
				}
				do {
					// get full exe path
					HANDLE processHandle = NULL;
					TCHAR filename[MAX_PATH];
					processHandle = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
					if (processHandle != NULL) {
						if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH)) {
							if (m_TAexe.CompareIC(filename) == 0) {
								tarunning = true;
								CloseHandle(processHandle);
								break;
							}
						}
						CloseHandle(processHandle);
					}
				} while (Process32Next(hProcSnap, &pe32));
				CloseHandle(hProcSnap);

				if (!tarunning) { // launch TA process
					UnicodeString tadir(ExtractFilePath(m_TAexe));
					tadir = tadir.Delete(tadir.Length(), 1); // remove last separator
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory(&si, sizeof(si));
					si.cb = sizeof(si);
					ZeroMemory(&pi, sizeof(pi));
					if (CreateProcess(m_TAexe.c_str(), NULL, NULL, NULL, FALSE, 0, NULL,
							tadir.c_str(), &si, &pi)) {
						CloseHandle(pi.hThread);
						CloseHandle(pi.hProcess);
					}
				}
			}
		} else {
			if (IsHungAppWindow(hw)) { // terminate TA window if hung
				TerminateProcess(hw);
			}
		}
	}

	// Suppress Taskbar reappearance by itself despite being set to invisible
	ToggleTaskbarVisibility1Click((TObject*)1);

	if (!notimer) TimerTALcheck->Enabled = true;
	return hw;
}
//---------------------------------------------------------------------------
void TFormTAHelper::STRFind()
{
	if (m_Systran->FindToolbar()) {
		if (LabelSTRstatus->Font->Color != clGreen) {
			LabelSTRstatus->Font->Color = clGreen;
			LabelSTRstatus->Caption = L"Hooked with SYSTRAN";
		}
	} else {
		if (LabelSTRstatus->Font->Color != clRed) {
			LabelSTRstatus->Font->Color = clRed;
			LabelSTRstatus->Caption = L"Please run Translation Toolbar";
		}
	}
}
//---------------------------------------------------------------------------
void TFormTAHelper::WKFind()
{
	m_Wakan->CompactControl();
	if (m_Wakan->DllControl(Handle)) {
		if (LabelWKstatus->Font->Color != clGreen) {
			LabelWKstatus->Font->Color = clGreen;
			LabelWKstatus->Caption = L"Hooked with WaKan";
		}
	} else {
		if (LabelWKstatus->Font->Color != clRed) {
			LabelWKstatus->Font->Color = clRed;
			LabelWKstatus->Caption = L"Please run WaKan";
		}
	}
}
//---------------------------------------------------------------------------
void TFormTAHelper::TALResult(HWND hw)
{
	if(hw != NULL) {
		LabelTALstatus->Font->Color = clGreen;
		LabelTALstatus->Caption = L"Translation Aggregator is active";
		// restore positions of TA windows
		FormTAHelper->TimerTALrefind->Enabled = true;
	} else {
		LabelTALstatus->Font->Color = clRed;
		LabelTALstatus->Caption = L"Please run Translation Aggregator";
	}
	// store TA handle
	LabelTALstatus->Tag = (int)hw;

	// update WD buttons for TA main window
	ComboBoxWDslotChange(this);
	SpeedButtonWDtopSync->Enabled = (hw != NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerTALrefindTimer(TObject *Sender)
{
	TimerTALrefind->Enabled = false;
	// find TA again
	TALFind();
	// restore positions of all detached windows
	WDrestoreAll();
}
//---------------------------------------------------------------------------
void TFormTAHelper::RefreshClipboardMonitor()
{
	// recreate clipboard monitoring
	if(CheckBoxCRenable->Checked) {
		CheckBoxCRenable->Checked = false;
		CheckBoxCRenable->Checked = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonCRapplyClick(TObject *Sender)
{
	// reset indication of changes in names text
	TimerAnime->Tag = 1;
	SpeedButtonCRapply->Caption = SpeedButtonCRapply->HelpKeyword;

	// refine editbox text and update memory list
	UpdateNames();
	// save names-memoboxes to disk files
	SaveNames(true); // global names
	SaveNames(false); // local names
}
//---------------------------------------------------------------------------
// dependent on m_ConfigFile
UnicodeString TFormTAHelper::GenNamesFileName(bool global)
{
	UnicodeString ret;
	if(global) {
		ret = ProgramDir() + TAH_FILE_NAMES;
	} else {
		if(!m_ConfigFile.Length()) throw Exception(L"TFormTAHelper::GenNamesFileName() invalid m_ConfigFile");
		ret = m_ConfigFile.SubString(1, m_ConfigFile.LastDelimiter(L".")) + TAH_FILE_NAMES;
	}
	return ret;
}
//---------------------------------------------------------------------------
void TFormTAHelper::SaveNames(bool global)
{
	// save names-memobox to disk file
	TFileStream *file = NULL;
	try {
		file = new TFileStream(GenNamesFileName(global), fmCreate);
		TMemo *pmemo = global ? MemoCRnamesGlobal : MemoCRnames;
		file->Write((void*)pmemo->Lines->Text.w_str() , pmemo->Lines->Text.Length()*pmemo->Lines->Text.ElementSize());
		delete file;
	} catch (Exception &e) {
		Application->ShowException(&e);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::MemoCRnamesChange(TObject *Sender)
{
	// indication of changes in names text
	if(!MemoCRnames->Tag) {
    if(!TimerAnime->Enabled) {
			TimerAnime->Enabled = true;
		}
	}
}
//---------------------------------------------------------------------------
void TFormTAHelper::SetWDhwnd(HWND hw)
{
	UnicodeString s;
	if(hw != NULL) s.printf(L"0x%08X", hw);
	EditWDhandle->Text = s;
	EditWDhandle->Tag = (int)hw;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::ComboBoxWDslotChange(TObject *Sender)
{
	HWND hw = NULL, hw1 = NULL;
	if(Sender != NULL) { // on combobox change
		// set handle from slot
		hw = (ComboBoxWDslot->ItemIndex == 0)? (HWND)LabelTALstatus->Tag : m_WDwindows[ComboBoxWDslot->ItemIndex].handle;
		hw1 = hw;
		SetWDhwnd(hw);
	} else { // on WD drag
		// get handle already assigned by drag func
		hw = (HWND)EditWDhandle->Tag;
		hw1 = m_WDwindows[ComboBoxWDslot->ItemIndex].handle;
	}
	SpeedButtonWDdetach->Enabled = (hw != NULL);
	// only allow save|restore if window was detached beforehand (hw1!=NULL)
	RECT r = m_Options.WDslots[ComboBoxWDslot->ItemIndex];
	SpeedButtonWDrestore->Enabled = (hw && hw1 && !tRECTEMPTY(r));
	SpeedButtonWDsave->Enabled = (hw && hw1);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonWDsaveClick(TObject *Sender)
{
	// store detached window rect
	HWND hw = (HWND)EditWDhandle->Tag;
	if(!hw) throw Exception(L"TFormTAHelper::SpeedButtonWDsaveClick() invalid hw");
	if(IsWindow(hw)) {
		RECT r;
		if(!GetWindowRect(hw, &r)) throw Exception(L"TFormTAHelper::SpeedButtonWDsaveClick() GetWindowRect() failed");
		m_Options.WDslots[ComboBoxWDslot->ItemIndex] = r;
		ComboBoxWDslotChange(this);
	} else {
		StatusbarError(L"The window is no more");
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonWDrestoreClick(TObject *Sender)
{
	// restore detached window position and size
	HWND hw = (HWND)EditWDhandle->Tag;
	if(!hw) throw Exception(L"TFormTAHelper::SpeedButtonWDrestoreClick() invalid hw");
	if(IsWindow(hw)) {
		RECT r = m_Options.WDslots[ComboBoxWDslot->ItemIndex];
		if(tRECTEMPTY(r)) throw Exception(L"TFormTAHelper::SpeedButtonWDrestoreClick() empty rect");
		MoveWindow(hw, r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, TRUE);
		// if TA selected
		// restore positions of detached windows
		if (ComboBoxWDslot->ItemIndex == 0) {
			FormTAHelper->TimerTALrefind->Enabled = true;
		}
  } else {
		StatusbarError(L"The window is no more");
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::CheckBoxCRremoveLinebreaksClick(
			TObject *Sender)
{
	if(CheckBoxCRremoveLinebreaks->Checked)
		CheckBoxCRseparateLinebreaks->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::CheckBoxCRseparateLinebreaksClick(
			TObject *Sender)
{
	if(CheckBoxCRseparateLinebreaks->Checked)
		CheckBoxCRremoveLinebreaks->Checked = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::DisplayHintDummy(TObject *Sender)
{
	// dummy func
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::DisplayHint(TObject *Sender)
{
	UnicodeString hint(GetLongHint(Application->Hint));
	if (hint != ImageFurniture->Hint &&
			hint != LabelSTRstatus->Hint &&
			hint != LabelWKstatus->Hint)
		StatusBarText(hint);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::FormCreate(TObject *Sender)
{
	Application->OnHint = DisplayHint;
	ListBoxCRhistory->Align = alClient;
	TrayIcon1->Hint = UnicodeString(TAH_APP_TITLE) + L" " + ProgramVer();
	ImageFurniture->Hint = TrayIcon1->Hint + L"\nat your service";
	MemoHKhotkeys->Lines->Text = MemoHKhotkeys->Lines->Text.Trim();
	SpeedButtonCRreprocess->Tag = 0; // "reprocess was pressed" flag
	SpeedButtonCRcopy->Tag = 0; // "copy/send was pressed" flag
	UINT tabs = 13; // distance between tabs (in some logical units)
	SendMessage(MemoCRcontents->Handle, EM_SETTABSTOPS, 1, (LONG)&tabs);
	SendMessage(MemoCRnames->Handle, EM_SETTABSTOPS, 1, (LONG)&tabs);
	SendMessage(MemoCRnamesGlobal->Handle, EM_SETTABSTOPS, 1, (LONG)&tabs);
	MemoCRcontents->Lines->Text = L"";
	PanelCRhistoryScroll->Tag = 0;
	// initialize default autohide taskbar options
	m_AutohideTaskbar = GetAutohideTaskbar();
	if (m_AutohideTaskbar == 1) {
		m_Options.MMautohideTaskbar = 1;
	}

	int i=0;
	m_WDwindows[i++].title = L"TA main window";
	m_WDwindows[i++].title = L"WWWJDIC";
	m_WDwindows[i++].title = L"Mecab";
	m_WDwindows[i++].title = L"ATLAS";
	m_WDwindows[i++].title = L"Original Text";
	m_WDwindows[i++].title = L"Google";
	m_WDwindows[i++].title = L"Honyaku";
	m_WDwindows[i++].title = L"Babel Fish";
	m_WDwindows[i++].title = L"FreeTranslation";
	m_WDwindows[i++].title = L"Excite"; // 10 == TOPT_WDSLOTS
	for(i=0; i<TOPT_WDSLOTS; i++) {
		ComboBoxWDslot->Items->Add(m_WDwindows[i].title);
		m_WDwindows[i].handle = NULL;
	}
}
//---------------------------------------------------------------------------
bool TFormTAHelper::AmongWDtitles(UnicodeString title)
{
	bool ret = false;
	for(int i=0; i<TOPT_WDSLOTS; i++) {
		if (title == m_WDwindows[i].title) {
			ret = true;
			break;
		}
	}
	return ret;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonWDtopSyncClick(TObject *Sender)
{
	// get TA window
	HWND hw = TALFind();
	if(!hw) {
		StatusbarError(L"TA not found");
		return;
	}
	// get TA window topmost state
	bool top = GetWinTopState(hw);
	//sync topmost state of all detached windows
	for(int i=0; i<m_StrayWinList->Count; i++) {
		if(!SetWinTopState((HWND)m_StrayWinList->Items[i], top)) {
			m_StrayWinList->Delete(i); // delete invalid windows
			i--;
		}
	}
}
//---------------------------------------------------------------------------
bool GetWinTopState(HWND hwnd)
{
	LONG xst = GetWindowLong(hwnd, GWL_EXSTYLE);
	if(!xst) throw Exception(L"TFormTAHelper::GetTopState() GetWindowLong() failed");
	return (xst&WS_EX_TOPMOST);
}
//---------------------------------------------------------------------------
bool SetWinTopState(HWND hwnd, bool top)
{
	return SetWindowPos(hwnd, top?HWND_TOPMOST:HWND_NOTOPMOST/*HWND_BOTTOM*/, 0,0,0,0, SWP_ASYNCWINDOWPOS|SWP_NOMOVE|SWP_NOSIZE);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::StatusBar1DrawPanel(TStatusBar *StatusBar,
      TStatusPanel *Panel, const TRect &Rect)
{
	if(StatusBar->Tag == 1) { //  red error
		StatusBar->Canvas->Brush->Color = clRed;
		StatusBar->Canvas->FillRect(Rect);
		StatusBar->Canvas->Font->Color = clYellow;
		StatusBar->Canvas->Font->Style = TFontStyles()<<fsBold;
	}
	else if(StatusBar->Tag == 2) { // green message
		StatusBar->Canvas->Brush->Color = clGreen;
		StatusBar->Canvas->FillRect(Rect);
		StatusBar->Canvas->Font->Color = clWhite;
		StatusBar->Canvas->Font->Style = TFontStyles()<<fsBold;
	} // else - regular colors
	StatusBar->Canvas->TextOut(Rect.Left, Rect.Top, Panel->Text);
}
//---------------------------------------------------------------------------
void TFormTAHelper::StatusBarText(UnicodeString text, int mode)
{
	if(!text.Length())
		StatusBar1->Tag = 0;
	else
		StatusBar1->Tag = mode;
	StatusBar1->Panels->Items[0]->Text = text;
}
//---------------------------------------------------------------------------
void TFormTAHelper::StatusbarError(UnicodeString text, int mode)
{
	TimerError->Enabled = false;
	if(mode == 1)
		TimerError->Interval = 4000; // 1 - red error
	else if(mode == 2)
		TimerError->Interval = 2000; // 2 - green message
	// display bold message in statusbar
	Application->OnHint = DisplayHintDummy; // disable hints
	StatusBarText(text, mode);
	TimerError->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerErrorTimer(TObject *Sender)
{
	// deactivate statusbar message on timeout
	TimerError->Enabled = false;
	Application->OnHint = DisplayHint; // restore hints
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::StatusBar1Click(TObject *Sender)
{
	// deactivate and dismiss statusbar message on click
	if(TimerError->Enabled) TimerErrorTimer(this);
	StatusBarText(L"");
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::FormMouseDown(TObject *Sender,
			TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if(Button == mbLeft) {
		// move by mouse
		ReleaseCapture();
		SendMessage(Handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::Minimize1Click(TObject *Sender)
{
	Application->Minimize();
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::StaticTextTALdragMouseDown(TObject *Sender, TMouseButton Button,
					TShiftState Shift, int X, int Y)
{
	if(Button == mbLeft) {
		// who is dragging ?
		if (Sender == StaticTextWDdrag)
			m_DragMode = WD;
		else if (Sender == StaticTextSTRdrag)
			m_DragMode = SYSTRAN;
		else
			throw Exception(L"StaticTextTALdragMouseDown() invalid Sender");

		// start drag tracking
		m_pGetWinDrag->BeginDrag();
		TimerDrag->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::StaticTextTALdragMouseUp(TObject *Sender, TMouseButton Button,
					TShiftState Shift, int X, int Y)
{
	if(Button == mbLeft) {
		// stop drag tracking
		TimerDrag->Enabled = false;
		m_pGetWinDrag->EndDrag();

		if (m_DragMode == SYSTRAN) {
			HWND hw = m_pGetWinDrag->Handle();
			StaticTextSTRdrag->Tag = (int)hw;
			if (hw) {
				m_Systran->wndEditBoxSetText(hw, L"SYSTRAN: translation will appear here");
			}
		}

		// reset drag mode
		m_DragMode = NONE;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::Open1Click(TObject *Sender)
{
	// load options from selected file
	if(OpenDialog1->Execute()) {
		LoadOptions(OpenDialog1->FileName);
		LoadNames(false); // load local names AFTER UpdateConfigFileValue()
		UpdateNames(); // do not forget to update memory list
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::Save1Click(TObject *Sender)
{
	// save current options
	SaveOptions(m_ConfigFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::Saveas1Click(TObject *Sender)
{
	// save current options as different name
	if(SaveDialog1->Execute()) {
		SaveOptions(SaveDialog1->FileName);
		SaveNames(false); // save local names AFTER UpdateConfigFileValue()
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::UpDownCRnameFromClick(TObject *Sender, TUDBtnType Button)
{
	UpDownCRnameFrom->Tag = UpDownCRnameFrom->Tag?0:1;
	UpdateCRnamesFrom();
}
//---------------------------------------------------------------------------
void TFormTAHelper::UpdateCRnamesFrom()
{
	CheckBoxCRnames->Caption = UpDownCRnameFrom->Tag?L"Name at beginning":L"Name at the end";
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::Clearslotpos1Click(TObject *Sender)
{
	// clear current slot's window position
	ZeroMemory((void*)(m_Options.WDslots + ComboBoxWDslot->ItemIndex), sizeof(RECT));
	ComboBoxWDslotChange(this);
}
//---------------------------------------------------------------------------
UnicodeString TFormTAHelper::ProgramDir()
{
	UnicodeString ret(GetModuleName(NULL));
	return ret.SubString(1, ret.LastDelimiter(L"\\"));
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerTALcheckTimer(TObject *Sender)
{
	TimerTALcheck->Enabled = false;
	TALFind(false,true); // continuous checking TA window
	STRFind(); // continuous checking SYSTRAN window
	WKFind(); // continuous checking WaKan window
	Clock(); // update clock
	TimerTALcheck->Enabled = true;
}
//---------------------------------------------------------------------------
UnicodeString TFormTAHelper::ProgramVer()
{
	UnicodeString ret;
	DWORD hnd=0, vis=0;
	if(0 != (vis = GetFileVersionInfoSize(GetModuleName(NULL).w_str(), &hnd))) {
		BYTE *data = new BYTE[vis];
		if(GetFileVersionInfo(GetModuleName(NULL).w_str(), hnd, vis, data)) {
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
void __fastcall TFormTAHelper::SpeedButtonCRcopyClick(TObject *Sender)
{
	// copy displayed text
	UnicodeString text(MemoGetText(MemoCRcontents));
//	if(text.Length()) {
		SpeedButtonCRcopy->Tag = 1;
		if(!CopyToClipboard(text.w_str(), Handle)) {
			SpeedButtonCRcopy->Tag = 0;
			StatusbarError(L"Copy failed");
		}
//	} else {
//		StatusbarError(L"No text to copy");
//	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerAnimeTimer(TObject *Sender)
{
	static int len = SpeedButtonCRapply->HelpKeyword.Length(), disp = len;
	static bool forw = true;
	// stop and reinit
	if(TimerAnime->Tag) {
		TimerAnime->Enabled = false;
		TimerAnime->Tag = 0;
		TimerAnime->Interval = 200;
		disp = len;
		forw = true;
		return;
	}
	if(TimerAnime->Interval == 5000) TimerAnime->Interval = 200;
	if(disp >= len || disp == 0) forw = !forw;
	disp += forw ? 1 : -1;
	SpeedButtonCRapply->Caption = SpeedButtonCRapply->HelpKeyword.SubString(1, disp);
	if(disp >= len) TimerAnime->Interval = 5000;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerCRnotifyTimer(TObject *Sender)
{
	TimerCRnotify->Enabled = false;
	PanelCRnotify->Color = clBtnFace;
}
//---------------------------------------------------------------------------
void TFormTAHelper::CRnotify(TColor color)
{
	TimerCRnotify->Enabled = false;
	PanelCRnotify->Color = color;
	TimerCRnotify->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::ListBoxCRhistoryMeasureItem(TWinControl *Control, int Index,
					int &Height)
{
	int Offset = 4;   // default text offset
	TListBox *plist = dynamic_cast<TListBox *>(Control);
	TCanvas *pcanv = plist->Canvas;

	// measure text height
	pcanv->Font = plist->Font;
	RECT r;
	r.left = r.top = r.bottom = 0;
	r.right = plist->Width - Offset * 4;
	DrawText(pcanv->Handle, plist->Items->Strings[Index].w_str(), -1, &r, DT_CALCRECT|DT_WORDBREAK|DT_EXPANDTABS);
	Height = r.bottom + Offset * 2;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::ListBoxCRhistoryDrawItem(TWinControl *Control, int Index,
					TRect &Rect, TOwnerDrawState State)
{
	int Offset = 4;   // default text offset
	TListBox *plist = dynamic_cast<TListBox *>(Control);
	TCanvas *pcanv = plist->Canvas;

	// clear bacground
	pcanv->FillRect(Rect);
	// display text
	RECT r;
	r.left		= Rect.Left + Offset;
	r.right		= Rect.Right - Offset;
	r.top			= Rect.Top + Offset;
	r.bottom	= Rect.Bottom;
	DrawText(pcanv->Handle, plist->Items->Strings[Index].w_str(), -1, (LPRECT)&r, DT_WORDBREAK|DT_EXPANDTABS);
	// draw frame
	pcanv->Pen->Color = clSkyBlue;
	pcanv->Brush->Style = bsClear;
	pcanv->MoveTo(Rect.Left, Rect.Bottom-1);
	pcanv->LineTo(Rect.Right, Rect.Bottom-1);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonCRhistoryClick(TObject *Sender)
{
	// open processed text history
	CRopenHistory(0);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::RawTextHistory1Click(TObject *Sender)
{
	// open raw text history
	CRopenHistory(1);
}
//---------------------------------------------------------------------------
void TFormTAHelper::CRhistoryBackgroundControls(bool show)
{
	// hide/show annoing background controls
	EditCRrepetitionCharacterLen->Visible = show;
	UpDownCRrepetitionCharacterLen->Visible = show;
}
//---------------------------------------------------------------------------
void TFormTAHelper::CRopenHistory(int whathist)
{
	// hide annoing background controls
	CRhistoryBackgroundControls(false);
	// set history we want
	ListBoxCRhistory->Tag = whathist; // 0==processed; 1==raw text history
	ListBoxCRhistory->Hint = (ListBoxCRhistory->Tag == 0)? L"Processed text history (DoubleClick or Enter - select; Esc - exit)" : L"Raw text history (DoubleClick or Enter - select; Esc - exit)";
	ListBoxCRhistory->Items->Clear();
	ListBoxCRhistory->Visible = true;
	CRhistoryBoxUpdate((whathist==0)? m_HistoryScroll.pos : -1/*last entry (always for raw history)*/);
	ListBoxCRhistory->SetFocus();
}
//---------------------------------------------------------------------------
void TFormTAHelper::CRhistoryBoxUpdate(int itemindex)
{
	TList *pHistLst = (ListBoxCRhistory->Tag == 0)? m_CRhistoryList : m_CRhistoryRawList;
	ListBoxCRhistory->Items->BeginUpdate();
	ListBoxCRhistory->Items->Clear();
	// fill history listbox
	for(int i=0; i<pHistLst->Count; i++) {
		ListBoxCRhistory->Items->Add(*(UnicodeString*)pHistLst->Items[i]);
	}
	// add dummy entry
	if(!ListBoxCRhistory->Count) ListBoxCRhistory->Items->Add(TAH_EMPTY_HISTORY);
	// set selection
	ListBoxCRhistory->ItemIndex = (itemindex == -1)? ListBoxCRhistory->Count-1 : itemindex;
	ListBoxCRhistory->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void TFormTAHelper::CRhistoryBoxSelect()
{
	// history doubleclick - > select + close
	if(ListBoxCRhistory->ItemIndex != -1) {
		if(ListBoxCRhistory->Items->Strings[ListBoxCRhistory->ItemIndex] != TAH_EMPTY_HISTORY) {
			MemoCRcontents->Lines->Text = ListBoxCRhistory->Items->Strings[ListBoxCRhistory->ItemIndex];
			if (ListBoxCRhistory->Tag == 0) { // update (processed)history scroller
				m_HistoryScroll.pos = ListBoxCRhistory->ItemIndex;
      }
    }
	}
	ListBoxCRhistory->Visible = false;
	// show annoing background controls
	CRhistoryBackgroundControls(true);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::ListBoxCRhistoryDblClick(TObject *Sender)
{
	// history doubleclick -> select
	CRhistoryBoxSelect();
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::FormResize(TObject *Sender)
{
	// rebuild history listbox on form resize (kind of overkill, but looks better)
	if(ListBoxCRhistory->Visible) CRhistoryBoxUpdate(ListBoxCRhistory->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::ListBoxCRhistoryKeyPress(TObject *Sender, wchar_t &Key)
{
	// history keyboard
	if(Key == 27) { // Esc -> close
		ListBoxCRhistory->Visible = false;
		CRhistoryBackgroundControls(true); // show annoing background controls
	} else if(Key == 13) {  // Enter -> select
		CRhistoryBoxSelect();
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonCRreprocessClick(TObject *Sender)
{
	// reprocess last grabbed text
	SpeedButtonCRreprocess->Tag = 1;
	ProcessText(m_TextRaw);
	SpeedButtonCRreprocess->Tag = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::ProcessCurrentlyDisplayedText1Click(TObject *Sender)
{
	// process currently displayed text
	SpeedButtonCRreprocess->Tag = 1;
	ProcessText(MemoGetText(MemoCRcontents).w_str());
	SpeedButtonCRreprocess->Tag = 0;
}
//---------------------------------------------------------------------------
UnicodeString TFormTAHelper::MemoGetText(TMemo *Memo)
{
	UnicodeString text;
	if(Memo->SelLength)
		text = Memo->Lines->Text.SubString(Memo->SelStart+1, Memo->SelLength);
	else
		text = Memo->Lines->Text;
	text = text.Trim(); // UnicodeString's Trim() sucks -> not support japanese spaces
	return text;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::MemoCRcontentsKeyPress(TObject *Sender, wchar_t &Key)
{
	if(Key == 0x0001) { // Ctrl-A
		Key = 0;
		((TMemo*)Sender)->SelectAll();
	}
}
//---------------------------------------------------------------------------
bool IsWindowObscured(HWND hwnd)
{
	bool ret = false;
	HDC hdc = GetDC(hwnd);
	if(hdc) {
		RECT rcClip;
		int cb = GetClipBox(hdc, &rcClip);
		if(cb == NULLREGION) {
			ret = true;
		} else if(cb == ERROR) {
			throw Exception(L"TFormTAHelper::IsWindowObscured() GetClipBox() failed");
		}
		ReleaseDC(hwnd, hdc);
	} else
		throw Exception(L"TFormTAHelper::IsWindowObscured() GetDC() failed");
	return ret;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TrayIcon1Click(TObject *Sender)
{
	TrayIcon1->Visible = false;
	ShowWindow(Application->Handle, SW_RESTORE);
	BringWindowToFront(Handle);
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
HWND TFormTAHelper::ValidateSTRtarget()
{
	HWND hw = (HWND)StaticTextSTRdrag->Tag;
	if (hw && !IsWindow(hw)) {
		hw = NULL;
		StaticTextSTRdrag->Tag = 0;
	}
	return hw;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerSTRreceiveTimer(TObject *Sender)
{
	static int count = 0;
	TimerSTRreceive->Enabled = false;

	if (TimerSTRreceive->Tag) {
		TimerSTRreceive->Tag = 0;
		count = 0;
	}

	HWND target = ValidateSTRtarget();
	if (!target) { // no target - no point waiting
		m_Systran->StopReceiveTimer();
		return;
	}
	UnicodeString trans(m_Systran->ReceiveText());
	if (trans.Length()) { // got text - display it
		m_Systran->wndEditBoxSetText(target, trans.w_str());
		return;
	} else if (count >= SYSTRAN_TIMEOUT) { // no text and timeout
		m_Systran->StopReceiveTimer();
		m_Systran->wndEditBoxSetText(target, L"SYSTRAN: timed out");
		return;
	}
	count++;

	TimerSTRreceive->Enabled = true;
}
//---------------------------------------------------------------------------
UnicodeString GetWindowTextPlus(HWND hwnd)
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
UnicodeString GetWindowTitlePlus(HWND hwnd)
{
	wchar_t *tbuf = new wchar_t[512]; tbuf[0] = 0;
	GetWindowText(hwnd, tbuf, 512);
	UnicodeString ret(tbuf);
	delete []tbuf;
	return ret;
}
//---------------------------------------------------------------------------
struct tEnumGameData {
	TPopupMenu *pmenu;
	bool start;
	tEnumGameData(): pmenu(NULL), start(false) {}
};
void __fastcall TFormTAHelper::PopupMenuFormPopup(TObject *Sender)
{
	PopupMenuForm->Items->Items[3]->Caption = UnicodeString(L"Save ") + FileFromPath(m_ConfigFile);

	// remove "windows" items from menu
	int gameItemIndex = 6, windowsStartFromIndex = 5;
	int cnt = PopupMenuForm->Items->Items[gameItemIndex]->Count - windowsStartFromIndex;
	for (int i = 0; i < cnt; i++)
		PopupMenuForm->Items->Items[gameItemIndex]->Items[windowsStartFromIndex]->Free();
	// enum game-like windows and fill menu
	tEnumGameData edata;
	edata.pmenu = PopupMenuForm;
	edata.start = true;
	EnumWindows((WNDENUMPROC)EnumGameWindowsProc, (LPARAM)&edata);
}
//---------------------------------------------------------------------------
BOOL CALLBACK EnumGameWindowsProc(HWND hwnd, LPARAM lParam)
{
	static int cnt = 0;
	tEnumGameData *pdata = (tEnumGameData*)lParam;
	if (pdata->start) {
		pdata->start = false;
		cnt = 0;
	}
	if (IsWindowVisible(hwnd)) {
		UnicodeString text(GetWindowTitlePlus(hwnd));
		// only get visible windows with title text
		// excluding no-game windows
		if (text.Length() &&
				text != L"Program Manager" &&
				text != L"TF_FloatingLangBar_WndTitle" &&
				text != TAH_TITLE_BACKCOVER &&
				!FormTAHelper->AmongWDtitles(text) &&
				!text.Pos(TAH_APP_TITLE) &&
				!text.Pos(L"ATLAS") &&
				!text.Pos(L"WinPick") &&
				!text.Pos(L"Translation Aggregator") &&
				!text.Pos(L"AGTH") &&
				!text.Pos(L"SYSTRAN") &&
				!text.Pos(L"Mozilla Firefox") &&
				!text.Pos(L"Internet Explorer") ) {
			TMenuItem *NewItem = new TMenuItem(FormTAHelper); // The owner will cleanup these menu items.
			NewItem->Caption = text;
			NewItem->Name = pdata->pmenu->Name + L"ItemWin" + IntToStr(cnt);
			NewItem->OnClick = FormTAHelper->PopupMenuItemWinClick;
			NewItem->Hint = L"Click on game here";
			NewItem->GroupIndex = 1;
			NewItem->RadioItem = true;
			NewItem->Tag = (int)hwnd; // store window
			if (pdata->pmenu->Tag == (int)hwnd)
				NewItem->Checked = true;
			pdata->pmenu->Items->Items[6]->Add(NewItem);
			cnt++;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK BackgroundWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_KEYDOWN:
			if (wParam == 27) // Esc - close
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------
HWND TFormTAHelper::CreateBackgroundWindow(HWND hwnd)
{
	HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi)) return NULL;
	return CreateWindow(TAH_CLASS_BACKCOVER,
											TAH_TITLE_BACKCOVER,
											WS_POPUP,
											mi.rcWork.left,
											mi.rcWork.top,
											mi.rcWork.right - mi.rcWork.left,
											mi.rcWork.bottom - mi.rcWork.top,
											NULL, NULL, HInstance, 0);
}
//---------------------------------------------------------------------------
int GetClientRectPlus(HWND hwnd, RECT *prect)
{
	GetClientRect(hwnd, prect);
	int menuheight = 0;
	if (GetMenu(hwnd)) {
		menuheight = GetSystemMetrics(SM_CYMENU);
		prect->bottom += menuheight;
	}
	return menuheight;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::PopupMenuItemWinClick(TObject *Sender)
{
	TMenuItem *pitem = (TMenuItem*)Sender;
	pitem->GetParentMenu()->Tag = pitem->Tag; // store selected window (for checking his menu item later)
	HWND hwnd = (HWND)pitem->Tag; // selected window

	// get monitor rect
	HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi)) throw Exception(L"TFormTAHelper::PopupMenuItemWinClick() GetMonitorInfo() failed");

	// get client area dimensions
	RECT winrect; ZeroMemory((void*)&winrect, sizeof(RECT));
	int menuheight = GetClientRectPlus(hwnd, &winrect);
	winrect.right = winrect.right - winrect.left;
	winrect.bottom = winrect.bottom - winrect.top;
	winrect.left = 0;
	winrect.top = 0;

	// calc left offset
	int loffset = 0;
	if (pitem->GetParentMenu()->Items->Items[6]->Items[2]->Checked) // top-center
		loffset = (mi.rcWork.right - mi.rcWork.left) / 2 - winrect.right / 2;

	// background cover
	HWND hwback = FindWindow(TAH_CLASS_BACKCOVER, NULL);
	if (pitem->GetParentMenu()->Items->Items[6]->Items[0]->Checked) {
		if (!hwback) {
			hwback = CreateBackgroundWindow(hwnd); // monitor same as game
		}
		SetWindowPos(hwback, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	} else {
		if (hwback) PostMessage(hwback, WM_CLOSE, 0, 0);
	}

	if (pitem->GetParentMenu()->Items->Items[6]->Items[3]->Checked) {
		// safe mode
		ShowWindow(hwnd, SW_HIDE);
		RECT r; GetWindowRect(hwnd, &r);
		r.bottom = r.bottom - r.top - winrect.bottom; // title bar height
		SetWindowPos(	hwnd, HWND_NOTOPMOST, mi.rcWork.left + loffset,
									mi.rcWork.top - menuheight - r.bottom + GetSystemMetrics(SM_CYFRAME),
									0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	} else {
		// remove borders and place window
		ShowWindow(hwnd, SW_HIDE);
		LONG wstyle = GetWindowLong(hwnd, GWL_STYLE);
		wstyle &= (~WS_CAPTION);
		SetWindowLong(hwnd, GWL_STYLE, wstyle);
		AdjustWindowRect(&winrect, wstyle, FALSE);
		SetWindowPos(hwnd, HWND_NOTOPMOST, mi.rcWork.left + loffset, mi.rcWork.top - menuheight, winrect.right, winrect.bottom, SWP_FRAMECHANGED|SWP_SHOWWINDOW);
	}
}
//---------------------------------------------------------------------------
void TFormTAHelper::Clock()
{
	LabelClock->Caption = TDateTime::CurrentTime().TimeString();
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::UpDownCRallNamesClick(TObject *Sender, TUDBtnType Button)
{
	UpDownCRallNames->Tag = UpDownCRallNames->Tag ? 0 : 1;
	UpdateCRallNames();
}
//---------------------------------------------------------------------------
void TFormTAHelper::UpdateCRallNames()
{
	CheckBoxCRreplaceAllNames->Caption = UpDownCRallNames->Tag ? L"Replace all names" : L"Remove all names";
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::PanelCRhistoryScrollMouseEnter(TObject *Sender)
{
	m_HistoryScroll.active = true;
}
void __fastcall TFormTAHelper::PanelCRhistoryScrollMouseLeave(TObject *Sender)
{
	m_HistoryScroll.active = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::FormMouseWheelUp(TObject *Sender, TShiftState Shift,
					TPoint &MousePos, bool &Handled)
{
	// history up
	if (m_HistoryScroll.active && !TimerCRhistoryScroll->Enabled) {
		HistoryScroll(true);
	}
}
void __fastcall TFormTAHelper::FormMouseWheelDown(TObject *Sender, TShiftState Shift,
					TPoint &MousePos, bool &Handled)
{
	// history down
	if (m_HistoryScroll.active && !TimerCRhistoryScroll->Enabled) {
		HistoryScroll(false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::PanelCRhistoryScrollClick(TObject *Sender)
{
	if (m_HistoryScroll.y < PanelCRhistoryScroll->Height / 2 - PanelCRhistoryScroll->Font->Size / 2) {
		HistoryScroll(true);
	} else if (m_HistoryScroll.y > PanelCRhistoryScroll->Height / 2 + PanelCRhistoryScroll->Font->Size / 2) {
		HistoryScroll(false);
	}
}
void __fastcall TFormTAHelper::PanelCRhistoryScrollMouseMove(TObject *Sender, TShiftState Shift,
					int X, int Y)
{
	m_HistoryScroll.y = Y;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerCRhistoryScrollTimer(TObject *Sender)
{
	TimerCRhistoryScroll->Enabled = false;
	PanelCRhistoryScroll->VerticalAlignment = taVerticalCenter;
	PanelCRhistoryScroll->Color = clBtnFace;
}
//---------------------------------------------------------------------------
void TFormTAHelper::HistoryScroll(bool up)
{
	TimerCRhistoryScroll->Enabled = false;
	int tpos=0;

	if (up) {
		tpos = m_HistoryScroll.pos - 1;
		PanelCRhistoryScroll->VerticalAlignment = taAlignTop;
	} else {
		tpos = m_HistoryScroll.pos + 1;
		PanelCRhistoryScroll->VerticalAlignment = taAlignBottom;
	}

	if (tBETWEEN(0, tpos, m_CRhistoryList->Count - 1)) {
		m_HistoryScroll.pos = tpos;
		MemoCRcontents->Lines->Text = *(UnicodeString*)m_CRhistoryList->Items[tpos];
	} else {
		PanelCRhistoryScroll->Color = (TColor)0x006C6CFF;
	}

	TimerCRhistoryScroll->Enabled = true;
}
//---------------------------------------------------------------------------
void TFormTAHelper::WDrestoreAll()
{
	// restore all detached windows posotions
	m_WDwindows[0].handle = (HWND)LabelTALstatus->Tag; // TA main window
	for (int i=0; i<TOPT_WDSLOTS; i++) {
		HWND hw = m_WDwindows[i].handle;
		RECT r = m_Options.WDslots[i];
		if (hw != NULL && IsWindow(hw) && !tRECTEMPTY(r)) {
			MoveWindow(hw, r.left, r.top, r.right-r.left+1, r.bottom-r.top+1, TRUE);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::AutohideTaskbar1Click(TObject *Sender)
{
	TMenuItem *pitem = (TMenuItem*)Sender;
	SetAutohideTaskbar(pitem->Checked?1:0);
}

//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::CheckBoxWKcompactClick(TObject *Sender)
{
	m_Wakan->CompactControl(CheckBoxWKcompact->Checked?1:0);
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonWKsavePosClick(TObject *Sender)
{
	if (m_Wakan->FindWakan()) {
		GetWindowRect(m_Wakan->GetRoot(), &(m_Options.WKpos));
	} else {
		StatusbarError(L"WaKan not found");
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::SpeedButtonWKrestorePosClick(TObject *Sender)
{
	if (m_Wakan->FindWakan()) {
		if (!tRECTEMPTY(m_Options.WKpos)) {
			SetWindowPos(m_Wakan->GetRoot(), HWND_NOTOPMOST,
										m_Options.WKpos.left, m_Options.WKpos.top,
										m_Options.WKpos.right - m_Options.WKpos.left + 1,
										m_Options.WKpos.bottom - m_Options.WKpos.top + 1, SWP_NOACTIVATE);
		}
	} else {
		StatusbarError(L"WaKan not found");
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::TimerWKclassTimer(TObject *Sender)
{
	TimerWKclass->Enabled = false;

	bool safe = m_Wakan->OnTimer();

	TimerWKclass->Tag = TimerWKclass->Tag + 1;
	// in case of patched exe, skip selection of all text before translation
	if (TimerWKclass->Tag == 3 && CheckBoxWKpatched->Checked) {
		TimerWKclass->Tag = 4;
	}

	if (safe && TimerWKclass->Tag < 7)
		TimerWKclass->Enabled = true;
}

//---------------------------------------------------------------------------
// show / hide panels
#define TAH_VIEW_PANEL_SPACE 2
void __fastcall TFormTAHelper::WaKan1Click(TObject *Sender)
{
	int space = 0;
	int top = ClientHeight - StatusBar1->Height;

	bool vis = PopupMenuForm->Items->Items[9]->Items[3]->Checked;
	GroupBoxWD->Visible = vis;
	LabelClock->Visible = vis;
	ImageFurniture->Visible = vis;
	if (vis) {
		top -= GroupBoxWD->Height + space;
		GroupBoxWD->Top = top;
		LabelClock->Top = top + (GroupBoxWD->Height/2 - LabelClock->Height/2);
		ImageFurniture->Top = top + (GroupBoxWD->Height - ImageFurniture->Height);
		space = TAH_VIEW_PANEL_SPACE;
	}

	vis = PopupMenuForm->Items->Items[9]->Items[2]->Checked;
	GroupBoxHK->Visible = vis;
	GroupBoxFMW->Visible = vis;
	if (vis) {
		top -= GroupBoxHK->Height + space;
		GroupBoxHK->Top = top;
		GroupBoxFMW->Top = top;
		space = TAH_VIEW_PANEL_SPACE;
	}

	vis = PopupMenuForm->Items->Items[9]->Items[1]->Checked;
	GroupBoxTA->Visible = vis;
	GroupBoxSTR->Visible = vis;
	if (vis) {
		top -= GroupBoxTA->Height + space;
		GroupBoxTA->Top = top;
		GroupBoxSTR->Top = top;
		space = TAH_VIEW_PANEL_SPACE;
	}

	vis = PopupMenuForm->Items->Items[9]->Items[0]->Checked;
	GroupBoxWK->Visible = vis;
	if (vis) {
		top -= GroupBoxWK->Height + space;
		GroupBoxWK->Top = top;
		space = TAH_VIEW_PANEL_SPACE;
	}

	GroupBoxCR->Height = top;
}

//---------------------------------------------------------------------------
// SYSTRAN translation
void TFormTAHelper::SystranTranslate(bool fromTA)
{
	// send text to SYSTRAN
	HWND target = ValidateSTRtarget();
	if (target) {
		m_Systran->Translate(Handle, fromTA, target);
	}
}

//---------------------------------------------------------------------------
// Redraw detached windows
void __fastcall TFormTAHelper::TimerWDredrawTimer(TObject *Sender)
{
	TimerWDredraw->Enabled = false;
	for(int i=0; i < m_StrayWinList->Count; i++) {
		HWND hw = (HWND)m_StrayWinList->Items[i];
		if (IsWindow(hw)) {
			SetWindowPos(hw, HWND_NOTOPMOST, 0,0,0,0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_NOCOPYBITS|SWP_FRAMECHANGED|SWP_ASYNCWINDOWPOS);
		}
	}
}

//---------------------------------------------------------------------------
// Make Taskbar visible/invisible
// Sender == 0 : called on application exit to force show
// Sender == 1 : called from code to restore current state
// Sender == TMenuItem : called on menu item click to toggle
void __fastcall TFormTAHelper::ToggleTaskbarVisibility1Click(TObject *Sender)
{
	HWND hWnd = FindWindow(L"Shell_traywnd", NULL);
	if (hWnd) {
		if (!Sender) { // force show taskbar on exit
			if (!IsWindowVisible(hWnd))
				ShowWindow(hWnd, SW_SHOW);
		} else {   // toggle taskbar visibility
			if ((bool)IsWindowVisible(hWnd) != (bool)ToggleTaskbarVisibility1->Checked) {
				ShowWindow(hWnd, ToggleTaskbarVisibility1->Checked? SW_SHOW : SW_HIDE);
			}
			// prevent TAHelper losing focus on taskbar appearance when menu item is clicked
			if (Sender != (TObject*)1 && ToggleTaskbarVisibility1->Checked)
				BringWindowToFront(Handle);
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormTAHelper::MemoHKhotkeysContextPopup(TObject *Sender, TPoint &MousePos,
					bool &Handled)
{
	Handled = true;
	FormTAHelper->PopupMenuForm->Popup(FormTAHelper->Left, FormTAHelper->Top);
}

//---------------------------------------------------------------------------



