object Form2: TForm2
  Left = 0
  Top = 0
  Caption = 'Form2'
  ClientHeight = 260
  ClientWidth = 739
  Color = cl3DLight
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 32
    Width = 126
    Height = 16
    Caption = #1056#1072#1079#1084#1077#1088' '#1082#1083#1072#1089#1090#1077#1088#1072' ('#1041'):'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 147
    Top = 32
    Width = 4
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 16
    Top = 10
    Width = 49
    Height = 16
    Caption = #1058#1080#1087' '#1060#1057':'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 71
    Top = 10
    Width = 4
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 16
    Top = 54
    Width = 141
    Height = 16
    Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1082#1083#1072#1089#1090#1077#1088#1086#1074': '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label6: TLabel
    Left = 160
    Top = 54
    Width = 4
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label7: TLabel
    Left = 16
    Top = 76
    Width = 98
    Height = 16
    Caption = #1056#1072#1079#1084#1077#1088' '#1060#1057' ('#1043#1041'):'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label8: TLabel
    Left = 119
    Top = 76
    Width = 4
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label9: TLabel
    Left = 381
    Top = 10
    Width = 206
    Height = 16
    Caption = #1055#1086#1080#1089#1082' '#1082#1083#1072#1089#1090#1077#1088#1086#1074' '#1087#1086' '#1089#1080#1075#1085#1072#1090#1091#1088#1077
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label10: TLabel
    Left = 200
    Top = 236
    Width = 237
    Height = 16
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Button1: TButton
    Left = 472
    Top = 162
    Width = 84
    Height = 33
    Caption = #1053#1072#1095#1072#1090#1100' '#1087#1086#1080#1089#1082
    TabOrder = 0
    OnClick = Button1Click
  end
  object VirtualStringTree1: TVirtualStringTree
    Left = 472
    Top = 32
    Width = 259
    Height = 124
    BorderWidth = 1
    Header.AutoSizeIndex = 0
    Header.Height = 20
    Header.Options = [hoColumnResize, hoDrag, hoShowSortGlyphs, hoVisible]
    TabOrder = 1
    TreeOptions.PaintOptions = [toShowButtons, toShowDropmark, toShowHorzGridLines, toShowRoot, toShowTreeLines, toShowVertGridLines, toThemeAware, toUseBlendedImages]
    TreeOptions.SelectionOptions = [toFullRowSelect]
    OnAddToSelection = VirtualStringTree1AddToSelection
    OnGetText = VirtualStringTree1GetText
    Touch.InteractiveGestures = [igPan, igPressAndTap]
    Touch.InteractiveGestureOptions = [igoPanSingleFingerHorizontal, igoPanSingleFingerVertical, igoPanInertia, igoPanGutter, igoParentPassthrough]
    Columns = <
      item
        Position = 0
        Text = #1053#1086#1084#1077#1088' '#1082#1083#1072#1089#1090#1077#1088#1072
        Width = 94
      end
      item
        Position = 1
        Text = #1056#1072#1089#1096#1080#1088#1077#1085#1080#1077' '#1092#1072#1081#1083#1072
        Width = 153
      end>
  end
  object GroupBox1: TGroupBox
    Left = 266
    Top = 76
    Width = 147
    Height = 121
    Caption = #1042#1080#1076#1099' '#1092#1072#1081#1083#1086#1074
    Color = cl3DLight
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentColor = False
    ParentFont = False
    TabOrder = 2
    object RadioButton1: TRadioButton
      Left = 25
      Top = 25
      Width = 113
      Height = 17
      Caption = #1048#1079#1086#1073#1088#1072#1078#1077#1085#1080#1103
      Checked = True
      TabOrder = 0
      TabStop = True
      OnClick = RadioButton1Click
    end
    object RadioButton2: TRadioButton
      Left = 25
      Top = 48
      Width = 113
      Height = 17
      Caption = #1040#1091#1076#1080#1086
      TabOrder = 1
      OnClick = RadioButton2Click
    end
    object RadioButton3: TRadioButton
      Left = 25
      Top = 71
      Width = 113
      Height = 17
      Caption = #1042#1080#1076#1077#1086
      TabOrder = 2
      OnClick = RadioButton3Click
    end
    object RadioButton4: TRadioButton
      Left = 25
      Top = 94
      Width = 113
      Height = 17
      Caption = #1044#1086#1082#1091#1084#1077#1085#1090#1099
      TabOrder = 3
      OnClick = RadioButton4Click
    end
  end
  object ComboBox1: TComboBox
    Left = 266
    Top = 32
    Width = 171
    Height = 21
    TabOrder = 3
    Text = #1050#1072#1082#1080#1077' '#1082#1083#1072#1089#1090#1077#1088#1099' '#1086#1073#1093#1086#1076#1080#1090#1100' ...'
    Items.Strings = (
      #1074#1089#1077' '#1082#1083#1072#1089#1090#1077#1088#1099
      #1090#1086#1083#1100#1082#1086' '#1085#1077#1087#1091#1089#1090#1099#1077)
  end
  object Button2: TButton
    Left = 642
    Top = 162
    Width = 89
    Height = 33
    Caption = #1059#1076#1072#1083#1080#1090#1100' '#1079#1072#1087#1080#1089#1100
    Enabled = False
    TabOrder = 4
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 642
    Top = 201
    Width = 89
    Height = 32
    Caption = #1054#1095#1080#1089#1090#1080#1090#1100' '#1074#1089#1077
    Enabled = False
    TabOrder = 5
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 472
    Top = 201
    Width = 84
    Height = 32
    Caption = #1054#1089#1090#1072#1085#1086#1074#1080#1090#1100
    Enabled = False
    TabOrder = 6
    OnClick = Button4Click
  end
  object ProgressBar1: TProgressBar
    Left = 236
    Top = 213
    Width = 201
    Height = 17
    Step = 5
    TabOrder = 7
  end
end
