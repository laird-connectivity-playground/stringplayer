/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      ATMYHOST.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include "AtMyHost.h"
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
AtMyHost::AtMyHost(TRegression *pRegression, unsigned int nLogicalPort) :
    TBaseUartHost(pRegression,nLogicalPort)
{
    mApdComPort = NULL;
    mAtHandle   = ATHANDLE_NULL;
    mRxBlockLen = 0;
    mpSubStr    = NULL;
    mSubStrBufLen  = 0;
    SetHostType("TAPDCOMPORT");
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
__fastcall AtMyHost::~AtMyHost()
{
    if( mpSubStr ) MiscMemFree(mpSubStr);
    mpSubStr = NULL;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TBaseHost *CreateAtMyHost(TRegression *pRegression, unsigned int nLogicalPort)
{
    TBaseHost *pHost = new AtMyHost(pRegression,nLogicalPort);
    return pHost;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
const char *AtMyHost::GetHostName(void)
{
    return "AtMyHost";
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void  AtMyHost::OnOpen(void)
{
    mAtHandle = AtHostCreateChannel( (void *)this,
                                     16384, //accommodates at least 55 inquiry responses with names
                                     AtMyHostTransmit,
                                     AtMyHostOnCrLf,
                                     AtMyHostOnAsyncResponse,
                                     AtMyHostOnBlockWait,
                                     AtMyHostOnResponseOverflow,
                                     AtMyHostGetTickCount
                                   );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void AtMyHost::OnRxData(unsigned char *pRxData, unsigned int nRxDataLen)
{
    unsigned int escaledLen;
    unsigned hostprocessed;

    mRxCount += nRxDataLen;  //keep total count

    escaledLen = SrcEscapeStringNullAnd0x8X(pRxData,nRxDataLen,mbaCache,sizeof(mbaCache));
    mbaCache[escaledLen]=0;
    memcpy(&mbaRxBlock[mRxBlockLen], mbaCache, escaledLen);
    mpRegression->TraceRx(GetLogicalPort(),mbaCache);
    mRxBlockLen += escaledLen;
    mbaRxBlock[mRxBlockLen]=0;
    hostprocessed = AtHostProcessRxData(mAtHandle,mbaRxBlock,mRxBlockLen);
    mRxBlockLen -= hostprocessed;
    if(mRxBlockLen>0)
    {
        memcpy(mbaRxBlock,&mbaRxBlock[hostprocessed],mRxBlockLen);
    }
}

//=============================================================================
//=============================================================================
unsigned short AtMyHost::TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen)
{
    return __TransmitBlock(pSendBlock,nBlockLen);
}

//=============================================================================
//=============================================================================
void AtMyHost::SendCommand(char *pCmd)
{
    if(mAtHandle!=ATHANDLE_NULL)
    {
        strcpy(mbaLastCmd,pCmd);
        //TracePrint("ATCMD:> %s",pCmd);
        mpCurSubStr = NULL;
        AtHostSendCommand(mAtHandle,pCmd);
        mpRegression->TraceTx(GetLogicalPort(),pCmd);
    }
}

//=============================================================================
//=============================================================================
void AtMyHost::SendString(char *pCmd)
{
    if(mAtHandle!=ATHANDLE_NULL)
    {
        mpCurSubStr = NULL;
        if( strlen(pCmd) > 0 )
        {
            AtHostSendString(mAtHandle,pCmd);
            mpRegression->TraceTx(GetLogicalPort(),pCmd);
        }
    }
}

//=============================================================================
//=============================================================================
void AtMyHost::SendBlock(unsigned char *pCmd, int nBlockLen)
{
    if(mAtHandle!=ATHANDLE_NULL)
    {
        mpCurSubStr = NULL;
        if( nBlockLen > 0 )
        {
            AtHostSendBlock(mAtHandle,pCmd,nBlockLen);
            mpRegression->TraceUnicodeTx(GetLogicalPort(),pCmd,nBlockLen);
        }
    }
}

//=============================================================================
//=============================================================================
void AtMyHost::SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen)
{
    SendBlock(pCmd, nBlockLen);
}

//=============================================================================
//=============================================================================
char * AtMyHost::GetFullResponse(void)
{
    return AtHostGetFullResponse(mAtHandle);
}

//=============================================================================
//=============================================================================
void AtMyHost::DropFromResponse(int nToDelete)
{
    return AtHostDiscardOldestBlock(mAtHandle,nToDelete);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int AtMyHost::PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg)
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

    case 2:
        //nRetVal = strlen(GetFullResponse());
        nRetVal = AtHostGetFullResponseLength(mAtHandle);
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
char * AtMyHost::SetRxFilterPattern(char *pPattern,unsigned short nPatternLen, int nSearchType)
{
    return AtHostSetRxPattern(mAtHandle,pPattern,nPatternLen,(nSearchType?true:false));
}

//=============================================================================
//=============================================================================
void AtMyHost::OnFlushRxBuffer(bool fPostFlush)
{
    AtHostFlushRespBuffer(mAtHandle);
}

//=============================================================================
//=============================================================================
void AtMyHost::OnFlushTxBuffer(bool fPostFlush)
{
}

//=============================================================================
//=============================================================================
bool AtMyHost::IsAllowedSendOutcome(EExpectedOutcome eOutCome)
{
    return true;
}

//=============================================================================
//=============================================================================
unsigned short AtMyHostTransmit(void *pContext,unsigned char *pSendBlock,unsigned short nBlockLen)
{
    AtMyHost *pAtMyHost = (AtMyHost *)pContext;

    //Routine to actually transmit data
    if(pAtMyHost)
    {
#if defined(_DEBUG)
        //TracePrint("AtMyHostTransmit called");
#endif
        return pAtMyHost->TransmitBlock(pSendBlock,nBlockLen);
    }
    return 0;
}

//=============================================================================
//=============================================================================
void AtMyHostOnCrLf(void *pContext,char *pResp)
{
    AtMyHost *pAtMyHost = (AtMyHost *)pContext;
    int nModuleId = pAtMyHost->GetLogicalPort();

    //pAtMyHost->mpRegression->DisplayResponse(pAtMyHost->GetLogicalPort(),pResp);

    char *pData = (char *)MiscMemAlloc(strlen(pResp)+1);
    strcpy(pData,pResp);
    ThrowStateMachineEvent(WIZMNGR_RESPONSE_CRLF,(void *)pData,nModuleId);
}

//=============================================================================
//=============================================================================
void AtMyHostOnAsyncResponse(void *pContext,ATRESPONSE atResponse, char *pResp)
{
    int evtData=0;
    AtMyHost *pAtMyHost = (AtMyHost *)pContext;

    int nModuleId = pAtMyHost->GetLogicalPort();


    switch(atResponse)
    {
    case ATRESPONSE_OK:
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_OK,nModuleId);
        break;
    case ATRESPONSE_ERROR:
        if( pResp[5] == ' ' )
        {
            evtData = atoi(&pResp[6]);
        }
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_ERROR,evtData,nModuleId);
        break;
    case ATRESPONSE_CONNECT:
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_CONNECT,(void *)pResp,nModuleId);
        break;
    case ATRESPONSE_PATTERN:
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_PATTERN,(void *)pResp,nModuleId);
        break;
    case ATRESPONSE_PARTIAL_PATTERN:
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_PARTIAL,(void *)pResp,nModuleId);
        break;
    case ATRESPONSE_NO_CARRIER:
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_NOCARRIER,nModuleId);
        break;
    case ATRESPONSE_TIMEOUT:
    case ATRESPONSE_UNKNOWN:
    default:
        //DEVMESSAGE("Code to be written");
        break;
    }
}

//=============================================================================
//=============================================================================
bool AtMyHostOnBlockWait(void *pContext)
{
    return ProcessApplicationMessages();
}

//=============================================================================
//=============================================================================
void AtMyHostOnResponseOverflow(void *pContext)
{
    //DEVMESSAGE("AtMyHostOnResponseOverflow called");
}

//=============================================================================
//=============================================================================
unsigned int AtMyHostGetTickCount(void *pContext)
{
    return GetTickCount();
}



/******************************************************************************/
// END OF FILE
/******************************************************************************/


