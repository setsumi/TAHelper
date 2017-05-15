object FormTAHsubst: TFormTAHsubst
  Left = 0
  Top = 0
  BorderWidth = 5
  Caption = 'TAHplugin - Substitutions'
  ClientHeight = 375
  ClientWidth = 393
  Color = clBtnFace
  Constraints.MinHeight = 419
  Constraints.MinWidth = 411
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000400E80200001600000028000000200000004000
    0000010004000000000000020000000000000000000010000000100000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    00000000000000000000000000000000000000000000000000000000000000FF
    FFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FF
    FFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFCCCCFFFFFFFFFFFF0000FF
    FFFFFFFFFFCCCCFFFFFFFFFFFF0000FFFFFFFFFFFFCCCCCCCCCCFFFFFF0000FF
    FFFFFFFFFFCCCCCCCCCCCFFFFF0000FFFFFFFFFFFFCCCCCCCCCCCFFFFF0000FF
    FFFFFFFFFFCCCCCCCCCCCFFFFF0000FFFFFFFFFFFFCCCCFFFCCCCFFFFF0000FF
    FFFFFFFFFFCCCCFFFCCCCFFFFF0000FFFFFFFFFFFFCCCCFFFCCCCFFFFF0000FF
    FFFFFFFFFFCCCCFFFCCCCFFFFF0000FFFFFFFFFFFFCCCCCCCCCCCFFFFF0000FF
    FFFFFFFFFFCCCCCCCCCCCFFFFF0000FFFFFFFFFFFFCCCCCCCCCCCFFFFF0000FF
    FFFFFFFFFFCCCCCCCCCCFFFFFF0000FFFFFFFFFFFF9999FFFFFFFFFFFF0000FF
    FF99FFFFFF9999FFFFFF99FFFF0000FFFF99FFFFFF9999FFFFFF99FFFF0000FF
    FF99FFFFFF9999FFFFFF99FFFF0000FFFF99FFFFFF9999FFFFFF99FFFF0000FF
    FF99999999999999999999FFFF0000FFFF99999999999999999999FFFF0000FF
    FF99999999999999999999FFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FF
    FFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000
    000000000000000000000000000000000000000000000000000000000000FFFF
    FFFF800000018000000180000001800000018000000180000001800000018000
    0001800000018000000180000001800000018000000180000001800000018000
    0001800000018000000180000001800000018000000180000001800000018000
    0001800000018000000180000001800000018000000180000001FFFFFFFF}
  KeyPreview = True
  OldCreateOrder = False
  ShowHint = True
  OnClose = FormClose
  OnCreate = FormCreate
  OnHide = FormHide
  OnMouseWheelDown = FormMouseWheelDown
  OnMouseWheelUp = FormMouseWheelUp
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBoxSubstProfile: TGroupBox
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 387
    Height = 67
    Align = alTop
    Caption = 'Profile'
    Padding.Left = 10
    Padding.Right = 10
    TabOrder = 0
    object Label1: TLabel
      Left = 12
      Top = 37
      Width = 361
      Height = 27
      AutoSize = False
      Caption = 
        'Active profiles are selected based on currently running programs' +
        '.  More than one profile can be active at a time.  The * profile' +
        ' is always active.'
      WordWrap = True
    end
    object ComboBoxProfiles: TComboBox
      Left = 12
      Top = 15
      Width = 363
      Height = 21
      Align = alTop
      Style = csDropDownList
      DropDownCount = 20
      ItemHeight = 0
      TabOrder = 0
      OnChange = ComboBoxProfilesChange
    end
  end
  object Panel1: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 332
    Width = 387
    Height = 43
    Margins.Top = 0
    Margins.Bottom = 0
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    DesignSize = (
      387
      43)
    object ButtonOK: TButton
      Left = 159
      Top = 12
      Width = 70
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = 'OK'
      TabOrder = 0
      OnClick = ButtonOKClick
    end
    object ButtonCancel: TButton
      Left = 235
      Top = 12
      Width = 70
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = ButtonCancelClick
    end
    object ButtonApply: TButton
      Left = 311
      Top = 12
      Width = 70
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = 'Apply'
      TabOrder = 2
      OnClick = ButtonApplyClick
    end
    object CheckBoxEnable: TCheckBox
      Left = 12
      Top = 22
      Width = 61
      Height = 17
      Hint = 'Enable/disable substitutions'
      Caption = 'Enable'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
    end
    object CheckBoxBeforeTA: TCheckBox
      Left = 12
      Top = 4
      Width = 71
      Height = 17
      Hint = 
        'Substitute before (or after) Translation Aggregator'#39's substituti' +
        'ons'
      Caption = 'Before TA'
      TabOrder = 4
    end
  end
  object GroupBoxSubstList: TGroupBox
    AlignWithMargins = True
    Left = 3
    Top = 76
    Width = 387
    Height = 256
    Margins.Bottom = 0
    Align = alClient
    Caption = 'Profile substitution list'
    Padding.Left = 10
    Padding.Top = 5
    Padding.Right = 10
    TabOrder = 1
    object GridSubst: TStringGrid
      Left = 12
      Top = 20
      Width = 363
      Height = 192
      Align = alClient
      ColCount = 1
      DefaultRowHeight = 16
      DefaultDrawing = False
      FixedCols = 0
      FixedRows = 0
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
      Font.Style = []
      Options = [goThumbTracking]
      ParentFont = False
      ScrollBars = ssVertical
      TabOrder = 0
      OnDrawCell = GridSubstDrawCell
      OnMouseWheelDown = GridSubstMouseWheelDown
      OnMouseWheelUp = GridSubstMouseWheelUp
      OnSelectCell = GridSubstSelectCell
    end
    object MemoInplace: TMemo
      Left = 94
      Top = 30
      Width = 83
      Height = 35
      BevelEdges = []
      Color = clCream
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
      Font.Style = []
      Lines.Strings = (
        '* Replacement profile.'
        '* file: <TA Folder>\Plugins\Replacements\names.txt'
        '*'
        '* Basic syntax is similar to TAHelper'#39's name replacements. '
        '* Lines starting with * character are ignored as "comments".'
        
          '* Original and New text should be in one line separated by Tab c' +
          'haracter.'
        
          '* If New text is not present then Original text just removed (i.' +
          'e. replaced'
        '* with empty string).'
        '* Example:'
        'OldText1'#9'NewText1'
        'OldText2'#9'NewText2'
        '* . . .'
        'TextToRemove1'
        'TextToRemove2'
        '*'
        '* By default, entries are treated as regular expressions.'
        '* This behaviour can be changed by [TEXT] and [REGEX] tags.'
        '* Entries after [TEXT] tag will be treated as plain text.'
        
          '* Entries after [REGEX] tag will be treated as regular expressio' +
          'ns.'
        '* Example:'
        'FindRegex1'#9'ReplaceRegex1'
        'FindRegex2'#9'ReplaceRegex2'
        '[TEXT]'
        'OldText1'#9'NewText1'
        'OldText2'#9'NewText2'
        'OldText3'#9'NewText3'
        '[REGEX]'
        'FindRegex3'#9'ReplaceRegex3'
        'FindRegex4'#9'ReplaceRegex4'
        'FindRegex5'#9'ReplaceRegex5')
      ParentFont = False
      TabOrder = 1
      WantReturns = False
      WordWrap = False
      OnExit = MemoInplaceExit
      OnKeyDown = MemoInplaceKeyDown
    end
    object PanelSubstList: TPanel
      Left = 12
      Top = 212
      Width = 363
      Height = 42
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 2
      DesignSize = (
        363
        42)
      object ButtonListDelete: TButton
        Left = 215
        Top = 9
        Width = 70
        Height = 25
        Anchors = [akRight, akBottom]
        Caption = 'Delete'
        TabOrder = 1
        OnClick = ButtonListDeleteClick
      end
      object ButtonListClear: TButton
        Left = 291
        Top = 9
        Width = 70
        Height = 25
        Anchors = [akRight, akBottom]
        Caption = 'Clear'
        TabOrder = 2
        OnClick = ButtonListClearClick
      end
      object ButtonListInsert: TButton
        Left = 139
        Top = 9
        Width = 70
        Height = 25
        Anchors = [akRight, akBottom]
        Caption = 'Insert'
        TabOrder = 0
        OnClick = ButtonListInsertClick
      end
    end
  end
  object TrayIcon1: TTrayIcon
    Hint = 'TAHplugin'
    Icon.Data = {
      0000010001002020100000000400E80200001600000028000000200000004000
      0000010004000000000000020000000000000000000010000000100000000000
      000000008000008000000080800080000000800080008080000080808000C0C0
      C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
      00000000000000000000000000000000000000000000000000000000000000FF
      FFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FF
      FFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFCCCCFFFFFFFFFFFF0000FF
      FFFFFFFFFFCCCCFFFFFFFFFFFF0000FFFFFFFFFFFFCCCCCCCCCCFFFFFF0000FF
      FFFFFFFFFFCCCCCCCCCCCFFFFF0000FFFFFFFFFFFFCCCCCCCCCCCFFFFF0000FF
      FFFFFFFFFFCCCCCCCCCCCFFFFF0000FFFFFFFFFFFFCCCCFFFCCCCFFFFF0000FF
      FFFFFFFFFFCCCCFFFCCCCFFFFF0000FFFFFFFFFFFFCCCCFFFCCCCFFFFF0000FF
      FFFFFFFFFFCCCCFFFCCCCFFFFF0000FFFFFFFFFFFFCCCCCCCCCCCFFFFF0000FF
      FFFFFFFFFFCCCCCCCCCCCFFFFF0000FFFFFFFFFFFFCCCCCCCCCCCFFFFF0000FF
      FFFFFFFFFFCCCCCCCCCCFFFFFF0000FFFFFFFFFFFF9999FFFFFFFFFFFF0000FF
      FF99FFFFFF9999FFFFFF99FFFF0000FFFF99FFFFFF9999FFFFFF99FFFF0000FF
      FF99FFFFFF9999FFFFFF99FFFF0000FFFF99FFFFFF9999FFFFFF99FFFF0000FF
      FF99999999999999999999FFFF0000FFFF99999999999999999999FFFF0000FF
      FF99999999999999999999FFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FF
      FFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FFFF800000018000000180000001800000018000000180000001800000018000
      0001800000018000000180000001800000018000000180000001800000018000
      0001800000018000000180000001800000018000000180000001800000018000
      0001800000018000000180000001800000018000000180000001FFFFFFFF}
    Visible = True
    OnClick = TrayIcon1Click
    OnMouseUp = TrayIcon1MouseUp
    Left = 192
    Top = 106
  end
  object PopupMenuListInsert: TPopupMenu
    AutoHotkeys = maManual
    AutoLineReduction = maManual
    AutoPopup = False
    Left = 238
    Top = 106
    object EXT1: TMenuItem
      Caption = '[TEXT]'
      OnClick = REGEX1Click
    end
    object REGEX1: TMenuItem
      Caption = '[REGEX]'
      OnClick = REGEX1Click
    end
  end
  object PopupMenuTray: TPopupMenu
    Left = 280
    Top = 108
    object Options1: TMenuItem
      Caption = 'Options'
      OnClick = Options1Click
    end
    object Substitutions1: TMenuItem
      Caption = 'Substitutions'
      OnClick = Substitutions1Click
    end
  end
  object TimerStartup: TTimer
    Enabled = False
    OnTimer = TimerStartupTimer
    Left = 330
    Top = 106
  end
end
