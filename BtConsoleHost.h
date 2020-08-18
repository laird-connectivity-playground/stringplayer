/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      BTCONSOLEHOST.H
**
*******************************************************************************/

#if !defined(_BTCONSOLEHOST_H)     /* prevent multiple inclusions */
#define _BTCONSOLEHOST_H

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
#include "AtHost.h"
#include "TCommandIf.h"

/******************************************************************************/
// Defines
/******************************************************************************/

/******************************************************************************/
// Macros
/******************************************************************************/
#define BTRESP_CACHE_SIZE           0x2000
#define ONDATACACHELEN              256

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

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class BtConsoleHost : public TBaseHost,  public CCommandIFCallback
{
//Data
private:
    CCommandIF    * mpCommandIf;
    char            mbaResponse[MIN_COMMAND_RESPONSE_CACHE_SIZE+1];
    int             mnRespCount;

    char          * mpPattern;
    unsigned short  mnPatternLen;
    int             mSearchType;
    char            mbaCacheRaw[ONDATACACHELEN+1];
    char            mbaCache[(ONDATACACHELEN*3)+4];


//Data
protected:

//Data
public:

//Functions
private:

//Functions
protected:

//Functions
public:
    BtConsoleHost(TRegression *pRegression, unsigned int nLogicalPort);
    virtual __fastcall ~BtConsoleHost();

    void        OnWindowsMessage( WPARAM wparam );
    virtual     void OnInitialise( void );
    virtual     void OnData( void );
    virtual     void OnReady( void );

//static functions and data

//other functions and data
    virtual void        Close(void);
    virtual bool        Open(int nComport, UBaseOpenParms *pParms);
    virtual bool        IsOpen(void);
    virtual bool        ChangeCommsParameters(UBaseOpenParms *pCommParms);

    virtual unsigned short TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen);

    virtual bool        GetCTS(void);
    virtual bool        GetDSR(void);
    virtual bool        GetRI(void);
    virtual bool        GetDCD(void);
    virtual void        SetDTR(bool newState);
    virtual void        SetRTS(bool newState);

    virtual void 	    SendCommand(char *pCmd);
    virtual void 	    SendString(char *pCmd);
    virtual void 	    SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen);
    virtual void 	    SendBlock(unsigned char *pCmd, int nBlockLen);
    virtual char *        SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType);

    virtual char      * GetFullResponse(void);
    virtual int         PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg);
    virtual void        DropFromResponse(int nToDelete){};

    virtual void        SetBreak(void);
    virtual void        ClearBreak(void);
    virtual void        DeAssertDTR(void);
    virtual void        AssertDTR(void);

    virtual void        FlushRxBuffer(void);
    virtual void        FlushTxBuffer(void);
    virtual bool        IsAllowedSendOutcome(EExpectedOutcome eOutCome);
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
BtConsoleHost *CreateBtConsoleHost(TRegression *pRegression, unsigned int nLogicalPort);

#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/


