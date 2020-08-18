/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     StringPlayer
**
** Module:      TFrmAggLogger.cpp
**
*******************************************************************************/
#include "Target.h"

#include <vcl.h>
#pragma hdrstop

#include "TFrmAggLogger.h"
#include "TMainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmAggLogger *frmAggLogger;
//---------------------------------------------------------------------------
__fastcall TfrmAggLogger::TfrmAggLogger(TComponent* Owner)
    : TForm(Owner)
{
#if 0
    int left,top;

    left = frmMain->Left;
    top = frmMain->Top + frmMain->Height;
    Left = left;
    Top = top;
#endif
    mfIsVisible = false;
    mfUpdateWindow = false;
    mfTimestamp = false;
    mLastType = 0;
    mMaxMemoLines=MAX_LINES_IN_AGGREGATE_WINDOW;
    Hide();
}
//---------------------------------------------------------------------------

//=============================================================================
//=============================================================================
void TfrmAggLogger::ScrollToEnd(TRichEdit *pRichEdit)
{
    //the following code scrolls the window if it is full when the new empty
    //line is added.

    pRichEdit->HideSelection = false;
    pRichEdit->SelLength = 0;
    pRichEdit->SelStart = pRichEdit->GetTextLen();
    ::SendMessage(pRichEdit->Handle, EM_SCROLLCARET, 0, 0);
}

//=============================================================================
//=============================================================================
void TfrmAggLogger::LimitContent(TRichEdit *pRichEdit)
{
    //keep memo view to have sensible number of lines
    while (pRichEdit->Lines->Count > mMaxMemoLines)
    {
        //delete the oldest line
        pRichEdit->Lines->Delete(0);
    }
}


//=============================================================================
//=============================================================================
void TfrmAggLogger::LogWriteLine(int port,TColor color, AnsiString asLine,bool fOutgoing)
{
    AnsiString asLineLabelled;

    try
    {
        LimitContent(reMemo);

        ScrollToEnd(reMemo);

        if(mfTimestamp)
        {
            //add a timediff stamp to the log line
            unsigned int nTickMs = GetTickCount() % 100000;  /* only interested in 100 seconds */
            if( fOutgoing )
            {
                asLineLabelled.printf(">%d>%05d>",port,nTickMs);
            }
            else
            {
                asLineLabelled.printf("<%d<%05d<",port,nTickMs);
            }
        }
        else
        {
            if( fOutgoing )
            {
                asLineLabelled.printf(">%d>",port);
            }
            else
            {
                asLineLabelled.printf("<%d<",port);
            }
        }
        asLineLabelled += asLine;

        reMemo->SelAttributes->Color = color;
        reMemo->Lines->Add(asLineLabelled);
    }
    catch( ... )
    {
    }
}

//=============================================================================
//=============================================================================
void TfrmAggLogger::DumpToLogFile(FILE *fp, unsigned int nLines)
{
    int nTotalLines  = reMemo->Lines->Count;
    int nLinesToDump = nLines;
    int nBegLine;
    int nIndex;
    AnsiString asLine;

    if(nLines<=0)
    {
        return;
    }

    if( nLinesToDump >  nTotalLines )
    {
        nLinesToDump = nTotalLines;
    }
    nBegLine = nTotalLines - nLinesToDump;

    MiscWriteLogFile(fp, LOG_COLOR, "++++++++++++++++++++++++++ COMMS ACTIVITY LOG BEGIN ++++++++++++++++++++++++++");
    for(nIndex=nBegLine;nIndex<nTotalLines;nIndex++)
    {
        asLine = reMemo->Lines->Strings[nIndex];
        MiscWriteLogFile(fp, LOG_COLOR, asLine);
    }
    MiscWriteLogFile(fp, LOG_COLOR, "++++++++++++++++++++++++++ COMMS ACTIVITY LOG END ++++++++++++++++++++++++++++");

}

//---------------------------------------------------------------------------
void __fastcall TfrmAggLogger::FormShow(TObject *Sender)
{
    mfIsVisible = true;
    mfUpdateWindow = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmAggLogger::FormHide(TObject *Sender)
{
    mfIsVisible = false;
    mfUpdateWindow = false;
}

//---------------------------------------------------------------------------
void __fastcall TfrmAggLogger::ShowLogger(int left,int top)
{
    Left = left;
    Top  = Top;
    Show();
}
