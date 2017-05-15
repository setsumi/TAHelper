//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitOptions.h"
#include "File1.h"
#include "UnitSubst.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOptions *FormOptions;
//---------------------------------------------------------------------------
__fastcall TFormOptions::TFormOptions(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::ButtonOKClick(TObject *Sender)
{
	// apply options
	Config.TApopupOnTrans = CheckBoxPopupTA->Checked?1:0;
	SaveConfig();
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::ButtonCancelClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TFormOptions::FormClose(TObject *Sender, TCloseAction &Action)
{
	FormTAHsubst->TrayIcon1->Visible = true;
	Action = caHide;
}
//---------------------------------------------------------------------------
void __fastcall TFormOptions::FormShow(TObject *Sender)
{
	// init form from options
	CheckBoxPopupTA->Checked = (Config.TApopupOnTrans == 1);
}
//---------------------------------------------------------------------------
