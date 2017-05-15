//---------------------------------------------------------------------------

#ifndef UnitSubstH
#define UnitSubstH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Menus.hpp>

//---------------------------------------------------------------------------
class TFormTAHsubst : public TForm
{
__published:	// IDE-managed Components
	TTrayIcon *TrayIcon1;
	TGroupBox *GroupBoxSubstProfile;
	TComboBox *ComboBoxProfiles;
	TLabel *Label1;
	TGroupBox *GroupBoxSubstList;
	TPanel *Panel1;
	TPanel *PanelSubstList;
	TButton *ButtonListClear;
	TButton *ButtonOK;
	TButton *ButtonCancel;
	TButton *ButtonApply;
	TStringGrid *GridSubst;
	TMemo *MemoInplace;
	TButton *ButtonListDelete;
	TCheckBox *CheckBoxEnable;
	TButton *ButtonListInsert;
	TPopupMenu *PopupMenuListInsert;
	TMenuItem *EXT1;
	TMenuItem *REGEX1;
	TCheckBox *CheckBoxBeforeTA;
	TPopupMenu *PopupMenuTray;
	TMenuItem *Options1;
	TMenuItem *Substitutions1;
	TTimer *TimerStartup;
	void __fastcall TrayIcon1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall GridSubstSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall GridSubstDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall MemoInplaceExit(TObject *Sender);
	void __fastcall MemoInplaceKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall ComboBoxProfilesChange(TObject *Sender);
	void __fastcall ButtonListClearClick(TObject *Sender);
	void __fastcall ButtonCancelClick(TObject *Sender);
	void __fastcall ButtonApplyClick(TObject *Sender);
	void __fastcall ButtonOKClick(TObject *Sender);
	void __fastcall ButtonListDeleteClick(TObject *Sender);
	void __fastcall GridSubstMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall GridSubstMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos,
          bool &Handled);
	void __fastcall ButtonListInsertClick(TObject *Sender);
	void __fastcall REGEX1Click(TObject *Sender);
	void __fastcall TrayIcon1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Substitutions1Click(TObject *Sender);
	void __fastcall Options1Click(TObject *Sender);
	void __fastcall TimerStartupTimer(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TFormTAHsubst(TComponent* Owner);

	UnicodeString m_LastProfile;

	void UpdateGrid(TStringGrid *StringGrid, TStringList *strings);
	void UpdateStrings(TStringList *strings, TStringGrid *StringGrid);
	UnicodeString NameFromCaption(UnicodeString capt);
	int GetProfileIndex(UnicodeString profile);
	void AddProfile(UnicodeString profile);
};

//---------------------------------------------------------------------------
extern PACKAGE TFormTAHsubst *FormTAHsubst;

//---------------------------------------------------------------------------
#endif
