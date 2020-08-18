/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      BTCONSOLEHOST.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include "BtConsoleHost.h"
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
BtConsoleHost::BtConsoleHost(TRegression *pRegression, unsigned int nLogicalPort) :
    TBaseHost(pRegression,nLogicalPort)
{
    SetHostType("BTCONSOLE");
    mpCommandIf = NULL;
    mnRespCount = 0;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
__fastcall BtConsoleHost::~BtConsoleHost()
{
    Close();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void BtConsoleHost::OnWindowsMessage( WPARAM wparam )
{
    if(mpCommandIf)
    {
        mpCommandIf->OnWindowsMessage(wparam);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BtConsoleHost *CreateBtConsoleHost(TRegression *pRegression, unsigned int nLogicalPort)
{
    BtConsoleHost *pHost = new BtConsoleHost(pRegression,nLogicalPort);
    return pHost;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::Close(void)
{
    if(mpCommandIf)
    {
        SendString("END\r");
        Sleep(3500);
        delete mpCommandIf;
        mpCommandIf=NULL;
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool BtConsoleHost::Open(int nComport, UBaseOpenParms *pParms)
{
    bool fRetVal = false;
    mpCommandIf = new CCommandIF(this, 0x4000, GetLogicalPort(), WM_BTCONSOLE_EVT);
    if(mpCommandIf)
    {
        if( mpCommandIf->Initialise("BtConsole.exe",".","") )
        {
            fRetVal = true;
        }
    }
    return fRetVal;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool BtConsoleHost::IsOpen(void)
{
    return (mpCommandIf==NULL) ? false : true ;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool BtConsoleHost::ChangeCommsParameters(UBaseOpenParms *pCommParms)
{
    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void BtConsoleHost::OnInitialise( void )
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void BtConsoleHost::OnData( void )
{
    char *pDest = &mbaResponse[mnRespCount];
    int  nRead = 0;
    char *pMatch;

    do
    {
        nRead = 0;
        if(mpCommandIf)
        {
            nRead = mpCommandIf->Read(mbaCacheRaw, ONDATACACHELEN );
            mbaCacheRaw[nRead]=0;
        }
        if(nRead)
        {
            unsigned escaledLen = SrcEscapeStringNullAnd0x8X(mbaCacheRaw,nRead,mbaCache,sizeof(mbaCache));
            mbaCache[escaledLen]=0;

            mpRegression->TraceRx(GetLogicalPort(),mbaCache);
            memcpy(pDest,mbaCache,escaledLen);
            mnRespCount += escaledLen;
            mbaResponse[mnRespCount]=0;
        }
    }
    while(nRead);

    //check for some standard responses
    if(strstr(mbaResponse,"\r\nOK\r\n"))
    {
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_OK,(void *)mbaResponse,GetLogicalPort());
    }
    else if((pMatch=strstr(mbaResponse,"\r\nERROR")) != NULL)
    {
        int evtData=0x1234;
        if( pMatch[7] == ' ' )
        {
            evtData = atoi(&mbaResponse[7]);
        }
        ThrowStateMachineEvent(WIZMNGR_RESPONSE_ERROR,evtData,GetLogicalPort());
    }

    //check if the pattern has been found
    if(mpPattern && (mnPatternLen>0))
    {
        if(mSearchType)
        {
            if( memcmp(mpPattern,mbaResponse,mnPatternLen)==0 )
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

                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PATTERN,(void *)mbaResponse,GetLogicalPort());
            }
        }
        else
        {
            if( strstr(mbaResponse,mpPattern) )
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

                ThrowStateMachineEvent(WIZMNGR_RESPONSE_PARTIAL,(void *)mbaResponse,GetLogicalPort());
            }
        }
    }

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void BtConsoleHost::OnReady( void )
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::SetBreak(void)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::ClearBreak(void)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::DeAssertDTR(void)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::AssertDTR(void)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::FlushTxBuffer(void)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool BtConsoleHost::GetCTS(void)
{
    return true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool BtConsoleHost::GetDSR(void)
{
    return true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool BtConsoleHost::GetRI(void)
{
    return false;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool BtConsoleHost::GetDCD(void)
{
    return false;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::SetDTR(bool newState)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::SetRTS(bool newState)
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::FlushRxBuffer(void)
{
    if(mpCommandIf)
    {
        mpCommandIf->Flush();
    }
    mnRespCount=0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * BtConsoleHost::GetFullResponse(void)
{
    return mbaResponse;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::SendCommand(char *pCmd)
{
    //does not make sense so just ....
    SendString(pCmd);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::SendString(char *pCmd)
{
    if(mpCommandIf)
    {
        AnsiString asStr = pCmd;
        //mpCommandIf->SendAsynch(pCmd);
        mpCommandIf->Send(asStr);
        mpRegression->TraceTx(GetLogicalPort(),pCmd);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void BtConsoleHost::SendBlock(unsigned char *pCmd, int nBlockLen)
{
    if(mpCommandIf)
    {
        mpCommandIf->Send((char *)pCmd,nBlockLen);
        mpRegression->TraceUnicodeTx(GetLogicalPort(),pCmd,nBlockLen);
    }
}

//=============================================================================
//=============================================================================
void BtConsoleHost::SendDataBlock(int nChannel, unsigned char *pCmd, int nBlockLen)
{
    SendBlock(pCmd, nBlockLen);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned short BtConsoleHost::TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen)
{
    unsigned short nRetVal = 0;
    if(mpCommandIf)
    {
        if(mpCommandIf->SendAsynch((char *)pSendBlock,(DWORD)nBlockLen))
        {
            nRetVal = nBlockLen;
        }
    }
    return nRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * BtConsoleHost::SetRxFilterPattern(char *pPattern,unsigned short nPatternLen,int nSearchType)
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
bool BtConsoleHost::IsAllowedSendOutcome(EExpectedOutcome eOutCome)
{
    return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int BtConsoleHost::PortIoctl(int ioctlId, int nArg, void *pInArg, void *pOutArg)
{
    return 0;
}


/******************************************************************************/
// END OF FILE
/******************************************************************************/


