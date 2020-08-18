/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      HEXMYHOST.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include "MPointMyHost.h"
#include "TRegression.h"
#include "Misc.h"
/******************************************************************************/
// Local Defines
/******************************************************************************/

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
MPointMyHost::MPointMyHost(TRegression *pRegression, unsigned int nLogicalPort) :
    HexMyHost(pRegression,nLogicalPort)
{
    mnMaxCleanPatternLen=MAX_PATTERN_LEN;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
__fastcall MPointMyHost::~MPointMyHost()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TBaseHost *CreateMPointMyHost(TRegression *pRegression, unsigned int nLogicalPort)
{
    TBaseHost *pHost = new MPointMyHost(pRegression,nLogicalPort);
    return pHost;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const char *MPointMyHost::GetHostName(void)
{
    return "MPointMyHost";
}

//=============================================================================
//=============================================================================
void MPointMyHost::SendCommand(char *pCmd)
{
    FlushRxBuffer();
    mpRegression->TraceNewLine(GetLogicalPort());
    SendBlock(pCmd, strlen(pCmd));
    mpRegression->TraceNewLine(GetLogicalPort());
}

//=============================================================================
//=============================================================================
void MPointMyHost::SendString(char *pCmd)
{
    SendBlock(pCmd, strlen(pCmd));
}

//=============================================================================
//=============================================================================
void MPointMyHost::SendBlock(unsigned char *pBlock, int nBlockLen)
{
    unsigned short nDecodedLen;
    if( nBlockLen > 0 )
    {
        nDecodedLen = DecodeStringToBinary(pBlock,nBlockLen,mbaBinaryBlock,mnBinaryBlockLen);
        if( nDecodedLen )
        {
            __TransmitBlock(mbaBinaryBlock,nDecodedLen);
            mpRegression->TraceUnicodeTx(GetLogicalPort(),pBlock,nBlockLen);
        }
    }
}

//=============================================================================
//=============================================================================
void MPointMyHost::SendDataBlock(int nChannel, unsigned char *pBlock, int nBlockLen)
{
    unsigned short nDecodedLen;
    char baHeaderVerbose[8];
    char baHeader[2];

    if( nBlockLen > 0 )
    {
        nDecodedLen = DecodeStringToBinary(pBlock,nBlockLen,mbaBinaryBlock,mnBinaryBlockLen);
        if( nDecodedLen )
        {
            baHeader[0]= (nDecodedLen+2)&0xFF;
            baHeader[1]= nChannel&0xFF;
            sprintf(baHeaderVerbose,"%02x%02X",baHeader[0],baHeader[1]);
            __TransmitBlock(baHeader,2);
            __TransmitBlock(mbaBinaryBlock,nDecodedLen);
            mpRegression->TraceUnicodeTx(GetLogicalPort(),baHeaderVerbose,4);
            mpRegression->TraceUnicodeTx(GetLogicalPort(),pBlock,nBlockLen);
        }
    }
}

//=============================================================================
//=============================================================================
char * MPointMyHost::SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType)
{
    unsigned short nDecodedLen;

    /*
    July 2017:
    Updated this routine so that it can accept pPattern==NULL
    */
    if( (pPattern==NULL) || (nPatternLen==0) )
    {
        mnPatternLen = 0;
        mpPattern    = NULL;
        mSearchType  = nSearchType;
        return  mpPattern;
    }
    /* reaching here means there is somethign to search */

    nPatternLen = CleanupHexString(pPattern,nPatternLen,mbaCleanPattern,mnMaxCleanPatternLen);
    mnPatternLen  = nPatternLen;
    mpPattern     = (mnPatternLen>0) ? mbaCleanPattern : NULL ;
    mSearchType   = nSearchType;

    if( mnPatternLen && (( mSearchType==3)||( mSearchType==4)) )
    {
        unsigned char *pSrc = mbaCleanPattern;
        unsigned char *pSrcNext;
        /* need to look out for one of many patterns, so split the patterns */
        /* and the way to to do that is convert the "|" characters into nulls */
        /* and then setting pointers to the following characters */
        mNumOfSubPatterns = 0;

        do
        {
            pSrcNext = MiscAdvanceToNextCharacter(pSrc,'|');
            if(pSrcNext)*pSrcNext=0;
            sMultiPattern[mNumOfSubPatterns].mpPattern= pSrc;
            sMultiPattern[mNumOfSubPatterns++].mPatternLen= strlen(pSrc);
            if(pSrcNext==NULL)break;
            pSrc = pSrcNext+1;
        }
        while( (mNumOfSubPatterns<MAX_MULTI_PATTERNS) );
    }
    return mpPattern;
}


//=============================================================================
//=============================================================================
bool MPointMyHost::IsAllowedSendOutcome(EExpectedOutcome eOutCome)
{
    switch(eOutCome)
    {
    case OUTCOME_NONE:
    case OUTCOME_PATTERN:
    case OUTCOME_MOD_TIMEOUT:
        return true;

    default:
        break;
    }
    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool MPointMyHost::IsPacketPresent(TPacketType pktType)
{
    switch(pktType)
    {
    case TPACKET_TYPE_MPOINT:
        if(mnRespCount >= mbaResponse[0])
        {
            return true;
        }
        break;

    case TPACKET_TYPE_HCI_LE_EVENT:
        if(mnRespCount >= 3)
        {
            if(mnRespCount >= (mbaResponse[2]+3))
            {
                return true;
            }
        }
        break;

    default:
        break;
    }

    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * MPointMyHost::GetPacket(int *pPktLen,TPacketType pktType)
{
    switch(pktType)
    {
    case TPACKET_TYPE_MPOINT:
        *pPktLen=mbaResponse[0]*2;
        return mbaHexResponse;

    case TPACKET_TYPE_HCI_LE_EVENT:
        *pPktLen=mbaResponse[2]*2 + 6;
        return mbaHexResponse;

    default:
        break;
    }
    return NULL;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void MPointMyHost::DropFromResponse(int nToDeleteHex)
{
    int nToDelete;
    nToDelete = nToDeleteHex / 2;
    nToDeleteHex = nToDelete * 2;

    if( nToDelete > mnRespCount )
    {
        nToDelete = mnRespCount;
    }

    memcpy(mbaHexResponse,&mbaHexResponse[nToDeleteHex],(mnHexRespCount-nToDeleteHex));
    memcpy(mbaResponse,&mbaResponse[nToDelete],(mnRespCount-nToDelete));

    mnHexRespCount -= nToDeleteHex;
    mnRespCount -= nToDelete;

    mbaHexResponse[mnHexRespCount]=0;
    mbaResponse[mnRespCount]=0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int MPointMyHost::DropPacket(TPacketType pktType)
{
    int nDropped = 0;
    int nPktLen;
    switch(pktType)
    {
    case TPACKET_TYPE_MPOINT:
        nPktLen = mbaResponse[0];
        if(mnRespCount >= nPktLen)
        {
            nDropped = nPktLen*2;
            if( mnRespCount > nPktLen )
            {
                memcpy(mbaHexResponse,&mbaHexResponse[nDropped],(mnHexRespCount-nDropped));
                memcpy(mbaResponse,&mbaResponse[nPktLen],(mnRespCount-nPktLen));
            }
            mnHexRespCount -= nDropped;
            mnRespCount -= nPktLen;
            mPacketCount++;
        }
        break;

    case TPACKET_TYPE_HCI_LE_EVENT:
        if(mnRespCount >= 3)
        {
            nPktLen = mbaResponse[2] + 3;
            if(mnRespCount >= nPktLen)
            {
                nDropped = nPktLen*2;
                if( mnRespCount > nPktLen )
                {
                    memcpy(mbaHexResponse,&mbaHexResponse[nDropped],(mnHexRespCount-nDropped));
                    memcpy(mbaResponse,&mbaResponse[nPktLen],(mnRespCount-nPktLen));
                }
                mnHexRespCount -= nDropped;
                mbaHexResponse[mnHexRespCount]=0;
                mnRespCount -= nPktLen;
                mbaResponse[mnRespCount]=0;
                mPacketCount++;
            }
        }
        break;

    default:
        break;
    }

    return nDropped;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool MPointMyHost::CheckFullPattern(void)
{
    return MiscCompareStrings(mbaHexResponse,mpPattern,mnPatternLen);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

bool MPointMyHost::CheckPartialPattern(void)
{
    return (MiscContainsPattern(mbaHexResponse,mnHexRespCount,mpPattern,mnPatternLen)!=NULL);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int MPointMyHost::CheckFullPatternMulti(void)
{
    int nMatchIndex = MiscCompareStringsMulti(mbaHexResponse,sMultiPattern,mNumOfSubPatterns);
    if(nMatchIndex >= 0)
    {
        mPatternMatchIndex = nMatchIndex;
    }
    return  nMatchIndex;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int MPointMyHost::CheckPartialPatternMulti(void)
{
    int nMatchIndex = MiscContainsPatternMulti(mbaHexResponse,mnHexRespCount,sMultiPattern,mNumOfSubPatterns);
    if(nMatchIndex >= 0)
    {
        mPatternMatchIndex = nMatchIndex;
    }
    return  nMatchIndex;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

bool MPointMyHost::CheckPartialDataPattern(unsigned int nChannel)
{
    char *pFullDataResp = GetDataInResponse(nChannel);
    return (MiscContainsPattern(pFullDataResp,mpPattern)!=NULL);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned int MPointMyHost::GetPacketCountInResp(void)
{
    unsigned char *pResp = (unsigned char *)mbaResponse;
    unsigned int nRespCount = mnRespCount;
    unsigned int nCount=0;
    unsigned int nPktLen;

    nPktLen = pResp[0];
    while(nRespCount >= nPktLen )
    {
        nCount++;
        nRespCount -= nPktLen;
        pResp = &pResp[nPktLen];
        nPktLen = pResp[0];
    }

    return nCount;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int MPointMyHost::PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg)
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
        nRetVal = mRxCount;
        break;
    }
    return  nRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#if 0
void MPointMyHost::FlushRxBuffer(void)
{
    mnHexRespCount   = 0;
    mbaHexResponse[0]= 0;
    TBaseUartHost::FlushRxBuffer();
}

static void mydummy(int a)
{
}
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void MPointMyHost::OnRxData(unsigned char *pRxData, unsigned int nRxDataLen)
{
    unsigned int nFreeSpace = mnHexMaxRespCount - mnHexRespCount ;
    unsigned int nConsumed;

    mRxCount += nRxDataLen;

    nConsumed = EncodeBinaryToHex(pRxData,
                                  nRxDataLen,
                                  &mbaHexResponse[mnHexRespCount],
                                  nFreeSpace);
    if( nConsumed != nRxDataLen )
    {
        TracePrint("%s::Not all rx data was consumed",GetHostName());
    }

    //display the hex digits received so far
    mpRegression->TraceRx(GetLogicalPort(),&mbaHexResponse[mnHexRespCount]);

    //update the hex digit count
    mnHexRespCount += nConsumed*2;

    //keep a copy of the raw data
    memcpy(&mbaResponse[mnRespCount],pRxData,nConsumed);
    mnRespCount += nConsumed;
    mbaResponse[mnRespCount]=0; /* don't really need to, but no harm */

    //check if at least one packet in buffer
    if(mnRespCount >= mbaResponse[0])
    {
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_PACKET,(void *)mbaHexResponse,GetLogicalPort());
    }
    if(mnRespCount >= 3)
    {
        if(mnRespCount >= (mbaResponse[2]+3))
        {
            ThrowStateMachineEvent(WIZMNGR_RESPONSE_TIEVENT,(void *)mbaHexResponse,GetLogicalPort());
        }
    }

    //check if the pattern has been found
    if(mpPattern && (mnPatternLen>0))
    {
        switch(mSearchType&0xFF)
        {
        case 0:
            if( CheckPartialPattern() )
            {
                /*
                Bug Fix : 25 Feb 2020
                If the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers.
                This can happen at fast baudrates or the main thread is busy with screen updates
                */
                mpPattern=NULL;
                mnPatternLen=0;

                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PARTIAL,(void *)mbaHexResponse,GetLogicalPort());
            }
            break;

        case 1:
            if( CheckFullPattern() )
            {
                /*
                Bug Fix : 25 Feb 2020
                If the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers.
                This can happen at fast baudrates or the main thread is busy with screen updates
                */
                mpPattern=NULL;
                mnPatternLen=0;

                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PATTERN,(void *)mbaHexResponse,GetLogicalPort());
            }
            break;

        case 2:
            if( CheckPartialDataPattern((mSearchType>>8)&0xFF) )
            {
                /*
                Bug Fix : 25 Feb 2020
                If the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers.
                This can happen at fast baudrates or the main thread is busy with screen updates
                */
                mpPattern=NULL;
                mnPatternLen=0;

                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PARTIAL,(void *)mbaHexResponse,GetLogicalPort());
            }
            break;

        case 3:
            if( CheckPartialPatternMulti() >= 0 )
            {
                /*
                Bug Fix : 25 Feb 2020
                If the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers.
                This can happen at fast baudrates or the main thread is busy with screen updates
                */
                mpPattern=NULL;
                mnPatternLen=0;

                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PARTIAL,(void *)mbaHexResponse,GetLogicalPort());
            }
            break;

        case 4:
            if( CheckFullPatternMulti() >= 0 )
            {
                /*
                Bug Fix : 25 Feb 2020
                If the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers.
                This can happen at fast baudrates or the main thread is busy with screen updates
                */
                mpPattern=NULL;
                mnPatternLen=0;

                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PATTERN,(void *)mbaHexResponse,GetLogicalPort());
            }
            break;

        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char *MPointMyHost::GetDataInResponse(unsigned int nChannel)
{
    char *pRetVal = NULL;
    char *pHexFullResp;

    if( (nChannel>0) && (nChannel<=255) )
    {
        pHexFullResp =  GetFullResponse();
        pRetVal  =  MiscMpExtractData(pHexFullResp,nChannel,mbaMpData,MAX_MPDATA_CACHELEN);
    }

    return pRetVal;
}


/******************************************************************************/
// END OF FILE
/******************************************************************************/


