//---------------------------------------------------------------------------
#ifndef StringUtilsH
#define StringUtilsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <shlwapi.h>
#include <boost/regex.hpp>

//---------------------------------------------------------------------------
// class for characters checking
class tCharacters {
private:
	static UnicodeString punctuation;
	static UnicodeString spacelike;
public:
	static UnicodeString Punct() { return punctuation; }
	static UnicodeString Space() { return spacelike; }
	static bool IsPunct(wchar_t ch) { return (punctuation.Pos(ch) > 0); }
	static bool IsSpace(wchar_t ch) { return (spacelike.Pos(ch) > 0); }
};
UnicodeString tCharacters::punctuation	= L"`~!@#$%^&*()_-+=[{]}\\|;:\'\",<.>/\?‘～！＠＃＄％＾＆＊（）＿－＋＝「｛」｝￥｜；：’”、＜。＞・？『』《》【】〔〕";
UnicodeString tCharacters::spacelike		= L"\r\n\t 　";

//---------------------------------------------------------------------------
void InitTStringList(TStringList *StringList, bool sorted);

UnicodeString strRegexReplaceAll(UnicodeString source, UnicodeString find, UnicodeString replace, UnicodeString caller);
UnicodeString strReplaceAll(UnicodeString str, UnicodeString find, UnicodeString repl);
int strFind(UnicodeString str, UnicodeString substr, int start);
UnicodeString strRemoveDuplicates(UnicodeString str, UnicodeString phrase);
UnicodeString strTrimL(UnicodeString str, UnicodeString chars=L"");
UnicodeString strTrimR(UnicodeString str, UnicodeString chars=L"");
UnicodeString strTrim(UnicodeString str, UnicodeString chars=L"");
UnicodeString strStripChars(UnicodeString str, UnicodeString chars);

//---------------------------------------------------------------------------
#endif // StringUtilsH
