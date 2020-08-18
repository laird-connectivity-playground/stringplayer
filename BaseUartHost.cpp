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

#include "BaseUartHost.h"
#include "TRegression.h"
#include "Misc.h"
/******************************************************************************/
// Local Defines
/******************************************************************************/

#define __TXBUF_SIZE__          (1024*32)
#define __RXBUF_SIZE__          (1024*32)

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
unsigned int TBaseUartHost::mInstanceCount = 0;

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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TParity TBaseUartHost::GetParityEnum(int nParity)
{
    TParity retVal = pNone;

    switch(nParity)
    {
    case 0: retVal = pNone; break;
    case 1: retVal = pOdd; break;
    case 2: retVal = pEven; break;
    default:retVal = pNone; break;
    }
    return retVal;
}

/******************************************************************************/
/******************************************************************************/
// Global Functions or Public Members
/******************************************************************************/
/******************************************************************************/

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
TBaseUartHost::TBaseUartHost(TRegression *pRegression, unsigned int nLogicalPort) :
    TBaseHost(pRegression,nLogicalPort)
{
    mfCalled     = false;
    mApdComPort  = NULL;
    mCacheRawLen = READCACHELEN;
    SetHostType("TAPDCOMPORT");

    mnPostOpenWaitMs=0;
    mnPostCloseWaitMs=0;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
__fastcall TBaseUartHost::~TBaseUartHost()
{
    Close();
}

//------------------------------------------------------------------------------
#if 0
// This is a sample of what the derived class needs to do
//------------------------------------------------------------------------------
TBaseHost *CreateTBaseUartHost(TRegression *pRegression, unsigned int nLogicalPort)
{
    TBaseHost *pHost = new TBaseUartHost(pRegression,nLogicalPort);
    return pHost;
}
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int  TBaseUartHost::PortExists(int PortNo)
{
    //Returns:-
    // 0 if port exists
    // 1 if port does not exist
    // 2 if port is already open
    // 3 if TApdComPort Class could not be created
    // 4 if port cannot be opened for some other reason

    int nRetVal = 3;
    TApdComPort    *pApd = new TApdComPort(NULL);
    if(pApd)
    {
        nRetVal = 0;
        try
        {
            pApd->ComNumber = (Word)PortNo;
            //TracePrint("Closing COM%d",PortNo);
            pApd->Open = FALSE;
            //TracePrint("Opening COM%d",PortNo);
            pApd->Open = TRUE;
            //TracePrint("Closing COM%d",PortNo);
            pApd->Open = FALSE;
            nRetVal = 0;
        }
        catch (EBadId*)
        {
            nRetVal = 1;
        }
        catch (EAlreadyOpen &)
        {
            nRetVal = 2;
        }
        catch (...)
        {
            nRetVal = 4;
        }

        delete pApd;
    }
    return nRetVal;
}

//=============================================================================
//=============================================================================
bool TBaseUartHost::IsOpen(void)
{
    bool fIsOpen = false;

    if(mApdComPort)
    {
        if( mApdComPort->ComNumber > 0)
        {
            fIsOpen = mApdComPort->Open;
        }
    }

    return fIsOpen;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TBaseUartHost::Close()
{
    TracePrint("Enter TBaseUartHost::Close()");
    if( mApdComPort )
    {
        //TracePrint("Closing COM%d",mApdComPort->ComNumber);
        mApdComPort->Open  = false;

        if(mnPostCloseWaitMs)
        {
            Sleep(mnPostCloseWaitMs);
        }

        delete mApdComPort;
        mApdComPort = NULL;
        if(mInstanceCount)
        {
            mInstanceCount--;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool  TBaseUartHost::CheckForPort(TApdCustomComPort* Port, int PortNo)
{
    try
    {
        Port->Open = FALSE;
        Port->ComNumber = (Word)PortNo;
        Port->Open = TRUE;
    }
    catch (EBadId*)
    {
        return  FALSE;
    }
    catch (...)
    {
        return  FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TBaseUartHost::Open(int nComport, UBaseOpenParms *pParms)
{
    bool fRetVal = false; //assume cannot open the port

    TracePrint("Open [%s]: COM%d, Baud=%d, Parity=%d, Stopbits= %d, Databits=%d, Handshaking=%d, OpenWait=%d, CloseWait=%d",
                     GetHostName(),
                               nComport,
                                       pParms->mComms.nBaudrate,
                                                  pParms->mComms.nParity,
                                                                pParms->mComms.nStopBits,
                                                                             pParms->mComms.nDataBits,
                                                                                        pParms->mComms.fHandshaking,
                                                                                                 pParms->mComms.nPostOpenWaitMs,
                                                                                                              pParms->mComms.nPostCloseWaitMs);

    if( IsOpen() )
    {
        //TracePrint("Port already open, closing ....");
        Close();
        TracePrint("Port Closed");
    }

    try
    {
        //Create and initialise the apd comport object
        if(mApdComPort==NULL)mApdComPort = new TApdComPort(NULL);
        if(mApdComPort)
        {
            mnPostOpenWaitMs  =  pParms->mComms.nPostOpenWaitMs;
            mnPostCloseWaitMs =  pParms->mComms.nPostCloseWaitMs;

#if defined(CHECKPORT)
            mApdComPort->Open = false;
            if(mPostCloseWaitMs)
            {
                Sleep(mnPostCloseWaitMs);
            }
            if( CheckForPort(mApdComPort, nComport) )
#endif
            {
                try
                {
                    //TracePrint("%s::Open #001",GetHostName());

                    //TracePrint("Closing COM%d",nComport);
                    mApdComPort->Open                    = false;
                    if(mnPostCloseWaitMs)
                    {
                        Sleep(mnPostCloseWaitMs);
                    }

                    mpRegression->UpdateModemDisplay(GetLogicalPort(),false,false,false,false,false);

                    mApdComPort->AutoOpen                = false;
                    mApdComPort->ComNumber               = nComport;
                    mApdComPort->Baud                    = pParms->mComms.nBaudrate;
                    mApdComPort->Parity                  = GetParityEnum(pParms->mComms.nParity);
                    mApdComPort->DataBits                = pParms->mComms.nDataBits;
                    mApdComPort->StopBits                = pParms->mComms.nStopBits;

                    mApdComPort->OnTriggerAvail          = ApdComPortTriggerAvail;
                    //mApdComPort->OnTriggerOutbuffFree    = ApdComPortTriggerOutbuffFree;
                    if(pParms->mComms.fModemTriggering)
                    {
                        mApdComPort->OnTriggerModemStatus    = ApdComPortTriggerModemStatus;
                    }

                    mApdComPort->OutSize    = __TXBUF_SIZE__;
                    mApdComPort->InSize     = __RXBUF_SIZE__;

                    //TracePrint("%s::Open #002",GetHostName());

                    //Enable rts/cts hardware handshaking
                    mApdComPort->HWFlowOptions.Clear();
                    if(pParms->mComms.fHandshaking)
                    {
                        mApdComPort->HWFlowOptions << hwfUseRTS << hwfRequireCTS;
                    }
                    mApdComPort->BufferFull = mApdComPort->InSize * 0.9;
                    mApdComPort->BufferResume = mApdComPort->InSize * 0.1;

                    //TracePrint("%s::Open #003",GetHostName());
                    //finally open it
                    //TracePrint("Opening COM%d",nComport);
                    mApdComPort->Open                    = true;
                    if(mnPostOpenWaitMs)
                    {
                        Sleep(mnPostOpenWaitMs);
                    }
                    mInstanceCount++;

                    mpRegression->UpdateModemDisplay(GetLogicalPort(),mApdComPort->Open,mApdComPort->CTS,mApdComPort->DSR,mApdComPort->RI,mApdComPort->DCD);

                    Word TrigMS = mApdComPort->AddStatusTrigger(stModem);
                    mApdComPort->SetStatusTrigger(TrigMS, msRingDelta | msDSRDelta | msCTSDelta | msDCDDelta, true);

                    //TracePrint("%s::Open #004",GetHostName());
                    //Setup event trigger
                    mTxTriggerLvl = mApdComPort->OutSize/2;
                    mTrigOBF = mApdComPort->AddStatusTrigger(stOutBuffFree);
                    mApdComPort->SetStatusTrigger(mTrigOBF, mTxTriggerLvl, true);

                    //TracePrint("%s::Open #005",GetHostName());
                    OnOpen();

                    fRetVal = true;

                }
                catch (EBadId*)
                {
                    delete mApdComPort;
                    mApdComPort = NULL;
                    fRetVal = false;
                    TracePrint("%s::Open exception = EBADID",GetHostName());
                }
                catch(...)
                {
                    delete mApdComPort;
                    mApdComPort = NULL;
                    fRetVal = false;
                    TracePrint("%s::Open exception = ...#1",GetHostName());
                }
            }
#if defined(CHECKPORT)
            else
            {
                delete mApdComPort;
                mApdComPort = NULL;
                fRetVal = false;
            }
#endif
        }
    }
    catch(...)
    {
        mApdComPort = NULL;
        fRetVal = false;
        TracePrint("%s::Open exception = ...#2",GetHostName());
    }

    return fRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TBaseUartHost::ChangeCommsParameters(UBaseOpenParms *pCommParms)
{
    bool fRetVal = false; //assume cannot change

    TracePrint("ChangeComms [%s]: Baud=%d,Par=%d,Stop=%d,Data=%d,flow=%d,OpenWait=%d,CloseWait=%d",
                             GetHostName(),
                                       pCommParms->mComms.nBaudrate,
                                               pCommParms->mComms.nParity,
                                                      pCommParms->mComms.nStopBits,
                                                               pCommParms->mComms.nDataBits,
                                                                       pCommParms->mComms.fHandshaking,
                                                                           pCommParms->mComms.nPostOpenWaitMs,
                                                                                     pCommParms->mComms.nPostCloseWaitMs);

    try
    {

        mnPostOpenWaitMs  =  pCommParms->mComms.nPostOpenWaitMs;
        mnPostCloseWaitMs =  pCommParms->mComms.nPostCloseWaitMs;

        mApdComPort->Open       = false;

        if( mnPostCloseWaitMs==0 )
        {
            //Default was specified, if developer really needs 0, then set to 1 in script.
            //this way the behaviour is as per how it was before this change
            Sleep(1000);
        }
        else
        {
            Sleep(mnPostCloseWaitMs);
        }

        mApdComPort->Baud       = pCommParms->mComms.nBaudrate;
        mApdComPort->Parity     = GetParityEnum(pCommParms->mComms.nParity);
        mApdComPort->DataBits   = pCommParms->mComms.nDataBits;
        mApdComPort->StopBits   = pCommParms->mComms.nStopBits;
        mApdComPort->OutSize    = __TXBUF_SIZE__;
        mApdComPort->Open       = true;
        if(mnPostOpenWaitMs)
        {
            Sleep(mnPostOpenWaitMs);
        }

        //Enable rts/cts hardware handshaking
        mApdComPort->HWFlowOptions.Clear();
        if(pCommParms->mComms.fHandshaking)
        {
            mApdComPort->HWFlowOptions << hwfUseRTS << hwfRequireCTS;
        }

        Sleep(1000);
        FlushRxBuffer();
        FlushTxBuffer();

        fRetVal = true;
    }
    catch(...)
    {
        fRetVal = false;
        TracePrint("%s::ChangeCommsParameters() exception = ...#2",GetHostName());
    }

    return fRetVal;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TBaseUartHost::ApdComPortTriggerModemStatus(TObject *Sender)
{
    bool  fCTS=mApdComPort->CTS;
    bool  fDSR=mApdComPort->DSR;
    bool  fDCD=mApdComPort->DCD;
    bool  fRI=mApdComPort->RI;

    if( fCTS != mfLastCTS )mToggleCountCTS++;
    if( fDSR != mfLastDSR )mToggleCountDSR++;
    if( fDCD != mfLastDCD )mToggleCountDCD++;
    if( fRI  != mfLastRI  )mToggleCountRI++;

    //mpRegression->UpdateModemDisplay(GetLogicalPort(),mApdComPort->Open,fCTS,fDSR,fRI,fDCD);
    // reactivate trigger
    Word TrigMS = mApdComPort->AddStatusTrigger(stModem);
    mApdComPort->SetStatusTrigger(TrigMS, msRingDelta | msDSRDelta | msCTSDelta | msDCDDelta, true);

    mfLastCTS = fCTS;
    mfLastDSR = fDSR;
    mfLastDCD = fDCD;
    mfLastRI  = fRI;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TBaseUartHost::ApdComPortTriggerAvail(TObject *CP, WORD Count)
{
    unsigned thisread;

    if( mApdComPort )
    {
        if(mfCalled==false)
        {
            mfCalled = true;
            /* Count = mApdComPort->InBuffUsed; */
            while(Count)
            {
                thisread = (Count > (WORD)mCacheRawLen) ? mCacheRawLen : Count;
                mApdComPort->GetBlock(mbaCacheRaw, thisread);
                mbaCacheRaw[ thisread ] = 0; //append null just in case - we have space
                OnRxData(mbaCacheRaw,thisread); //All Data HAS to be absorbed
                Count -= thisread;

                /* Count = mApdComPort->InBuffUsed; */
            }
            mfCalled = false;
        }
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TBaseUartHost::ApdComPortTriggerOutbuffFree(TObject *Sender)
{
#if 0
    if(mApdComPort && !mfIsSuspended)
    {
        mApdComPort->SetStatusTrigger(mTrigOBF, mTxTriggerLvl, true);
        ::PostMessage(mMsgQueHandle,WM_SERVICETXPUMP,0,(LPARAM)this);
    }
#endif
}

//=============================================================================
//=============================================================================
unsigned short TBaseUartHost::__TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen)
{
    if( IsOpen() )
    {
        return mApdComPort->PutBlock(pSendBlock, nBlockLen);
    }
    return 0;
}


//=============================================================================
//=============================================================================
void TBaseUartHost::SetBreak(void)
{
    if(mApdComPort)
    {
        HANDLE hFile = (HANDLE)mApdComPort->Dispatcher->ComHandle;
        SetCommBreak(hFile);
    }
}

//=============================================================================
//=============================================================================
void TBaseUartHost::ClearBreak(void)
{
    if(mApdComPort)
    {
        HANDLE hFile = (HANDLE)mApdComPort->Dispatcher->ComHandle;
        ClearCommBreak(hFile);
    }
}

//=============================================================================
//=============================================================================
void TBaseUartHost::DeAssertDTR(void)
{
    if(mApdComPort)
    {
        mApdComPort->DTR = false;
    }
}

//=============================================================================
//=============================================================================
void TBaseUartHost::AssertDTR(void)
{
    if(mApdComPort)
    {
        mApdComPort->DTR = true;
    }
}

//=============================================================================
//=============================================================================
bool TBaseUartHost::GetCTS(void)
{
    bool fRetVal = false;
    if(mApdComPort)
    {
        if(mApdComPort->Open)
        {
            fRetVal = mApdComPort->CTS;
        }
    }
    return fRetVal;
}
//=============================================================================
//=============================================================================
bool TBaseUartHost::GetDSR(void)
{
    bool fRetVal = false;
    if(mApdComPort)
    {
        if(mApdComPort->Open)
        {
            fRetVal = mApdComPort->DSR;
        }
    }
    return fRetVal;
}
//=============================================================================
//=============================================================================
bool TBaseUartHost::GetRI(void)
{
    bool fRetVal = false;
    if(mApdComPort)
    {
        if(mApdComPort->Open)
        {
            fRetVal = mApdComPort->RI;
        }
    }
    return fRetVal;
}
//=============================================================================
//=============================================================================
bool TBaseUartHost::GetDCD(void)
{
    bool fRetVal = false;
    if(mApdComPort)
    {
        if(mApdComPort->Open)
        {
            fRetVal = mApdComPort->DCD;
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
void TBaseUartHost::SetDTR(bool newState)
{
    if(mApdComPort)
    {
        if(mApdComPort->Open)
        {
            mApdComPort->DTR = newState;
        }
    }
}
//=============================================================================
//=============================================================================
void TBaseUartHost::SetRTS(bool newState)
{
    if(mApdComPort)
    {
        if(mApdComPort->Open)
        {
            mApdComPort->RTS = newState;
        }
    }
}

//=============================================================================
//=============================================================================
void TBaseUartHost::FlushRxBuffer(void)
{
    if(mApdComPort)
    {
        OnFlushRxBuffer(false);
        mApdComPort->FlushInBuffer();
        OnFlushRxBuffer(true);
    }
}

//=============================================================================
//=============================================================================
void TBaseUartHost::FlushTxBuffer(void)
{
    if(mApdComPort)
    {
        OnFlushTxBuffer(false);
        mApdComPort->FlushOutBuffer();
        OnFlushTxBuffer(true);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * TBaseUartHost::GetDataInResponse(unsigned int nChannel)
{
    return GetFullResponse();
}

/******************************************************************************/
// END OF FILE
/******************************************************************************/


