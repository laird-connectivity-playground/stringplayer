/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      OCHOST.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include "OcHost.h"
#include "TRegression.h"
#include "Misc.h"
/******************************************************************************/
// Local Defines
/******************************************************************************/

#define OC_STATE_NORMAL_DATA            0
#define OC_STATE_LINE_HDR               1
#define OC_STATE_CHANNEL_DATA           2
#define OC_STATE_INV_CHANNEL_DATA       3
#define OC_STATE_CHANNEL_TERMINATED     4


/******************************************************************************/
// Local Macros
/******************************************************************************/

/******************************************************************************/
// Local Forward Class,Struct & Union Declarations
/******************************************************************************/

/******************************************************************************/
// Local Class,Struct,Union Typedefs
/******************************************************************************/

/******************************************************************************/
// External Variable Declarations
/******************************************************************************/

/******************************************************************************/
// Global/Static Variable Declarations
/******************************************************************************/

/******************************************************************************/
// External Function Declarations
/******************************************************************************/
int TracePrint(char *pFmt,...);
bool ProcessApplicationMessages(void);

/******************************************************************************/
// Local Forward Function Declarations
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
// Local Functions or Private Members
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
// Global Functions or Public Members
/******************************************************************************/
/******************************************************************************/

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
OcHost::OcHost(TRegression *pRegression, unsigned int nLogicalPort) :
    TBaseUartHost(pRegression,nLogicalPort)
{
    mApdComPort = NULL;
    mRxTotal = 0;
    FlushReceiveBuffers();
    SetHostType("TAPDCOMPORT");
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
__fastcall OcHost::~OcHost()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TBaseHost *CreateOcHost(TRegression *pRegression, unsigned int nLogicalPort)
{
    TBaseHost *pHost = new OcHost(pRegression,nLogicalPort);
    return pHost;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void OcHost::FlushReceiveBuffers(void)
{
    int nChannel;

    mRxCount=0;

    mnLineHdrCount=0;

    for(nChannel=0;nChannel<OC_MAXCHANNELS;nChannel++)
    {
        mnDataCount[nChannel]=0;
    }

    mState = OC_STATE_NORMAL_DATA ;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const char *OcHost::GetHostName(void)
{
    return "OcHost";
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void  OcHost::OnOpen(void)
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void OcHost::OnRxData(unsigned char *pRxData, unsigned int nRxDataLen)
{
    int nModule = GetLogicalPort();
    char *pSrc = pRxData;
    unsigned int nRxLen = nRxDataLen;

    char *pDstPtr;
    int  *pDstCount;
    int  nDstMaxSize;

    char mbaInvChannel[8];
    int mInvCount=0;

    //display the data received so far
    unsigned char ch = pRxData[nRxDataLen];
    pRxData[nRxDataLen]=0;
    mpRegression->TraceRx(GetLogicalPort(),pRxData);
    pRxData[nRxDataLen]=ch;

    //determine destination data
    switch(mState)
    {
    case OC_STATE_NORMAL_DATA:
    case OC_STATE_CHANNEL_TERMINATED:
        pDstCount    = &mRxCount;
        pDstPtr      = &mbaRxBlock[*pDstCount];
        nDstMaxSize  = OC_MAXREADBLOCKLEN;
        mState       = OC_STATE_NORMAL_DATA;
        break;

    case OC_STATE_LINE_HDR:
        pDstCount    = &mnLineHdrCount;
        pDstPtr      = &mbaLineHeader[*pDstCount];
        nDstMaxSize  = OC_LINE_HEADER_LEN;
        break;

    case OC_STATE_CHANNEL_DATA:
        pDstCount    = &mnDataCount[mChannel];
        pDstPtr      = &mbaData[mChannel][*pDstCount];
        nDstMaxSize  = OC_MAXCHANNELDATALEN;
        break;

    case OC_STATE_INV_CHANNEL_DATA:
        pDstCount    = &mInvCount;
        pDstPtr      = mbaInvChannel;
        nDstMaxSize  = 0;
        break;

    default:
        return;
    }

    while(nRxLen--)
    {
        switch(mState)
        {
        case OC_STATE_CHANNEL_TERMINATED:
            pDstCount    = &mRxCount;
            pDstPtr      = &mbaRxBlock[*pDstCount];
            nDstMaxSize  = OC_MAXREADBLOCKLEN;
            mState       = OC_STATE_NORMAL_DATA;
            // fall through to the next case
        case OC_STATE_NORMAL_DATA:
            if( *pSrc == '\n' )
            {
                mnLineHdrCount = 0;
                pDstPtr      = &mbaLineHeader[mnLineHdrCount];
                pDstCount    = &mnLineHdrCount;
                nDstMaxSize  = OC_LINE_HEADER_LEN;
                mState = OC_STATE_LINE_HDR;
            }
            break;

        case OC_STATE_LINE_HDR:
            if(mnLineHdrCount==1)
            {
                if(*pSrc != ':')
                {
                    mbaRxBlock[mRxCount++] = '\n';
                    pDstPtr      = &mbaRxBlock[mRxCount];
                    pDstCount    = &mRxCount;
                    nDstMaxSize  = OC_MAXREADBLOCKLEN;
                    mState       = OC_STATE_NORMAL_DATA;
                }
            }
            else
            {
                if(mnLineHdrCount>=4)
                {
                    if(    ( mbaLineHeader[2] >= '0')
                        && ( mbaLineHeader[2] <  ('0'+ OC_MAXCHANNELS ))
                        && ( mbaLineHeader[3] == ' ')
                      )
                    {
                        mChannel= mbaLineHeader[2] - '0';
                        pDstCount    = &mnDataCount[mChannel];
                        pDstPtr      = &mbaData[mChannel][*pDstCount];
                        nDstMaxSize  = OC_MAXCHANNELDATALEN;
                        mState       = OC_STATE_CHANNEL_DATA;
                    }
                    else
                    {
                        pDstCount    = &mInvCount;
                        pDstPtr      = mbaInvChannel;
                        nDstMaxSize  = 0;
                        mState       = OC_STATE_INV_CHANNEL_DATA;
                    }
                }
            }
            break;

        case OC_STATE_CHANNEL_DATA:
        case OC_STATE_INV_CHANNEL_DATA:
            if( *pSrc == '\r' )
            {
                pDstCount    = &mInvCount;
                pDstPtr      = mbaInvChannel;
                nDstMaxSize  = 0;
                mState       = OC_STATE_CHANNEL_TERMINATED;
            }
            break;

        }

        //copy to appropriate buffer
        if( *pDstCount < nDstMaxSize )
        {
            *pDstPtr++ = *pSrc++;
            *pDstPtr   = 0; //terminate it
            *pDstCount = *pDstCount + 1;
        }
        else
        {
            pSrc++;
        }
    }

    //check if the pattern has been found
    if(mnPatternLen>0)
    {
        int nDropLen;
        int nSearchType = mSearchType&0xFF;
        int nChannel    = ((mSearchType>>8)&0xFF);

        switch(nSearchType)
        {
        case 0:
            nDropLen = CheckPartialPattern();
            if( nDropLen )
            {
                DropFromResponse( nDropLen );
                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PARTIAL,(void *)mbaRxBlock,nModule);
            }
            break;

        case 1:
            if( CheckFullPattern() )
            {
                DropFromResponse( mnPatternLen );
                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PATTERN,(void *)mbaRxBlock,nModule);
            }
            break;

        case 2:
            {
                nDropLen = CheckPartialDataPattern(nChannel);
                if( nDropLen )
                {
                    mpRegression->DisplayChannelResponse(nModule,nChannel,mbaData[nChannel]);
                    DropFromChannel(nChannel,nDropLen);

                    ThrowStateMachineEvent(WIZMNGR_RESPONSE_PARTIAL,(void *)mbaData[nChannel],nModule);
                }
            }
            break;

        }
    }
}

//=============================================================================
//=============================================================================
unsigned short OcHost::TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen)
{
    return __TransmitBlock(pSendBlock,nBlockLen);
}

//=============================================================================
//=============================================================================
void OcHost::SendCommand(char *pCmd)
{
    SendBlock(pCmd, strlen(pCmd));
}

//=============================================================================
//=============================================================================
void OcHost::SendString(char *pCmd)
{
    SendBlock(pCmd, strlen(pCmd));
}

//=============================================================================
//=============================================================================
void OcHost::SendBlock(unsigned char *pCmd, int nBlockLen)
{
    __TransmitBlock(pCmd,nBlockLen);
    mpRegression->TraceUnicodeTx(GetLogicalPort(),pCmd,nBlockLen);
}

//=============================================================================
//=============================================================================
void OcHost::SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen)
{
    SendBlock(pCmd, nBlockLen);
}

//=============================================================================
//=============================================================================
char * OcHost::GetFullResponse(void)
{
    return mbaRxBlock;
}

//=============================================================================
//=============================================================================
void OcHost::DropFromResponse(int nToDelete)
{
    if( nToDelete>=mRxCount )
    {
        mRxCount=0;
    }
    else
    {
        int nLeft= mRxCount - nToDelete;
        memcpy(&mbaRxBlock[0],&mbaRxBlock[nToDelete],nLeft);
        mbaRxBlock[nLeft]=0;
        mRxCount=nLeft;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int OcHost::PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg)
{
    int nRetVal=0;
    switch(ioctlId)
    {
    case 0:
        nRetVal = mRxCount;
        mRxCount = 0;
        break;

    case 1:
        nRetVal = mRxCount;
        break;

    case 16:
        nRetVal = mApdComPort->OutBuffFree;
        break;

    case 32: /* CTS Toggle count */
        nRetVal = mToggleCountCTS;
        break;
    case 33: /* DSR Toggle count */
        nRetVal = mToggleCountDSR;
        break;
    case 34: /* DCD Toggle count */
        nRetVal = mToggleCountDCD;
        break;
    case 35: /* RI Toggle count */
        nRetVal = mToggleCountRI;
        break;

    default:
        break;
    }
    return  nRetVal;
}

//=============================================================================
//=============================================================================
char * OcHost::SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType)
{
    if( pPattern && (nPatternLen>0))
    {
        memcpy(mbaPattern,pPattern, nPatternLen);
    }
    mnPatternLen  = nPatternLen;
    mSearchType   = nSearchType;
    mbaPattern[nPatternLen] = 0;

    return mbaPattern;
}

//=============================================================================
//=============================================================================
void OcHost::OnFlushRxBuffer(bool fPostFlush)
{
    FlushReceiveBuffers();
}

//=============================================================================
//=============================================================================
void OcHost::OnFlushTxBuffer(bool fPostFlush)
{
}

//=============================================================================
//=============================================================================
bool OcHost::IsAllowedSendOutcome(EExpectedOutcome eOutCome)
{
    return true;
}

//=============================================================================
//=============================================================================
int OcHost::CharacterPosition(char *pBuf, char ch)
{
    int nPos=0;
    while(*pBuf)
    {
        if( *pBuf == ch )
        {
            return nPos;
        }
        nPos++;
        pBuf++;
    }
    return -1;
}

//=============================================================================
//=============================================================================
bool OcHost::CheckFullPattern(void)
{
    return MiscCompareStrings(mbaRxBlock,mbaPattern,mnPatternLen);
}

//=============================================================================
//=============================================================================
int OcHost::CheckPartialPattern(void)
{
    char *pFound = MiscContainsPattern(mbaRxBlock,mRxCount,mbaPattern,mnPatternLen);
    if(pFound==NULL)
    {
        return 0;
    }
    int nDropLen =  pFound - mbaRxBlock + mnPatternLen;
    return nDropLen;
}

//=============================================================================
// returns 0 of not found, otherwise number of characters to drop
//=============================================================================
int OcHost::CheckPartialDataPattern(unsigned int nChannel)
{
    char *pFound = MiscContainsPattern(mbaData[nChannel],mnDataCount[nChannel],mbaPattern,mnPatternLen);
    if(pFound==NULL)
    {
        return 0;
    }
    int nDropLen =  pFound - mbaData[nChannel] + mnPatternLen;
    return nDropLen;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char *OcHost::GetDataInResponse(unsigned int nChannel)
{
    char *pRetVal = NULL;

    if( (nChannel>0) && (nChannel<OC_MAXCHANNELS) )
    {
        pRetVal =  &mbaData[nChannel][0];
    }

    return pRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void OcHost::DropFromChannel(int nChannel,int nDropLen)
{
    int nLeft;

    if( (nChannel<0) || (nChannel>=OC_MAXCHANNELS) )
    {
        return;
    }

    if( nDropLen >= mnDataCount[nChannel] )
    {
        nLeft=0;
    }
    else
    {
        nLeft = mnDataCount[nChannel]-nDropLen;
        memcpy( mbaData[nChannel], &mbaData[nChannel][nDropLen], nLeft );
    }

    mbaData[nChannel][nLeft]=0;
    mnDataCount[nChannel]=nLeft;
}

/******************************************************************************/
// END OF FILE
/******************************************************************************/


