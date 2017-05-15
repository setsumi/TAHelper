//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitOSD.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOSD *FormOSD;
//---------------------------------------------------------------------------
__fastcall TFormOSD::TFormOSD(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormOSD::Timer1Timer(TObject *Sender)
{
	// hide form
	Timer1->Enabled = false;
	Hide();
}
//---------------------------------------------------------------------------
void TFormOSD::ShowOSD(UnicodeString text)
{
	if (IsVisible()) Timer1->Enabled = false;
	AssignData(text);
	ShowWindow(Handle, SW_SHOWNA);
	Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void TFormOSD::ShowMsg(UnicodeString text)
{
	Timer1->Enabled = false;
	AssignData(text);
	ShowWindow(Handle, SW_SHOWNA);
}
//---------------------------------------------------------------------------
bool TFormOSD::IsVisible()
{
	return IsWindowVisible(Handle) == TRUE;
}
//---------------------------------------------------------------------------
void TFormOSD::Hide()
{
	ShowWindow(Handle, SW_HIDE);
}
//---------------------------------------------------------------------------
void TFormOSD::AssignData(UnicodeString text)
{
	// set text and form size
	Label1->Caption = text;
	Width = Label1->Width;
	Height = Label1->Height;

	// set form position
	HMONITOR hmon = MonitorFromWindow(Handle, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi))
		throw Exception(L"TFormOSD::Assign() GetMonitorInfo() failed");
	Left = mi.rcMonitor.left;
	Top = mi.rcMonitor.bottom - Height;
}
//---------------------------------------------------------------------------
void __fastcall TFormOSD::Label1Click(TObject *Sender)
{
	Timer1Timer(NULL);
}
//---------------------------------------------------------------------------

