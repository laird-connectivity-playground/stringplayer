/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      HEXMYHOST.H
**
*******************************************************************************/

#if !defined(_HEXMYHOST_H)     /* prevent multiple inclusions */
#define _HEXMYHOST_H

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
/******************************************************************************/
// Defines
/******************************************************************************/

#define MAXREADHEXBLOCKLEN          (8192)
#define MAX_BINARY_BLOCK_LEN        (1024)

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

class HexMyHost : public TBaseUartHost
{
//Data
private:

//Data
protected:
    unsigned char  mbaBinaryBlock[MAX_BINARY_BLOCK_LEN];
    unsigned short mnBinaryBlockLen;

    int            mnHexRespCount;
    char           mbaHexResponse[(MAXREADHEXBLOCKLEN*2)+2];
    int            mnHexMaxRespCount;

    int            mnRespCount;
    char           mbaResponse[MAXREADHEXBLOCKLEN+2];
    int            mnMaxRespCount;

    char          * mpPattern;
    unsigned short  mnPatternLen;
    int            mSearchType;
    char           mbaHex[256][4];

//Data
public:

//Functions
private:

//Functions
protected:

//Functions
public:
    HexMyHost(TRegression *pRegression, unsigned int nLogicalPort);
    virtual __fastcall ~HexMyHost();

    //helper function
    unsigned short      DecodeStringToBinary(
                            const unsigned char *pString,
                            int nStringLen,
                            unsigned char *pDstStr,
                            unsigned short nMaxDstStrLen
                            );
    unsigned short      EncodeBinaryToHex(
                            const unsigned char *pBinStr,
                            unsigned short nBinStrLen,
                            unsigned char *pDstStr,
                            unsigned short nMaxDstStrLen
                            );
    unsigned short      CleanupHexString(
                            const unsigned char *pString, //null terminated string
                            int nStringLen,
                            unsigned char *pDstStr,
                            unsigned short nMaxDstStrLen
                            );

    //virtual functions from abstract class
    virtual unsigned short TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen);
    virtual char      * GetFullResponse(void);
    virtual void        OnOpen(void);
    virtual void        OnFlushRxBuffer(bool fPostFlush);
    virtual void        OnFlushTxBuffer(bool fPostFlush);

    //pure virtual functions from abstract class
    virtual const char *GetHostName(void)=0;
    virtual void        OnRxData(unsigned char *pRxData, unsigned int nRxDataLen)=0;
    virtual void        SendCommand(char *pCmd)=0;
    virtual void        SendString(char *pCmd)=0;
    virtual void 	    SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen)=0;
    virtual void        SendBlock(unsigned char *pCmd, int nBlockLen)=0;
    virtual char *        SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType)=0;

    virtual void        FlushRxBuffer(void);
    virtual void        FlushTxBuffer(void);

    virtual bool        IsPacketPresent(TPacketType pktType);
    virtual char *      GetPacket(int *pPktLen,TPacketType pktType);
    virtual int         DropPacket(TPacketType pktType);

    virtual bool        CheckFullPattern(void)=0;
    virtual bool        CheckPartialPattern(void)=0;

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


