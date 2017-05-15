//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitSubst.h"
#include "Utils.h"
#include "File1.h"
#include "WindowUtils.h"
#include "UnitOptions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormTAHsubst *FormTAHsubst;

//-----------------------------------------------------------------------------
#define GRIDCELL_BORDER 4 // border

//-----------------------------------------------------------------------------
__fastcall TFormTAHsubst::TFormTAHsubst(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::TrayIcon1Click(TObject *Sender)
{
	TrayIcon1->Visible = false;
	if (Config.ShowModal) {
		ShowModal(); // focusing by Tab is OK
	} else {
		Show(); // focusing by Tab is not working
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::FormClose(TObject *Sender, TCloseAction &Action)
{
	TrayIcon1->Visible = true;
	Action = caHide;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::FormCreate(TObject *Sender)
{
	KeyPreview = true;
	TrayIcon1->Hint = UnicodeString(L"TAHplugin ") + GetModuleVersion() + L"\nRightClick - menu\nLeftClick - substitutions";
	//config
	if (Config.ShowModal) { // disable minimize button in modal mode
		TBorderIcons tempBI = BorderIcons;
		tempBI >> biMinimize;
		BorderIcons = tempBI;
	}
	Left		= Config.SubstWindowLeft;
	Top			= Config.SubstWindowTop;
	Width		= Config.SubstWindowWidth;
	Height	= Config.SubstWindowHeight;

	MemoInplace->Visible = false;
	MemoInplace->WordWrap = false;
	MemoInplace->WantReturns = false;
	MemoInplace->WantTabs = false;
	MemoInplace->BorderStyle = bsNone;
	MemoInplace->Font = GridSubst->Font;
	UINT tabs = 13; // distance between tabs (in some logical units)
	SendMessage(MemoInplace->Handle, EM_SETTABSTOPS, 1, (LONG)&tabs);

	GridSubst->DefaultDrawing = false;
	GridSubst->RowCount = 0;
	GridSubst->Tag = -1; // last selected row
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::FormResize(TObject *Sender)
{
	GridSubst->ColWidths[0] = GridSubst->ClientWidth;
	MemoInplace->Width = GridSubst->ColWidths[0] - GRIDCELL_BORDER;
	Label1->Width = ComboBoxProfiles->Width;
}
//---------------------------------------------------------------------------

void __fastcall TFormTAHsubst::GridSubstSelectCell(TObject *Sender, int ACol, int ARow,
					bool &CanSelect)
{
	RECT R;
	R = GridSubst->CellRect(ACol, ARow);
	R.left = R.left + GridSubst->Left;
	R.right = R.right + GridSubst->Left;
	R.top = R.top + GridSubst->Top;
	R.bottom = R.bottom + GridSubst->Top;
	// Show the Editbox
	MemoInplace->Left = R.left + GRIDCELL_BORDER;
	MemoInplace->Top = R.top + GRIDCELL_BORDER;
	MemoInplace->Width = R.right - R.left - GRIDCELL_BORDER / 2;
	MemoInplace->Height = R.bottom - R.top - GRIDCELL_BORDER / 2;
	MemoInplace->Visible = true;
	MemoInplace->Text = GridSubst->Cells[ACol][ARow];
	MemoInplace->SetFocus();

	CanSelect = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::GridSubstDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State)
{
	GridSubst->Canvas->Font = GridSubst->Font;
	GridSubst->Canvas->Brush->Color = (State.Contains(gdSelected) && !MemoInplace->Visible)? clHighlight : GridSubst->Color;
	GridSubst->Canvas->FillRect(Rect);
	UnicodeString text(GridSubst->Cells[ACol][ARow]);
	if (text.Length()) {
		TColor allcolor = clBtnFace; // passive by default
		if (text[1] == L'*') {
			allcolor = clGreen; // comment
		} else
		if (IsTag(text)) {
			allcolor = clMaroon; // tag
			GridSubst->Canvas->Font->Style = TFontStyles() << fsBold;
		}
		int pos = Rect.Left + 2;
		for (int i=1; i <= text.Length(); i++) {
			UnicodeString ch(text.SubString(i,1));
			if (ch == L"\t") {
				ch = L"\x21D2"; // Tab arrow
				GridSubst->Canvas->Font->Color = (allcolor == clBtnFace)? clBlue : allcolor;
			} else {
				GridSubst->Canvas->Font->Color = (allcolor == clBtnFace)? GridSubst->Font->Color : allcolor;
			}
			GridSubst->Canvas->TextOut(pos, Rect.Top + 2, ch);
			pos = GridSubst->Canvas->PenPos.x;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::MemoInplaceExit(TObject *Sender)
{
	MemoInplace->Text = FormatSubstEntry(MemoInplace->Text);
	GridSubst->Cells[GridSubst->Col][GridSubst->Row] = MemoInplace->Text;
	MemoInplace->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::MemoInplaceKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	bool CanSelect = true;
	UnicodeString str1;
  int sel = 0;

	switch (Key) {
	case VK_UP:
	case VK_DOWN:
	case VK_PRIOR:
	case VK_NEXT:
		GridSubst->SetFocus();
		GridSubst->Perform(WM_KEYDOWN, Key, 0);
		GridSubstSelectCell(GridSubst, GridSubst->Col, GridSubst->Row, CanSelect);
		break;
	case VK_RETURN:
		Key = 0;
		str1 = MemoInplace->Text.SubString(MemoInplace->SelStart + 1, MemoInplace->Text.Length() - MemoInplace->SelStart);
		MemoInplace->Text = MemoInplace->Text.SubString(1, MemoInplace->SelStart);
		GridSubst->SetFocus();
		InsertRow(GridSubst, GridSubst->Row);
		GridSubst->Perform(WM_KEYDOWN, VK_DOWN, 0);
		GridSubstSelectCell(GridSubst, GridSubst->Col, GridSubst->Row, CanSelect);
		MemoInplace->Text = str1;
		break;
	case VK_DELETE:
		if (MemoInplace->SelStart == MemoInplace->Text.Length() && GridSubst->Row < GridSubst->RowCount - 1) {
			Key = 0;
			sel = MemoInplace->SelStart;
			str1 = MemoInplace->Text;
			GridSubst->SetFocus();
			RemoveRow(GridSubst, GridSubst->Row);
			GridSubstSelectCell(GridSubst, GridSubst->Col, GridSubst->Row, CanSelect);
			MemoInplace->Text = str1 + MemoInplace->Text;
			MemoInplace->SelStart = sel;
		}
		break;
	}
}

//---------------------------------------------------------------------------
int TFormTAHsubst::GetProfileIndex(UnicodeString profile)
{
	int rv = -1;
	for (int i=0; i<ComboBoxProfiles->Items->Count; i++)
		if (NameFromCaption(ComboBoxProfiles->Items->Strings[i]) == profile) {
			rv = i;
			break;
		}
	return rv;
}
//---------------------------------------------------------------------------
void TFormTAHsubst::AddProfile(UnicodeString profile)
{
	TStringList *subst = new TStringList();
	InitTStringList(subst, false);
	LoadNames(profile, subst); // load names file
	if (subst->Count) {
		profile = profile + L": " + IntToStr(subst->Count) + ((subst->Count>1)?L" lines":L" line");
	}
	ComboBoxProfiles->AddItem(profile, subst); // associate names file content with combobox item
}
//---------------------------------------------------------------------------
// Init all
void __fastcall TFormTAHsubst::FormShow(TObject *Sender)
{
	// update form from options
	CheckBoxEnable->Checked = (Config.SubstEnable == 1);
	CheckBoxBeforeTA->Checked = (Config.SubstBeforeTA == 1);

	ComboBoxProfiles->SetFocus();

	// update profiles list
	UpdateProfiles();

	// add profiles to combobox
	AddProfile(TAHPLUGIN_GLOBAL_PROFILE);
	for (int i=0; i < pActiveProfileList->Count; i++) {
		AddProfile(pActiveProfileList->Strings[i]);
	}
	for (int i=0; i < pProfileList->Count; i++) {
		UnicodeString profile(pProfileList->Strings[i]);
		if (GetProfileIndex(profile) == -1) {
			AddProfile(profile);
		}
	}
	ComboBoxProfiles->Tag = -1; // previous setected profile, used in ComboBoxProfilesChange()

	// select last used profile
	int ind = -1;
	if (m_LastProfile.Length()) ind = GetProfileIndex(m_LastProfile);
	if (ind > -1)
		ComboBoxProfiles->ItemIndex = ind;
	else
		ComboBoxProfiles->ItemIndex = (pActiveProfileList->Count)? 1 : 0;
	// update grid
	ComboBoxProfilesChange(ComboBoxProfiles);
	// restore last selected row
	if (ind > -1 && GridSubst->Tag > -1 && tBETWEEN(0, GridSubst->Tag, GridSubst->RowCount-1))
		GridSubst->Row = GridSubst->Tag;
	GridSubst->SetFocus();
}

//---------------------------------------------------------------------------
// Cleanup all
void __fastcall TFormTAHsubst::FormHide(TObject *Sender)
{
	//config
	Config.SubstWindowLeft		= Left;
	Config.SubstWindowTop			= Top;
	Config.SubstWindowWidth		= Width;
	Config.SubstWindowHeight	= Height;

	// remember selected profile
	m_LastProfile = NameFromCaption(ComboBoxProfiles->Items->Strings[ComboBoxProfiles->ItemIndex]);
	// remember selected row
	GridSubst->Tag = GridSubst->Row;
	// clear grid
	GridSubst->RowCount = 0;
	// clear profiles combobox
	for (int i=0; i < ComboBoxProfiles->Items->Count; i++) {
		delete (TStringList*)(ComboBoxProfiles->Items->Objects[i]);
	}
	ComboBoxProfiles->Items->Clear();
}

//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::ComboBoxProfilesChange(TObject *Sender)
{
	if (ComboBoxProfiles->Tag >= 0) { // store previous grid in it's strings
		UpdateStrings((TStringList*)ComboBoxProfiles->Items->Objects[ComboBoxProfiles->Tag], GridSubst);
	}
	ComboBoxProfiles->Tag = ComboBoxProfiles->ItemIndex;
	UpdateGrid(GridSubst, (TStringList*)ComboBoxProfiles->Items->Objects[ComboBoxProfiles->ItemIndex]);
}

//---------------------------------------------------------------------------
void TFormTAHsubst::UpdateStrings(TStringList *strings, TStringGrid *StringGrid)
{
	assert(StringGrid != NULL);
	assert(strings != NULL);

	strings->Clear();
	if (!(StringGrid->RowCount == 1 && !StringGrid->Cells[0][0].Length())) {
		for (int i=0; i < StringGrid->RowCount; i++) {
			strings->Add(StringGrid->Cells[0][i]);
		}
	}
}

//---------------------------------------------------------------------------
void TFormTAHsubst::UpdateGrid(TStringGrid *StringGrid, TStringList *strings)
{
	assert(StringGrid != NULL);
	assert(strings != NULL);
	SNDMSG(StringGrid->Handle, WM_SETREDRAW, false, 0);
	try
	{
		StringGrid->RowCount = strings->Count? strings->Count : 1;
		StringGrid->Rows[0]->Clear();
		for (int i=0; i < strings->Count; i++) {
			StringGrid->Cells[0][i] = strings->Strings[i];
		}
		StringGrid->Row = 0;
		StringGrid->Col = 0;
	}
	__finally
	{
		SNDMSG(StringGrid->Handle, WM_SETREDRAW, true, 0);
	}
	// update (repaint)
	StringGrid->Invalidate();
}

//---------------------------------------------------------------------------
UnicodeString TFormTAHsubst::NameFromCaption(UnicodeString capt)
{
	if (int p = capt.Pos(L":")) {
  	return capt.SubString(1, p - 1);
	}
	return capt;
}

//---------------------------------------------------------------------------
// Clear grid
void __fastcall TFormTAHsubst::ButtonListClearClick(TObject *Sender)
{
	GridSubst->Row = 0;
	GridSubst->Col = 0;
	GridSubst->RowCount = 1;
	GridSubst->Rows[0]->Clear();
	MemoInplace->Text = L"";
}

//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::ButtonCancelClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::ButtonOKClick(TObject *Sender)
{
	ButtonApplyClick(ButtonApply);
	Close();
}

//---------------------------------------------------------------------------
// Save all
void __fastcall TFormTAHsubst::ButtonApplyClick(TObject *Sender)
{
	//config
	Config.SubstEnable = CheckBoxEnable->Checked?1:0;
	Config.SubstBeforeTA = CheckBoxBeforeTA->Checked?1:0;
	SaveConfig();
	// update current strings from grid
	UpdateStrings((TStringList*)ComboBoxProfiles->Items->Objects[ComboBoxProfiles->ItemIndex], GridSubst);
	// save all profiles to files
	for (int i=0; i < ComboBoxProfiles->Items->Count; i++) {
		SaveNames(NameFromCaption(ComboBoxProfiles->Items->Strings[i]), (TStringList*)ComboBoxProfiles->Items->Objects[i]);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::ButtonListDeleteClick(TObject *Sender)
{
	if (GridSubst->RowCount > 1) {
		bool fix = (GridSubst->Row == GridSubst->RowCount - 1);
		RemoveRow(GridSubst, GridSubst->Row);
		if (fix) {
			GridSubst->Row = GridSubst->RowCount - 1;
			ButtonListDelete->SetFocus();
		}
	}
}

//---------------------------------------------------------------------------
// Scroll grid down
void __fastcall TFormTAHsubst::GridSubstMouseWheelDown(TObject *Sender, TShiftState Shift,
					TPoint &MousePos, bool &Handled)
{
	GridSubst->SetFocus();
	int step = 4; // number of lines to scroll
	int top = GridSubst->TopRow;
	while (step > 0 && top + GridSubst->VisibleRowCount < GridSubst->RowCount) {
		step--;
		top++;
	}
	GridSubst->TopRow = top;
	Handled = true;
}
//---------------------------------------------------------------------------
// Scroll grid up
void __fastcall TFormTAHsubst::GridSubstMouseWheelUp(TObject *Sender, TShiftState Shift,
					TPoint &MousePos, bool &Handled)
{
	GridSubst->SetFocus();
	int step = 4; // number of lines to scroll
	int top = GridSubst->TopRow;
	while (step > 0 && top > 0) {
		step--;
		top--;
	}
	GridSubst->TopRow = top;
	Handled = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::FormMouseWheelDown(TObject *Sender, TShiftState Shift,
					TPoint &MousePos, bool &Handled)
{
	if (MemoInplace->Visible) {
		GridSubstMouseWheelDown(Sender, Shift, MousePos, Handled);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::FormMouseWheelUp(TObject *Sender, TShiftState Shift,
					TPoint &MousePos, bool &Handled)
{
	if (MemoInplace->Visible) {
		GridSubstMouseWheelUp(Sender, Shift, MousePos, Handled);
	}
}
//---------------------------------------------------------------------------
// Display insert menu
void __fastcall TFormTAHsubst::ButtonListInsertClick(TObject *Sender)
{
	TPoint p;
	p = ButtonListInsert->ClientToScreen(Point(0, 0));
	PopupMenuListInsert->Popup(p.x, p.y);
}

//---------------------------------------------------------------------------
// Insert line with tag
void __fastcall TFormTAHsubst::REGEX1Click(TObject *Sender)
{
	TMenuItem *pitem = (TMenuItem*)Sender;
	InsertRow(GridSubst, GridSubst->Row - 1);
	GridSubst->Cells[0][GridSubst->Row] = pitem->Caption;
}

//---------------------------------------------------------------------------
// Show Tray menu
void __fastcall TFormTAHsubst::TrayIcon1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if (Button == mbRight) {
		POINT p;
		GetCursorPos(&p);
		PopupMenuTray->Popup(p.x, p.y);
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormTAHsubst::Substitutions1Click(TObject *Sender)
{
	TrayIcon1Click(TrayIcon1);
}
//---------------------------------------------------------------------------

void __fastcall TFormTAHsubst::Options1Click(TObject *Sender)
{
	// show options form
	TrayIcon1->Visible = false;
	if (Config.ShowModal) {
		FormOptions->ShowModal(); // focusing by Tab is OK
	} else {
		FormOptions->Show(); // focusing by Tab is not working
	}
}
//---------------------------------------------------------------------------
// Delayed Init
void __fastcall TFormTAHsubst::TimerStartupTimer(TObject *Sender)
{
	// this thing is disabled because screws up Hints
	TimerStartup->Enabled = false;
	HWND hwta = GetTAWindow();
	if (hwta) {
		Application->Handle = hwta;
	} else {
		TimerStartup->Enabled = true;
	}
}
//---------------------------------------------------------------------------

