object FormOSD: TFormOSD
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'FormOSD'
  ClientHeight = 82
  ClientWidth = 189
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clLime
  Font.Height = -21
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 25
  object Label1: TLabel
    Left = 0
    Top = 0
    Width = 60
    Height = 25
    Caption = 'Label1'
    OnClick = Label1Click
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 98
    Top = 16
  end
end
