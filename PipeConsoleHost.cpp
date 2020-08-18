/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      PIPECONSOLEHOST.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include "PipeConsoleHost.h"
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
PipeConsoleHost::PipeConsoleHost(TRegression *pRegression, unsigned int nLogicalPort, char *pFullPathAppName, char *pCurrentDir) :
    TBaseHost(pRegression,nLogicalPort)
{
    SetHostType("PIPE");

    //save application name and options for later use
    strncpy(mbaFullPathAppNameAndOptions,pFullPathAppName, PIPE_APP_FULLPATHpOPTIONS_MAXLEN );
    mbaFullPathAppNameAndOptions[PIPE_APP_FULLPATHpOPTIONS_MAXLEN]=0;

    strncpy(mbaAppCurrentDir,pCurrentDir, PIPE_APP_CURDIR_MAXLEN );
    mbaAppCurrentDir[PIPE_APP_CURDIR_MAXLEN]=0;

    mpChildProc = NULL;
    mnRespCount = 0;
    mTotalRxCount = 0;
    mnLogicalPort = nLogicalPort;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
__fastcall PipeConsoleHost::~PipeConsoleHost()
{
    Close();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void PipeConsoleHost::OnWindowsMessage( WPARAM wparam )
{
    if(mpChildProc)
    {
        mpChildProc->OnWindowsMessage(wparam);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PipeConsoleHost *CreatePipeConsoleHost(TRegression *pRegression, unsigned int nLogicalPort, char *pAppName, char *pAppOptions)
{
    PipeConsoleHost *pHost = new PipeConsoleHost(pRegression,nLogicalPort,pAppName,pAppOptions);
    return pHost;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::Close(void)
{
    if(mpChildProc)
    {
        delete mpChildProc;
        mpChildProc=NULL;
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PipeConsoleHost::Open(int nComport, UBaseOpenParms *pParms)
{
    bool fRetVal = false;
    mpChildProc = new CChildProc(this, 0x4000, GetLogicalPort(), WM_PIPECONSOLE_EVT, mnLogicalPort);
    if(mpChildProc)
    {
        fRetVal = mpChildProc->Launch(mbaFullPathAppNameAndOptions,mbaAppCurrentDir);
    }
    return fRetVal;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PipeConsoleHost::IsOpen(void)
{
    return (mpChildProc==NULL) ? false : true ;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PipeConsoleHost::ChangeCommsParameters(UBaseOpenParms *pCommParms)
{
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void PipeConsoleHost::OnRxData( void )
{
    int  nRead = 0;
    char *pMatch;

    do
    {
        nRead = 0;
        if(mpChildProc)
        {
            nRead = mpChildProc->Read(mbaCacheRaw, ONDATACACHELEN );
            mbaCacheRaw[nRead]=0;
            mTotalRxCount += nRead;
        }
        if(nRead)
        {
            unsigned escapedLen = SrcEscapeStringNullAnd0x8X(mbaCacheRaw,nRead,mbaCache,sizeof(mbaCache));
            mbaCache[escapedLen]=0;

            mpRegression->TraceRx(GetLogicalPort(),mbaCache);
            memcpy(&mbaResponse[mnRespCount],mbaCache,escapedLen);
            mnRespCount += escapedLen;
            mbaResponse[mnRespCount]=0;
        }
    }
    while(nRead);

    //check if the pattern has been found
    if(mpPattern && (mnPatternLen>0))
    {
        if(mSearchType)
        {
            if( memcmp(mpPattern,mbaResponse,mnPatternLen)==0 )
            {
                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PATTERN,(void *)mbaResponse,GetLogicalPort());

                /*
                Bug Fix : 9 Aug 2014
                At fast baudrates if the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers
                */
                mpPattern=NULL;
                mnPatternLen=0;
            }
        }
        else
        {
            char *pFound=strstr(mbaResponse,mpPattern);
            if( pFound )
            {
                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PARTIAL,(void *)mbaResponse,GetLogicalPort());

                /*
                Bug Fix : 9 Aug 2014
                At fast baudrates if the pattern happens multiple times before
                the event in the windows message loop is processed, we end up with
                multiple triggers
                */
                mpPattern=NULL;
                mnPatternLen=0;
            }
        }
    }

}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::SetBreak(void)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::ClearBreak(void)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::DeAssertDTR(void)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::AssertDTR(void)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::FlushTxBuffer(void)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PipeConsoleHost::GetCTS(void)
{
    return true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PipeConsoleHost::GetDSR(void)
{
    return true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PipeConsoleHost::GetRI(void)
{
    return false;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool PipeConsoleHost::GetDCD(void)
{
    return false;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::SetDTR(bool newState)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::SetRTS(bool newState)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::FlushRxBuffer(void)
{
    if(mpChildProc)
    {
        mpChildProc->Flush();
    }
    mnRespCount=0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * PipeConsoleHost::GetFullResponse(void)
{
    return mbaResponse;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::DropFromResponse(int nToDelete)
{
    if( nToDelete <  mnRespCount )
    {
        memcpy(mbaResponse,&mbaResponse[nToDelete],(mnRespCount-nToDelete));
        mnRespCount -= nToDelete;
    }
    else
    {
        mnRespCount = 0;
    }
    mbaResponse[mnRespCount]=0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned short PipeConsoleHost::TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen)
{
    unsigned short nRetVal = 0;
    if(mpChildProc)
    {
        if(mpChildProc->Send((char *)pSendBlock,(DWORD)nBlockLen))
        {
            nRetVal = nBlockLen;
        }
    }
    return nRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::SendBlock(unsigned char *pCmd, int nBlockLen)
{
    if(mpChildProc)
    {
        mpChildProc->Send((char *)pCmd,nBlockLen);
        mpRegression->TraceUnicodeTx(GetLogicalPort(),pCmd,nBlockLen);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::SendString(char *pCmd)
{
    if(mpChildProc)
    {
        int nCmdLen = strlen(pCmd);
        mpChildProc->Send(pCmd,nCmdLen);
        mpRegression->TraceTx(GetLogicalPort(),pCmd);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PipeConsoleHost::SendCommand(char *pCmd)
{
    SendString(pCmd);
}

//=============================================================================
//=============================================================================
void PipeConsoleHost::SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen)
{
    SendBlock(pCmd, nBlockLen);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * PipeConsoleHost::SetRxFilterPattern(char *pPattern,unsigned short nPatternLen,int nSearchType)
{
    mpPattern     = pPattern;
    mnPatternLen  = nPatternLen;
    mSearchType   = nSearchType;
    if(pPattern)
    {
        if(nPatternLen==0)
        {
            mpPattern = NULL;
        }
    }
    return mpPattern;
}

//=============================================================================
//=============================================================================
bool PipeConsoleHost::IsAllowedSendOutcome(EExpectedOutcome eOutCome)
{
    return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int PipeConsoleHost::PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg)
{
    int nRetVal=0;
    switch(ioctlId)
    {
    case 0:
        nRetVal = mTotalRxCount;
        mTotalRxCount = 0;
        break;

    case 1:
        nRetVal = mTotalRxCount;
        break;

    case 2:
        nRetVal = strlen(GetFullResponse());
        break;

    default:
        break;
    }
    return  nRetVal;
}

/******************************************************************************/
// END OF FILE
/******************************************************************************/


