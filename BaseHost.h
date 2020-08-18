/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      BASEHOST.H
**
*******************************************************************************/

#if !defined(_BASEHOST_H)     /* prevent multiple inclusions */
#define _BASEHOST_H

/******************************************************************************/
// Include Files
/******************************************************************************/

#include "project.h"
#include "MiscEx.h"

/******************************************************************************/
// Defines
/******************************************************************************/

#define MAX_HOSTTYPE_NAMELEN        32

/******************************************************************************/
// Macros
/******************************************************************************/

/******************************************************************************/
// Enum Typedefs
/******************************************************************************/

typedef enum TPacketTypeTag
{
    TPACKET_TYPE_MPOINT = 0
   ,TPACKET_TYPE_HCI_LE_EVENT
}
TPacketType;

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

typedef struct
{
    int     nBaudrate;
    int     nParity;
    int     nStopBits;
    int     nDataBits;
    bool    fHandshaking;
    bool    fModemTriggering;
    int     nPostOpenWaitMs;
    int     nPostCloseWaitMs;
}
TBaseCommParms;

typedef union
{
    TBaseCommParms mComms;
}
UBaseOpenParms;

class TBaseHost
{
//Data
private:
    char baHostType[MAX_HOSTTYPE_NAMELEN+1];
    unsigned int    mnLogicalPort;

//Data
protected:
    TRegression    *mpRegression;

//Data
public:
    unsigned int    mPacketCount;
    int  mPatternMatchIndex;


//Functions
private:

//Functions
protected:

//Functions
public:
    TBaseHost(TRegression *pRegression, unsigned int nLogicalPort);
    virtual __fastcall ~TBaseHost();

    //functions
    void SetHostType(char *pName);
    const char *GetHostType(void);
    int  GetLogicalPort(void) { return mnLogicalPort; };

    //virtual functions

    //pure virtual functions
    virtual void        Close(void)=0;
    virtual bool        Open(int nComport, UBaseOpenParms *pParms)=0;
    virtual bool        IsOpen(void)=0;
    virtual bool        ChangeCommsParameters(UBaseOpenParms *pParms)=0;

    virtual unsigned short TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen)=0;

    virtual bool        GetCTS(void)=0;
    virtual bool        GetDSR(void)=0;
    virtual bool        GetRI(void)=0;
    virtual bool        GetDCD(void)=0;
    virtual void        SetDTR(bool newState)=0;
    virtual void        SetRTS(bool newState)=0;

    virtual void 	    SendCommand(char *pCmd)=0;
    virtual void 	    SendString(char *pCmd)=0;
    virtual void 	    SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen)=0;
    virtual void 	    SendBlock(unsigned char *pCmd, int nBlockLen)=0;
    virtual char *      SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType)=0;

    virtual char      * GetFullResponse(void)=0;
    virtual char      * GetDataInResponse(unsigned int nChannel);
    virtual int         GetPatternIndexNumber(void){return mPatternMatchIndex;};
    virtual void        DropFromResponse(int nToDelete)=0;
    virtual void        DropFromChannel(int nChannel,int nDropLen){};

    virtual bool        IsAllowedSendOutcome(EExpectedOutcome eOutCome)=0;

    virtual void        SetBreak(void)=0;
    virtual void        ClearBreak(void)=0;
    virtual void        DeAssertDTR(void)=0;
    virtual void        AssertDTR(void)=0;

    virtual void        FlushRxBuffer(void)=0;
    virtual void        FlushTxBuffer(void)=0;
    virtual int         PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg)=0;

    virtual bool        IsPacketPresent(TPacketType pktType);
    virtual char *      GetPacket(int *pPktLen,TPacketType pktType);
    virtual int         DropPacket(TPacketType pktType);
    virtual unsigned int GetPacketCount(void);
    virtual unsigned int SetPacketCount(unsigned int nNewCount);
    virtual unsigned int GetPacketCountInResp(void);
    virtual int         CheckFullPatternMulti(void) { return -1; };
    virtual int         CheckPartialPatternMulti(void)  { return -1; };
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


