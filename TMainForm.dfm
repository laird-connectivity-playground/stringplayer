object frmMain: TfrmMain
  Left = 196
  Top = 628
  Width = 620
  Height = 383
  Caption = 'See Constructor'
  Color = clBtnFace
  DefaultMonitor = dmPrimary
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 604
    Height = 29
    Caption = 'ToolBar1'
    TabOrder = 0
    object btnQuit: TButton
      Left = 0
      Top = 2
      Width = 38
      Height = 22
      Caption = 'Quit'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
      OnClick = btnQuitClick
    end
    object btnPause: TButton
      Left = 38
      Top = 2
      Width = 44
      Height = 22
      Caption = 'Pause'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
      OnClick = btnPauseClick
    end
    object btnSkip: TButton
      Left = 82
      Top = 2
      Width = 40
      Height = 22
      Caption = 'Skip'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 10
      OnClick = btnSkipClick
    end
    object Panel1: TPanel
      Left = 122
      Top = 2
      Width = 59
      Height = 22
      TabOrder = 3
      object shpCTS0: TShape
        Left = 8
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label6: TLabel
        Left = 8
        Top = 0
        Width = 11
        Height = 10
        Caption = 'CT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label7: TLabel
        Left = 21
        Top = 0
        Width = 11
        Height = 10
        Caption = 'DS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDSR0: TShape
        Left = 21
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label8: TLabel
        Left = 34
        Top = 0
        Width = 12
        Height = 10
        Caption = 'DC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDCD0: TShape
        Left = 34
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label9: TLabel
        Left = 48
        Top = 0
        Width = 8
        Height = 10
        Caption = 'RI'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpRI0: TShape
        Left = 47
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label10: TLabel
        Left = 0
        Top = -1
        Width = 8
        Height = 13
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object Panel2: TPanel
      Left = 181
      Top = 2
      Width = 59
      Height = 22
      TabOrder = 2
      object shpCTS1: TShape
        Left = 8
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label1: TLabel
        Left = 9
        Top = 0
        Width = 11
        Height = 10
        Caption = 'CT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label2: TLabel
        Left = 22
        Top = 0
        Width = 11
        Height = 10
        Caption = 'DS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDSR1: TShape
        Left = 22
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label3: TLabel
        Left = 35
        Top = 0
        Width = 12
        Height = 10
        Caption = 'DC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDCD1: TShape
        Left = 35
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label4: TLabel
        Left = 48
        Top = 0
        Width = 8
        Height = 10
        Caption = 'RI'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpRI1: TShape
        Left = 47
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label5: TLabel
        Left = 1
        Top = -1
        Width = 8
        Height = 13
        Caption = '1'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object Panel8: TPanel
      Left = 240
      Top = 2
      Width = 59
      Height = 22
      TabOrder = 9
      object shpCTS2: TShape
        Left = 8
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label36: TLabel
        Left = 8
        Top = 0
        Width = 11
        Height = 10
        Caption = 'CT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label37: TLabel
        Left = 21
        Top = 0
        Width = 11
        Height = 10
        Caption = 'DS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDSR2: TShape
        Left = 21
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label38: TLabel
        Left = 34
        Top = 0
        Width = 12
        Height = 10
        Caption = 'DC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDCD2: TShape
        Left = 34
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label39: TLabel
        Left = 48
        Top = 0
        Width = 8
        Height = 10
        Caption = 'RI'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpRI2: TShape
        Left = 47
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label40: TLabel
        Left = 0
        Top = -1
        Width = 8
        Height = 13
        Caption = '2'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object Panel7: TPanel
      Left = 299
      Top = 2
      Width = 59
      Height = 22
      TabOrder = 8
      object shpCTS3: TShape
        Left = 8
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label31: TLabel
        Left = 8
        Top = 0
        Width = 11
        Height = 10
        Caption = 'CT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label32: TLabel
        Left = 21
        Top = 0
        Width = 11
        Height = 10
        Caption = 'DS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDSR3: TShape
        Left = 21
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label33: TLabel
        Left = 34
        Top = 0
        Width = 12
        Height = 10
        Caption = 'DC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDCD3: TShape
        Left = 34
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label34: TLabel
        Left = 48
        Top = 0
        Width = 8
        Height = 10
        Caption = 'RI'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpRI3: TShape
        Left = 47
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label35: TLabel
        Left = 0
        Top = -1
        Width = 8
        Height = 13
        Caption = '3'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object Panel6: TPanel
      Left = 358
      Top = 2
      Width = 59
      Height = 22
      TabOrder = 7
      object shpCTS4: TShape
        Left = 8
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label26: TLabel
        Left = 8
        Top = 0
        Width = 11
        Height = 10
        Caption = 'CT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label27: TLabel
        Left = 21
        Top = 0
        Width = 11
        Height = 10
        Caption = 'DS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDSR4: TShape
        Left = 21
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label28: TLabel
        Left = 34
        Top = 0
        Width = 12
        Height = 10
        Caption = 'DC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDCD4: TShape
        Left = 34
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label29: TLabel
        Left = 48
        Top = 0
        Width = 8
        Height = 10
        Caption = 'RI'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpRI4: TShape
        Left = 47
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label30: TLabel
        Left = 0
        Top = -1
        Width = 8
        Height = 13
        Caption = '4'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object Panel5: TPanel
      Left = 417
      Top = 2
      Width = 62
      Height = 22
      TabOrder = 6
      object shpCTS5: TShape
        Left = 8
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label21: TLabel
        Left = 8
        Top = 0
        Width = 11
        Height = 10
        Caption = 'CT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label22: TLabel
        Left = 21
        Top = 0
        Width = 11
        Height = 10
        Caption = 'DS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDSR5: TShape
        Left = 21
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label23: TLabel
        Left = 34
        Top = 0
        Width = 12
        Height = 10
        Caption = 'DC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDCD5: TShape
        Left = 34
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label24: TLabel
        Left = 48
        Top = 0
        Width = 8
        Height = 10
        Caption = 'RI'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpRI5: TShape
        Left = 47
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label25: TLabel
        Left = 0
        Top = -1
        Width = 8
        Height = 13
        Caption = '5'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object Panel4: TPanel
      Left = 479
      Top = 2
      Width = 59
      Height = 22
      TabOrder = 5
      object shpCTS6: TShape
        Left = 8
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label16: TLabel
        Left = 8
        Top = 0
        Width = 11
        Height = 10
        Caption = 'CT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label17: TLabel
        Left = 21
        Top = 0
        Width = 11
        Height = 10
        Caption = 'DS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDSR6: TShape
        Left = 21
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label18: TLabel
        Left = 34
        Top = 0
        Width = 12
        Height = 10
        Caption = 'DC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDCD6: TShape
        Left = 34
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label19: TLabel
        Left = 48
        Top = 0
        Width = 8
        Height = 10
        Caption = 'RI'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpRI6: TShape
        Left = 47
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label20: TLabel
        Left = 0
        Top = -1
        Width = 8
        Height = 13
        Caption = '6'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
    object Panel3: TPanel
      Left = 538
      Top = 2
      Width = 59
      Height = 22
      TabOrder = 4
      object shpCTS7: TShape
        Left = 6
        Top = 11
        Width = 13
        Height = 8
        Shape = stCircle
      end
      object Label11: TLabel
        Left = 8
        Top = 0
        Width = 11
        Height = 10
        Caption = 'CT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object Label12: TLabel
        Left = 21
        Top = 0
        Width = 11
        Height = 10
        Caption = 'DS'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDSR7: TShape
        Left = 21
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label13: TLabel
        Left = 34
        Top = 0
        Width = 12
        Height = 10
        Caption = 'DC'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpDCD7: TShape
        Left = 34
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label14: TLabel
        Left = 48
        Top = 0
        Width = 8
        Height = 10
        Caption = 'RI'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -8
        Font.Name = 'Arial'
        Font.Style = []
        ParentFont = False
      end
      object shpRI7: TShape
        Left = 47
        Top = 11
        Width = 10
        Height = 8
        Shape = stCircle
      end
      object Label15: TLabel
        Left = 0
        Top = -1
        Width = 8
        Height = 13
        Caption = '7'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -8
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
  end
  object statusBar: TStatusBar
    Left = 0
    Top = 326
    Width = 604
    Height = 19
    Panels = <
      item
        Text = 'PC:12345'
        Width = 65
      end
      item
        Text = 'SZ:12345'
        Width = 65
      end
      item
        Text = 'FAIL:0'
        Width = 175
      end
      item
        Text = 'TEST:'
        Width = 200
      end>
    SimplePanel = False
  end
  object panel: TPanel
    Left = 0
    Top = 29
    Width = 604
    Height = 297
    Align = alClient
    Caption = 'panel'
    TabOrder = 2
    object reMemo: TRichEdit
      Left = 1
      Top = 1
      Width = 602
      Height = 295
      Align = alClient
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      Lines.Strings = (
        'reMemo')
      MaxLength = 16384
      ParentFont = False
      ReadOnly = True
      ScrollBars = ssBoth
      TabOrder = 0
    end
  end
  object tmrOnStart: TTimer
    Interval = 100
    OnTimer = tmrOnStartTimer
    Left = 336
    Top = 37
  end
  object OpenDialog1: TOpenDialog
    Filter = 'Stringplayer Files|*.scr;*.sub;*.sbr'
    Left = 368
    Top = 37
  end
  object tmrUpTime: TTimer
    OnTimer = tmrUpTimeTimer
    Left = 400
    Top = 40
  end
end
