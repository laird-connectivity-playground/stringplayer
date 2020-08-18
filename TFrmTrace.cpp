/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     StringPlayer
**
** Module:      TFrmTrace.cpp
**
*******************************************************************************/
#include "Target.h"

#include <vcl.h>
#pragma hdrstop

#include "TFrmTrace.h"
#include "TFrmAggLogger.h"
#include "Misc.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfrmTrace::TfrmTrace(TComponent* Owner)
    : TForm(Owner)
{
    mfIsVisible = false;
    mLastType = 0;
    mfSend=false;
    reMemo->Lines->Add("****");
    Hide();
}
//---------------------------------------------------------------------------

//=============================================================================
//=============================================================================
void TfrmTrace::ScrollToEnd(TRichEdit *pRichEdit)
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
void TfrmTrace::AppendToLastLine(AnsiString asNewText)
{
    //the following code scrolls the window if it is full when the new empty
    //line is added.

    reMemo->HideSelection = false;
    reMemo->SelStart = reMemo->GetTextLen();
    reMemo->SelLength = 1;
    reMemo->SelText = asNewText;
}

//=============================================================================
//=============================================================================
void TfrmTrace::EscapeCharacter(unsigned char ch, char *pDest)
{
    if(ch == '\r')
    {
        strcpy(pDest,"\\r\r");
    }
    else if(ch == '\n')
    {
        strcpy(pDest,"\\n\n");
    }
    else if(ch == 0x08)
    {
        strcpy(pDest,"\\b\b");
    }
    else if(ch == '\\')
    {
        strcpy(pDest,"\\\\");
    }
    else if(ch < ' ')
    {
        sprintf(pDest,"\\%02X",ch);
    }
    else if(ch > 0x7F)
    {
        sprintf(pDest,"\\%02X",ch);
    }
    else
    {
        pDest[0] = ch;
        pDest[1] = 0;
    }
}

//=============================================================================
//=============================================================================
void TfrmTrace::EscapeString(char *pString,AnsiString &asStr)
{
    unsigned char *pSrc = (unsigned char *)pString;
    asStr="";
    if(pSrc)
    {
        int nParmLen = strlen(pSrc);
        char baBuf[16];
        for(int j=0;j<nParmLen;j++)
        {
            EscapeCharacter(*pSrc++, baBuf);
            asStr += baBuf;
        }
    }
}

//=============================================================================
//=============================================================================
void TfrmTrace::EscapeUnicodeString(unsigned char *pString,int nLen, AnsiString &asStr)
{
    unsigned char *pSrc = (unsigned char *)pString;
    asStr="";
    if(pSrc)
    {
        char baBuf[8];
        for(int j=0;j<nLen;j++)
        {
            if( *pSrc )
            {
                EscapeCharacter(*pSrc, baBuf);
            }
            else
            {
                baBuf[0]='.';
                baBuf[1]=0 ;
            }
            asStr += baBuf;
            *pSrc++;
        }
    }
}

//=============================================================================
//=============================================================================
void TfrmTrace::DisplayWriteLine(TColor color, AnsiString asLine)
{
    try
    {
        reMemo->SelAttributes->Color = color;
        reMemo->Lines->Add(asLine);
        ScrollToEnd(reMemo);
    }
    catch( ... )
    {
    }
}

//=============================================================================
//=============================================================================
void TfrmTrace::DisplayUpdateLine(TColor color, AnsiString asLine)
{
    try
    {
        if(reMemo->Lines->Count)
        {
            reMemo->SelAttributes->Color = color;
            AppendToLastLine(asLine);
        }
        else
        {
            DisplayWriteLine(color,asLine);
        }
    }
    catch( ... )
    {
    }
}

//=============================================================================
//=============================================================================
void TfrmTrace::OnTransmit(char *pTx)
{
    try
    {
        if(pTx)
        {
            AnsiString asLine;
            if(mfIsVisible || frmAggLogger->mfUpdateWindow)
            {
                EscapeString(pTx,asLine);
            }
            if(!mfSend)
            {
                ForceNewLine();
                mfSend = true;
            }
            if(mfIsVisible)
            {
                DisplayUpdateLine(CMDPKT_COLOR,asLine);
            }
            if(frmAggLogger->mfUpdateWindow)
            {
                frmAggLogger->LogWriteLine( mPortNum,CMDPKT_COLOR,asLine,true);
            }
        }
    }
    catch( ... )
    {
    }
}

//=============================================================================
//=============================================================================
void TfrmTrace::OnTransmitUnicode(unsigned char *pTx, int nLen)
{
    try
    {
        if(pTx)
        {
            AnsiString asLine;
            if(mfIsVisible || frmAggLogger->mfUpdateWindow)
            {
                EscapeUnicodeString(pTx,nLen,asLine);
            }
            if(!mfSend)
            {
                ForceNewLine();
                mfSend = true;
            }
            if(mfIsVisible)
            {
                DisplayUpdateLine(CMDPKT_COLOR,asLine);
            }
            if(frmAggLogger->mfUpdateWindow)
            {
                frmAggLogger->LogWriteLine( mPortNum,CMDPKT_COLOR,asLine,true);
            }
        }
    }
    catch( ... )
    {
    }
}

//=============================================================================
//=============================================================================
void TfrmTrace::ForceNewLine(void)
{
    try
    {
        if(mfIsVisible)
        {
            AnsiString asLine="\n";
            DisplayUpdateLine(CMDPKT_COLOR,asLine);

            /*
            if(frmAggLogger->mfUpdateWindow)
            {
                frmAggLogger->LogWriteLine( mPortNum,CMDPKT_COLOR,asLine,true);
            }
            */
        }
    }
    catch( ... )
    {
    }
}


//=============================================================================
//=============================================================================
void TfrmTrace::OnReceive(char *pRx)
{
    try
    {
        if(pRx)
        {
            AnsiString asLine;
            if(mfIsVisible || frmAggLogger->mfUpdateWindow)
            {
                EscapeString(pRx,asLine);
            }
            if(mfSend)
            {
                ForceNewLine();
                mfSend = false;
            }
            if(mfIsVisible)
            {
                DisplayUpdateLine(RSPPKT_COLOR,asLine);
            }
            if(frmAggLogger->mfUpdateWindow)
            {
                frmAggLogger->LogWriteLine( mPortNum,RSPPKT_COLOR,asLine,false);
            }
        }
    }
    catch( ... )
    {
    }
}

void __fastcall TfrmTrace::FormShow(TObject *Sender)
{
    mfIsVisible = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTrace::FormHide(TObject *Sender)
{
    mfIsVisible = false;
}
//---------------------------------------------------------------------------

