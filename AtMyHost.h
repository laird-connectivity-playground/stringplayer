/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      ATMYHOST.H
**
*******************************************************************************/

#if !defined(_ATMYHOST_H)     /* prevent multiple inclusions */
#define _ATMYHOST_H

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
#include "BaseUartHost.h"
#include "AtHost.h"
/******************************************************************************/
// Defines
/******************************************************************************/

#define MAXREADBLOCKLEN         (1024*32)
#define MAXCMDLEN               1024

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

class AtMyHost : public TBaseUartHost
{
//Data
private:

//Data
protected:
    ATHANDLE        mAtHandle;
    unsigned char   mbaRxBlock[MAXREADBLOCKLEN];
    int             mRxBlockLen;
    char           *mpSubStr;
    unsigned int    mSubStrBufLen;
    char           *mpCurSubStr;
    char            mbaLastCmd[MAXCMDLEN];
    char            mbaCache[(READCACHELEN*3)+4];
    int             mRxCount;

//Data
public:

//Functions
private:

//Functions
protected:

//Functions
public:
    AtMyHost(TRegression *pRegression, unsigned int nLogicalPort);
    virtual __fastcall ~AtMyHost();


//static functions and data

//other functions and data
    virtual unsigned short TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen);
    virtual void        SendCommand(char *pCmd);
    virtual void        SendString(char *pCmd);
    virtual void 	    SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen);
    virtual void        SendBlock(unsigned char *pCmd, int nBlockLen);
    virtual char *      SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType);
    virtual char      * GetFullResponse(void);
    virtual void        DropFromResponse(int nToDelete);
    virtual void        OnOpen(void);
    virtual void        OnRxData(unsigned char *pRxData, unsigned int nRxDataLen);
    virtual void        OnFlushRxBuffer(bool fPostFlush);
    virtual void        OnFlushTxBuffer(bool fPostFlush);
    virtual const char *GetHostName(void);
    virtual bool        IsAllowedSendOutcome(EExpectedOutcome eOutCome);
    virtual int         PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg);

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
unsigned short AtMyHostTransmit(void *pContext,unsigned char *pSendBlock,unsigned short nBlockLen);
void AtMyHostOnCrLf(void *pContext,char *pResp);
void AtMyHostOnAsyncResponse(void *pContext,ATRESPONSE, char *pResp);
bool AtMyHostOnBlockWait(void *pContext);
void AtMyHostOnResponseOverflow(void *pContext);
unsigned int AtMyHostGetTickCount(void *pContext);

#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/


