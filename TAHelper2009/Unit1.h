//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>

#include <shlwapi.h>
#include <Graphics.hpp>

#include "Wakan.h"
#include "WindowUtils.h"

//---------------------------------------------------------------------------
#define TAH_APP_TITLE				L"TAHelper"
#define TAH_REGKEY					L"Software\\TAHelper"  // obsolete, replaced by .ini file
#define TAH_FILE_NAMES			L"names.txt"
#define TAH_MAX_HISTORY			200
#define TAH_EMPTY_HISTORY		L"History is empty"
#define TAH_CLASS_BACKCOVER	L"TAHBackgroundCoverWindow"
#define TAH_TITLE_BACKCOVER	L"Background Cover"

#define TAHPLUGIN_WNDCLASS_MESSAGE L"TAHPluginMessageWindow"

//---------------------------------------------------------------------------
#define TA_WCLASS				L"Translation Aggregator Main Window"
#define TA_DRAGW_CLASS	L"Translation Aggregator Drag Window"

#define WMA_TRANSLATE_ALL			(WM_APP + 0x11)
#define ID_VIEW_HIDETOOLBAR             40046
#define ID_VIEW_HIDEWINDOWFRAME         40047
#define ID_LAYOUT_0                     40026
#define ID_LAYOUT_1                     40027
#define ID_LAYOUT_2                     40028
#define ID_LAYOUT_3                     40029
#define ID_LAYOUT_4                     40030
#define ID_LAYOUT_5                     40031
#define ID_LAYOUT_6                     40032
#define ID_LAYOUT_7                     40033
#define ID_LAYOUT_8                     40034
#define ID_LAYOUT_9                     40035

//---------------------------------------------------------------------------
struct tOptionsGlobal {
	int WDforceRedraw;
	int WDredrawDelay;
	int CRforceJpnClipbrdLocale;

	tOptionsGlobal() {
		WDforceRedraw = 1;
		WDredrawDelay = 200;
		CRforceJpnClipbrdLocale = 1;
	}
};
//---------------------------------------------------------------------------
#define TOPT_WDSLOTS 10
struct tOptions {
	int Left, Top, Width, Height;
	int Topmost;
	int NamesSplitterPos;

	int CRenable;
	int CRremoveLinebreaks;
	int CRseparateLinebreaks;
	int CRsuppressCharRepetition;
	int CRcharRepetitionNumber;
	int CRcharRepetitionLen;
	int CRnames; // process edge names
	int CRnamesFrom; // from beginning/end
	int CRnamesRemove;
	int CRnamesSeparate;
	int CRnamesSeparateBy;
	int CRnameSearchDeviation;
	int CRreplaceAllNames;
	int CRreplaceAllNamesWhat; // replace/remove all names
	int CRremoveStrings;
	UnicodeString CRremoveStringsWhat;
	int CRtimeShift;
	int CRtimeShiftNumber;

	int FSenable;
	int FSnumberOfLines;

	int HKenable;

	int WDslot;
	RECT WDslots[TOPT_WDSLOTS];

	int MMdisplayBackgroung;
	int MMtopLeft;
	int MMtopCenter;
	int MMsafeMode;
	int MMautohideTaskbar;
	int MMviewPanelWK;
	int MMviewPanelTA;
	int MMviewPanelHK;
	int MMviewPanelWD;

	int WKenable;
	int WKcompact;
	RECT WKpos;
	int WKpatched;

	int STRtextFromTA;

	tOptions() {
		Left = Top = 10;
		Width = 540;
		Height = 450;
		Topmost = 1;
		NamesSplitterPos = 181;

		CRenable = 0;
		CRremoveLinebreaks = 0;
		CRseparateLinebreaks = 0;
		CRsuppressCharRepetition = 0;
		CRcharRepetitionNumber = 2;
		CRcharRepetitionLen = 3;
		CRnames = 0; // process edge names
		CRnamesFrom = 1; // from beginning/end
		CRnamesRemove = 1;
		CRnamesSeparate = 0;
		CRnamesSeparateBy = 0;
		CRnameSearchDeviation = 0;
		CRreplaceAllNames = 0;
		CRreplaceAllNamesWhat = 1; // replace/remove all names
		CRremoveStrings = 0;
		CRremoveStringsWhat = L"";
		CRtimeShift = 0;
		CRtimeShiftNumber = 1;

		FSenable = 0;
		FSnumberOfLines = 1;

		HKenable = 0;

		WDslot = 0;
		ZeroMemory((void*)WDslots, sizeof(RECT)*TOPT_WDSLOTS);

		MMdisplayBackgroung = 1;
		MMtopLeft = 0;
		MMtopCenter = 1;
		MMsafeMode = 0;
		MMautohideTaskbar = 0;
		MMviewPanelWK = 1;
		MMviewPanelTA = 1;
		MMviewPanelHK = 1;
		MMviewPanelWD = 1;

		WKenable = 0;
		WKcompact = 0;
		ZeroMemory((void*)&WKpos, sizeof(RECT));
		WKpatched = 0;

		STRtextFromTA = 1;
	}
};
//==================================================================
// Old configuration struct for importing old config files (*.cfg)
// DO NOT MODIFY !!!
//==================================================================
#define TOPT_WDSLOTS0 10
struct tOptions0 {
	int Left, Top, Width, Height;
	int Topmost;
	int NamesSplitterPos;
	int CRenable;
	int CRremoveLinebreaks;
	int CRseparateLinebreaks;
	int CRsuppressCharRepetition;
	int CRcharRepetitionNumber;
	int CRnames;
	int CRnamesFrom;
	int CRnamesRemove;
	int CRnamesSeparate;
	int CRnamesSeparateBy;
	int CRnameSearchDeviation;
	int CRreplaceAllNames;
	int CRreplaceAllNamesWhat;
	int CRremoveStrings;
	int FSenable;
	int FSnumberOfLines;
	int HKenable;
	int WDslot;
	RECT WDslots[TOPT_WDSLOTS0];
	tOptions0() {
		ZeroMemory((void*)this, sizeof(tOptions0));
		Topmost	= 1;
		NamesSplitterPos = 181;
		CRcharRepetitionNumber = 2;
		CRnamesFrom = 1;
		CRnamesRemove = 1;
		CRreplaceAllNamesWhat = 1;
		FSnumberOfLines = 1;
	}
};

//---------------------------------------------------------------------------
#define tBETWEEN(x,y,z)	(((y)>=(x))&&((y)<=(z)))
#define tRECTEMPTY(r)		(((r).left==0)&&((r).top==0)&&((r).right==0)&&((r).bottom==0))

#define STR_CRLF		L"\r\n"
#define CHR_CR			((wchar_t*)"\r\0")
#define CHR_LF			((wchar_t*)"\n\0")
#define CHR_TAB			((wchar_t*)"\t\0")
#define CHR_SPACE		((wchar_t*)" \0")
#define CHR_JSPACE	((wchar_t*)"\0\x30")
//#define CHR_JDOT_MARU			((wchar_t*)"\x2\x30")
//#define CHR_JSQBR_THICK_L	((wchar_t*)"\xE\x30")
//#define CHR_JSQBR_THICK_R	((wchar_t*)"\xF\x30")
//#define CHR_JSQBR_THIN_L	((wchar_t*)"\xC\x30")
//#define CHR_JSQBR_THIN_R	((wchar_t*)"\xD\x30")
//#define CHR_JSQBR_SQTHICK_L	((wchar_t*)"\x10\x30")
//#define CHR_JSQBR_SQTHICK_R	((wchar_t*)"\x11\x30")

//---------------------------------------------------------------------------
class tWString
{
public:
	// subclass for symbols checking (initialization at the end of tWString)
	class tSymbols {
	private:
		static tWString punctuation;
		static tWString spacelike;
	public:
		static wchar_t* w_punct() { return punctuation.w_str(); }
		static wchar_t* w_space() { return spacelike.w_str(); }
		static bool IsPunctuation(wchar_t ch) { return (punctuation.Find(ch) != -1); }
		static bool IsSpacelike(wchar_t ch) { return (spacelike.Find(ch) != -1); }
	};

private:
	wchar_t *string;
	int length;

	void Init() {
		length = 0;
		string = (wchar_t*)malloc(sizeof(wchar_t));
		if(!string) throw Exception(L"tWString::Init() malloc() failed");
		*string = 0;
	}

public:
	tWString() {
		Init();
	}
	tWString(wchar_t *str) {
		Init();
		operator=(str);
	}
	tWString(wchar_t ch) {
		Init();
		wchar_t str[2] = {ch, 0};
		operator=(str);
	}
	tWString(const tWString &str) {
		Init();
		operator=(str);
	}
	~tWString() {
		if(!string) throw Exception(L"tWString::~tWString() invalid string");
		free(string);
	}
//---------------------------------------------------------------------------
	void Clear() {
		if(!string) throw Exception(L"tWString::Clear() invalid string");
		free(string);
		Init();
	}
//---------------------------------------------------------------------------
	int Length() const { return length; }
	int BufferSizeByte() const { return (length+1)*sizeof(wchar_t); }
	int BufferSizeWChar() const { return length+1; }
	wchar_t* w_str() const { return string; }
	operator wchar_t *() const { return string; }
//---------------------------------------------------------------------------
  wchar_t& operator[](int index) const {
		if(!tBETWEEN(0, index, length-1)) throw Exception(L"tWString::operator[] index out of range");
    return string[index];
  }
//---------------------------------------------------------------------------
	tWString& operator=(const wchar_t *str) {
		if(!str) throw Exception(L"tWString::operator=(wchar_t*) invalid str");
		length = wcslen(str);
		// allocate new string
		wchar_t *buf = (wchar_t*)malloc((length+1)*sizeof(wchar_t));
		if(!buf) throw Exception(L"tWString::operator+=(wchar_t*) malloc() failed");
		// copy string-to-assign to new string
		wcscpy(buf, str);
		// free old string and assign new
		if(!string) throw Exception(L"tWString::operator=(wchar_t*) invalid string");
		free(string);
		string = buf;
  	return *this;
  }
//---------------------------------------------------------------------------
	tWString& operator=(const tWString &str) {
		return operator=((wchar_t*)str);
	}
//---------------------------------------------------------------------------
	tWString& operator=(const tWString *pstr) {
		return operator=(pstr->w_str());
	}
//---------------------------------------------------------------------------
	bool operator==(const tWString &str) const {
		return (0 == wcscmp(string, (wchar_t*)str));
  }
//---------------------------------------------------------------------------
	bool operator!=(const tWString &str) const {
		return (0 != wcscmp(string, (wchar_t*)str));
	}
//---------------------------------------------------------------------------
	void Delete(int pos, int cnt) {
		if(!tBETWEEN(0, pos, length-1)) throw Exception(L"tWString::Delete() pos out of range");
    if(pos+cnt > length) throw Exception(L"tWString::Delete() invalid cnt");
    if(pos+cnt == length) { // delete until the end
    	string[pos] = 0;
			length = pos;
    } else { // delete in the middle
    	memmove((void*)(string+pos), (void*)(string+pos+cnt), (length-pos-cnt+1) * sizeof(wchar_t));
      length -= cnt;
    }
		if(length != (int)wcslen(string)) throw Exception(L"tWString::Delete() invalid length");
	}
//---------------------------------------------------------------------------
	int Find(const wchar_t *str, int start=0, wchar_t **fnd=NULL) const {
		int ret = -1; // not found by default
		if(!str) throw Exception(L"tWString::Find() invalid str");
		if(!wcslen(str)) throw Exception(L"tWString::Find() empty str");
		if(!length) return ret; // nowhere to find
		if(!tBETWEEN(0, start, length-1)) throw Exception(L"tWString::Find() start out of range");
		wchar_t *p = wcsstr(string+start, str); // find from start
		if(p) {
			if(fnd) *fnd = p;
			ret = (int)(p-string);
		}
		return ret;
	}
//---------------------------------------------------------------------------
// NOT USED
// find first occurence of chars
	int FindFirstOf(wchar_t *str, int start=0) const {
		int ret = -1, cur = -1, len = wcslen(str);
		if(!len) throw Exception(L"tWString::FindFirstOf() empty str");
		for (int i=0; i<len; i++) {
			cur = Find(str[i], start);
			if (cur > 0 && (ret == -1 || cur < ret))
				ret = cur;
		}
		return ret;
	}
//---------------------------------------------------------------------------
	int Find(wchar_t ch, int start=0, wchar_t **fnd=NULL) const {
		wchar_t str[2] = {ch, 0};
		return Find(str, start, fnd);
	}
//---------------------------------------------------------------------------
	int Find(const tWString &str, int start=0, wchar_t **fnd=NULL) const {
		return Find((wchar_t*)str, start, fnd);
	}
//---------------------------------------------------------------------------
	int FindReverse(const wchar_t *str, wchar_t **fnd=NULL) const {
		int ret = -1; // not found by default
		if(!str) throw Exception(L"tWString::Find() invalid str");
		int sl = wcslen(str);
		if(!sl) throw Exception(L"tWString::Find() empty str");
		if(!length || length < sl) return ret; // impossible to find
		wchar_t *p = StrRStrIW(string, NULL, str); // find from the end
		if(p) {
			if(fnd) *fnd = p;
			ret = (int)(p-string);
		}
		return ret;
	}
//---------------------------------------------------------------------------
	int FindReverse(wchar_t ch, wchar_t **fnd=NULL) const {
		wchar_t str[2] = {ch, 0};
		return FindReverse(str, fnd);
	}
//---------------------------------------------------------------------------
	int FindReverse(const tWString &str, wchar_t **fnd=NULL) const {
		return FindReverse((wchar_t*)str, fnd);
	}
//---------------------------------------------------------------------------
	tWString SubString(int pos, int cnt) const {
		if(!tBETWEEN(0, pos, length-1)) throw Exception(L"tWString::SubString() pos out of range");
		if(cnt < 0 || pos+cnt > length) throw Exception(L"tWString::SubString() invalid cnt");
    wchar_t *buff = new wchar_t[cnt+1];
    memmove((void*)buff, (void*)(string+pos), cnt*sizeof(wchar_t));
    buff[cnt] = 0;
		tWString ret(buff);
    delete []buff;
    return ret;
  }
//---------------------------------------------------------------------------
	void DeleteAll(const wchar_t *str) {
		if(!str) throw Exception(L"tWString::DeleteAll() invalid str");
    int seek = 0;
    int sl = wcslen(str);
		if(!sl) throw Exception(L"tWString::DeleteAll() empty str");
		while(true) {
    	int fi = Find(str, seek);
    	if(fi >= 0) {
				Delete(fi, sl);
				seek = fi;
				if (seek >= Length()) break;
			} else
      	break;
		}
	}
//---------------------------------------------------------------------------
	void DeleteAll(const tWString &str) {
  	DeleteAll((wchar_t*)str);
  }
//---------------------------------------------------------------------------
	void DeleteAll(wchar_t ch) {
  	wchar_t str[2] = {ch, 0};
  	DeleteAll(str);
  }
//---------------------------------------------------------------------------
	void Pack() {
		string = (wchar_t*)realloc((void*)string, (length+1)*sizeof(wchar_t));
		if(!string) throw Exception(L"tWString::Pack() realloc() failed");
	}
//---------------------------------------------------------------------------
	void TrimL(bool punctuation = false) {
		while(length &&
					(tSymbols::IsSpacelike(string[0]) ||
					(punctuation && tSymbols::IsPunctuation(string[0])))) {
			Delete(0 , 1);
		}
	}
//---------------------------------------------------------------------------
	void TrimR(bool punctuation = false) {
		while(length &&
					(tSymbols::IsSpacelike(string[length-1]) ||
					(punctuation && tSymbols::IsPunctuation(string[length-1])))) {
			Delete(length-1 , 1);
		}
	}
//---------------------------------------------------------------------------
	void Trim(bool punctuation = false) {
		TrimL(punctuation);
		TrimR(punctuation);
	}
//---------------------------------------------------------------------------
	tWString& operator+=(const wchar_t *str) {
		if(!str) throw Exception(L"tWString::operator+=(wchar_t*) invalid str");
		int sl = wcslen(str);
		if(sl > 0) {
			// copy string-to-add to temp buffer
			wchar_t *buf = (wchar_t*)malloc((sl+1)*sizeof(wchar_t));
			if(!buf) throw Exception(L"tWString::operator+=(wchar_t*) malloc() failed");
			wcscpy(buf, str);
			// increase current string buffer
			string = (wchar_t*)realloc((void*)string, BufferSizeByte() + sl*sizeof(wchar_t));
			if(!string) throw Exception(L"tWString::operator+=(wchar_t*) realloc() failed");
			// append string-to-add
			memmove((void*)(string+length), (void*)buf, (sl+1)*sizeof(wchar_t));
			// increase length
			length += sl;
			if(length != (int)wcslen(string)) throw Exception(L"tWString::operator+=(wchar_t*) invalid length");
			// free temp buffer
			free(buf);
		}
		return *this;
	}
//---------------------------------------------------------------------------
	tWString& operator+=(const tWString &str) {
		return operator+=((wchar_t*)str);
	}
//---------------------------------------------------------------------------
	tWString operator+(const wchar_t *str) {
		return tWString(string) += str;
	}
//---------------------------------------------------------------------------
	tWString operator+(const tWString &other) {
		return tWString(string) += other;
	}
//---------------------------------------------------------------------------
	int Replace(const wchar_t *str, const wchar_t *repl, int cnt=0) {
		if(!str || !wcslen(str)) throw Exception(L"tWString::Replace(wchar_t*) invalid str");
		if(!repl) throw Exception(L"tWString::Replace(wchar_t*) invalid repl");
		if(cnt<0) throw Exception(L"tWString::Replace(wchar_t*) invalid cnt");
		int ret=0, seek=0;
		int strln = wcslen(str);
		for(int i=0; cnt==0/*all occurrences*/ || i<cnt; i++) {
			int fi = Find(str, seek);
			if(fi >= 0) { // found
				ret++; // number of occurrences
				tWString trail;
				int tv = fi+strln;
				if(tv < length) trail = SubString(tv, length-fi-strln);
					else
						if(tv == length) tv = -1;
							else
								if(tv > length) throw Exception(L"tWString::Replace(wchar_t*) fi+strln out of range, possible bug in tWString::Find()");
				operator=(SubString(0, fi));
				operator+=(repl);
				if(tv == -1) break; // end of string reached, no need to seek further
				seek = length;
				operator+=(trail);
			} else
				break;
		}
		if(length != (int)wcslen(string)) throw Exception(L"tWString::Replace(wchar_t*) invalid length");
		return ret;
	}
//---------------------------------------------------------------------------
	int Replace(const tWString &str, const tWString &repl, int cnt=0) {
		return Replace((wchar_t*)str, (wchar_t*)repl, cnt);
	}
};
// tSymbols initialization
tWString tWString::tSymbols::punctuation	= L"`~!@#$%^&*()_-+=[{]}\\|;:\'\",<.>/\?‘～！＠＃＄％＾＆＊（）＿－＋＝「｛」｝￥｜；：’”、＜。＞・？『』《》【】〔〕";
tWString tWString::tSymbols::spacelike		= L"\r\n\t 　";

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class WMark
{
private:
	HDC m_hDC;
	RECT m_Rect;
	bool m_Visible;
	TPen* m_pPen;
	TBrush* m_pBrush;
public:
//------------------------------------------------------------
	WMark()
	{
		m_hDC = GetDC(NULL);
    ZeroMemory((void*)&m_Rect, sizeof(RECT));
    m_Visible = false;
    m_pPen = new TPen();
		m_pPen->Width = 5;
    m_pPen->Color = clGreen;
    SelectObject(m_hDC, m_pPen->Handle);
    m_pBrush = new TBrush();
    m_pBrush->Style = bsClear;
		SelectObject(m_hDC, m_pBrush->Handle);
    SetROP2(m_hDC, R2_XORPEN);
	}
//------------------------------------------------------------
	~WMark()
  {
		ReleaseDC(NULL, m_hDC);
    delete m_pPen;
    delete m_pBrush;
  }
//------------------------------------------------------------
  void Show(HWND hw)
  {
		RECT r; ZeroMemory((void*)&r, sizeof(RECT));
    ::GetWindowRect(hw, &r);

    if(!m_Visible)
			Draw(r);
    else
    { // if position changed
			if(memcmp(&r, &m_Rect, sizeof(RECT)) != 0)
      {
      	Draw(m_Rect); // erase old
				Draw(r); // draw new
			}
    }
    m_Rect = r;
    m_Visible = true;
  }
//------------------------------------------------------------
  void Hide()
  {
  	if(m_Visible)
    {
			Draw(m_Rect);
			m_Visible = false;
    }
  }
//------------------------------------------------------------
  void Draw(RECT r)
  {
    Rectangle(m_hDC, r.left, r.top, r.right, r.bottom);
  }
};

//---------------------------------------------------------------------------
class tGetWindowDrag
{
	HWND m_hWnd;
	POINT m_CPos;
	WMark *m_pMark;

public:
	tGetWindowDrag() {
		m_hWnd = NULL;
		ZeroMemory((void*)&m_CPos, sizeof(POINT));
		m_pMark = new WMark();
	}
	~tGetWindowDrag() {
		delete m_pMark;
	}
	HWND Handle() { return m_hWnd; }
	void EndDrag() {
		m_pMark->Hide();
	}
	void BeginDrag() {
		ZeroMemory((void*)&m_CPos, sizeof(POINT));
		m_hWnd = NULL;
	}
	bool OnDrag() {
		bool ret=false;
		POINT p; GetCursorPos(&p);
		if(memcmp(&m_CPos, &p, sizeof(POINT))) { // if cursor pos. changed
			HWND hw = WindowFromPoint(p);
			if(hw != m_hWnd) {
				ret = true; // window handle changed
				if(hw) { // there's window under cursor
					m_pMark->Show(hw); // draw mark
				}
				m_hWnd = hw; // store current handle value for later
			}
		}
		memcpy(&m_CPos, &p, sizeof(POINT)); // store cursor pos. for later
		return ret;
	}
};
//---------------------------------------------------------------------------
enum tDragMode { NONE, WD, SYSTRAN };

struct tWDwin {
	UnicodeString title;
	HWND handle;
	tWDwin() { handle == NULL; }
};
class tName {
public:
	tWString orig;
	tWString repl;
};
struct tCRhistoryScroll {
	bool active; // mouse above scroller
	int y; // y mouse pos
	int pos; // history index
	tCRhistoryScroll() { ZeroMemory((void*)this, sizeof(tCRhistoryScroll)); }
};

//---------------------------------------------------------------------------
bool GetWinTopState(HWND hwnd);
bool SetWinTopState(HWND hwnd, bool top);
bool IsWindowObscured(HWND hwnd);
BOOL BringWindowToFront(HWND hwnd);
BOOL CALLBACK EnumGameWindowsProc(HWND hwnd, LPARAM lParam);
UnicodeString GetWindowTextPlus(HWND hwnd);
UnicodeString GetWindowTitlePlus(HWND hwnd);
int GetClientRectPlus(HWND hwnd, RECT *prect);
LRESULT CALLBACK BackgroundWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void WriteString(TFileStream *fs, wchar_t *str);
UnicodeString ReadString(TFileStream *fs);
tWString GetClipboard(HWND handle);
std::wstring regexReplaceAll(std::wstring source, std::wstring find, std::wstring replace);

//---------------------------------------------------------------------------
#define SYSTRAN_TEXTBUF_LENGTH	1024
#define SYSTRAN_INTERVAL				100
#define SYSTRAN_TIMEOUT					150
class tSystran
{
private:
	HWND hwToolbar, hwTranslate, hwInput, hwOutput;
	int iCnt;
	TTimer *TimerReceive;
public:
	HWND GetToolbar() const { return hwToolbar; }
	tSystran(TTimer *timer) {
		TimerReceive = timer;
		TimerReceive->Enabled = false;
		TimerReceive->Tag = 0;
		Init();
	}
	void StopReceiveTimer() {
		TimerReceive->Enabled = false;
		TimerReceive->Interval = SYSTRAN_INTERVAL;
		TimerReceive->Tag = 0;
	}
	void Translate(HWND hwclip, bool fromTA, HWND target) {
		UnicodeString text;
		if (fromTA) {
			HWND hw = FindWindow(TAHPLUGIN_WNDCLASS_MESSAGE, NULL); // text from TA plugin
			if(hw) {
				// get text from msg window
				tWString *str = new tWString(wndGetWindowText(hw).w_str());
				// patch linebreaks
				str->Replace(L"\r\n", L"\n");
				str->Replace(L"\n", L"\r\n");
				text = str->w_str(); // assign fixed text
				delete str;
			} else {
				text = L"SYSTRAN: unable to get text from TA plugin";
			}
		} else {
			text = GetClipboard(hwclip).w_str(); // text from clipboard
		}
		if (text.Length() && SendText(text))
			wndEditBoxSetText(target, L"SYSTRAN: requesting data");
	}
	bool FindSystran() {
		bool ret = false;
		if (FindToolbar()) {
			FindTheRest();
			ret = true;
		} else {
			Init();
    }
		return ret;
	}
	bool FindToolbar() {
		hwToolbar = FindWindow(L"WindowsForms10.Window.8.app4", L"SYSTRAN Translation Toolbar");
		return (hwToolbar != NULL);
	}
	bool SendText(UnicodeString text) {
		bool ret = FindSystran();
		if (ret) {
			wndEditBoxClear(hwInput); // paranoid clean first
			wndEditBoxClear(hwOutput);
			wndEditBoxSetText(hwInput, text.w_str());
//			wndButtonClick(hwTranslate);
			TimerReceive->Enabled = false; // reset and start receive timer
			TimerReceive->Tag = 1; // reset timer counter
			TimerReceive->Enabled = true;
		}
		return ret;
	}
	UnicodeString ReceiveText() {
		UnicodeString ret;
		if (FindSystran()) {
			ret = GetWindowTextPlus(hwOutput);
			if (ret.Length()) StopReceiveTimer(); // got text - stop timer
		} else {
			StopReceiveTimer(); // no SYSTRAN - no point in waiting
		}
		return ret;
	}
	bool ValidateWindows() {
		bool ret = (hwToolbar && IsWindow(hwToolbar) &&
								hwTranslate && IsWindow(hwTranslate) &&
								hwInput && IsWindow(hwInput) &&
								hwOutput && IsWindow(hwOutput));
		if (!ret) Init();
		return ret;
	}
	void wndEditBoxSetText(HWND hwnd, wchar_t *text) {
		wndEditBoxClear(hwnd);
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)text);
	}
	void wndEditBoxClear(HWND hwnd) {
		SendMessage(hwnd, EM_SETSEL, (WPARAM)0, (LPARAM)-1);
		SendMessage(hwnd, WM_KEYDOWN, (WPARAM)VK_DELETE, (LPARAM)0);
		SendMessage(hwnd, WM_KEYUP, (WPARAM)VK_DELETE, (LPARAM)3);
	}
private:
	void FindTheRest() {
		if (!hwToolbar || !IsWindow(hwToolbar)) throw Exception(L"tSystran::FindTheRest() invalid hwToolbar");
		hwTranslate = hwInput = hwOutput = NULL;
		iCnt = 0;
		EnumChildWindows(hwToolbar, (WNDENUMPROC)EnumChildProc, (LPARAM)this);
		if (!ValidateWindows()) throw Exception(L"tSystran::FindTheRest() one or more windows not found");
	}
	void Init() {
		hwToolbar = hwTranslate = hwInput = hwOutput = NULL;
		iCnt = 0;
	}
	static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
		tSystran *caller = (tSystran*)lParam;
		wchar_t *tbuf = new wchar_t[SYSTRAN_TEXTBUF_LENGTH]; *tbuf = 0;
		UnicodeString s;
		GetClassName(hwnd, tbuf, SYSTRAN_TEXTBUF_LENGTH-1);
		s = tbuf;
		if (s == L"WindowsForms10.Window.8.app4") {
			GetWindowText(hwnd, tbuf, SYSTRAN_TEXTBUF_LENGTH-1);
			s = tbuf;
			if (s == L"Translate") {
				caller->hwTranslate = hwnd;
			}
		} else if (s == L"WindowsForms10.EDIT.app4") {
			if (caller->iCnt == 0)
				caller->hwInput = hwnd;
			else if (caller->iCnt == 1)
				caller->hwOutput = hwnd;
			caller->iCnt++;
		}
		delete []tbuf;
		return TRUE;
	}
	void wndButtonClick(HWND hwnd) {
		SendMessage(hwnd, WM_LBUTTONDOWN, (WPARAM)0, (LPARAM)0);
		SendMessage(hwnd, WM_LBUTTONUP, (WPARAM)0, (LPARAM)0);
	}
};

//---------------------------------------------------------------------------
class TFormTAHelper : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBoxCR;
	TGroupBox *GroupBoxFMW;
	TCheckBox *CheckBoxCRenable;
	TCheckBox *CheckBoxFSenable;
	TEdit *EditFSstep;
	TUpDown *UpDownFSstep;
	TLabel *Label1;
	TMemo *MemoCRcontents;
	TCheckBox *CheckBoxCRremoveLinebreaks;
	TCheckBox *CheckBoxCRnames;
	TRadioButton *RadioButtonCRseparateNames;
	TRadioButton *RadioButtonCRremoveNames;
	TCheckBox *CheckBoxCRrepetitionCharacter;
	TEdit *EditCRrepetitionCharacterNumber;
	TUpDown *UpDownCRrepetitionCharacterNumber;
	TLabel *Label2;
	TLabel *Label3;
	TComboBox *ComboBoxCRseparateNamesBy;
	TEdit *EditCRnameSearchDeviation;
	TUpDown *UpDownCRnameFindDeviation;
	TLabel *Label4;
	TLabel *Label5;
	TGroupBox *GroupBoxWD;
	TEdit *EditWDhandle;
	TTimer *TimerDrag;
	TGroupBox *GroupBoxHK;
	TCheckBox *CheckBoxHKenable;
	TMemo *MemoHKhotkeys;
	TGroupBox *GroupBoxTA;
	TTimer *TimerTALrefind;
	TSpeedButton *SpeedButtonTopmost;
	TComboBox *ComboBoxWDslot;
	TSpeedButton *SpeedButtonWDsave;
	TSpeedButton *SpeedButtonWDrestore;
	TSpeedButton *SpeedButtonWDdetach;
	TPopupMenu *PopupMenuWD;
	TMenuItem *Clearallwindows1;
	TCheckBox *CheckBoxCRseparateLinebreaks;
	TStatusBar *StatusBar1;
	TSpeedButton *SpeedButtonWDtopSync;
	TTimer *TimerError;
	TPopupMenu *PopupMenuForm;
	TMenuItem *Minimize1;
	TStaticText *StaticTextWDdrag;
	TMenuItem *N1;
	TMenuItem *Open1;
	TMenuItem *Save1;
	TMenuItem *Saveas1;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;
	TUpDown *UpDownCRnameFrom;
	TCheckBox *CheckBoxCRreplaceAllNames;
	TImage *ImageFurniture;
	TBalloonHint *BalloonHintFurniture;
	TPanel *PanelCRnamesApply;
	TSpeedButton *SpeedButtonCRapply;
	TPanel *PanelCRnames;
	TMemo *MemoCRnames;
	TSplitter *SplitterCRnames;
	TMemo *MemoCRnamesGlobal;
	TLabel *LabelTALstatus;
	TTimer *TimerTALcheck;
	TPanel *PanelCRtextButtons;
	TSpeedButton *SpeedButtonCRcopy;
	TTimer *TimerAnime;
	TPanel *PanelCRnotify;
	TTimer *TimerCRnotify;
	TListBox *ListBoxCRhistory;
	TSpeedButton *SpeedButtonCRhistory;
	TSpeedButton *SpeedButtonCRreprocess;
	TPopupMenu *PopupMenuCRhistory;
	TMenuItem *RawTextHistory1;
	TPopupMenu *PopupMenuCRreprocess;
	TMenuItem *ReprocessCurrentlyDisplayedText1;
	TTrayIcon *TrayIcon1;
	TGroupBox *GroupBoxSTR;
	TLabel *LabelSTRstatus;
	TStaticText *StaticTextSTRdrag;
	TTimer *TimerSTRreceive;
	TBalloonHint *BalloonHintSTR;
	TMenuItem *N2;
	TMenuItem *GameWindow1;
	TMenuItem *DisplayBlackCover1;
	TMenuItem *N3;
	TMenuItem *opLeft1;
	TMenuItem *opCenter1;
	TLabel *LabelClock;
	TUpDown *UpDownCRallNames;
	TCheckBox *CheckBoxCRremoveStrings;
	TEdit *EditCRremoveStrings;
	TPanel *PanelCRhistoryScroll;
	TTimer *TimerCRhistoryScroll;
	TEdit *EditCRrepetitionCharacterLen;
	TUpDown *UpDownCRrepetitionCharacterLen;
	TLabel *Label7;
	TMenuItem *SafeMode1;
	TMenuItem *AutohideTaskbar1;
	TGroupBox *GroupBoxWK;
	TLabel *LabelWKstatus;
	TCheckBox *CheckBoxWKenable;
	TCheckBox *CheckBoxWKcompact;
	TSpeedButton *SpeedButtonWKsavePos;
	TSpeedButton *SpeedButtonWKrestorePos;
	TTimer *TimerWKclass;
	TMenuItem *N4;
	TMenuItem *ViewPanels1;
	TMenuItem *WaKan1;
	TMenuItem *ranslationAggregatorSYSTRAN1;
	TMenuItem *HotKeysFloatingMouseWheel1;
	TMenuItem *WindowDetacher1;
	TCheckBox *CheckBoxWKpatched;
	TCheckBox *CheckBoxSTRtextFromTA;
	TTimer *TimerWDredraw;
	TCheckBox *CheckBoxCRtimeShift;
	TEdit *EditCRtimeShift;
	TUpDown *UpDownCRtimeShift;
	TLabel *LabelCRtimeShift;
	void __fastcall CheckBoxCRenableClick(TObject *Sender);
	void __fastcall CheckBoxFSenableClick(TObject *Sender);
	void __fastcall SpeedButtonTopmostClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall TimerDragTimer(TObject *Sender);
	void __fastcall ButtonWDdetachClick(TObject *Sender);
	void __fastcall CheckBoxHKenableClick(TObject *Sender);
	void __fastcall TimerTALrefindTimer(TObject *Sender);
	void __fastcall SpeedButtonCRapplyClick(TObject *Sender);
	void __fastcall ComboBoxWDslotChange(TObject *Sender);
	void __fastcall SpeedButtonWDsaveClick(TObject *Sender);
	void __fastcall SpeedButtonWDrestoreClick(TObject *Sender);
	void __fastcall CheckBoxCRremoveLinebreaksClick(TObject *Sender);
	void __fastcall CheckBoxCRseparateLinebreaksClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall SpeedButtonWDtopSyncClick(TObject *Sender);
	void __fastcall StatusBar1DrawPanel(TStatusBar *StatusBar,
          TStatusPanel *Panel, const TRect &Rect);
	void __fastcall TimerErrorTimer(TObject *Sender);
	void __fastcall StatusBar1Click(TObject *Sender);
	void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall Minimize1Click(TObject *Sender);
	void __fastcall StaticTextTALdragMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall StaticTextTALdragMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Open1Click(TObject *Sender);
	void __fastcall Save1Click(TObject *Sender);
	void __fastcall Saveas1Click(TObject *Sender);
	void __fastcall PopupMenuFormPopup(TObject *Sender);
	void __fastcall UpDownCRnameFromClick(TObject *Sender, TUDBtnType Button);
	void __fastcall Clearslotpos1Click(TObject *Sender);
	void __fastcall MemoCRnamesChange(TObject *Sender);
	void __fastcall TimerTALcheckTimer(TObject *Sender);
	void __fastcall SpeedButtonCRcopyClick(TObject *Sender);
	void __fastcall TimerAnimeTimer(TObject *Sender);
	void __fastcall TimerCRnotifyTimer(TObject *Sender);
	void __fastcall SpeedButtonCRhistoryClick(TObject *Sender);
	void __fastcall ListBoxCRhistoryMeasureItem(TWinControl *Control, int Index, int &Height);
	void __fastcall ListBoxCRhistoryDrawItem(TWinControl *Control, int Index, TRect &Rect,
          TOwnerDrawState State);
	void __fastcall ListBoxCRhistoryDblClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall ListBoxCRhistoryKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall SpeedButtonCRreprocessClick(TObject *Sender);
	void __fastcall RawTextHistory1Click(TObject *Sender);
	void __fastcall ProcessCurrentlyDisplayedText1Click(TObject *Sender);
	void __fastcall MemoCRcontentsKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall TrayIcon1Click(TObject *Sender);
	void __fastcall TimerSTRreceiveTimer(TObject *Sender);
	void __fastcall UpDownCRallNamesClick(TObject *Sender, TUDBtnType Button);
	void __fastcall PanelCRhistoryScrollMouseEnter(TObject *Sender);
	void __fastcall PanelCRhistoryScrollMouseLeave(TObject *Sender);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall TimerCRhistoryScrollTimer(TObject *Sender);
	void __fastcall PanelCRhistoryScrollClick(TObject *Sender);
	void __fastcall PanelCRhistoryScrollMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall AutohideTaskbar1Click(TObject *Sender);
	void __fastcall CheckBoxWKcompactClick(TObject *Sender);
	void __fastcall SpeedButtonWKsavePosClick(TObject *Sender);
	void __fastcall SpeedButtonWKrestorePosClick(TObject *Sender);
	void __fastcall TimerWKclassTimer(TObject *Sender);
	void __fastcall WaKan1Click(TObject *Sender);
	void __fastcall TimerWDredrawTimer(TObject *Sender);

private:	// User declarations
public:		// User declarations
	HHOOK m_hMouseHook, m_hKeyboardHook;
	HWND m_hNextClipboardListener;
	tWString m_TextRaw, m_Text;
  tOptions m_Options;
  tOptionsGlobal m_OptionsGlobal;
	TList *m_NameList;
	bool Boot; // app starting flag for initialization
	tGetWindowDrag *m_pGetWinDrag;
	tDragMode m_DragMode;
	tWDwin m_WDwindows[TOPT_WDSLOTS];
	TList *m_StrayWinList;
	UnicodeString m_ConfigFile;
	TList *m_CRhistoryList, *m_CRhistoryRawList;
	tSystran *m_Systran;
	tCRhistoryScroll m_HistoryScroll;
	int m_AutohideTaskbar;
	tWakan *m_Wakan;
	HWND m_hBackgroundCover;

	void ClearNameList();
  void LoadNames(bool global);
	void UpdateNames();
	void AddNamesFrom(TMemo *pmemo);
	void SaveOptions(UnicodeString file);
	void LoadOptions(UnicodeString file);
	void LoadOptions0(UnicodeString file); // import old config file (*.cfg)
	void UpdateOptions(bool store);
	void TALResult(HWND hw);
	HWND TALFind(bool forcefind=false, bool notimer=false);
	void RefreshClipboardMonitor();
	void StatusBarText(UnicodeString text, int mode=0);
	void StatusbarError(UnicodeString text, int mode=1);
	UnicodeString FileFromPath(UnicodeString path);
	void UpdateConfigFileValue(UnicodeString file);
	UnicodeString GetLastConfig();
	void SetLastConfig(UnicodeString file);
	void UpdateCRnamesFrom();
	void SaveNames(bool global);
	UnicodeString GenNamesFileName(bool global);
	UnicodeString ProgramDir();
	UnicodeString ProgramVer();
	bool CopyToClipboard(tWString text, HWND owner);
	void CRnotify(TColor color);
	void CRhistoryBoxUpdate(int itemindex);
	void CRhistoryBoxSelect();
	void SetWDhwnd(HWND hw);
	bool ProcessText(tWString s);
	void CRopenHistory(int whathist);
	UnicodeString MemoGetText(TMemo *Memo);
	void STRFind();
	HWND ValidateSTRtarget();
	HWND CreateBackgroundWindow(HWND hwnd);
	void Clock();
	void UpdateCRallNames();
	bool AmongWDtitles(UnicodeString title);
	void HistoryScroll(bool up);
	void HistoryMove(bool moveup, bool copy);
	bool AmongDetachedWindows(HWND hwnd);
	void WDrestoreAll();
	void CRhistoryBackgroundControls(bool show);
	void WKFind();
	void SystranTranslate(bool fromTA);
	void SaveOptionsGlobal();
	void LoadOptionsGlobal();

	void __fastcall WndProc(Messages::TMessage &Message);
	void __fastcall DisplayHint(TObject *Sender);
	void __fastcall DisplayHintDummy(TObject *Sender);
	void __fastcall PopupMenuItemWinClick(TObject *Sender);

	__fastcall TFormTAHelper(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormTAHelper *FormTAHelper;
//---------------------------------------------------------------------------

#endif
