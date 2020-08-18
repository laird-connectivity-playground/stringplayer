/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      OCHOST.H
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
#include "Misc.h"
/******************************************************************************/
// Defines
/******************************************************************************/

#define OC_MAXREADBLOCKLEN         (1024*32)
#define OC_LINE_HEADER_LEN         (8)

#define OC_MAXPATTERNLEN           (1024*4)
#define OC_MAX_MULTI_PATTERNS          (16)

#define OC_MAXCHANNELS             (10)
#define OC_MAXCHANNELDATALEN       (1024*2)


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

class OcHost : public TBaseUartHost
{
//Data
private:
    unsigned char   mState;   /* See  OC_STATE_XXX */
    int             mChannel; /* -1 means rxBlock, 0..(OC_MAXCHANNELS-1) means a channel */

//Data
protected:
    char            mbaRxBlock[OC_MAXREADBLOCKLEN+8];
    int             mRxCount;

    unsigned int    mRxTotal;

    char            mbaPattern[OC_MAXPATTERNLEN];
    unsigned short  mnPatternLen;

    int             mSearchType;

    char            mbaData[OC_MAXCHANNELS][OC_MAXCHANNELDATALEN+8];
    int             mnDataCount[OC_MAXCHANNELS];

    char            mbaLineHeader[OC_LINE_HEADER_LEN+8];
    int             mnLineHdrCount;

//Data
public:

//Functions
private:
    void            FlushReceiveBuffers(void);

//Functions
protected:

//Functions
public:
    OcHost(TRegression *pRegression, unsigned int nLogicalPort);
    virtual __fastcall ~OcHost();


//static functions and data

//other functions and data
    virtual unsigned short TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen);
    virtual void        SendCommand(char *pCmd);
    virtual void        SendString(char *pCmd);
    virtual void 	    SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen);
    virtual void        SendBlock(unsigned char *pCmd, int nBlockLen);
    virtual char *      SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType);
    virtual char      * GetFullResponse(void);
    virtual char      * GetDataInResponse(unsigned int nChannel);
    virtual void        DropFromResponse(int nToDelete);
    virtual void        DropFromChannel(int nChannel,int nDropLen);
    virtual void        OnOpen(void);
    virtual void        OnRxData(unsigned char *pRxData, unsigned int nRxDataLen);
    virtual void        OnFlushRxBuffer(bool fPostFlush);
    virtual void        OnFlushTxBuffer(bool fPostFlush);
    virtual const char *GetHostName(void);
    virtual bool        IsAllowedSendOutcome(EExpectedOutcome eOutCome);
    virtual int         PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg);

    virtual bool        CheckFullPattern(void);
    virtual int         CheckPartialPattern(void);
    virtual int         CheckPartialDataPattern(unsigned int nChannel);

    int                 CharacterPosition(char *pBuf, char ch);
    //int                 ExtractDataChannels(char *pBuffer, int nDataLen);
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


