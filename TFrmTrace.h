/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     StringPlayer
**
** Module:      TFrmTrace.h
**
*******************************************************************************/
#ifndef TFrmTraceH
#define TFrmTraceH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "Misc.h"
//---------------------------------------------------------------------------
class TfrmTrace : public TForm
{
__published:	// IDE-managed Components
    TRichEdit *reMemo;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
private:	// User declarations
    void ScrollToEnd(TRichEdit *pRichEdit);
    bool mfIsVisible;
    int  mLastType;

    void EscapeCharacter(unsigned char ch, char *pDest);
    void AppendToLastLine(AnsiString asNewText);

public:		// User declarations
    __fastcall TfrmTrace(TComponent* Owner);

    void DisplayWriteLine(TColor color, AnsiString asLine);
    void DisplayUpdateLine(TColor color, AnsiString asLine);
    void EscapeString(char *pString,AnsiString &asStr);
    void EscapeUnicodeString(unsigned char *pString,int nLen, AnsiString &asStr);

    void OnTransmit(char *pTx);
    void OnTransmitUnicode(unsigned char *pTx, int nLen);
    void OnReceive(char *pRx);

    void ForceNewLine(void);

    bool IsShown(){return mfIsVisible;};

    int  mPortNum;
    bool mfSend;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
