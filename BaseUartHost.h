/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      BASEUARTHOST.H
**
*******************************************************************************/

#if !defined(_BASEUARTHOST_H)     /* prevent multiple inclusions */
#define _BASEUARTHOST_H

/******************************************************************************/
// Include Files
/******************************************************************************/
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include "AdPort.hpp"

#include "project.h"
#include "BaseHost.h"
/******************************************************************************/
// Defines
/******************************************************************************/

#define READCACHELEN            1024
/******************************************************************************/
// Macros
/******************************************************************************/

/******************************************************************************/
// Enum Typedefs
/******************************************************************************/

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

/******************************************************************************/
// Forward declaration of Class, Struct & Unions
/******************************************************************************/
class TRegression;

/******************************************************************************/
// Class definitions
/******************************************************************************/

class TBaseUartHost : public TBaseHost
{
//Data
private:
    bool            mfCalled;

//Data
protected:
    unsigned        mTxTriggerLvl;
    Word            mTrigOBF;
    unsigned char   mbaCacheRaw[READCACHELEN+2];
    unsigned int    mCacheRawLen;

//Data
public:
    TApdComPort    *mApdComPort;
    bool           mfLastCTS;
    bool           mfLastDSR;
    bool           mfLastDCD;
    bool           mfLastRI;
    unsigned int   mToggleCountCTS;
    unsigned int   mToggleCountDSR;
    unsigned int   mToggleCountDCD;
    unsigned int   mToggleCountRI;
    int            mnPostOpenWaitMs;
    int            mnPostCloseWaitMs;

//Functions
private:

    TParity     GetParityEnum(int nParity);
    bool        CheckForPort(TApdCustomComPort* Port, int PortNo);

//Functions
protected:
    void __fastcall ApdComPortTriggerAvail(TObject *CP, WORD Count);
    void __fastcall ApdComPortTriggerOutbuffFree(TObject *Sender);
    void __fastcall ApdComPortTriggerModemStatus(TObject *Sender);

//Functions
public:
    TBaseUartHost(TRegression *pRegression, unsigned int nLogicalPort);
    virtual __fastcall ~TBaseUartHost();


//static functions and data
    static  unsigned int mInstanceCount;
    static  int PortExists(int PortNo);

//other functions and data
    virtual void        Close(void);
    virtual bool        Open(int nComport, UBaseOpenParms *pParms);
    virtual bool        IsOpen(void);
    virtual bool        ChangeCommsParameters(UBaseOpenParms *pCommParms);

    virtual bool        GetCTS(void);
    virtual bool        GetDSR(void);
    virtual bool        GetRI(void);
    virtual bool        GetDCD(void);
    virtual void        SetDTR(bool newState);
    virtual void        SetRTS(bool newState);

    virtual void        SetBreak(void);
    virtual void        ClearBreak(void);
    virtual void        DeAssertDTR(void);
    virtual void        AssertDTR(void);

    virtual void        FlushRxBuffer(void);
    virtual void        FlushTxBuffer(void);

    unsigned short      __TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen);

    /* pure virtual functions */
    virtual unsigned short TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen)=0;
    virtual void        SendCommand(char *pCmd)=0;
    virtual void        SendString(char *pCmd)=0;
    virtual void 	    SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen)=0;
    virtual void        SendBlock(unsigned char *pCmd, int nBlockLen)=0;
    virtual char *      SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType)=0;
    virtual char      * GetFullResponse(void)=0;
    virtual char      * GetDataInResponse(unsigned int nChannel);
    virtual void        OnOpen(void)=0;
    virtual void        OnRxData(unsigned char *pRxData, unsigned int nRxDataLen)=0;
    virtual void        OnFlushRxBuffer(bool fPostFlush)=0;
    virtual void        OnFlushTxBuffer(bool fPostFlush)=0;
    virtual const char *GetHostName(void)=0;


};

/******************************************************************************/
// Struct definitions
/******************************************************************************/

/******************************************************************************/
// Union definitions
/******************************************************************************/

/******************************************************************************/
// Global Functions (API etc) exported for other modules
/******************************************************************************/

#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/


