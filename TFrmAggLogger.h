/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     StringPlayer
**
** Module:      TFrmAggLogger.h
**
*******************************************************************************/
//---------------------------------------------------------------------------

#ifndef TFrmAggLoggerH
#define TFrmAggLoggerH
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

#define MAX_LINES_IN_AGGREGATE_WINDOW    500

//---------------------------------------------------------------------------
class TfrmAggLogger : public TForm
{
__published:	// IDE-managed Components
    TRichEdit *reMemo;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
private:	// User declarations
    void ScrollToEnd(TRichEdit *pRichEdit);
    int  mLastType;

public:		// User declarations
    __fastcall TfrmAggLogger(TComponent* Owner);

    void LogWriteLine(int port,TColor color, AnsiString asLine,bool fOutgoing);
    void __fastcall TfrmAggLogger::ShowLogger(int left,int top);

    void DumpToLogFile(FILE *fp, unsigned int nLines);
    void LimitContent(TRichEdit *pRichEdit)  ;

    bool IsShown(){return mfIsVisible;};

    bool mfIsVisible;
    bool mfUpdateWindow;
    bool mfTimestamp;
    int  mMaxMemoLines;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmAggLogger *frmAggLogger;
//---------------------------------------------------------------------------
#endif
