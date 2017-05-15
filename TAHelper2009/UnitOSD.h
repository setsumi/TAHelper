//---------------------------------------------------------------------------

#ifndef UnitOSDH
#define UnitOSDH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormOSD : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TTimer *Timer1;
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall Label1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFormOSD(TComponent* Owner);

	void AssignData(UnicodeString text);
	void ShowOSD(UnicodeString text);
	void ShowMsg(UnicodeString text);
	bool IsVisible();
	void Hide();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOSD *FormOSD;
//---------------------------------------------------------------------------
#endif
