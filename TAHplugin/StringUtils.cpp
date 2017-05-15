//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "StringUtils.h"

//---------------------------------------------------------------------------
// Quick tuning of TStringList object
void InitTStringList(TStringList *StringList, bool sorted)
{
	StringList->CaseSensitive = true;
	if (sorted) {
		StringList->Duplicates = dupIgnore;
		StringList->Sorted = true;
	} else {
		StringList->Duplicates = dupAccept;
		StringList->Sorted = false;
	}
}

//---------------------------------------------------------------------------
// Replace all, using Regex
// Returns new string
UnicodeString strRegexReplaceAll(UnicodeString source, UnicodeString find, UnicodeString replace, UnicodeString caller)
{
	std::wstring rv;
	try {
		std::wstring sSource(source.w_str());
		std::wstring sFind(find.w_str());
		std::wstring sReplace(replace.w_str());

		boost::wregex e(sFind);  // match regex
		rv = boost::regex_replace(sSource, e, sReplace); // result
	} catch (...) {
		UnicodeString err;
		err.printf(L"%sstrRegexReplaceAll(・) : Error in Regex: %s　　%s", caller, find, replace);
		return err;
	}
	return rv.c_str();
}

//---------------------------------------------------------------------------
// Replace all, using plain text
// Returns new string
UnicodeString strReplaceAll(UnicodeString str, UnicodeString find, UnicodeString repl)
{
	TList *points = new TList();
	while (int i = str.Pos(find)) {
		points->Add((void*)i);
		str.Delete(i, find.Length());
	}
	if (repl.Length()) {
		for (int i = points->Count-1; i >= 0 ; i--) {
			str.Insert(repl, (int)points->Items[i]);
		}
	}
	delete points;
	return str;
}

//---------------------------------------------------------------------------
// Returns 0 if substr not found
// indexes are 1-based
int strFind(UnicodeString str, UnicodeString substr, int start)
{
	int rv = str.SubString(start, str.Length()-start+1).Pos(substr);
	if (rv) rv += start-1;
	return  rv;
}

//---------------------------------------------------------------------------
// Remove repetition of given character/phrase
// Returns new string
UnicodeString strRemoveDuplicates(UnicodeString str, UnicodeString phrase)
{
	const int phrlen=phrase.Length();
	if (!phrlen) return str;

	int pos=1, lb0=0;
	while(1) {
		int fpos = strFind(str, phrase, pos);
		if(fpos > 0) {
			if(lb0 > 0 && fpos-phrlen == lb0) {
				str.Delete(fpos, phrlen);
			} else {
				pos = fpos+phrlen;
				if(pos > str.Length()) break;
				lb0 = fpos;
			}
		} else
			break;
	}
	return str;
}

//---------------------------------------------------------------------------
// Trim starting and/or trailing characters
// Returns new string
UnicodeString strTrimL(UnicodeString str, UnicodeString chars)
{
	if (str.Length()) {
		if (!chars.Length()) chars = tCharacters::Space();
		while (int i = chars.Pos(str[1])) {
			str.Delete(1,1);
			if (!str.Length()) break;
		}
	}
	return str;
}
UnicodeString strTrimR(UnicodeString str, UnicodeString chars)
{
	if (str.Length()) {
		if (!chars.Length()) chars = tCharacters::Space();
		while (int i = chars.Pos(str[str.Length()])) {
			str.Delete(str.Length(),1);
			if (!str.Length()) break;
		}
	}
	return str;
}
UnicodeString strTrim(UnicodeString str, UnicodeString chars)
{
	str = strTrimL(str, chars);
	str = strTrimR(str, chars);
	return str;
}

//---------------------------------------------------------------------------
// Strip characters from string
// Returns new string
UnicodeString strStripChars(UnicodeString str, UnicodeString chars)
{
	if (!str.Length() || !chars.Length()) return str;

	for (int i=1; i <= str.Length(); i++) {
		if (chars.Pos(str[i])) {
			str.Delete(i,1);
			if (!str.Length()) break;
			i--;
		}
	}
	return str;
}

//---------------------------------------------------------------------------

