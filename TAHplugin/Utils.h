//---------------------------------------------------------------------------
#ifndef UtilsH
#define UtilsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
#define tBETWEEN(x,y,z)	(((y)>=(x))&&((y)<=(z)))
//---------------------------------------------------------------------------
UnicodeString GetModuleVersion();
UnicodeString GetModuleName();
UnicodeString GetModuleDir();

void RemoveRow(TStringGrid* StringGrid, int Index);
void InsertRow(TStringGrid* StringGrid, int AfterIndex);
AnsiString BinToHex(void *data, int size);
AnsiString StrToHex(UnicodeString text);
UnicodeString HexToStr(AnsiString hex);

BOOL BringWindowToFront(HWND hwnd);

//---------------------------------------------------------------------------
#endif // UtilsH
