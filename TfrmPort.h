/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     StringPlayer
**
** Module:      tfrmPort.h
**
*******************************************************************************/
#ifndef TfrmPortH
#define TfrmPortH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmPort : public TForm
{
__published:	// IDE-managed Components
    TRichEdit *reMemo;
private:	// User declarations
public:		// User declarations
    __fastcall TfrmPort(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPort *frmPort;
//---------------------------------------------------------------------------
#endif
