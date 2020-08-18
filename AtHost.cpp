/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      ATHOST.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"
#include "Misc.h"
#include "AtHost.h"

/******************************************************************************/
// Local Defines
/******************************************************************************/

#define MAX_ATHOSTS                 16

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

/******************************************************************************/
// Local Forward Function Declarations
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
// Local Functions or Private Members
/******************************************************************************/
/******************************************************************************/

//=============================================================================
//=============================================================================
static void AtHostParseResponse(ATHOST *pAtHost, unsigned char *pResp)
{
    //unsigned short nMask = pAtHost->nRespMask;
    unsigned short nMask = 0xFFFF;

    if( (strncmpi("OK",pResp,2)==0) && (nMask & ATRESPONSE_OK) )
    {
        pAtHost->nRxResponseType |= ATRESPONSE_OK;
        pAtHost->OnAsyncResp(pAtHost->pContext,ATRESPONSE_OK,pResp);
    }
    else if( (strncmpi("ERROR",pResp,5)==0) && (nMask & ATRESPONSE_ERROR) )
    {
        pAtHost->nRxResponseType |= ATRESPONSE_ERROR;
        pAtHost->OnAsyncResp(pAtHost->pContext,ATRESPONSE_ERROR,pResp);
    }
    else if( (strncmpi("CONNECT",pResp,7)==0) && (nMask & ATRESPONSE_CONNECT) )
    {
        pAtHost->nRxResponseType |= ATRESPONSE_CONNECT;
        pAtHost->OnAsyncResp(pAtHost->pContext,ATRESPONSE_CONNECT,pResp);
    }
    else if( (strncmpi("NO CARRIER",pResp,10)==0) && (nMask & ATRESPONSE_NO_CARRIER) )
    {
        pAtHost->nRxResponseType |= ATRESPONSE_NO_CARRIER;
        pAtHost->OnAsyncResp(pAtHost->pContext,ATRESPONSE_NO_CARRIER,pResp);
    }
}

/******************************************************************************/
/******************************************************************************/
// Global Functions or Public Members
/******************************************************************************/
/******************************************************************************/

//=============================================================================
//=============================================================================
ATHANDLE AtHostCreateChannel(void *pContext,
                             unsigned short nMaxRespLen,
                             ATHOSTCALLBACK_TRANSMIT Transmit,
                             ATHOSTCALLBACK_ONCRLF OnCrlf,
                             ATHOSTCALLBACK_ONASYNCRESP OnAsyncResp,
                             ATHOSTCALLBACK_ONBLOCKWAIT OnWait,
                             ATHOSTCALLBACK_ONRESP_OVERFLOW OnOverFlow,
                             ATHOSTCALLBACK_GET_TICK_COUNT GetTickCount)
{
    ATHOST *pRetVal = (ATHOST *)MiscMemAlloc(sizeof(ATHOST)+nMaxRespLen);
    if(pRetVal && (pContext != NULL))
    {
        pRetVal->pContext    = pContext;
        pRetVal->nMaxRespLen = nMaxRespLen;
        pRetVal->Transmit    = Transmit;
        pRetVal->OnCrLf      = OnCrlf;
        pRetVal->OnAsyncResp = OnAsyncResp;
        pRetVal->OnWait      = OnWait;
        pRetVal->OnOverFlow  = OnOverFlow;
        pRetVal->GetTickCount= GetTickCount;
        pRetVal->pPattern    = NULL;

        pRetVal->pRespBuf    = (unsigned char *)(pRetVal+1);
        pRetVal->nRespLen    = 0;
        pRetVal->busyMask    = 0x0000;  //no routines are busy
    }

    return (ATHANDLE)pRetVal;
}

//=============================================================================
//=============================================================================
void AtHostFlushRespBuffer(ATHANDLE handle)
{
    ATHOST *pAtHost = (ATHOST *)handle;

    pAtHost->nRespLen     = 0;
    pAtHost->nLastCrLfEnd = 0;
    pAtHost->pRespBuf[0]  = 0;
}

//=============================================================================
//=============================================================================
void AtHostSendCommand(ATHANDLE handle,char *pCmd)
{
    ATHOST *pAtHost = (ATHOST *)handle;

    AtHostFlushRespBuffer(handle);
    pAtHost->nRespMask       = ATRESPONSE_ALL;
    pAtHost->nRxResponseType = 0x0000;
    pAtHost->Transmit(pAtHost->pContext,pCmd,strlen(pCmd));
    //pAtHost->Transmit(pAtHost->pContext,"\r",1);
}

//=============================================================================
//=============================================================================
void AtHostSendString(ATHANDLE handle,char *pCmd)
{
    ATHOST *pAtHost = (ATHOST *)handle;

    pAtHost->Transmit(pAtHost->pContext,pCmd,strlen(pCmd));
}

//=============================================================================
//=============================================================================
ATRESPONSE AtHostSendCommandAndWait(ATHANDLE handle,char *pCmd,ATRESPONSE respMask, unsigned int nTimeoutMs)
{
    ATHOST *pAtHost = (ATHOST *)handle;
    unsigned int nTimeStart;
    unsigned int nTimeNow;
    bool fTerminate=false;

    pAtHost->nRespMask       = respMask;
    pAtHost->nRxResponseType = 0x0000;
    AtHostSendCommand(handle,pCmd);
    nTimeStart = pAtHost->GetTickCount(pAtHost->pContext);
    while((pAtHost->nRxResponseType == 0x0000) && !fTerminate)
    {
        fTerminate = pAtHost->OnWait(pAtHost->pContext);
        if(fTerminate)
        {
            pAtHost->nRxResponseType |= ATRESPONSE_UNKNOWN;
        }
        else
        {
            nTimeNow = pAtHost->GetTickCount(pAtHost->pContext);;
            if( (nTimeNow-nTimeStart) > nTimeoutMs )
            {
                pAtHost->nRxResponseType |= ATRESPONSE_TIMEOUT;
            }
        }
    }
    return (ATRESPONSE)pAtHost->nRxResponseType;
}

//=============================================================================
//=============================================================================
void AtHostSendBlock(ATHANDLE handle,unsigned char *pTxBlock, unsigned short nLen)
{
    ATHOST *pAtHost = (ATHOST *)handle;

    pAtHost->Transmit(pAtHost->pContext,pTxBlock,nLen);
}

//=============================================================================
//=============================================================================
unsigned short  AtHostProcessRxData(ATHANDLE handle,unsigned char *pRxBlock, unsigned short nLen)
{
    ATHOST *pAtHost = (ATHOST *)handle;
    int     nFree;
    char   *pTerminator;
    int     nSubRespLen;
    unsigned char bTempChr;

    if( pAtHost->busyMask & ATHOST_MASK_ROUTINE_PROCESS_RXDATA )
    {   //Already in this routine so return immediately
        return 0;
    }
    else
    {   //prevent re-entry
        pAtHost->busyMask |= ATHOST_MASK_ROUTINE_PROCESS_RXDATA;

        /*
        If the cache in pAtHost->pRespBuf does not have enough free
        space to accomodate the nLen bytes that have just arrived then
        drop whole CR terminate lines
        */
        nFree = pAtHost->nMaxRespLen - pAtHost->nRespLen;
        while(nLen > nFree)
        {
            pAtHost->OnOverFlow(pAtHost->pContext);
            AtHostDiscardToFirstCrLf(handle);
            nFree = pAtHost->nMaxRespLen - pAtHost->nRespLen;
        }
        /*
        There is space in the cache, so copy the new data into it
        */
        memcpy(&pAtHost->pRespBuf[pAtHost->nRespLen],pRxBlock,nLen);
        pAtHost->nRespLen += nLen;
        pAtHost->pRespBuf[pAtHost->nRespLen]=0;

        /* Check for OK/ERROR/CONNECT/NO CARRIER  strings in the cache since the last chunk*/
        if( (pAtHost->nRespLen - pAtHost->nLastCrLfEnd) > 2)
        {
            pTerminator = strstr( (char *)&pAtHost->pRespBuf[pAtHost->nLastCrLfEnd],(const char *)"\r\n");
            while( pTerminator )
            {
                nSubRespLen = (pTerminator - (char *)&pAtHost->pRespBuf[pAtHost->nLastCrLfEnd]) + 2;
                bTempChr = pAtHost->pRespBuf[pAtHost->nLastCrLfEnd + nSubRespLen];
                pAtHost->pRespBuf[pAtHost->nLastCrLfEnd + nSubRespLen] = 0;
                pAtHost->OnCrLf(pAtHost->pContext,&pAtHost->pRespBuf[pAtHost->nLastCrLfEnd]);

                /* Check for OK/ERROR/CONNECT/NO CARRIER  */
                AtHostParseResponse(pAtHost, &pAtHost->pRespBuf[pAtHost->nLastCrLfEnd]);

                pAtHost->pRespBuf[pAtHost->nLastCrLfEnd + nSubRespLen] = bTempChr;
                pAtHost->nLastCrLfEnd += nSubRespLen;
                pTerminator = strstr( (char *)&pAtHost->pRespBuf[pAtHost->nLastCrLfEnd],(const char *)"\r\n");
            }
        }
        pAtHost->busyMask &= ~ATHOST_MASK_ROUTINE_PROCESS_RXDATA;
    }

    //check if the pattern has been found
    if(pAtHost->pPattern && (pAtHost->nPatternLen>0))
    {
        if(pAtHost->fFullString)
        {
            if( memcmp(pAtHost->pPattern,pAtHost->pRespBuf,pAtHost->nPatternLen)==0 )
            {
                pAtHost->OnAsyncResp(pAtHost->pContext,ATRESPONSE_PATTERN,pAtHost->pRespBuf);
                /*
                Bug Fix : 9 Aug 2014
                At fast baudrates if the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers
                */
                pAtHost->pPattern=NULL;
                pAtHost->nPatternLen=0;
            }
        }
        else
        {
            /* this will allow for * wildcard */
            if( MiscContainsPattern((char *)pAtHost->pRespBuf,(char *)pAtHost->pPattern) )
            {
                pAtHost->OnAsyncResp(pAtHost->pContext,ATRESPONSE_PARTIAL_PATTERN,pAtHost->pRespBuf);
                /*
                Bug Fix : 9 Aug 2014
                At fast baudrates if the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers
                */
                pAtHost->pPattern=NULL;
                pAtHost->nPatternLen=0;
            }
        }
    }

    return nLen;
}

//=============================================================================
// Discard everything or up to the first CRLF
//=============================================================================
void AtHostDiscardToFirstCrLf(ATHANDLE handle)
{
    ATHOST *pAtHost = (ATHOST *)handle;
    char   *pTerminator;

    pTerminator = strstr( (char *)pAtHost->pRespBuf,(const char *)"\r\n");
    if(pTerminator)
    {
        int nToDelete  = (pTerminator - pAtHost->pRespBuf) + 2;
        int nRemainder = pAtHost->nRespLen-nToDelete;
        memcpy(pAtHost->pRespBuf,(pTerminator+2),nRemainder);
        pAtHost->nRespLen = nRemainder;
    }
    else
    {   //discard everything
        pAtHost->nRespLen = 0;
    }
    pAtHost->pRespBuf[pAtHost->nRespLen]=0;
}

//=============================================================================
//=============================================================================
void AtHostDiscardOldestBlock(ATHANDLE handle, int nToDelete)
{
    ATHOST *pAtHost = (ATHOST *)handle;

    if(nToDelete < pAtHost->nRespLen)
    {
        pAtHost->nRespLen = pAtHost->nRespLen-nToDelete;
        memcpy(pAtHost->pRespBuf,&pAtHost->pRespBuf[nToDelete],pAtHost->nRespLen);
    }
    else
    {   //discard everything
        pAtHost->nRespLen = 0;
    }
    pAtHost->pRespBuf[pAtHost->nRespLen]=0;
}

//=============================================================================
//=============================================================================
char *AtHostGetFullResponse(ATHANDLE handle)
{
    ATHOST *pAtHost = (ATHOST *)handle;

    return  (char *)pAtHost->pRespBuf;
}

//=============================================================================
//=============================================================================
unsigned short AtHostGetFullResponseLength(ATHANDLE handle)
{
    ATHOST *pAtHost = (ATHOST *)handle;

    return  pAtHost->nRespLen;
}

//=============================================================================
//=============================================================================
char * AtHostSetRxPattern(ATHANDLE handle,char *pPattern,unsigned short nPatternLen, bool fFullStr)
{
    ATHOST *pAtHost = (ATHOST *)handle;

    pAtHost->pPattern    = pPattern;
    pAtHost->nPatternLen = nPatternLen;
    pAtHost->fFullString = fFullStr;
    if(pPattern)
    {
        if(nPatternLen==0)
        {
            pAtHost->pPattern    = NULL;
        }
    }
    return pPattern;
}


/******************************************************************************/
// END OF FILE
/******************************************************************************/



