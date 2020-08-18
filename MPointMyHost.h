/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      MPOINTMYHOST.H
**
*******************************************************************************/

#if !defined(_MPOINTMYHOST_H)     /* prevent multiple inclusions */
#define _MPOINTMYHOST_H

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
#include "HexMyHost.h"
#include "Misc.h"
/******************************************************************************/
// Defines
/******************************************************************************/

#define MAX_PATTERN_LEN             (1024*32)
#define MAX_MPDATA_CACHELEN         (1024*32)

#define MAX_MULTI_PATTERNS          (16)

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

class MPointMyHost : public HexMyHost
{
//Data
private:
    unsigned char mbaCleanPattern[MAX_PATTERN_LEN+2];
    unsigned short mnMaxCleanPatternLen;
    unsigned char  mbaMpData[MAX_MPDATA_CACHELEN];

    MiscMultiPattern  sMultiPattern[MAX_MULTI_PATTERNS];
    int            mNumOfSubPatterns;
    int            mRxCount;

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
    MPointMyHost(TRegression *pRegression, unsigned int nLogicalPort);
    virtual __fastcall ~MPointMyHost();

    //virtual functions from abstract class
    virtual void        OnRxData(unsigned char *pRxData, unsigned int nRxDataLen);
    virtual const char *GetHostName(void);
    virtual void        SendCommand(char *pCmd);
    virtual void        SendString(char *pCmd);
    virtual void 	    SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen);
    virtual void        SendBlock(unsigned char *pCmd, int nBlockLen);
    virtual char *        SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType);
    virtual bool        IsAllowedSendOutcome(EExpectedOutcome eOutCome);
    virtual bool        IsPacketPresent(TPacketType pktType);
    virtual char *      GetPacket(int *pPktLen,TPacketType pktType);
    virtual int         DropPacket(TPacketType pktType);
    virtual unsigned int GetPacketCountInResp(void);
    virtual int         PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg);
    virtual void        DropFromResponse(int nToDelete);

    //virtual void        FlushRxBuffer(void);

    //other helper functions
    virtual bool        CheckFullPattern(void);
    virtual bool        CheckPartialPattern(void);
    virtual int         CheckFullPatternMulti(void);
    virtual int         CheckPartialPatternMulti(void);
    virtual bool        CheckPartialDataPattern(unsigned int nChannel);
    virtual char      * GetDataInResponse(unsigned int nChannel);
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


