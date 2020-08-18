/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     StringPlayer
**
** Module:      TMainForm.cpp
**
*******************************************************************************/
#include "Target.h"

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include <Filectrl.hpp>

#include "TMainForm.h"
#include "TFrmAggLogger.h"
#include "TScriptSource.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#pragma package(smart_init)
#pragma link "AdPort"
#pragma link "OoMisc"
#pragma resource "*.dfm"
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
TfrmMain *frmMain;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

//=============================================================================
//=============================================================================
int TracePrint(char *pFmt,...)
{
    int cnt=0;

    if( frmMain->mpRegression )
    {
        if( frmMain->mpRegression->mfTrace )
        {
            char baPrnBuf[512];

            va_list argptr;

            va_start(argptr, pFmt);
            cnt = vsprintf(baPrnBuf, pFmt, argptr);
            va_end(argptr);

            frmMain->DisplayTraceMessage(baPrnBuf);
        }
    }
    return(cnt);
}

//=============================================================================
//=============================================================================
void TfrmMain::ScrollToEnd(TRichEdit *pRichEdit)
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
void TfrmMain::LimitContent(TRichEdit *pRichEdit)
{
    //keep memo view to have sensible number of lines
    while (pRichEdit->Lines->Count > mpRegression->mMaxMemoLines)
    {
        //delete the oldest line
        pRichEdit->Lines->Delete(0);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayWriteLine(TColor color, AnsiString asLine)
{
    LimitContent(reMemo);

    ScrollToEnd(reMemo);
    reMemo->SelAttributes->Color = color;
    reMemo->Lines->Add(asLine);
    MiscWriteLogFile(mfpLog, color, asLine);
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayTraceMessage(char *pMsg)
{
    if( fUpdateConsole )
    {
        DisplayWriteLine(TRACE_COLOR, pMsg);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayUserMessage(char *pMsg)
{
    if( fUpdateConsole )
    {
        AnsiString asRxMsg;
        SrcEscapeString(pMsg,asRxMsg);
        DisplayWriteLine(MESSAGE_COLOR, asRxMsg);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayUserMessage(char *pMsg,TColor color)
{
    if( fUpdateConsole )
    {
        AnsiString asRxMsg;
        SrcEscapeString(pMsg,asRxMsg);
        DisplayWriteLine(color, asRxMsg);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayFailMessage(char *pMsg)
{
    if( fUpdateConsole )
    {
        AnsiString asRxMsg;
        SrcEscapeString(pMsg,asRxMsg);
        DisplayWriteLine(FAIL_COLOR, asRxMsg);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayFailMessage(char *pMsg, bool fEscaped)
{
    if( fUpdateConsole )
    {
        AnsiString asRxMsg;
        if( fEscaped )
        {
            SrcEscapeString(pMsg,asRxMsg);
        }
        else
        {
            asRxMsg = pMsg;
        }
        DisplayWriteLine(FAIL_COLOR, asRxMsg);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayPrintMessage(char *pMsg, bool fEscaped)
{
    if( fUpdateConsole )
    {
        if(fEscaped)
        {
            AnsiString asMsg;
            SrcEscapeString(pMsg,asMsg);
            DisplayWriteLine(PRINT_COLOR, asMsg);
        }
        else
        {
            DisplayWriteLine(PRINT_COLOR, pMsg);
        }
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayCommand(unsigned int nModule,char *pCmd)
{
    if( fUpdateConsole )
    {
        AnsiString asLine;
        AnsiString asCmd;
        SrcEscapeString(pCmd,asCmd);
        asLine.printf("%d>",nModule);
        asLine += asCmd;
        DisplayWriteLine(CMDPKT_COLOR, asLine);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayCommand(unsigned int nModule,char *pHdr, char *pData)
{
    if( fUpdateConsole )
    {
        AnsiString asLine;
        AnsiString asCmd;

        asLine.printf("%d>",nModule);
        SrcEscapeString(pHdr,asCmd);
        asLine += asCmd;
        SrcEscapeString(pData,asCmd);
        asLine += asCmd;
        DisplayWriteLine(CMDPKT_COLOR, asLine);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayUnicodeCommand(unsigned int nModule,char *pCmd)
{
    if( fUpdateConsole )
    {
        AnsiString asLine;
        AnsiString asCmd;
        SrcEscapeString(pCmd,asCmd);
        asLine.printf("%d:UNICODED>",nModule);
        asLine += asCmd;
        DisplayWriteLine(CMDPKT_COLOR, asLine);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayResponse(unsigned int nModule,char *pResp)
{
    if( fUpdateConsole )
    {
        AnsiString asLine;
        AnsiString asRxMsg;
        SrcEscapeString(pResp,asRxMsg);
        asLine.printf("Got: %d<",nModule);
        asLine += asRxMsg;
        DisplayWriteLine(RSPPKT_COLOR, asLine);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayChannelResponse(unsigned int nModule, int channel, char *pResp)
{
    if( fUpdateConsole )
    {
        AnsiString asLine;
        AnsiString asRxMsg;
        SrcEscapeString(pResp,asRxMsg);
        asLine.printf("Got: %d[chn:%d]<",nModule,channel);
        asLine += asRxMsg;
        DisplayWriteLine(RSPPKT_COLOR, asLine);
    }
}

//=============================================================================
//=============================================================================
void  TfrmMain::OnAccept(void)
{
    reMemo->Clear();

    ThrowStateMachineEvent(WIZMNGR_INIT,0);
}

//=============================================================================
//=============================================================================
void TfrmMain::StatusText(AnsiString asMsg)
{
    statusBar->SimpleText = asMsg;
}

//=============================================================================
//=============================================================================
bool ProcessApplicationMessages(void)
{
    //Returns true if application is to be closed
    Application->ProcessMessages();

    return frmMain->mfCloseApp;
}

//=============================================================================
//=============================================================================
void TfrmMain::StatusBarProgramCounter(int nPc)
{
    char baMsg[24];
    sprintf(baMsg,"PC: %d",nPc);
    statusBar->Panels->Items[0]->Text = baMsg;
}

//=============================================================================
//=============================================================================
void TfrmMain::StatusBarScriptSize(int nSize)
{
    char baMsg[24];
    sprintf(baMsg,"SZ: %d",nSize);
    statusBar->Panels->Items[1]->Text = baMsg;
}

//=============================================================================
//=============================================================================
void TfrmMain::StatusBarFailCount(int nFailCount, int nRetries, int maxRetries)
{
    char baMsg[64];
    if(  maxRetries <= 1 )
    {
        sprintf(baMsg,"FAIL:%d",nFailCount);
    }
    else
    {
        sprintf(baMsg,"FAIL:%d(%d) (MAXTRY:%d)",nFailCount,nRetries,maxRetries);
    }
    statusBar->Panels->Items[2]->Text = baMsg;
}

//=============================================================================
//=============================================================================
void TfrmMain::DisplayScriptFilename(AnsiString asFilename)
{
    AnsiString asMsg;
    AnsiString asNewCaption;
    int  nLength = asFilename.Length();
    int  nDispLen = 40;
    int  nIndex;
    if(nLength > nDispLen)
    {
        nIndex = nLength - nDispLen+1;
        asMsg = "...";
        asMsg += asFilename.SubString(nIndex,nDispLen);
    }
    else
    {
        asMsg =  asFilename;
    }
    asNewCaption = TITLE_BAR_TEXT;
    asNewCaption += " - ";
    asNewCaption += asMsg;

    masWinTitle =  asNewCaption;
    mfWinTitle = true;
    Caption = asNewCaption;
    //statusBar->Panels->Items[4]->Text = asFilename;
}

//=============================================================================
//=============================================================================
void TfrmMain::StatusBarTestNumId(AnsiString asTestId)
{
    statusBar->Panels->Items[3]->Text = asTestId;
}

//=============================================================================
//=============================================================================
void TfrmMain::UpdateModemDisplay(int nId, bool fOpen, bool fCTS, bool fDSR, bool fRI, bool fDCD)
{
    TShape *pShpCTS;
    TShape *pShpDSR;
    TShape *pShpRI;
    TShape *pShpDCD;

    switch(nId)
    {
    case 0:
        pShpCTS = shpCTS0;
        pShpDSR = shpDSR0;
        pShpRI  = shpRI0;
        pShpDCD = shpDCD0;
        break;
    case 1:
        pShpCTS = shpCTS1;
        pShpDSR = shpDSR1;
        pShpRI  = shpRI1;
        pShpDCD = shpDCD1;
        break;
    case 2:
        pShpCTS = shpCTS2;
        pShpDSR = shpDSR2;
        pShpRI  = shpRI2;
        pShpDCD = shpDCD2;
        break;
    case 3:
        pShpCTS = shpCTS3;
        pShpDSR = shpDSR3;
        pShpRI  = shpRI3;
        pShpDCD = shpDCD3;
        break;
    case 4:
        pShpCTS = shpCTS4;
        pShpDSR = shpDSR4;
        pShpRI  = shpRI4;
        pShpDCD = shpDCD4;
        break;
    case 5:
        pShpCTS = shpCTS5;
        pShpDSR = shpDSR5;
        pShpRI  = shpRI5;
        pShpDCD = shpDCD5;
        break;
    case 6:
        pShpCTS = shpCTS6;
        pShpDSR = shpDSR6;
        pShpRI  = shpRI6;
        pShpDCD = shpDCD6;
        break;
    case 7:
        pShpCTS = shpCTS7;
        pShpDSR = shpDSR7;
        pShpRI  = shpRI7;
        pShpDCD = shpDCD7;
        break;
    default:
        return;
    }

    if(fOpen)
    {
        pShpCTS->Brush->Color = fCTS ? ASSERTED_COLOR : DEASSERTED_COLOR;
        pShpDSR->Brush->Color = fDSR ? ASSERTED_COLOR : DEASSERTED_COLOR;
        pShpRI->Brush->Color  = fRI  ? ASSERTED_COLOR : DEASSERTED_COLOR;
        pShpDCD->Brush->Color = fDCD ? ASSERTED_COLOR : DEASSERTED_COLOR;
    }
    else
    {
        pShpCTS->Brush->Color = NOTOPEN_COLOR;
        pShpDSR->Brush->Color = NOTOPEN_COLOR;
        pShpRI->Brush->Color  = NOTOPEN_COLOR;
        pShpDCD->Brush->Color = NOTOPEN_COLOR;
    }

}

//=============================================================================
//=============================================================================
void TfrmMain::LogToFailFile(TColor color, AnsiString asLine)
{
    if(mfpFailLog)
    {
        MiscWriteLogFile(mfpFailLog, color, asLine);
    }
}

//=============================================================================
//=============================================================================
void TfrmMain::LogToPlainFailFile(AnsiString asLine)
{
    if( masFilePathFailTxtFile != "" )
    {
        FILE *fpFailTitle = MiscOpenLogFile(masFilePathFailTxtFile, true, false, false);
        if( fpFailTitle )
        {
            MiscWriteLogFilePlain(fpFailTitle, asLine);
            MiscCloseLogFile(fpFailTitle);
        }
    }
}

//=============================================================================
void TfrmMain::DumpToLogFile(FILE *fp, unsigned int nLines)
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

    MiscWriteLogFile(fp, LOG_COLOR, "++++++++++++++++++++++++++ STRINGPLAYER ACTIVITY LOG BEGIN ++++++++++++++++++++++++++");
    for(nIndex=nBegLine;nIndex<nTotalLines;nIndex++)
    {
        asLine = reMemo->Lines->Strings[nIndex];
        MiscWriteLogFile(fp, LOG_COLOR, asLine);
    }
    MiscWriteLogFile(fp, LOG_COLOR, "++++++++++++++++++++++++++ STRINGPLAYER  ACTIVITY LOG END ++++++++++++++++++++++++++++");

}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
    : TForm(Owner)
{
    //Update Title Bar
    Caption = TITLE_BAR_TEXT;
    mUpTimeHours=0;
    mUpTimeMins=0;
    mUpTimeSecs=0;
    mfWinTitle=false;

    mfpLog = NULL;
    mfpFailLog = NULL;

    fUpdateConsole = true;

    reMemo->Clear();

    statusBar->Panels->Items[0]->Text = "";
    statusBar->Panels->Items[1]->Text = "";
    statusBar->Panels->Items[2]->Text = "";
    statusBar->Panels->Items[3]->Text = "";

    mpRegression = new TRegression();

    btnPause->Enabled = false;
    btnSkip->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
    if(mpRegression)
    {
        delete mpRegression;
        mpRegression=NULL;
    }
    MiscCloseLogFile(mfpLog);
    mfpLog=NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::tmrOnStartTimer(TObject *Sender)
{
    tmrOnStart->Enabled=false;
#define AUTORUNFILENAME  "AutoRun.Scr"

    AnsiString asAutoRun = AUTORUNFILENAME;
    AnsiString as,asFolder;

    mpRegression->ReadCommandLineOptions();
    masFilePathFailTxtFile = "";

    if(frmAggLogger->mfIsVisible)
    {
        int scrWidth = Screen->Width;
        int totalw = Width + frmAggLogger->Width;
        if(totalw > scrWidth)
        {
            frmAggLogger->Width = scrWidth - Width;
            Left=0;
            frmAggLogger->Left   = Left+Width;
            frmAggLogger->Height = Height;
        }
        else if( frmAggLogger->Width <= ((scrWidth-Width)/2) )
        {
            frmAggLogger->Left   = Left+Width;
            frmAggLogger->Height = Height;
        }
        else
        {
            Left = (scrWidth-totalw)/2;
            frmAggLogger->Left   = Left+Width;
            frmAggLogger->Height = Height;
        }
    }

    if( mpRegression->GetScriptFileName() == "")
    {
        mpRegression->SetAppFolder(GetCurrentDir());

        if( ! FileExists(asAutoRun) )
        {
            if(OpenDialog1->Execute())
            {
                AnsiString asFname;
                as = OpenDialog1->FileName;
                asFolder = MiscExtractFolder( as, asFname);

                mpRegression->SetScriptFileName(asFname);
                mpRegression->SetScriptFolder(asFolder);
            }
        }
        else
        {
            mpRegression->SetScriptFileName(asAutoRun);
            mpRegression->SetScriptFolder(GetCurrentDir());
        }
    }
    reMemo->SetFocus();

    if( mpRegression->GetScriptFileName() != "")
    {
        AnsiString asFile = mpRegression->GetScriptFileName();
        AnsiString asLogFolder = mpRegression->GetLogFileFolder();
        if(asLogFolder == "")
        {
            asLogFolder = mpRegression->GetScriptFolder() + "Log";
        }

        if( ! DirectoryExists(asLogFolder) )
        {
            CreateDir(asLogFolder);
        }

        if( DirectoryExists(asLogFolder) )
        {
            AnsiString asFullLogFilename;
            AnsiString asFullLogFilePath;

            if(  asFile == asAutoRun )
            {
                asFullLogFilename = asLogFolder + "\\" + "AtProtocolTestLog";
                asFullLogFilePath = asFullLogFilename + ".htm";
                mfpLog = MiscOpenLogFile(asFullLogFilePath, true, true, true);
            }
            else
            {
                AnsiString asFileOnly = MiscExtractFilenameOnly(asFile);
                asFullLogFilename = asLogFolder + "\\" + asFileOnly;
                asFullLogFilePath = asFullLogFilename + ".htm";
                mfpLog = MiscOpenLogFile(asFullLogFilePath, false, true, true);
            }
            //save the name of logfile for debugging
            masFullLogFilePath = asFullLogFilePath;

            //open the log file where only the fail fragments are saved - as append write
            asFullLogFilePath = asFullLogFilename + "-FAIL-LOG.htm";
            mfpFailLog = MiscOpenLogFile(asFullLogFilePath, true, true, true);

            //open the log file where only the fail TITLES are saved - as append write
            masFilePathFailTxtFile = asFullLogFilename + "-FAIL-TITLE.txt";
            FILE *fpFailTitle = MiscOpenLogFile(masFilePathFailTxtFile, true, true, false);
            MiscCloseLogFile(fpFailTitle);

            OnAccept();
        }
        else
        {
            DisplayUserMessage("Log Folder does not exist and cannot be created");
            DisplayUserMessage("Regression test cannot be performed");
        }
    }
    else
    {
        DisplayUserMessage("Script Filename not specified in command line");
        DisplayUserMessage("Regression test cannot be performed");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnQuitClick(TObject *Sender)
{
    Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnPauseClick(TObject *Sender)
{
    if( btnPause->Caption == "Pause" )
    {   //Pause
        mpRegression->PauseScript();
        btnPause->Caption = "Resume";
    }
    else
    {   //Resume
        mpRegression->ResumeScript();
        btnPause->Caption = "Pause";
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::tmrUpTimeTimer(TObject *Sender)
{
    AnsiString asTime;
    AnsiString asCap;

    mUpTimeSecs++;
    if(mUpTimeSecs>=60)
    {
        mUpTimeSecs=0;
        mUpTimeMins++;
        if(mUpTimeMins>=60)
        {
            mUpTimeMins=0;
            mUpTimeHours++;
        }
    }

    if( mfWinTitle )
    {
        asCap  = masWinTitle;
    }
    else
    {
        asCap  = TITLE_BAR_TEXT;
    }
    asTime.printf("  [%d:%02d:%02d]",mUpTimeHours,mUpTimeMins,mUpTimeSecs);
    asCap += asTime;
    Caption = asCap;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    frmMain->mfCloseApp = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnSkipClick(TObject *Sender)
{
    mpRegression->DoSkip();
}
//---------------------------------------------------------------------------


