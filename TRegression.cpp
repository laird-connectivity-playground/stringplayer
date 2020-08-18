/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Upgrade Class
**
** Module:      TREGRESSION.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <Filectrl.hpp>

#include "TRegression.h"
#include "TScriptSource.h"
#include "TMainForm.h"
#include "TfrmMessage.h"
#include "TfrmAggLogger.h"
#include "BaseHost.h"
#include "PipeConsoleHost.h"
/******************************************************************************/
// Local Defines
/******************************************************************************/

#define MAX_COMPILED_SCRIPT_LINES           (0x8000)
#define MAX_COMPILED_SCRIPT_VARIABLES       (1024)

#define MAX_POST_OPEN_WAIT_MS               (5000)
#define MAX_POST_CLOSE_WAIT_MS              (5000)

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

extern int gnReturnCode;

/******************************************************************************/
// Global/Static Variable Declarations
/******************************************************************************/
int gnMaximumLogicalPorts=MAX_LOGICAL_PORTS;


/******************************************************************************/
// External Function Declarations
/******************************************************************************/
TBaseHost *CreateAtMyHost(TRegression *pRegression, unsigned int nLogicalPort);
TBaseHost *CreateMPointMyHost(TRegression *pRegression, unsigned int nLogicalPort);
TBaseHost *CreateEmptyHost(TRegression *pRegression, unsigned int nLogicalPort);
TBaseHost *CreateOcHost(TRegression *pRegression, unsigned int nLogicalPort);

/******************************************************************************/
// Local Forward Function Declarations
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
// Local Functions or Private Members
/******************************************************************************/
/******************************************************************************/


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
char *TRegression::GetEventNameString(WIZMNGR_EVENT evt)
{
    static char baName[32];
#define EVENTCASE(_x_)  case _x_: return # _x_ ;

    baName[0]=0;
    switch(evt)
    {
    EVENTCASE(WIZMNGR_INIT);
    EVENTCASE(WIZMNGR_NEXTOPCODE);
    EVENTCASE(WIZMNGR_RESPONSE_OK);
    EVENTCASE(WIZMNGR_RESPONSE_ERROR);
    EVENTCASE(WIZMNGR_RESPONSE_CRLF);
    EVENTCASE(WIZMNGR_RESPONSE_CONNECT);
    EVENTCASE(WIZMNGR_RESPONSE_NOCARRIER);
    EVENTCASE(WIZMNGR_RESPONSE_PATTERN);
    EVENTCASE(WIZMNGR_RESPONSE_PARTIAL);
    EVENTCASE(WIZMNGR_RESPONSE_PACKET);
    EVENTCASE(WIZMNGR_RESPONSE_TIEVENT);
    default:
        sprintf(baName,"UNKNOWN %d,(0x%x)",evt,evt);
        break;
    }
#undef EVENTCASE
    return baName;
}

//=============================================================================
//=============================================================================
void TRegression::ResetTickCountImage(void)
{
    mTickCount = GetTickCount();
}

//=============================================================================
//=============================================================================
unsigned int TRegression::ElapsedTickCount(bool fReset)
{
    unsigned int nNowTick = GetTickCount();
    unsigned int nElapsed = nNowTick - mTickCount;
    if(fReset)mTickCount = nNowTick;
    return nElapsed;
}

//=============================================================================
//=============================================================================
void TRegression::DisplayTicks(bool fReset)
{
    unsigned int nElapsed = ElapsedTickCount(fReset);
    AnsiString asMsg;
    asMsg.printf("Time Elapsed = %d ms",nElapsed);
    frmMain->DisplayUserMessage(asMsg.c_str());
}

//=============================================================================
//=============================================================================
bool TRegression::AnsiStringToInt(const AnsiString asInt, int *pRetVal)
{
    bool fRetVal = true;
    try
    {
        *pRetVal = StrToInt(asInt);
    }
    catch (...)
    {
        *pRetVal = 0;
        fRetVal = false;
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
bool TRegression::InterpretCmdOption(char *pOption)
{
    bool fRetVal = true;

    int nEqualPos=0;
    int nColonPos=0;
    AnsiString asTemp = pOption;

    nEqualPos = asTemp.AnsiPos("=");
    if( nEqualPos == 0 )
    {
        nColonPos = asTemp.AnsiPos(":");
    }
    if( (nEqualPos == 0) && (nColonPos == 0) )
    {   //No = sign in option
        asTemp = asTemp.UpperCase();

        if(asTemp == "TRACE")
        {
            mfTrace = true;
        }
        else if(asTemp == "AGGLOG")
        {
            frmAggLogger->Show();
            frmAggLogger->mfIsVisible = true;
            frmAggLogger->mfUpdateWindow = true;
            mfLoggerUpdate = true;
        }
        else if(asTemp == "AGGLOGTIMESTAMP")
        {
            frmAggLogger->mfTimestamp = true;
        }
        else if(asTemp == "TOPLEFT")
        {
            frmMain->Top=50;
            frmMain->Left=50;
            frmMain->Position=poDesigned;
        }
        else if(asTemp == "QUIT")
        {
            mfQuit = true;
        }
        else if(asTemp == "SKIP")
        {
            //Allow skipping tests
            mfAllowSkip = true;
        }
        else if(asTemp == "SMALLLOG")
        {
            //Clear log window when > SMALLLOG_MAIN_MEMO_LINES lines
            mMaxMemoLines = SMALLLOG_MAIN_MEMO_LINES;
        }
        else
        {
            fRetVal = false;
        }
    }
    else if(nColonPos != 0 )
    {   //No = colon in option
        AnsiString asValue;
        AnsiString asName;

        asName  = asTemp.SubString(1, nColonPos-1);
        asValue = asTemp.SubString(nColonPos+1, asTemp.Length()-1);

        if(mArgVarCount<MAX_COMMANDLINE_VARS)
        {
            mArgVarName[mArgVarCount]  =  asName.UpperCase();
            mArgVarValue[mArgVarCount] =  asValue;
            mArgVarCount++;
        }
        else
        {
            fRetVal = false;
        }
    }
    else if(nEqualPos != 0 )  // there is equal in the arg
    {
        AnsiString asValue;
        AnsiString asTag;

        asTag   = asTemp.SubString(1, nEqualPos-1);
        asValue = asTemp.SubString(nEqualPos+1, asTemp.Length()-1);

        asTag = asTag.UpperCase();


        if( asTag == "SCRIPT" ) /* script=somename */
        {
            SetScriptFileName(asValue);
        }
        else if( asTag == "CWD" ) /* cwd=somefoldername */
        {
            SetCurrentWorkingFolder(asValue);
        }
        else if( asTag == "LOGPATH" ) /* logpath=somefoldername */
        {
            SetLogfileFolder(asValue);
        }
        else if( asTag == "MAXTRY" ) /* maxtry=decimal_number */
        {
            mMaxRetryPerTitleTest = asValue.ToInt();
            if(mMaxRetryPerTitleTest<1)
            {
                mMaxRetryPerTitleTest=1;
            }
            else if( mMaxRetryPerTitleTest > 100 )
            {
                mMaxRetryPerTitleTest=100;
            }
        }
        else if( asTag == "MAXPORTS" ) /* maxports=decimal_number */
        {
            int nMaxPorts = asValue.ToInt();
            if(nMaxPorts<MAX_LOGICAL_PORTS)
            {
                nMaxPorts=MAX_LOGICAL_PORTS;
            }
            else if( nMaxPorts > MAX_LOGICAL_PORTS_LIMIT )
            {
                nMaxPorts=MAX_LOGICAL_PORTS_LIMIT;
            }
            gnMaximumLogicalPorts=nMaxPorts;
        }
        else if( asTag == "MAXLINES" ) /* maxlines=decimal_number */
        {
            int nMaxLines = asValue.ToInt();
            if(nMaxLines<MIN_MAIN_MEMO_LINES)
            {
                nMaxLines=MIN_MAIN_MEMO_LINES;
            }
            else if( nMaxLines > MAX_MAIN_MEMO_LINES )
            {
                nMaxLines=MAX_MAIN_MEMO_LINES;
            }
            mMaxMemoLines=nMaxLines;
        }
        else if( asTag == "ARG" ) /* arg=something */
        {
            if(mArgc<MAX_COMMANDLINE_ARGS)
            {
                mArgv[mArgc] =  asValue;
                mArgc++;
            }
            else
            {
                fRetVal = false;
            }
        }
        else
        {
            fRetVal = false;
        }
    }
    else
    {
        fRetVal = false;
    }

    return fRetVal;
}


//=============================================================================
//=============================================================================
bool TRegression::IsValidAti3Version(char *pVer)
{
    //The string can contain decimals only AND exactly 2 '.'s
    bool fRetVal = true; //assume it is
    int nDotCount=0;
    int nStrLen = strlen(pVer);
    for(int i=0;i<nStrLen;i++)
    {
        if( ! isdigit(*pVer) )
        {   //Not a digit
            if( *pVer == '.' )
            {
                if( *(pVer+1) == '.' ){fRetVal=false;break;}
                nDotCount++;
                if(nDotCount>2){fRetVal=false;break;}
                pVer++;
            }
            else
            {   //Not a '.'
                fRetVal=false;
                break;
            }
        }
        else
        {   //A digit
            pVer++;
        }
    }

    //check that we had exactly 2 '.'s
    if(fRetVal && (nDotCount!=2))fRetVal = false;

    return fRetVal;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TRegression::AppMessageHandler(tagMSG &Msg, bool &Handled)
{
    if(mOldAppOnMessageHandler)
    {
        mOldAppOnMessageHandler(Msg,Handled);
    }
    if(! Handled )
    {
        Handled = ServiceTestStateMachine(Msg);
    }
    if(! Handled )
    {
        if(Msg.message==WM_PIPECONSOLE_EVT)
        {
            int nLogicalPort = Msg.lParam;
            if(nLogicalPort < gnMaximumLogicalPorts)
            {
                PipeConsoleHost *pBaseHost = (PipeConsoleHost *)mpModule[nLogicalPort].mpHost;
                if(pBaseHost)
                {
                    pBaseHost->OnWindowsMessage(Msg.wParam);
                }
            }
            Handled = true;
        }
    }

    /* for all other messages, Handled remains False so that other message handlers can respond */
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void __fastcall TRegression::tmrOnModuleTimer(TObject *Sender)
{
    TTimer *pTimer = (TTimer *)Sender;

    pTimer->Enabled=false;
    OnModuleTimerExpire(pTimer->Tag);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void __fastcall TRegression::tmrScriptTimer(TObject *Sender)
{
    mScriptDelayTimer->Enabled = false;

    if( mpScrManager->mCurrentAction.generic.eOutcome == OUTCOME_SCR_TIMEOUT )
    {
        TriggerNextAction(__LINE__);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::StartModuleTimer(TModule *pModule, int nTimeoutMs, bool fApplyScalar)
{
    pModule->mTimer->Enabled = false;
    if( nTimeoutMs > 0)
    {
        if(  fApplyScalar )
        {
            pModule->mTimer->Interval = ScaleTimeout(nTimeoutMs);
        }
        else
        {
            pModule->mTimer->Interval = nTimeoutMs;
        }
        pModule->mTimer->Enabled = true;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::StopModuleTimer(TModule *pModule)
{
    pModule->mTimer->Enabled = false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::StopModuleTimer(unsigned int nModule)
{
    if( nModule < (unsigned int)gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nModule];
        pModule->mTimer->Enabled = false;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::StopAllModuleTimers(void)
{
    TModule *pModule = mpModule;
    for(int nModuleId=0;nModuleId<gnMaximumLogicalPorts;nModuleId++)
    {
        StopModuleTimer(pModule);

        pModule++;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::OnScriptStart(void)
{
    AnsiString asMsg;

    frmMain->DisplayUserMessage("################################### ");
    frmMain->DisplayUserMessage("Start of Script");
    asMsg = MiscGetTimeDateString();
    frmMain->DisplayUserMessage(asMsg.c_str());
    frmMain->DisplayUserMessage("################################### ");

    frmMain->DisplayScriptFilename(GetScriptFileName());
    frmMain->StatusBarScriptSize(mpScrManager->mnActionCount);
    frmMain->StatusBarFailCount(0,0,mMaxRetryPerTitleTest);
    frmMain->StatusBarProgramCounter(0);
    mpScrManager->mpPC = (TScriptAction *)mpScrManager->mpCode;
    mpScrManager->mpCurrentTitle = mpScrManager->mpPC;
    mpScrManager->mnIncFilenameStackTopAtTitle = mnIncFilenameStackTop;

    mStartTickCount = GetTickCount();
    TriggerNextAction(__LINE__);

    frmMain->btnPause->Enabled = true;

    if (mfAllowSkip == true)
    {
        frmMain->btnSkip->Enabled = true;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::OnScriptEnd(void)
{
    AnsiString asMsg;
    //TModule *pModule = mpModule;
    StopAllModuleTimers();

    //if tracing has been disabled, then re-enable it
    if( mfConsoleUpdate==false)
    {
        frmMain->DisplayUserMessage("Console Window Update:: ON");
        frmMain->ConsoleUpdateEnable();
    }
    if( mfLoggerUpdate==false)
    {
        frmAggLogger->mfUpdateWindow = true;
        frmMain->DisplayUserMessage("Aggregate Trace Window :: ON");
    }

    frmMain->DisplayUserMessage("########## End of Script ########## ");
    if(mpScrManager->mTotalFailCount>0)
    {
        asMsg.printf("FAIL: %d tests failed",mpScrManager->mTotalFailCount);
        frmMain->DisplayUserMessage(asMsg.c_str());
    }
    else
    {
        frmMain->DisplayUserMessage("PASS");
    }
    gnReturnCode =  mpScrManager->mTotalFailCount;
    asMsg = MiscGetTimeDateString();
    frmMain->DisplayUserMessage(asMsg.c_str());

    asMsg = "Duration: ";
    asMsg += MiscGetElapsedTimeString(mStartTickCount);
    frmMain->DisplayUserMessage(asMsg.c_str());

    frmMain->DisplayUserMessage("########## End of Script ########## ");

    CloseAllModules();
    CloseAllFileHandles();

    if( mfQuit )
    {
        Application->Terminate();
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::OnScriptExit(void)
{
    AnsiString asMsg;
    //TModule *pModule = mpModule;
    StopAllModuleTimers();

    frmMain->DisplayUserMessage("########## Exit Script ########## ");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char *ParseCopyDigits(char *pSrc, char *pDst, int nMaxLen)
{
    int nCount=0;
    while( isdigit(*pSrc) )
    {
        *pDst++ = *pSrc++;
        nCount++;
        if(nCount>=nMaxLen)
        {
            *pDst++ = 0;
            return pSrc;
        }
    }
    *pDst = 0;
    return pSrc;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TRegression::ParseCommsString(char *pCommParm, TBaseCommParms *pCommParms)
{
#define TEMPBUF_SIZE     64
    char baBuf[TEMPBUF_SIZE+1];
    char *pComma;
    int  nLen;
    char *pSrc = pCommParm;
    int nBaudrate,nParity, nStopBits, nDataBits, nTemp;

    //initialise option values
    bool fHandshaking=true;
    bool fModemTriggering=true;
    int nPostOpenWaitMs=0;
    int nPostCloseWaitMs=0;

    //Initialise the output object with defaults
    if( pCommParms )
    {
        pCommParms->nBaudrate = 9600;
        pCommParms->nParity   = 0;
        pCommParms->nStopBits = 1;
        pCommParms->nDataBits = 8;
        pCommParms->fHandshaking     = fHandshaking;
        pCommParms->fModemTriggering = fModemTriggering;
        pCommParms->nPostOpenWaitMs  = nPostOpenWaitMs;
        pCommParms->nPostCloseWaitMs = nPostCloseWaitMs;
    }
    
    if(pCommParm && pCommParms)
    {
        //---------------------------------------------------------
        //Baudrate
        //---------------------------------------------------------
        pSrc = ParseCopyDigits(pSrc,baBuf,TEMPBUF_SIZE);
        nBaudrate = atoi(baBuf);
        if( (nBaudrate<1200)||(nBaudrate>(921600*16)))
        {
            return false;
        }

        //---------------------------------------------------------
        //Parity
        //---------------------------------------------------------
        //mandatory ','
        if(*pSrc++ != ',')
        {
            return false;
        }
        //Parity
        switch(*pSrc++)
        {
        case 'N': case 'n':
            nParity=0;
            break;
        case 'O': case 'o':
            nParity=1;
            break;
        case 'E': case 'e':
            nParity=2;
            break;
        default:
            return false;
        }

        //---------------------------------------------------------
        //databits
        //---------------------------------------------------------
        //mandatory ','
        if(*pSrc++ != ',')
        {
            return false;
        }
        //databits
        pSrc = ParseCopyDigits(pSrc,baBuf,TEMPBUF_SIZE);
        nDataBits = atoi(baBuf);
        if( (nDataBits<7)||(nDataBits>8))
        {
            return false;
        }

        //---------------------------------------------------------
        //stopbits
        //---------------------------------------------------------
        //mandatory ','
        if(*pSrc++ != ',')
        {
            return false;
        }
        //stopbits
        pSrc = ParseCopyDigits(pSrc,baBuf,TEMPBUF_SIZE);
        nStopBits = atoi(baBuf);
        if( (nStopBits<1)||(nStopBits>2))
        {
            return false;
        }

        //---------------------------------------------------------
        //handshaking
        //---------------------------------------------------------
        //optional',0 or ,1' for flow control
        if(*pSrc++ == ',')
        {
            //handshaking
            if(*pSrc == ',')
            {
                /* use default value */
                pSrc++;
            }
            else
            {
                pSrc = ParseCopyDigits(pSrc,baBuf,TEMPBUF_SIZE);
                nTemp = atoi(baBuf);
                if( (nTemp<0)||(nTemp>1))
                {
                    return false;
                }
                fHandshaking = nTemp ? true : false;
            }
        }

        //---------------------------------------------------------
        //Modem Triggering
        //---------------------------------------------------------
        //optional',0 or ,1' for modem triggering
        if(*pSrc++ == ',')
        {
            //Modem Triggering
            if(*pSrc == ',')
            {
                /* use default value */
                pSrc++;
            }
            else
            {
                pSrc = ParseCopyDigits(pSrc,baBuf,TEMPBUF_SIZE);
                nTemp = atoi(baBuf);
                if( (nTemp<0)||(nTemp>1))
                {
                    return false;
                }
                fModemTriggering = nTemp ? true : false;
            }
        }

        //---------------------------------------------------------
        //Post Open Wait MS
        //---------------------------------------------------------
        //optional
        if(*pSrc++ == ',')
        {
            //Modem Triggering
            if(*pSrc == ',')
            {
                /* use default value */
                pSrc++;
            }
            else
            {
                pSrc = ParseCopyDigits(pSrc,baBuf,TEMPBUF_SIZE);
                nTemp = atoi(baBuf);
                if( (nTemp<0)||(nTemp>MAX_POST_OPEN_WAIT_MS))
                {
                    return false;
                }
                nPostOpenWaitMs = nTemp;
            }
        }

        //---------------------------------------------------------
        //Post Close Wait MS
        //---------------------------------------------------------
        //optional
        if(*pSrc++ == ',')
        {
            //Modem Triggering
            if(*pSrc == ',')
            {
                /* use default value */
                pSrc++;
            }
            else
            {
                pSrc = ParseCopyDigits(pSrc,baBuf,TEMPBUF_SIZE);
                nTemp = atoi(baBuf);
                if( (nTemp<0)||(nTemp>MAX_POST_CLOSE_WAIT_MS))
                {
                    return false;
                }
                nPostCloseWaitMs = nTemp;
            }
        }

        //reaching here means all OK
        pCommParms->nBaudrate = nBaudrate;
        pCommParms->nParity   = nParity;
        pCommParms->nStopBits = nStopBits;
        pCommParms->nDataBits = nDataBits;
        pCommParms->fHandshaking = fHandshaking;
        pCommParms->fModemTriggering = fModemTriggering;
        pCommParms->nPostOpenWaitMs  = nPostOpenWaitMs;
        pCommParms->nPostCloseWaitMs = nPostCloseWaitMs;

    }
    else return false;

    return true;
#undef TEMPBUF_SIZE
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * TRegression::SetRxPattern(TModule *pModule,char *pPattern,unsigned short nPatternLen, int nSearchType)
{
    if(pModule->mpHost)
    {
        if(pModule->mpHost->IsOpen())
        {
            return pModule->mpHost->SetRxFilterPattern(pPattern,nPatternLen,nSearchType);
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * TRegression::SetRxPattern(TModule *pModule,char *pPattern)
{
    unsigned short nPatternLen = strlen(pPattern);
    return SetRxPattern(pModule,pPattern,nPatternLen,1);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * TRegression::SetPartialRxPattern(TModule *pModule,char *pPattern)
{
    unsigned short nPatternLen = strlen(pPattern);
    return SetRxPattern(pModule,pPattern,nPatternLen,0);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::StartDelay(int nTimeoutMs, bool fApplyScalar)
{
    if( fApplyScalar )
    {
        nTimeoutMs = ScaleTimeout(nTimeoutMs);
    }
    mScriptDelayTimer->Enabled  = false;
    mScriptDelayTimer->Interval = nTimeoutMs;
    mScriptDelayTimer->Enabled  = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TRegression::SaveResp(unsigned int nModule, TScriptVariable *pVariable)
{
    bool fRetVal = false;

    if( pVariable->pValue )
    {
        MiscMemFree(pVariable->pValue);
        pVariable->pValue=NULL;
    }

    if( nModule < (unsigned int)gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nModule];
        if(pModule->mpHost)
        {
            if(pModule->mpHost->IsOpen())
            {
                char baEmpty[2];
                char *pFullResp = pModule->mpHost->GetFullResponse();
                if(pFullResp==NULL)
                {
                    baEmpty[0]=0;
                    pFullResp = baEmpty;
                }
                fRetVal = SrcSaveVariableValue(pVariable, pFullResp);
            }
        }
    }
    return fRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TRegression::SaveBlockToVariable(char *pSrc, int nOffset, int nLen, TScriptVariable *pVariable)
{
    char cCurVal;
    bool fRetVal;

    if( pVariable == NULL )
    {
        return FALSE;
    }

    if( pVariable->pValue )
    {
        MiscMemFree(pVariable->pValue);
        pVariable->pValue=NULL;
    }
    if( nOffset >= nLen )
    {
        char baEmpty[2];
        baEmpty[0]=0;
        fRetVal = SrcSaveVariableValue(pVariable, baEmpty);
    }
    else
    {
        int nIndex=nLen+nOffset;
        cCurVal=pSrc[nIndex];
        pSrc[nIndex]=0;
        fRetVal = SrcSaveVariableValue(pVariable, &pSrc[nOffset]);
        pSrc[nIndex]=cCurVal;
    }

    return fRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TRegression::GetPortInfo(unsigned int nModule, TScriptVariable *pVariable, int nArg)
{
    int nPortInfo=0;

    if( pVariable->pValue )
    {
        MiscMemFree(pVariable->pValue);
        pVariable->pValue=NULL;
    }

    if( nModule < (unsigned int)gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nModule];
        if(pModule->mpHost)
        {
            if(pModule->mpHost->IsOpen())
            {
                nPortInfo = pModule->mpHost->PortIoctl(nArg,0,NULL,NULL);
            }
        }
    }
    return SrcSaveVariableValueInteger(pVariable, nPortInfo);
}

//------------------------------------------------------------------------------
// the packet will get dropped if the port is open
//------------------------------------------------------------------------------
bool TRegression::SavePacket(TBaseHost *pHost, TScriptVariable *pVariable,TPacketType pktType)
{
    bool fRetVal = false;

    if( pVariable->pValue )
    {
        MiscMemFree(pVariable->pValue);
        pVariable->pValue=NULL;
    }

    if( pHost )
    {
        if(pHost->IsOpen())
        {
            char *pPacket;
            int nPktLen;
            pPacket = pHost->GetPacket(&nPktLen,pktType);
            fRetVal = SrcSaveVariableValueEx(pVariable, pPacket, nPktLen );
            pHost->DropPacket(pktType);
        }
    }
    return fRetVal;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TranslateWhiteSpace( char *pResp )
{
    while(*pResp)
    {
        if( *pResp < ' ')
        {
            switch(*pResp)
            {
            case 0x0D:
                *pResp = '.';
                break;
            case 0x0A:
                *pResp = '.';
                break;
            default:
                *pResp = '.';
                break;
            }
        }
        pResp++;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::DisplayResponse(unsigned int nModule,char *pResp)
{
    //TranslateWhiteSpace(pResp);
    frmMain->DisplayResponse(nModule,pResp);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::DisplayChannelResponse(unsigned int nModule,int channel, char *pResp)
{
    //TranslateWhiteSpace(pResp);
    frmMain->DisplayChannelResponse(nModule,channel,pResp);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TRegression::IsValidBluetoothAddress(char *pBdAddr)
{
    bool fValid = false;
    int nLen = strlen(pBdAddr);

    if( nLen >= 12)
    {
        for(int i=0;i<=11;i++)
        {
            if( ! isxdigit(pBdAddr[i]) )
            {
                return false;
            }
        }
        if( !((pBdAddr[12]==' ') || (pBdAddr[12]==0) || (pBdAddr[12]=='.')) )
        {
            return false;
        }
        fValid = true;
    }
    return fValid;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TRegression::IsPortOpen(int nModuleId)
{
    bool fOpen = false;

    if( (nModuleId>=0) && (nModuleId<gnMaximumLogicalPorts) )
    {
        fOpen =  mpModule[nModuleId].mpHost->IsOpen();
    }

    return fOpen;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char *TRegression::GetFullResponse(int nModuleId)
{
    char *pRetVal = NULL;

    if( (nModuleId>=0) && (nModuleId<gnMaximumLogicalPorts) )
    {
        pRetVal =  mpModule[nModuleId].mpHost->GetFullResponse();
    }

    return pRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::TriggerNextAction(int src)
{
    /* Helps with tracing and debugging */
    memcpy(&mTriggerFifo[0],&mTriggerFifo[1], sizeof(mTriggerFifo[1])*16);
    mTriggerFifo[16]=mTriggerSrc;
    mTriggerSrc = src;
    /* end of debug tracing */

    if( mfPauseReq == false)
    {
        ThrowStateMachineEvent(WIZMNGR_NEXTOPCODE,0);
    }
    else
    {
        mfPaused = true;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RetreatToStartOfThisTest(void)
{
    /* adjust the program counter to the start of current test */
    mpScrManager->mpPC  = mpScrManager->mpCurrentTitle;

    /* adjust the include push/pop stack pointer to where it was at the last TITLE statement */
    mnIncFilenameStackTop =    mpScrManager->mnIncFilenameStackTopAtTitle;

    /* the following will advance so we will not process the title again */
    /* whcih we fon't want to do otherwise counts will be reset */
    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::AdvanceToNextTest(void)
{
    TScriptAction *pSrc  = mpScrManager->mpPC;  //points to next action to execute

    while( (pSrc->generic.eAction != ACTION_END) && (pSrc->generic.eAction != ACTION_EXIT) )
    {
        if( pSrc->generic.eAction == ACTION_TITLE )
        {
            mpScrManager->mpPC = pSrc;
            //mpScrManager->mpPC--; //because next action will advance it again
            TriggerNextAction(__LINE__);
            break;
        }
        else if( pSrc->generic.eAction == ACTION_INCLUDE_PUSH )
        {
            //Push the filename onto the include stack
            IncludePush(&pSrc->includePush) ;
        }
        else if( pSrc->generic.eAction == ACTION_INCLUDE_POP )
        {
            //Pop the include stack
            IncludePop( &pSrc->includePop );
        }
        pSrc++;
    }
    if(pSrc->generic.eAction == ACTION_END)
    {
        if( mfQuit )
        {
            Application->Terminate();
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool  TRegression::GetVariableValueAsInteger(char *pVarName, int *pIntVal, int nDefVal)
{
    bool fRetVal=false;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pVarName);
    if(pVariable)
    {
        if(pVariable->pValue)
        {
            *pIntVal = atoi(pVariable->pValue);
            fRetVal=true;
        }
    }

    if(!fRetVal)
    {
        *pIntVal = nDefVal;
    }

    return fRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void  TRegression::OnFail(char *pFailMsg, int nExitCode, bool fNextTest)
{
    //--------------------------------------------------------------------------------------
    //if tracing has been disabled, then re-enable it
    //--------------------------------------------------------------------------------------
    if( mfConsoleUpdate==false)
    {
        frmMain->DisplayUserMessage("Console Window Update:: ON");
        frmMain->ConsoleUpdateEnable();
    }
    if( mfLoggerUpdate==false)
    {
        frmAggLogger->mfUpdateWindow = true;
        frmMain->DisplayUserMessage("Aggregate Trace Window :: ON");
    }

    //--------------------------------------------------------------------------------------
    //process the fail
    //--------------------------------------------------------------------------------------
    bool fDefault = TRUE;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, "SysVarOnFailMessage");
    if(pVariable)
    {
        if(pVariable->pValue)
        {
            fDefault = strlen(pVariable->pValue) > 0 ? FALSE : TRUE;
        }
    }

    //--------------------------------------------------------------------------------------
    // Check if Max Retries on command line was specified to be greater than 1
    //--------------------------------------------------------------------------------------
    if ( ++mpScrManager->mTitleFailCount <  mMaxRetryPerTitleTest )
    {
        /* let us redo the test */
        AnsiString asMsg;

        asMsg =      "++++++++++++++++++++++ TITLE   : ";
        asMsg += mpCurrentTestTitle;
        frmMain->DisplayFailMessage(asMsg.c_str());

        asMsg.printf("++++++++++++++++++++++ ATTEMPT : %d of %d FAILED",mpScrManager->mTitleFailCount, mMaxRetryPerTitleTest);
        frmMain->DisplayFailMessage(asMsg.c_str());

        mTotalRetryCounts++;

        //--------------------------------------------------------------------------------------
        // Retreat to most recent TITLE statement
        //--------------------------------------------------------------------------------------
        RetreatToStartOfThisTest();

        //restore console update state before exiting
        if( mfConsoleUpdate==false)
        {
            frmMain->ConsoleUpdateDisable();
        }
        if( mfLoggerUpdate==false)
        {
            frmAggLogger->mfUpdateWindow = false;
            frmMain->DisplayUserMessage("Aggregate Trace Window :: OFF");
        }

        return ;
    }
    else
    {
        /* too many retries so go to the next test */
        mpScrManager->mTotalFailCount++;
        mpScrManager->mTitleFailCount = 0;
    }

    if(fDefault)
    {
        AnsiString asMsg;

        frmMain->StatusBarFailCount(mpScrManager->mTotalFailCount,mTotalRetryCounts,mMaxRetryPerTitleTest);
        asMsg = "*********** Script Fail ***********";
        frmMain->DisplayFailMessage(asMsg.c_str());

        asMsg.printf("FAIL#%d: ",mpScrManager->mTotalFailCount);
        if(pFailMsg)
        {
            asMsg += pFailMsg;
        }
        else
        {
            asMsg += "Fail Msg is NULL pointer";
        }
        frmMain->DisplayFailMessage(asMsg.c_str());

        if( mMaxRetryPerTitleTest > 1 )
        {
              asMsg.printf("(TRIED %d times)",mMaxRetryPerTitleTest);
              frmMain->DisplayFailMessage(asMsg.c_str());
        }

        if(mpCurrentTestTitle)
        {
            asMsg = "In Test: ";
            asMsg += mpCurrentTestTitle;
        }
        else
        {
            asMsg = "In Test: Main Test (No Title)";
        }
        frmMain->DisplayFailMessage(asMsg.c_str());

        if(mpCurrentTestTitle)
        {
            asMsg = mpCurrentTestTitle;
        }
        else
        {
            asMsg = "<<Main Test (No Title)>>";
        }
        frmMain->LogToPlainFailFile(asMsg);

        asMsg = MiscGetTimeDateString();
        frmMain->DisplayFailMessage(asMsg.c_str());

        asMsg = "Duration: ";
        asMsg += MiscGetElapsedTimeString(mStartTickCount);
        frmMain->DisplayFailMessage(asMsg.c_str());

        //Print the source code info
        int nTop     = mnIncFilenameStackTop;
        uint32 nLine = mnCurrentActionLineNum;
        if(nTop>=0)
        {
            asMsg.printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            frmMain->DisplayFailMessage(asMsg.c_str());
            asMsg.printf("StringPlayer Script File :=");
            frmMain->DisplayFailMessage(asMsg.c_str());
            asMsg.printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            frmMain->DisplayFailMessage(asMsg.c_str());
        }
        while(nTop>=0)
        {
            asMsg.printf("%s : Ln %d",mbaIncFilename[nTop], nLine);
            frmMain->DisplayFailMessage(asMsg.c_str(), false);
            nLine = mnaIncludeLineNum[nTop];
            nTop--;
        }

        asMsg = "*********** Script Fail ***********";
        frmMain->DisplayFailMessage(asMsg.c_str());

#define DUMP_LINES_MAIN                  40
#define DUMP_LINES_AGGREGATE             100

        /* Dump last X lines in main window to cummulative fail log file */
        if( frmMain->mfpFailLog )
        {
            frmMain->DumpToLogFile( frmMain->mfpFailLog, DUMP_LINES_MAIN );
        }

        /* dump comms activity to html file */
        if(frmAggLogger->mfIsVisible)
        {
            frmAggLogger->DumpToLogFile( frmMain->mfpLog,     DUMP_LINES_AGGREGATE );
            if( frmMain->mfpFailLog )
            {
                frmAggLogger->DumpToLogFile( frmMain->mfpFailLog, (DUMP_LINES_AGGREGATE*2) );
            }
        }
    }
    else
    {
        frmMain->DisplayFailMessage(pVariable->pValue);
    }

    if(fNextTest)
    {
        //--------------------------------------------------------------------------------------
        // Advance to the next TITLE statement in the script
        //--------------------------------------------------------------------------------------
        AdvanceToNextTest();
    }
    else
    {
        // stop test or exit app to OS
        if( mfQuit )
        {
            gnReturnCode = nExitCode;
            Application->Terminate();
        }
    }
    
    //--------------------------------------------------------------------------------------
    //restore console update state before exiting
    //--------------------------------------------------------------------------------------
    if( mfConsoleUpdate==false)
    {
        frmMain->ConsoleUpdateDisable();
    }
    if( mfLoggerUpdate==false)
    {
        frmAggLogger->mfUpdateWindow = false;
        frmMain->DisplayUserMessage("Aggregate Trace Window :: OFF");
    }

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ESCRERROR TRegression::AddSystemVariables(void)
{
    ESCRERROR nError;
    char baBuf[32];
    int i;

    nError = SrcAddSystemVariable(mpScrManager,"SysVarAppVersion",APP_VERSION);

    if(nError==SCRERROR_NONE)
    {
        sprintf(baBuf,"%d",gnMaximumLogicalPorts);
        nError = SrcAddSystemVariable(mpScrManager,"SysVarMaxPorts",baBuf);
    }

    if(nError==SCRERROR_NONE)
    {
        nError = SrcAddSystemVariable(mpScrManager,"SysVarMagicCmd",STR_MAGIC_CMD);
    }

    if(nError==SCRERROR_NONE)
    {
        nError = SrcAddSystemVariable(mpScrManager,"SysVarMatchCount","0");
    }

    if(nError==SCRERROR_NONE)
    {
        nError = SrcAddSystemVariable(mpScrManager,"SysVarMsgBoxBtnVal","0");
    }

    if(nError==SCRERROR_NONE)
    {
        nError = SrcAddSystemVariable(mpScrManager,"SysVarMsgBoxBtnName","ok");
    }

    if(nError==SCRERROR_NONE)
    {
        nError = SrcAddSystemVariable(mpScrManager,"SysVarFailOnVarMissing","1");
    }

    for(i=0;i<MAX_COMMANDLINE_ARGS;i++)
    {
        if(nError==SCRERROR_NONE)
        {
            sprintf(baBuf,"SysVarArg%d",i);
            nError = SrcAddSystemVariable(mpScrManager,baBuf,mArgv[i].c_str());
        }
    }

    for(i=0;i<mArgVarCount;i++)
    {
        if(nError==SCRERROR_NONE)
        {
            nError = SrcAddSystemVariable(mpScrManager,mArgVarName[i].c_str(),mArgVarValue[i].c_str());
        }
    }

    if(nError==SCRERROR_NONE)
    {
        nError = SrcAddSystemVariable(mpScrManager,"SysVarOnFailMessage","");
    }

/*
    if(nError==SCRERROR_NONE)
    {
        nError = SrcAddSystemVariable(mpScrManager,"","");
    }
*/
    return nError;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtInit(void)
{
    if(mpScrManager)
    {
        OnInit();

        SrcClearScript(mpScrManager);
        AddSystemVariables();
        AnsiString asFilename = GetFullScriptFileName();
        AnsiString asLogFolder = GetLogFileFolder();
        ESCRERROR nError = SrcCompileScriptFile(asFilename.c_str(), mpScrManager, asLogFolder);
        if( nError == SCRERROR_NONE)
        {
            OnScriptStart();
        }
        else if( nError == SCRERROR_FILE_EMPTY)
        {
            frmMain->DisplayUserMessage("Script File generated no actions");
            frmMain->DisplayUserMessage("Regression test cannot be performed");
        }
        else
        {
            // else reason was displayed by SrcCompileFile

            if( mfQuit )
            {
                gnReturnCode = -1;
                Application->Terminate();
            }
        }
    }
    else
    {
        frmMain->DisplayUserMessage("Script Manager not created");
        frmMain->DisplayUserMessage("Regression test cannot be performed");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionUnknown(TScriptAction *pScriptPC)
{
    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionInRange(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    int nTgtVal, nPerc, nTemp;
    long long nActTgtVal, nActVal;

    nPerc =  pScriptPC->inRange.nPerc ;
    if( (nPerc >= 0) && (nPerc <= 100) )
    {
        int nPercVal ;
        long long nTgtValLo, nTgtValHi ;

        nTemp    = atoi( pScriptPC->inRange.pValue );
        nActVal = nTemp;
        nTemp = atoi( pScriptPC->inRange.pTargetValue );
        nActTgtVal = nTemp;
        nTgtVal = abs(nTemp);

        //calculate half bound
        if(  nTgtVal > (0x7FFFFFFF/150) )
        {
            nPercVal = (nTgtVal/100) *  nPerc;
        }
        else
        {
            nPercVal = (nTgtVal *  nPerc) / 100 ;
        }

        //calculate lower value of the range
        nTgtValLo =  nActTgtVal - nPercVal;

        //calculate the upper value of the range
        nTgtValHi =  nActTgtVal + nPercVal;

        //now check it is in the bound
        if( (nActVal <  nTgtValLo) || (nActVal >  nTgtValHi) )
        {
            asMsg.printf("TargetVal: %d",nActTgtVal);
            frmMain->DisplayUserMessage(asMsg.c_str());
            asMsg.printf("ActualVal: %d",nActVal);
            frmMain->DisplayUserMessage(asMsg.c_str());

            asMsg.printf("Value is outside range");
            OnFail(asMsg.c_str());
        }
        else
        {
            TriggerNextAction(__LINE__);
        }
    }
    else
    {
        asMsg.printf("Percentage value %d is not 0..100",pScriptPC->inRange.nPerc);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::IncludePush(TActionIncludePush *pIncludePush)
{
    char *pDest;
    if( mnIncFilenameStackTop < MAX_FILE_NEST_DEPTH)
    {
        mnIncFilenameStackTop++;
        pDest = &mbaIncFilename[ mnIncFilenameStackTop][0];
        strncpy(pDest,  pIncludePush->pFilename, MAX_INCFILEPATHNAME_LEN);
        pDest[MAX_INCFILEPATHNAME_LEN]=0;

        // save the line number where this #include was encountered
        mnaIncludeLineNum[mnIncFilenameStackTop] =  pIncludePush->nLineNum;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionIncludePush(TScriptAction *pScriptPC)
{
    TActionIncludePush *pActionIncludePush = &pScriptPC->includePush;
    IncludePush( pActionIncludePush );

    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::IncludePop(TActionIncludePop *pIncludePop)
{
    if( (mnIncFilenameStackTop >= 0) && (mnIncFilenameStackTop <= MAX_FILE_NEST_DEPTH) )
    {
        mnIncFilenameStackTop--;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionIncludePop(TScriptAction *pScriptPC)
{
    TActionIncludePop *pActionIncludePop = &pScriptPC->includePop;
    IncludePop( pActionIncludePop );

    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSend(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    if( pScriptPC->send.nPortA < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->send.nPortA];
        SetRxPattern(pModule,pScriptPC->send.pRespStr);
        if(pModule->mpHost && pModule->mpHost->IsOpen() )
        {
            if( ! pModule->mpHost->IsAllowedSendOutcome(pScriptPC->send.eOutcome))
            {
                asMsg.printf("Port %d cannot deal with this SEND outcome = %s",pScriptPC->send.nPortA, MiscOutcomeString(pScriptPC->send.eOutcome));
                OnFail(asMsg.c_str());
                return;
            }

            pModule->mpHost->SendCommand(pScriptPC->send.pSendStr);
            int nTimeoutMs = pScriptPC->send.nTimeout;
            if(nTimeoutMs==USE_DEFAULT_TIMEOUT)
            {
                nTimeoutMs = mDefaultTimeoutMs;
            }
            StartModuleTimer(pModule,nTimeoutMs,true);
            frmMain->DisplayCommand(pScriptPC->send.nPortA,pScriptPC->send.pSendStr);

            if(pScriptPC->send.nPortB>=0)
            {
                if(pScriptPC->send.eOutcome == OUTCOME_PATTERN)
                {
                    asMsg.printf("Exp: %d<%s",pScriptPC->send.nPortB,pScriptPC->send.pRespStr);
                    frmMain->DisplayUserMessage(asMsg.c_str());
                }
                else if(pScriptPC->send.eOutcome == OUTCOME_OK)
                {
                    asMsg.printf("Expecting: <..>OK");
                    frmMain->DisplayUserMessage(asMsg.c_str());
                }
                else if(pScriptPC->send.eOutcome == OUTCOME_ERROR)
                {
                    asMsg.printf("Expecting: <..>ERROR");
                    frmMain->DisplayUserMessage(asMsg.c_str());
                }
            }
            else
            {
                frmMain->DisplayUserMessage("Expecting: Timeout");
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->breakOn.nPortA);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->breakOn.nPortA);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionBreakOn(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    asMsg.printf("Port %d: Break ON",pScriptPC->breakOn.nPortA);
    frmMain->DisplayUserMessage(asMsg.c_str());
    StopAllModuleTimers();

    if( pScriptPC->breakOn.nPortA < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->breakOn.nPortA];

        if(pModule->mpHost && pModule->mpHost->IsOpen() )
        {
            int nTimeoutMs;
            if(pScriptPC->breakOn.pTimeoutMs)
            {
                nTimeoutMs = atoi(pScriptPC->breakOn.pTimeoutMs);
            }
            else
            {
                nTimeoutMs = pScriptPC->breakOn.nTimeoutMs;
            }
            if(nTimeoutMs)
            {
                pModule->mpHost->SetBreak();
                StartModuleTimer(pModule, nTimeoutMs,false);
            }
            else
            {
                OnFail("0 ms BreakOn timeout specified");
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->breakOn.nPortA);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->breakOn.nPortA);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionBreakOff(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    asMsg.printf("Port %d: Break OFF",pScriptPC->breakOff.nPortA);
    frmMain->DisplayUserMessage(asMsg.c_str());
    StopAllModuleTimers();

    if( pScriptPC->breakOff.nPortA < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->breakOff.nPortA];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            int nTimeoutMs;
            if(pScriptPC->breakOff.pTimeoutMs)
            {
                if(strlen(pScriptPC->breakOff.pTimeoutMs)>0)
                {
                    nTimeoutMs = atoi(pScriptPC->breakOff.pTimeoutMs);
                }
                else
                {
                    nTimeoutMs = pScriptPC->breakOff.nTimeoutMs;
                }
            }
            else
            {
                nTimeoutMs = pScriptPC->breakOff.nTimeoutMs;
            }
            if(nTimeoutMs)
            {
                pModule->mpHost->ClearBreak();
                StartModuleTimer(pModule, nTimeoutMs,false);
            }
            else
            {
                OnFail("0 ms BreakOff timeout specified");
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->breakOn.nPortA);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->breakOff.nPortA);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSetVar(TScriptAction *pScriptPC)
{
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->setVar.pVarName);
    if(pVariable)
    {
        if( SrcSaveVariableValue(pVariable, pScriptPC->setVar.pVarValue) )
        {
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: SETVAR mem alloc failure");
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionMid(TScriptAction *pScriptPC)
{
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->mid.pVarName);
    if(pVariable)
    {
        int nOffset = pScriptPC->mid.offset;
        int nLength = pScriptPC->mid.length;
        int nActLen;
        int nSrcLen = strlen(pScriptPC->mid.pSrcString);
        char *pTempBuf = (char *)MiscMemAlloc(nLength+1);
        if(pTempBuf)
        {
            bool fSaved;

            if(nOffset < 0)
            {
                nOffset = nSrcLen + nOffset;
                if(nOffset<0)
                {
                    nOffset=0;
                }
            }

            if(nOffset<nSrcLen)
            {
                nActLen = (nSrcLen-nOffset);
                if(nActLen > nLength)nActLen = nLength;
                memcpy(pTempBuf, &pScriptPC->mid.pSrcString[nOffset], nActLen);
                pTempBuf[nActLen]=0;
            }
            else
            {
                pTempBuf[0]=0;
            }

            fSaved = SrcSaveVariableValue(pVariable, pTempBuf);
            MiscMemFree( pTempBuf );
            if(fSaved)
            {
                TriggerNextAction(__LINE__);
            }
            else
            {
                OnFail("Runtime: MID mem alloc failure");
            }
        }
        else
        {
            OnFail("Runtime: MID mem alloc failure");
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionStrCat(TScriptAction *pScriptPC)
{
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->strCat.pVarName);
    if(pVariable)
    {
        if( SrcConCatVariableValue(pVariable, pScriptPC->strCat.pVarValue) )
        {
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: STRCAT mem alloc failure");
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionTimer(TScriptAction *pScriptPC)
{
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->timer.pParmA);
    char baBuf[24];
    if(pVariable)
    {
        switch(pScriptPC->timer.nParmA)
        {
        case TIMER_START:
            sprintf(baBuf,"%d",GetTickCount());
            if(SrcSaveVariableValue(pVariable, baBuf))
            {
                TriggerNextAction(__LINE__);
            }
            else
            {
                OnFail("Runtime: TIMER mem alloc failure");
            }
            break;
        case TIMER_ELAPSED:
            TScriptVariable *pDestVar;
            pDestVar = SrcGetScriptVariable(mpScrManager, pScriptPC->timerElapse.pVarElapsed);
            if(pDestVar)
            {
                int nElapsedTime = GetTickCount() - atoi( pVariable->pValue );
                sprintf(baBuf,"%d",nElapsedTime);
                if(SrcSaveVariableValue(pDestVar, baBuf))
                {
                    TriggerNextAction(__LINE__);
                }
                else
                {
                    OnFail("Runtime: TIMER mem alloc failure for elapsed time");
                }
            }
            else
            {
                OnFail("Runtime: Unknown Variable to store time elapsed");
            }
            break;
        case TIMER_EXPIRED:
            if( pVariable->pValue )
            {
                int nExpTime,nElapsedTime,nExpired;
                if(pScriptPC->timerExpired.pExpTime)
                {
                    nExpTime   = atoi( pScriptPC->timerExpired.pExpTime );
                }
                else
                {
                    nExpTime   = 0;
                }
                nElapsedTime = GetTickCount() - atoi( pVariable->pValue );
                nExpired = (nElapsedTime >= nExpTime) ? 1 : 0;
                UpdateConditionFlags(nExpired);
                TriggerNextAction(__LINE__);
            }
            else
            {
                OnFail("Runtime: TIMER start tick missing");
            }
            break;
        default:
            OnFail("Runtime: Illegal TIMER action specified");
            break;
        }
    }
    else
    {
        OnFail("Runtime: Unknown Timer Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionConst(TScriptAction *pScriptPC)
{
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->setVar.pVarName);
    if(pVariable)
    {
        if( SrcSaveVariableValue(pVariable, pScriptPC->setVar.pVarValue) )
        {
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: CONST mem alloc failure");
        }
    }
    else
    {
        OnFail("Runtime: Unknown CONST Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionContain(TScriptAction *pScriptPC, bool fPresent, bool fFailAllow)
{
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->setVar.pVarName);
    if(pVariable)
    {
        if(mExpansionBuf)
        {
            if(SrcCopyAndExpandVariable(mpScrManager, pVariable->pValue, mExpansionBuf))
            {
                bool fPass;
                int nMatchCount=0;
                char *pBuf=mExpansionBuf;
                int nValLen = strlen(pScriptPC->checkVar.pVarValue);

                do
                {
                    if(*pBuf)
                    {
                        pBuf = strstr(pBuf,pScriptPC->checkVar.pVarValue);
                        if(pBuf)
                        {
                            pBuf += nValLen;
                            nMatchCount++;
                        }
                    }
                    else
                    {
                        pBuf=NULL;
                    }
                }
                while(pBuf);

                if(fPresent)
                {
                    fPass = (nMatchCount!=0) ? true : false ;
                }
                else
                {
                    fPass = (nMatchCount==0) ? true : false ;
                }

                if( (! fPass) && fFailAllow)
                {
                    asMsg.printf("VARVAL: %s",mExpansionBuf);
                    frmMain->DisplayUserMessage(asMsg.c_str());
                    asMsg.printf("MATCH : %s",pScriptPC->checkVar.pVarValue);
                    frmMain->DisplayUserMessage(asMsg.c_str());

                    if(fPresent )
                    {
                        OnFail("Runtime: Variable does not contain a match");
                    }
                    else
                    {
                        OnFail("Runtime: Variable does contain a match");
                    }
                }
                else
                {
                    /* save the match count */
                    char baVal[16];
                    sprintf(baVal,"%d", nMatchCount );
                    SrcSaveVariableValue(mpScrManager,"SysVarMatchCount", baVal);
                    TriggerNextAction(__LINE__);
                }
            }
            else
            {
                OnFail("Runtime: CHECK variable could not be expanded");
            }
        }
        else
        {
            OnFail("Runtime: CHECK out of memory");
        }
    }
    else
    {
        OnFail("Runtime: CHECK Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSprintInt(TScriptAction *pScriptPC)
{
    int nValue=0;
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->sprintInt.pVarName);
    static char sbaValBuf[1024];

    if(pVariable)
    {
        if(pScriptPC->sprintInt.pVarValue)nValue = atoi(pScriptPC->sprintInt.pVarValue);

        if(mExpansionBuf)
        {
            if(SrcCopyAndExpandVariable(mpScrManager, pVariable->pValue, mExpansionBuf))
            {
                if(strlen(mExpansionBuf))
                {
                    sprintf(sbaValBuf,mExpansionBuf,nValue);
                }
                else
                {
                    sprintf(sbaValBuf,"%d",nValue);
                }

                if( SrcSaveVariableValue(pVariable, sbaValBuf) )
                {
                    TriggerNextAction(__LINE__);
                }
                else
                {
                    OnFail("Runtime: SPRINTINT mem alloc failure");
                }
            }
            else
            {
                OnFail("Runtime: SPRINTINT variable could not be expanded");
            }
        }
        else
        {
            OnFail("Runtime: SPRINTINT out of memory");
        }
    }
    else
    {
        OnFail("Runtime: SPRINTINT Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSprintHex(TScriptAction *pScriptPC)
{
    int nValue=0;
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->sprintHex.pVarName);
    static char sbaValBuf[1024];

    if(pVariable)
    {
        if(pScriptPC->sprintInt.pVarValue)
        {
            sscanf(pScriptPC->sprintHex.pVarValue,"%x",&nValue);
        }

        if(mExpansionBuf)
        {
            if(SrcCopyAndExpandVariable(mpScrManager, pVariable->pValue, mExpansionBuf))
            {
                if(strlen(mExpansionBuf))
                {
                    sprintf(sbaValBuf,mExpansionBuf,nValue);
                }
                else
                {
                    sprintf(sbaValBuf,"%X",nValue);
                }

                if( SrcSaveVariableValue(pVariable, sbaValBuf) )
                {
                    TriggerNextAction(__LINE__);
                }
                else
                {
                    OnFail("Runtime: SPRINTHEX mem alloc failure");
                }
            }
            else
            {
                OnFail("Runtime: SPRINTHEX variable could not be expanded");
            }
        }
        else
        {
            OnFail("Runtime: SPRINTHEX out of memory");
        }
    }
    else
    {
        OnFail("Runtime: SPRINTHEX Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionHex2Dec(TScriptAction *pScriptPC)
{
    int nValue=0;
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->hex2Dec.pVarName);
    char sbaValBuf[128];

    if(pVariable)
    {
        if(pScriptPC->hex2Dec.pVarValue)
        {
            sscanf(pScriptPC->hex2Dec.pVarValue,"%x",&nValue);
        }
        else
        {
            nValue=0;
        }

        sprintf(sbaValBuf,"%d",nValue);

        if( SrcSaveVariableValue(pVariable, sbaValBuf) )
        {
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: HEX2DEC mem alloc failure");
        }
    }
    else
    {
        OnFail("Runtime: HEX2DEC Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionDec2Hex(TScriptAction *pScriptPC)
{
    int nValue=0;
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->dec2Hex.pVarName);
    char sbaValBuf[128];

    if(pVariable)
    {
        if(pScriptPC->dec2Hex.pVarValue)
        {
            nValue = atoi(pScriptPC->hex2Dec.pVarValue);
        }
        else
        {
            nValue=0;
        }

        sprintf(sbaValBuf,"%X",nValue);

        if( SrcSaveVariableValue(pVariable, sbaValBuf) )
        {
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: DEC2HEX mem alloc failure");
        }
    }
    else
    {
        OnFail("Runtime: DEC2HEX Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionConsoleUpdate(TScriptAction *pScriptPC)
{
    if(pScriptPC->consoleUpdate.nEnable)
    {
        frmMain->DisplayUserMessage("Console Window Update:: ON");
        frmMain->ConsoleUpdateEnable();
        mfConsoleUpdate=true;
    }
    else
    {
        //frmMain->DisplayUserMessage("Console Window Update:: OFF");
        frmMain->ConsoleUpdateDisable();
        mfConsoleUpdate=false;
    }
    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionTrace(TScriptAction *pScriptPC)
{
    mfShowTrace = pScriptPC->trace.nEnable ? true : false;
    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionTraceWin(TScriptAction *pScriptPC)
{
    if(pScriptPC->trace.nEnable==1)
    {
        frmAggLogger->mfUpdateWindow = true;
        frmMain->DisplayUserMessage("Aggregate Trace Window :: ON");
        mfLoggerUpdate = true;
    }
    else if(pScriptPC->trace.nEnable==0)
    {
        frmAggLogger->mfUpdateWindow = false;
        frmMain->DisplayUserMessage("Aggregate Trace Window :: OFF");
        mfLoggerUpdate = false;
    }
    else if(pScriptPC->trace.nEnable==2)
    {
        //clear the window
        frmAggLogger->reMemo->Clear();
    }
    /*
    ELSE
       just ignore it
    */

    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionExistFile(TScriptAction *pScriptPC)
{
    int nState = FileExists(pScriptPC->existFile.pFilename) ? 1 : 0;
    int nLogic = (pScriptPC->existFile.nLogic != 0) ? 1 : 0;
    if(nState ^ nLogic)
    {
        AnsiString asFile=pScriptPC->existFile.pFilename;
        AnsiString asMsg;
        if(nState)
        {
            asMsg="File exists <";
        }
        else
        {
            asMsg="File does not exist <";
        }
        asMsg += asFile;
        asMsg += ">";
        OnFail(asMsg.c_str());
    }
    else
    {
        TriggerNextAction(__LINE__);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionExistFolder(TScriptAction *pScriptPC)
{
    int nState = DirectoryExists(pScriptPC->existFolder.pFoldername) ? 1 : 0;
    int nLogic = (pScriptPC->existFile.nLogic != 0) ? 1 : 0 ;
    if(nState ^ nLogic)
    {
        AnsiString asFile=pScriptPC->existFile.pFilename;
        AnsiString asMsg;
        if(nState)
        {
            asMsg="Folder exists <";
        }
        else
        {
            asMsg="Folder does not exist <";
        }
        asMsg += asFile;
        asMsg += ">";
        OnFail(asMsg.c_str());
    }
    else
    {
        TriggerNextAction(__LINE__);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool FileCompare(char *pFileA, char *pFileB)
{
#define MAX_COMPARE_BLOCKSIZE       1024
    bool fRetVal = false;
    char *pBufA=(char *)MiscMemAlloc(MAX_COMPARE_BLOCKSIZE);
    char *pBufB=(char *)MiscMemAlloc(MAX_COMPARE_BLOCKSIZE);
    FILE *fpA=NULL;
    FILE *fpB=NULL;

    if(pBufA && pBufB)
    {
        fpA=fopen(pFileA,"rb");
        if(fpA)
        {
            fpB=fopen(pFileB,"rb");
            if(fpB)
            {
                int nReadA,nReadB;
                bool fContinue = true;
                while(fContinue)
                {
                    nReadA = fread(pBufA,1,MAX_COMPARE_BLOCKSIZE,fpA);
                    nReadB = fread(pBufB,1,MAX_COMPARE_BLOCKSIZE,fpB);

                    if( (nReadA == 0) && (nReadB == 0) )
                    {
                        fRetVal = true;
                        fContinue=false;
                    }
                    else if( (nReadA == 0) || (nReadB == 0) )
                    {
                        fContinue=false;
                    }
                    else if(nReadA == nReadB)
                    {
                        if(memcmp(pBufA,pBufB,nReadA)!=0)
                        {
                            fContinue=false;
                        }
                    }
                    else
                    {
                        fContinue=false;
                    }
                }
            }
        }
    }

    if(fpA)fclose(fpA);
    if(fpB)fclose(fpB);

    if(pBufA)MiscMemFree(pBufA);
    if(pBufB)MiscMemFree(pBufB);

    return fRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFileCompare(TScriptAction *pScriptPC)
{
    int nState = FileCompare(pScriptPC->fileCompare.pFilenameA,pScriptPC->fileCompare.pFilenameB) ? 1 : 0;
    int nLogic = (pScriptPC->fileCompare.nLogic != 0) ? 1 : 0 ;
    if(nState ^ nLogic)
    {
        AnsiString asFile=pScriptPC->fileCompare.pFilenameA;
        AnsiString asMsg="Files Mismatch: <";
        asMsg += asFile;
        asMsg += ">,<";
        asFile=pScriptPC->fileCompare.pFilenameB;
        asMsg += asFile;
        asMsg += ">";
        OnFail(asMsg.c_str());
    }
    else
    {
        TriggerNextAction(__LINE__);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFileOpen(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    if(pScriptPC->fileOpen.nHandle < MAX_FILE_HANDLES )
    {
        if(pScriptPC->fileOpen.pPermissions)
        {
            char baMode[16];
            strcpy(baMode,"UNKNOWN");
            if( strcmpi(pScriptPC->fileOpen.pPermissions,"read")==0)
            {
                strcpy(baMode,"rb");
            }
            else if( strcmpi(pScriptPC->fileOpen.pPermissions,"write")==0)
            {
                strcpy(baMode,"wb");
            }
            else if( strcmpi(pScriptPC->fileOpen.pPermissions,"append")==0)
            {
                strcpy(baMode,"ab+");
            }

            if( strcmpi(baMode,"UNKNOWN") == 0 )
            {
                asMsg.sprintf("File Open illegal mode:<%s>",pScriptPC->fileOpen.pPermissions);
                OnFail(asMsg.c_str());
            }
            else
            {
                if( pScriptPC->fileOpen.pFilename )
                {
                    int len = strlen(pScriptPC->fileOpen.pFilename);
                    if(len)
                    {
                        FILE *fp = fopen(pScriptPC->fileOpen.pFilename,baMode);
                        if(fp)
                        {
                            if( mpFileHandle[pScriptPC->fileOpen.nHandle] )
                            {
                                fclose(mpFileHandle[pScriptPC->fileOpen.nHandle]);
                            }
                            mpFileHandle[pScriptPC->fileOpen.nHandle] = fp;
                            TriggerNextAction(__LINE__);
                        }
                        else
                        {
                            AnsiString asFile=pScriptPC->fileOpen.pFilename;
                            AnsiString asMsg="File Open Fail: <";
                            asMsg += asFile;
                            asMsg += ">";
                            OnFail(asMsg.c_str());
                        }
                    }
                    else
                    {
                        asMsg.sprintf("File Open Error : filename is empty");
                        OnFail(asMsg.c_str());
                    }
                }
                else
                {
                    asMsg.sprintf("File Open Error : filename is null");
                    OnFail(asMsg.c_str());
                }
            }
        }
        else
        {
            asMsg.sprintf("File Open illegal mode:<>");
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        OnFail("Illegal File Handle");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFileClose(TScriptAction *pScriptPC)
{
    if(pScriptPC->fileClose.nHandle < MAX_FILE_HANDLES )
    {
        if(mpFileHandle[pScriptPC->fileClose.nHandle])
        {
            fclose(mpFileHandle[pScriptPC->fileClose.nHandle]);
            mpFileHandle[pScriptPC->fileClose.nHandle] = NULL;
            TriggerNextAction(__LINE__);
        }
        else
        {
#if 1
            AnsiString asMsg;
            asMsg.printf("Warning: File Handle %d is already closed",pScriptPC->fileClose.nHandle);
            frmMain->DisplayUserMessage(asMsg.c_str());
            TriggerNextAction(__LINE__);
#else
            OnFail("File Handle is already closed");
#endif
        }
    }
    else
    {
        OnFail("Illegal File Handle");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFileRead(TScriptAction *pScriptPC)
{
    if(pScriptPC->fileRead.nHandle < MAX_FILE_HANDLES )
    {
        if(mpFileHandle[pScriptPC->fileRead.nHandle])
        {
            AnsiString asMsg;
            TScriptVariable *pVarData = SrcGetScriptVariable(mpScrManager, pScriptPC->fileRead.pVarData);
            if(pVarData)
            {
                TScriptVariable *pVarLen = SrcGetScriptVariable(mpScrManager, pScriptPC->fileRead.pVarLen);
                if(pVarLen)
                {
                    int nReadLen=0;
                    if( pVarLen->pValue == NULL)
                    {
                        pVarLen->pValue = (char *)MiscMemAlloc(MAX_INT_VALUE_STRING_SIZE+1);
                        if(pVarLen->pValue)
                        {
                            strcpy(pVarLen->pValue,"0");
                        }
                    }
                    if( pVarLen->pValue)
                    {
                        nReadLen = atoi(pVarLen->pValue);
                        if( nReadLen )
                        {
                            MiscMemFree(pVarData->pValue);
                            pVarData->pValue = (char *)MiscMemAlloc(nReadLen+1);
                            if( pVarData->pValue)
                            {
                                nReadLen = fread(pVarData->pValue,1,nReadLen,mpFileHandle[pScriptPC->fileRead.nHandle]);
                                sprintf(pVarLen->pValue,"%d",nReadLen);
                                pVarData->pValue[nReadLen]=0;
                                TriggerNextAction(__LINE__);
                            }
                            else
                            {
                                OnFail("Runtime: FILEREAD mem alloc failure for varData");
                            }
                        }
                        else
                        {
                            MiscMemFree(pVarData->pValue);
                            pVarData->pValue = NULL;
                            TriggerNextAction(__LINE__);
                        }
                    }
                    else
                    {
                        OnFail("Runtime: FILEREAD mem alloc failure for varLen");
                    }
                }
                else
                {
                    OnFail("Runtime: Unknown pVarLen");
                }
            }
            else
            {
                OnFail("Runtime: Unknown pVarData");
            }
        }
        else
        {
            AnsiString asMsg;
            asMsg.sprintf("FILEREAD handle %d is not open",pScriptPC->fileRead.nHandle);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        OnFail("Illegal File Handle");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFileWrite(TScriptAction *pScriptPC)
{
    if(pScriptPC->fileWrite.nHandle < MAX_FILE_HANDLES )
    {
        if(mpFileHandle[pScriptPC->fileWrite.nHandle])
        {
            AnsiString asMsg;
            TScriptVariable *pVarData = SrcGetScriptVariable(mpScrManager, pScriptPC->fileWrite.pVarData);
            if(pVarData)
            {
                if( pVarData->pValue)
                {
                    int nWriteLen   = strlen(pVarData->pValue);
                    int nWrittenLen = fwrite(pVarData->pValue,1,nWriteLen,mpFileHandle[pScriptPC->fileWrite.nHandle]);
                    if( nWriteLen == nWrittenLen )
                    {
                        TriggerNextAction(__LINE__);
                    }
                    else
                    {
                        AnsiString asMsg;
                        asMsg.sprintf("FILEWRITE handle %d : write error",pScriptPC->fileRead.nHandle);
                        OnFail(asMsg.c_str());
                    }
                }
                else
                {
                    TriggerNextAction(__LINE__);
                }
            }
            else
            {
                OnFail("Runtime: Unknown pVarData");
            }
        }
        else
        {
            AnsiString asMsg;
            asMsg.sprintf("FILEWRITE handle %d is not open",pScriptPC->fileRead.nHandle);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        OnFail("Illegal File Handle");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionCheckVar(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->setVar.pVarName);
    if(pVariable)
    {
        if(mExpansionBuf)
        {
            if(SrcCopyAndExpandVariable(mpScrManager, pVariable->pValue, mExpansionBuf))
            {
                if( strcmp(pScriptPC->checkVar.pVarValue,mExpansionBuf)!=0 )
                {
                    asMsg.printf("EXP: %s",pScriptPC->checkVar.pVarValue);
                    frmMain->DisplayUserMessage(asMsg.c_str());
                    asMsg.printf("GOT: %s",mExpansionBuf);
                    frmMain->DisplayUserMessage(asMsg.c_str());

                    OnFail("Runtime: CHECK variable mismatch");
                }
                else
                {
                    TriggerNextAction(__LINE__);
                }
            }
            else
            {
                OnFail("Runtime: CHECK variable could not be expanded");
            }
        }
        else
        {
            OnFail("Runtime: CHECK out of memory");
        }
    }
    else
    {
        OnFail("Runtime: CHECK Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::UpdateConditionFlags(int nCurValue)
{
    mConditionFlags.z   = (nCurValue == 0)?true:false;
    mConditionFlags.lt  = (nCurValue <  0)?true:false;
    mConditionFlags.le  = (nCurValue <=  0)?true:false;
    mConditionFlags.gt  = (nCurValue >  0)?true:false;
    mConditionFlags.ge  = (nCurValue >=  0)?true:false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionArithmetic(TScriptAction *pScriptPC, bool fHexMode)
{
    int nCurValue=0;
    int nAddValue=0;
    int nType = pScriptPC->alu.nAluType;
    bool fUpdateVar=true;

    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->alu.pVarName);
    if(pVariable)
    {
        if( pVariable->pValue )
        {
            if(fHexMode)
            {
                sscanf(pVariable->pValue,"%x",&nCurValue);
            }
            else
            {
                nCurValue = atoi(pVariable->pValue);
            }
            /* MiscMemFree(pVariable->pValue);                      */
            /* commented out to cater for TST and HEXTST statements */
        }

        if(pScriptPC->alu.pVarValue)
        {
            if(fHexMode)
            {
                sscanf(pScriptPC->alu.pVarValue,"%x",&nAddValue);
            }
            else
            {
                nAddValue = atoi(pScriptPC->alu.pVarValue);
            }
        }

        if( (nType==ALU_DIV) && (nAddValue==0) )
        {
            OnFail("Runtime: Divide by 0 Error");
        }
        else
        {
            switch(nType)
            {
            case ALU_ABS:
                nCurValue = abs(nAddValue); break;

            case ALU_HEXADD:
            case ALU_ADD:
                nCurValue += nAddValue; break;

            case ALU_HEXMUL:
            case ALU_MUL:
                nCurValue *= nAddValue; break;

            case ALU_HEXDIV:
            case ALU_DIV:
                nCurValue /= nAddValue; break;

            case ALU_HEXSUB:
            case ALU_SUB:
                nCurValue -= nAddValue; break;

            case ALU_HEXTST:
            case ALU_TST:
                nCurValue -= nAddValue; fUpdateVar = false; break;

            case ALU_HEXMOD:
            case ALU_MOD:
                nCurValue %= nAddValue; break;

            case ALU_HEXAND:
            case ALU_AND:
                nCurValue &= nAddValue; break;

            case ALU_HEXOR :
            case ALU_OR :
                nCurValue |= nAddValue; break;

            case ALU_HEXXOR:
            case ALU_XOR:
                nCurValue ^= nAddValue; break;

            case ALU_HEXROR:
            case ALU_ROR:
                nCurValue >>= nAddValue; break;

            case ALU_HEXROL:
            case ALU_ROL:
                nCurValue <<= nAddValue; break;

            default: break; //do nothing
            }

            if(fUpdateVar)
            {
                if( pVariable->pValue )
                {
                    MiscMemFree(pVariable->pValue);
                }
                pVariable->pValue = (char *)MiscMemAlloc(MAX_INT_VALUE_STRING_SIZE+1);
                if( pVariable->pValue )
                {
                    if(fHexMode)
                    {
                        sprintf(pVariable->pValue,"%X",nCurValue);
                    }
                    else
                    {
                        sprintf(pVariable->pValue,"%d",nCurValue);
                    }
                    UpdateConditionFlags(nCurValue);
                    TriggerNextAction(__LINE__);
                }
                else
                {
                    OnFail("Runtime: ARITHMETIC mem alloc failure");
                }
            }
            else
            {
                UpdateConditionFlags(nCurValue);
                TriggerNextAction(__LINE__);
            }
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionDecArithmetic(TScriptAction *pScriptPC)
{
    RunActionArithmetic( pScriptPC,false);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionHexArithmetic(TScriptAction *pScriptPC)
{
    RunActionArithmetic( pScriptPC,true);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSaveResp(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->saveResp.pVarName);
    if(pVariable)
    {
        if( pVariable->pValue )
        {
            MiscMemFree(pVariable->pValue);
            pVariable->pValue=NULL;
        }
        SaveResp(pScriptPC->saveResp.nPort, pVariable);
        if(pScriptPC->saveResp.fEcho)
        {
            asMsg.printf("FullResp: %d<%s",pScriptPC->saveResp.nPort,pVariable->pValue);
            frmMain->DisplayUserMessage(asMsg.c_str());
        }
        TriggerNextAction(__LINE__);
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPortInfo(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->rxCount.pVarName);
    if(pVariable)
    {
        if( pVariable->pValue )
        {
            MiscMemFree(pVariable->pValue);
            pVariable->pValue=NULL;
        }
        GetPortInfo(pScriptPC->rxCount.nPort, pVariable, pScriptPC->rxCount.nParmB);
        TriggerNextAction(__LINE__);
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionMultiIndex(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    int nModule;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->multiIndex.pVarName);
    if(pVariable)
    {
        if( pVariable->pValue )
        {
            MiscMemFree(pVariable->pValue);
            pVariable->pValue=NULL;
        }
        nModule = pScriptPC->multiIndex.nPort;
        if( nModule < gnMaximumLogicalPorts)
        {
            TModule *pModule = &mpModule[nModule];
            if(pModule->mpHost)
            {
                if(pModule->mpHost->IsOpen())
                {
                    char baIndexNumber[24];
                    sprintf(baIndexNumber,"%d", pModule->mpHost->GetPatternIndexNumber());
                    if( ! SrcSaveVariableValue(pVariable, baIndexNumber) )
                    {
                        OnFail("Runtime: Failed to save variable");
                    }
                }
            }
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: Illegal port number");
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPktCountGet(TScriptAction *pScriptPC)
{
    char baCountStr[32];
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->pktCountGet.pVarName);
    if(pVariable)
    {
        TModule *pModule = &mpModule[pScriptPC->pktCountGet.nPort];

        if( pVariable->pValue )
        {
            MiscMemFree(pVariable->pValue);
            pVariable->pValue=NULL;
        }
        sprintf(baCountStr,"%u",pModule->mpHost->GetPacketCount());
        if(SrcSaveVariableValue(pVariable, baCountStr))
        {
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: Variable could not save");
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPktCountSet(TScriptAction *pScriptPC)
{
    char baCountStr[32];
    unsigned int nCount;
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->pktCountGet.pVarName);
    if(pVariable)
    {
        TModule *pModule = &mpModule[pScriptPC->pktCountGet.nPort];

        if( pVariable->pValue )
        {
            nCount = atoi(pVariable->pValue);
        }
        else
        {
            nCount=0;
        }
        pModule->mpHost->SetPacketCount(nCount);
        TriggerNextAction(__LINE__);
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPktCountResp(TScriptAction *pScriptPC)
{
    char baCountStr[32];
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->pktCountGet.pVarName);
    if(pVariable)
    {
        TModule *pModule = &mpModule[pScriptPC->pktCountGet.nPort];

        if( pVariable->pValue )
        {
            MiscMemFree(pVariable->pValue);
            pVariable->pValue=NULL;
        }
        sprintf(baCountStr,"%u",pModule->mpHost->GetPacketCountInResp());
        if(SrcSaveVariableValue(pVariable, baCountStr))
        {
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: Variable could not save");
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionGetCwd(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->getCwd.pVarName);
    if(pVariable)
    {
        AnsiString asFolder = GetCurrentDir();
        if(SrcSaveVariableValue(pVariable, asFolder.c_str()))
        {
            asMsg="Current Folder: <";
            asMsg += asFolder;
            asMsg += ">";
            frmMain->DisplayUserMessage(asMsg.c_str());
            TriggerNextAction(__LINE__);
        }
        else
        {
            OnFail("Runtime: GETCWD mem alloc failure");
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionString(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    TScriptVariable *pVariable1 = SrcGetScriptVariable(mpScrManager, pScriptPC->string.pVarName1);
    if(pVariable1)
    {
        TScriptVariable *pVariable2 = SrcGetScriptVariable(mpScrManager, pScriptPC->string.pVarName2);
        if(pVariable2)
        {
            switch(pScriptPC->string.nFuncType)
            {
            case STR_LEN:
                if( pVariable2->pValue == NULL)
                {
                    pVariable2->pValue = (char *)MiscMemAlloc(MAX_INT_VALUE_STRING_SIZE+1);
                }
                if( pVariable2->pValue)
                {
                    int nLen = 0;
                    if( pVariable1->pValue ) nLen = strlen(pVariable1->pValue);
                    sprintf(pVariable2->pValue,"%d",nLen);
                    TriggerNextAction(__LINE__);
                }
                else
                {
                    OnFail("Runtime: STRLEN mem alloc failure");
                }
                break;
            case STR_CMP:
            case STR_CMPI:
                mConditionFlags.z = false;
                if(  pVariable1->pValue == pVariable2->pValue )
                {   //both point to the same memory location (can be NULL)
                    mConditionFlags.z = true;
                }
                else if ( pVariable1->pValue && pVariable2->pValue)
                {
                    switch(pScriptPC->string.nFuncType)
                    {
                    case STR_CMP:
                        if( strcmp(pVariable1->pValue,pVariable2->pValue)== 0)
                        {
                            mConditionFlags.z = true;
                        }
                        break;
                    case STR_CMPI:
                        if( strcmpi(pVariable1->pValue,pVariable2->pValue)== 0)
                        {
                            mConditionFlags.z = true;
                        }
                        break;
                    default:
                        break;
                    }
                }
                TriggerNextAction(__LINE__);
                break;
            case STR_STR:
            case STR_STRI:
                mConditionFlags.z = false;
                if(  pVariable1->pValue == pVariable2->pValue )
                {   //both point to the same memory location (can be NULL)
                    mConditionFlags.z = true;
                }
                else if ( pVariable1->pValue && pVariable2->pValue)
                {
                    switch(pScriptPC->string.nFuncType)
                    {
                    case STR_STR:  //case sensitive
                        if( MiscStrContains(pVariable1->pValue,pVariable2->pValue,true))
                        {
                            mConditionFlags.z = true;
                        }
                        break;
                    case STR_STRI: //case insensitive
                        if( MiscStrContains(pVariable1->pValue,pVariable2->pValue,false))
                        {
                            mConditionFlags.z = true;
                        }
                        break;
                    default:
                        break;
                    }
                }
                TriggerNextAction(__LINE__);
                break;
            default:
                OnFail("Runtime: Unknown STRING Function type");
                break;
            }
        }
        else
        {
            OnFail("Runtime: Unknown Variable2");
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable1");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFilterMove(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    TScriptVariable *pVariableDst = SrcGetScriptVariable(mpScrManager, pScriptPC->filterMove.pVarnameDst);
    if(pVariableDst)
    {
        TScriptVariable *pVariableSrc = SrcGetScriptVariable(mpScrManager, pScriptPC->filterMove.pVarnameSrc);
        if(pVariableSrc)
        {
            if( pVariableDst == pVariableSrc )
            {
                OnFail("Runtime: VariableSrc same as pVariableDst");
            }
            else
            {
                if( SrcFilterMoveVariable(pVariableDst, pVariableSrc, pScriptPC->filterMove.nMask, pScriptPC->filterMove.fInvert ) )
                {
                    TriggerNextAction(__LINE__);
                }
                else
                {
                    OnFail("Runtime: FILTERMOVE mem alloc failure");
                }
            }
        }
        else
        {
            OnFail("Runtime: Unknown VariableSrc");
        }
    }
    else
    {
        OnFail("Runtime: Unknown VariableDst");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionStrDeHexize(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    TScriptVariable *pVariableDst = SrcGetScriptVariable(mpScrManager, pScriptPC->strDeHexize.pVarnameDst);
    if(pVariableDst)
    {
        TScriptVariable *pVariableSrc = SrcGetScriptVariable(mpScrManager, pScriptPC->strDeHexize.pVarnameSrc);
        if(pVariableSrc)
        {
            if( pVariableDst == pVariableSrc )
            {
                OnFail("Runtime: VariableSrc same as pVariableDst");
            }
            else
            {
                if( SrcDeHexizeVariable(pVariableDst, pVariableSrc, pScriptPC->strDeHexize.nFlags ) )
                {
                    TriggerNextAction(__LINE__);
                }
                else
                {
                    OnFail("Runtime: STRDEHEXIZE mem alloc failure");
                }
            }
        }
        else
        {
            OnFail("Runtime: Unknown VariableSrc");
        }
    }
    else
    {
        OnFail("Runtime: Unknown VariableDst");
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitResp(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    char *pFullResp;
    int nPort = pScriptPC->waitResp.nPort;
    char *pExpResp = pScriptPC->waitResp.pRespStr;
    char *pCleanExpResp;

    //First check if already in buffer
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            pFullResp = pModule->mpHost->GetFullResponse();

            pCleanExpResp = SetRxPattern(pModule,pExpResp,strlen(pExpResp),0);
            if( MiscContainsPattern(pFullResp,pCleanExpResp) )
            {   //already in buffer
                DisplayResponse(nPort,pFullResp);
                pScriptPC->generic.eOutcome = OUTCOME_DONE;
                TriggerNextAction(__LINE__);
            }
            else
            {
                int nTimeoutMs;
                nTimeoutMs = pScriptPC->waitResp.nTimeout;
                StartModuleTimer(pModule,nTimeoutMs,true);
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitRespEx(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    char *pFullResp;
    int nPort = pScriptPC->waitRespEx.nPort;
    char *pExpResp = pScriptPC->waitRespEx.pRespStr;
    TScriptVariable *pVariable=NULL;
    if(  pScriptPC->waitRespEx.pVarName )
    {
        pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->waitRespEx.pVarName);
    }

    //First check if already in buffer
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            char *pFound;
            mpWaitRespExCleanExpResp = SetRxPattern(pModule,pExpResp,strlen(pExpResp),0);
            asMsg.printf("exp: %d<%s",pScriptPC->waitRespEx.nPort,mpWaitRespExCleanExpResp);
            frmMain->DisplayUserMessage(asMsg.c_str());
            pFullResp = pModule->mpHost->GetFullResponse();
            pFound = MiscContainsPattern(pFullResp,mpWaitRespExCleanExpResp);
            if( pFound )
            {   //already in buffer
                int nToDelete = (pFound - pFullResp) + strlen(mpWaitRespExCleanExpResp);
                DisplayResponse(nPort,pFullResp);
                SaveBlockToVariable(pFullResp, 0, nToDelete, pVariable);
                pModule->mpHost->DropFromResponse(nToDelete);
                pScriptPC->generic.eOutcome = OUTCOME_DONE;

                ServiceNextOpcode();
                return;
            }
            else
            {
                int nTimeoutMs;
                nTimeoutMs = pScriptPC->waitRespEx.nTimeout;
                StartModuleTimer(pModule,nTimeoutMs,true);
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitTimeout(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    char *pFullResp;
    int nPort = pScriptPC->waitTimeout.nPort;
    char *pExpResp = pScriptPC->waitTimeout.pRespStr;
    TScriptVariable *pVariable=NULL;
    if(  pScriptPC->waitTimeout.pVarName )
    {
        pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->waitTimeout.pVarName);
    }

    //First check if already in buffer
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            char *pFound;
            mpWaitRespExCleanExpResp = SetRxPattern(pModule,pExpResp,strlen(pExpResp),0);
            asMsg.printf("NOT exp: %d<%s",pScriptPC->waitTimeout.nPort,mpWaitRespExCleanExpResp);
            frmMain->DisplayUserMessage(asMsg.c_str());
            pFullResp = pModule->mpHost->GetFullResponse();
            pFound = MiscContainsPattern(pFullResp,mpWaitRespExCleanExpResp);
            if( pFound )
            {   //already in buffer
                int nToDelete = (pFound - pFullResp) + strlen(mpWaitRespExCleanExpResp);
                DisplayResponse(nPort,pFullResp);
                SaveBlockToVariable(pFullResp, 0, nToDelete, pVariable);
                pModule->mpHost->DropFromResponse(nToDelete);
                pScriptPC->generic.eOutcome = OUTCOME_DONE;

                //fail test
                AnsiString asMsg;
                asMsg.printf("Module %d did not timeout, got matching string",nPort);
                OnFail(asMsg.c_str());
            }
            else
            {
                int nTimeoutMs;
                nTimeoutMs = pScriptPC->waitTimeout.nTimeout;
                StartModuleTimer(pModule,nTimeoutMs,true);
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitUntil(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    char *pFullResp;
    int nPort = pScriptPC->waitUntil.nPort;
    char *pExpResp = pScriptPC->waitUntil.pRespStr;
    TScriptVariable *pVariable=NULL;
    if(  pScriptPC->waitUntil.pVarName )
    {
        pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->waitUntil.pVarName);
    }

    //First check if already in buffer
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            char *pFound;
            mpWaitRespExCleanExpResp = SetRxPattern(pModule,pExpResp,strlen(pExpResp),0);
            asMsg.printf("Until: %d<%s",pScriptPC->waitUntil.nPort,mpWaitRespExCleanExpResp);
            frmMain->DisplayUserMessage(asMsg.c_str());
            pFullResp = pModule->mpHost->GetFullResponse();
            pFound = MiscContainsPattern(pFullResp,mpWaitRespExCleanExpResp);
            if( pFound )
            {   //already in buffer
                int nToDelete = (pFound - pFullResp) + strlen(mpWaitRespExCleanExpResp);
                DisplayResponse(nPort,pFullResp);
                SaveBlockToVariable(pFullResp, 0, nToDelete, pVariable);
                pModule->mpHost->DropFromResponse(nToDelete);
                pScriptPC->generic.eOutcome = OUTCOME_DONE;

                //Set the Z flag
                mConditionFlags.z = true;

                ServiceNextOpcode();
                return;
            }
            else
            {
                int nTimeoutMs;
                nTimeoutMs = pScriptPC->waitUntil.nTimeout;
                StartModuleTimer(pModule,nTimeoutMs,true);
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitMulti(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    char *pFullResp;
    int nPort = pScriptPC->waitResp.nPort;
    char *pExpResp = pScriptPC->waitResp.pRespStr;

    //First check if already in buffer
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            int nCheckResult;
            pFullResp = pModule->mpHost->GetFullResponse();
            SetRxPattern(pModule,pExpResp,strlen(pExpResp),3);
            nCheckResult = pModule->mpHost->CheckPartialPatternMulti();
            if( nCheckResult >= 0 )
           {   //already in buffer
                DisplayResponse(nPort,pFullResp);
                pScriptPC->generic.eOutcome = OUTCOME_DONE;
                TriggerNextAction(__LINE__);
            }
            else
            {
                int nTimeoutMs;
                nTimeoutMs = pScriptPC->waitResp.nTimeout;
                StartModuleTimer(pModule,nTimeoutMs,true);
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitData(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    char *pFullResp;
    char *pFullDataResp;
    int nPort = pScriptPC->waitData.nPort;
    int nChannel = pScriptPC->waitData.nChannel;
    char *pExpResp = pScriptPC->waitData.pRespStr;
    int nExpRespLen = strlen(pExpResp);
    char *pCleanExpResp;


    //First check if already in buffer
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            pFullDataResp = pModule->mpHost->GetDataInResponse((unsigned int) nChannel);
            pFullResp     = pModule->mpHost->GetFullResponse();
            pCleanExpResp = SetRxPattern(pModule,pExpResp,nExpRespLen,((nChannel<<8)+2));
            if( MiscContainsPattern(pFullDataResp,pCleanExpResp) )
            {   //already in buffer
                DisplayResponse(nPort,pFullResp);
                pScriptPC->generic.eOutcome = OUTCOME_DONE;
                TriggerNextAction(__LINE__);
            }
            else
            {
                int nTimeoutMs;
                nTimeoutMs = pScriptPC->waitData.nTimeout;
                StartModuleTimer(pModule,nTimeoutMs,true);
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitChannel(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    char *pFullResp;
    char *pFullDataResp;
    int nPort = pScriptPC->waitChannel.nPort;
    int nChannel = pScriptPC->waitChannel.nChannel;
    char *pExpResp = pScriptPC->waitChannel.pRespStr;
    int nExpRespLen = strlen(pExpResp);
    char *pCleanExpResp;


    //First check if already in buffer
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            pFullDataResp = pModule->mpHost->GetDataInResponse((unsigned int) nChannel);
            pCleanExpResp = SetRxPattern(pModule,pExpResp,nExpRespLen,((nChannel<<8)+2));
            char *pFound  = MiscContainsPattern(pFullDataResp,pCleanExpResp);
            if( pFound )
            {   //already in buffer
                DisplayChannelResponse(nPort,nChannel,pFullDataResp);
                int nDropLen = (pFound-pFullDataResp)+strlen( pCleanExpResp );
                pModule->mpHost->DropFromChannel( nChannel, nDropLen );
                pScriptPC->generic.eOutcome = OUTCOME_DONE;
                TriggerNextAction(__LINE__);
            }
            else
            {
                int nTimeoutMs;
                nTimeoutMs = pScriptPC->waitChannel.nTimeout;
                StartModuleTimer(pModule,nTimeoutMs,true);
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitPacket(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    int nPort = pScriptPC->waitPacket.nPort;

    mConditionFlags.z = false;
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            /* added the following in July 2017, because we search for pattern in  OnTiEvent()
               See method MPointMyHost::OnRxData() which looks for the pattern in the background
               and there is no pattern to search there
            */
            SetRxPattern(pModule,NULL,0,0);

            if( pModule->mpHost->IsPacketPresent(TPACKET_TYPE_MPOINT) )
            {   //already in buffer
                SavePacketInVariable(pModule,pScriptPC,TPACKET_TYPE_MPOINT);
                mConditionFlags.z = true;
                TriggerNextAction(__LINE__);
            }
            else
            {
                //packet has not yet arrived so we need to wait for a specific time
                int nTimeoutMs = pScriptPC->waitPacket.nTimeout;
                StartModuleTimer(pModule,nTimeoutMs,true);
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionWaitTiEvent(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    int nPort = pScriptPC->waitTiEvent.nPort;

    SrcStripWhitespace(pScriptPC->waitTiEvent.pMatch);

    mConditionFlags.z = false;
    if( nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            /* added the following in July 2017, because we search for pattern in  OnTiEvent()
               See method MPointMyHost::OnRxData() which looks for the pattern in the background
               and there is no pattern to search there
            */
            SetRxPattern(pModule,NULL,0,0);

            while( pModule->mpHost->IsPacketPresent(TPACKET_TYPE_HCI_LE_EVENT) )
            {   //already in buffer
                if( OnTiEvent(pModule,pScriptPC) )
                {
#if defined(SPDBG_TIEVENTS)
                    AnsiString asMsg = "";
                    asMsg.printf("?TrigNextA\n");
                    frmMain->DisplayUserMessage(asMsg.c_str());
#endif

                    ServiceNextOpcode();
                    /* need to return otherwise the code after this while block
                       that starts a timer will get called and is going to confuse
                       the behaviour of the next statement */
                    return;
                }
            }
            //packet has not yet arrived so we need to wait for a specific time
            int nTimeoutMs = pScriptPC->waitPacket.nTimeout;
            StartModuleTimer(pModule,nTimeoutMs,true);
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSetComms(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    asMsg.printf("Port %d: Set comms %s",pScriptPC->setComms.nPort,pScriptPC->setComms.pCommParm);
    frmMain->DisplayUserMessage(asMsg.c_str());
    StopAllModuleTimers();


    if( pScriptPC->setComms.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->setComms.nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            TBaseCommParms commParms;
            if(ParseCommsString(pScriptPC->setComms.pCommParm,&commParms))
            {
                pModule->mpHost->ChangeCommsParameters((UBaseOpenParms *)&commParms);
                TriggerNextAction(__LINE__);
            }
            else
            {
                asMsg.printf("Runtime: Failed to set new comms parameters on Port %d",pScriptPC->breakOn.nPortA);
                OnFail(asMsg.c_str());
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->setComms.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->setComms.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionTxString(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    if( pScriptPC->txString.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->txString.nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            pModule->mpHost->SendString(pScriptPC->txString.pTxStr);
            frmMain->DisplayCommand(pScriptPC->txString.nPort,pScriptPC->txString.pTxStr);
            /* July 2017 added the stop timer */
            StopModuleTimer(pModule);
            TriggerNextAction(__LINE__);
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->txString.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->txString.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionTxStringEx(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    if( pScriptPC->txString.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->txString.nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            int nCount;
            int nIndex;
            if(pScriptPC->txString.pRepeatCount)
            {
                nCount = atoi(pScriptPC->txString.pRepeatCount);
            }
            else
            {
                nCount = 0;
            }
            if(nCount)
            {
                for(nIndex=0;nIndex<nCount;nIndex++)
                {
                    pModule->mpHost->SendString(pScriptPC->txString.pTxStr);
                }
                asMsg.printf("%s (%d times)",pScriptPC->txString.pTxStr,nCount );
                frmMain->DisplayCommand(pScriptPC->txString.nPort,asMsg.c_str());
            }
            /* July 2017 added the stop timer */
            StopModuleTimer(pModule);
            TriggerNextAction(__LINE__);
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->txString.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->txString.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSendCmd(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    if( pScriptPC->txString.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->txString.nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            pModule->mpHost->FlushRxBuffer();
            pModule->mpHost->SendString(pScriptPC->txString.pTxStr);
            frmMain->DisplayCommand(pScriptPC->txString.nPort,pScriptPC->txString.pTxStr);
            TriggerNextAction(__LINE__);
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->txString.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->txString.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSendMpData(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    char baHeader[32];
    int nPort = pScriptPC->sendMpData.nPort;
    int nChannel = pScriptPC->sendMpData.nChannel;
    int txlen;

    if( pScriptPC->txString.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            pModule->mpHost->FlushRxBuffer();
            txlen = strlen(pScriptPC->sendMpData.pTxStr);
            pModule->mpHost->SendDataBlock(nChannel,pScriptPC->sendMpData.pTxStr,txlen );
            sprintf(baHeader,"data>%02X %02X ",((txlen/2)+2),nChannel);
            frmMain->DisplayCommand(nPort,baHeader,pScriptPC->sendMpData.pTxStr);
            TriggerNextAction(__LINE__);
        }
        else
        {
            asMsg.printf("Port %d is not open",nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionTxUnicode(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    if( pScriptPC->txString.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->txString.nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            unsigned char baBuf[2];
            baBuf[0]=0;
            char *pSrc = pScriptPC->txString.pTxStr;
            while(*pSrc)
            {
                baBuf[1]=*pSrc++;
                pModule->mpHost->SendBlock(baBuf,2);
            }
            frmMain->DisplayUnicodeCommand(pScriptPC->txString.nPort,pScriptPC->txString.pTxStr);
            TriggerNextAction(__LINE__);
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->txString.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->txString.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPortUart(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    bool fParseOK= false;
    int nComPort;
    bool fOpen;

    asMsg.printf("Associate Logical Port %d with %s with %s",pScriptPC->port.nPort,pScriptPC->port.pComPort,pScriptPC->port.pComParm);
    frmMain->DisplayUserMessage(asMsg.c_str());
    StopAllModuleTimers();

    if( pScriptPC->port.nPort < gnMaximumLogicalPorts)
    {
        if( strncmpi(pScriptPC->port.pComPort,"com",3)==0)
        {
            char *pSrc = &pScriptPC->port.pComPort[3];
            fParseOK= true;
            while(*pSrc)
            {
                if(!isdigit(*pSrc))
                {
                    fParseOK= false;
                    break;
                }
                pSrc++;
            }
        }

        nComPort = atoi( &pScriptPC->port.pComPort[3] );
        if(fParseOK && (nComPort>0) && (nComPort<=255))
        {
            TModule *pModule = &mpModule[pScriptPC->port.nPort];

            if(pModule->mpHost)
            {
                if(strcmpi(pModule->mpHost->GetHostType(),"TAPDCOMPORT") == 0)
                {
                    if(pModule->mnComPort > 0)
                    {
                        pModule->mpHost->Close();
                        Sleep(250);
                        pModule->mnComPort=-1;
                    }
                }
                else
                {
                    delete pModule->mpHost;
                    pModule->mpHost=CreateAtMyHost(this,pModule->mTimer->Tag);
                    pModule->mnComPort=-1;
                }
            }
            else
            {
                pModule->mpHost=CreateAtMyHost(this,pModule->mTimer->Tag);
                pModule->mnComPort=-1;
            }

            if(pModule->mpHost==NULL)
            {
                asMsg.printf("Host Creation Error");
                OnFail(asMsg.c_str(),-3, false);
                return;
            }

            TBaseCommParms commParms;
            if(ParseCommsString(pScriptPC->port.pComParm,&commParms))
            {
                pModule->mnComPort = nComPort;

                fOpen = pModule->mpHost->Open(  pModule->mnComPort, (UBaseOpenParms *)&commParms );
                if( ! fOpen )
                {
                    asMsg.printf("Serial port %s failed to open",pScriptPC->port.pComPort);
                    OnFail(asMsg.c_str(),-4,false);
                }
                else
                {
                    if(mfShowTrace)
                    {
                        AnsiString asCaption = pModule->mpHost->GetHostType();
                        asCaption += " ";
                        asCaption += pModule->mpHost->GetLogicalPort();
                        pModule->mpFrmTrace->Caption = asCaption;
                        pModule->mpFrmTrace->Top  = 0;
                        pModule->mpFrmTrace->Left = mTraceLeft;
                        mTraceLeft += 250;
                        pModule->mpFrmTrace->Show();
                    }
                    TriggerNextAction(__LINE__);
                }
            }
            else
            {
                asMsg.printf("Comms Parameter %s is invalid",pScriptPC->port.pComParm);
                OnFail(asMsg.c_str(),-5,false);
            }
        }
        else
        {
            asMsg.printf("%s is not a valid name for a serial port",pScriptPC->port.pComPort);
            OnFail(asMsg.c_str(),-6,false);
        }
    }
    else
    {
        asMsg.printf("Logical Port %d cannot be specified",pScriptPC->port.nPort);
        OnFail(asMsg.c_str(),-7,false);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPortClose(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    asMsg.printf("Close Logical Port %d",pScriptPC->port.nPort);
    frmMain->DisplayUserMessage(asMsg.c_str());
    StopAllModuleTimers();

    if( pScriptPC->port.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->port.nPort];

        if(pModule->mpHost)
        {
            if(strcmpi(pModule->mpHost->GetHostType(),"TAPDCOMPORT") == 0)
            {
                if(pModule->mnComPort > 0)
                {
                    pModule->mpHost->Close();
                    Sleep(250);
                    pModule->mnComPort=-1;
                }
            }
            else if(strcmpi(pModule->mpHost->GetHostType(),"PIPE") == 0)
            {
                pModule->mpHost->Close();
                Sleep(250);
            }
            else
            {
                asMsg.printf("Logical Port Host Type unexpected");
                OnFail(asMsg.c_str(),-8,false);
            }

            delete pModule->mpHost;
            pModule->mpHost=NULL;

            TriggerNextAction(__LINE__);

        }
    }
    else
    {
        asMsg.printf("Logical Port %d invalid",pScriptPC->port.nPort);
        OnFail(asMsg.c_str(),-9,false);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPortMPoint(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    bool fParseOK= false;
    int nComPort;
    bool fOpen;

    asMsg.printf("Associate Logical Port %d with %s with %s with MPOINT protocol",pScriptPC->port.nPort,pScriptPC->port.pComPort,pScriptPC->port.pComParm);
    frmMain->DisplayUserMessage(asMsg.c_str());
    StopAllModuleTimers();

    if( pScriptPC->port.nPort < gnMaximumLogicalPorts)
    {
        if( strncmpi(pScriptPC->port.pComPort,"com",3)==0)
        {
            char *pSrc = &pScriptPC->port.pComPort[3];
            fParseOK= true;
            while(*pSrc)
            {
                if(!isdigit(*pSrc))
                {
                    fParseOK= false;
                    break;
                }
                pSrc++;
            }
        }

        nComPort = atoi( &pScriptPC->port.pComPort[3] );
        if(fParseOK && (nComPort>0) && (nComPort<=255))
        {
            TModule *pModule = &mpModule[pScriptPC->port.nPort];

            if(pModule->mpHost)
            {
                if(strcmpi(pModule->mpHost->GetHostType(),"TAPDCOMPORT") == 0)
                {
                    if(pModule->mnComPort > 0)
                    {
                        pModule->mpHost->Close();
                        Sleep(250);
                        pModule->mnComPort=-1;
                    }
                }
                else
                {
                    delete pModule->mpHost;
                    pModule->mpHost=CreateMPointMyHost(this,pModule->mTimer->Tag);
                    pModule->mnComPort=-1;
                }
            }
            else
            {
                pModule->mpHost=CreateMPointMyHost(this,pModule->mTimer->Tag);
                pModule->mnComPort=-1;
            }

            if(pModule->mpHost==NULL)
            {
                asMsg.printf("Host Creation Error");
                OnFail(asMsg.c_str(),-13,false);
                return;
            }

            TBaseCommParms commParms;
            if(ParseCommsString(pScriptPC->port.pComParm,&commParms))
            {
                pModule->mnComPort = nComPort;

                fOpen = pModule->mpHost->Open(  pModule->mnComPort, (UBaseOpenParms *)&commParms);
                if( ! fOpen )
                {
                    asMsg.printf("Serial port %s failed to open",pScriptPC->port.pComPort);
                    OnFail(asMsg.c_str(),-14,false);
                }
                else
                {
                    if(mfShowTrace)
                    {
                        AnsiString asCaption = pModule->mpHost->GetHostType();
                        asCaption += " ";
                        asCaption += pModule->mpHost->GetLogicalPort();
                        pModule->mpFrmTrace->Caption = asCaption;
                        pModule->mpFrmTrace->Top  = 0;
                        pModule->mpFrmTrace->Left = mTraceLeft;
                        mTraceLeft += 250;
                        pModule->mpFrmTrace->Show();
                    }
                    TriggerNextAction(__LINE__);
                }
            }
            else
            {
                asMsg.printf("Comms Parameter %s is invalid",pScriptPC->port.pComParm);
                OnFail(asMsg.c_str(),-15,false);
            }
        }
        else
        {
            asMsg.printf("%s is not a valid name for a serial port",pScriptPC->port.pComPort);
            OnFail(asMsg.c_str(),-16,false);
        }
    }
    else
    {
        asMsg.printf("Logical Port %d cannot be specified",pScriptPC->port.nPort);
        OnFail(asMsg.c_str(),-17,false);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPortOC(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    bool fParseOK= false;
    int nComPort;
    bool fOpen;

    asMsg.printf("Associate Logical Port %d with %s with %s with OC protocol",pScriptPC->port.nPort,pScriptPC->port.pComPort,pScriptPC->port.pComParm);
    frmMain->DisplayUserMessage(asMsg.c_str());
    StopAllModuleTimers();

    if( pScriptPC->port.nPort < gnMaximumLogicalPorts)
    {
        if( strncmpi(pScriptPC->port.pComPort,"com",3)==0)
        {
            char *pSrc = &pScriptPC->port.pComPort[3];
            fParseOK= true;
            while(*pSrc)
            {
                if(!isdigit(*pSrc))
                {
                    fParseOK= false;
                    break;
                }
                pSrc++;
            }
        }

        nComPort = atoi( &pScriptPC->port.pComPort[3] );
        if(fParseOK && (nComPort>0) && (nComPort<=255))
        {
            TModule *pModule = &mpModule[pScriptPC->port.nPort];

            if(pModule->mpHost)
            {
                if(strcmpi(pModule->mpHost->GetHostType(),"TAPDCOMPORT") == 0)
                {
                    if(pModule->mnComPort > 0)
                    {
                        pModule->mpHost->Close();
                        Sleep(250);
                        pModule->mnComPort=-1;
                    }
                }
                else
                {
                    delete pModule->mpHost;
                    pModule->mpHost=CreateOcHost(this,pModule->mTimer->Tag);
                    pModule->mnComPort=-1;
                }
            }
            else
            {
                pModule->mpHost=CreateOcHost(this,pModule->mTimer->Tag);
                pModule->mnComPort=-1;
            }

            if(pModule->mpHost==NULL)
            {
                asMsg.printf("Host Creation Error");
                OnFail(asMsg.c_str(),-43,false);
                return;
            }

            TBaseCommParms commParms;
            if(ParseCommsString(pScriptPC->port.pComParm,&commParms))
            {
                pModule->mnComPort = nComPort;

                fOpen = pModule->mpHost->Open(  pModule->mnComPort, (UBaseOpenParms *)&commParms);
                if( ! fOpen )
                {
                    asMsg.printf("Serial port %s failed to open",pScriptPC->port.pComPort);
                    OnFail(asMsg.c_str(),-44,false);
                }
                else
                {
                    if(mfShowTrace)
                    {
                        AnsiString asCaption = pModule->mpHost->GetHostType();
                        asCaption += " ";
                        asCaption += pModule->mpHost->GetLogicalPort();
                        pModule->mpFrmTrace->Caption = asCaption;
                        pModule->mpFrmTrace->Top  = 0;
                        pModule->mpFrmTrace->Left = mTraceLeft;
                        mTraceLeft += 250;
                        pModule->mpFrmTrace->Show();
                    }
                    TriggerNextAction(__LINE__);
                }
            }
            else
            {
                asMsg.printf("Comms Parameter %s is invalid",pScriptPC->port.pComParm);
                OnFail(asMsg.c_str(),-45,false);
            }
        }
        else
        {
            asMsg.printf("%s is not a valid name for a serial port",pScriptPC->port.pComPort);
            OnFail(asMsg.c_str(),-46,false);
        }
    }
    else
    {
        asMsg.printf("Logical Port %d cannot be specified",pScriptPC->port.nPort);
        OnFail(asMsg.c_str(),-47,false);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionMessageBox(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    int eButtons;
    int nUserSelect;

    StopAllModuleTimers();

    switch( pScriptPC->messageBox.nButtons)
    {
    case 0: //YES, NO
        eButtons = MB_YESNO;
        break;
    case 1: //YES, NO, CANCEL
        eButtons = MB_YESNOCANCEL;
        break;
    case 2: //OK
        eButtons = MB_OK;
        break;
    case 3: //OK, CANCEL
        eButtons = MB_OKCANCEL;
        break;
    case 4: //RETRY, CANCEL
        eButtons = MB_RETRYCANCEL;
        break;
    case 5: //ABORT,RETRY,IGNORE
    default:
        eButtons = MB_ABORTRETRYIGNORE;
        break;
    }

    Application->NormalizeTopMosts();
    nUserSelect=Application->MessageBox(pScriptPC->messageBox.pText, pScriptPC->messageBox.pCaption, eButtons);
    Application->RestoreTopMosts();

    char baVal[16];
    sprintf(baVal,"%d", nUserSelect );
    SrcSaveVariableValue(mpScrManager,"SysVarMsgBoxBtnVal", baVal);
    switch(nUserSelect)
    {
    case 1:  strcpy(baVal,"ok");break;
    case 2:  strcpy(baVal,"cancel");break;
    case 3:  strcpy(baVal,"abort");break;
    case 4:  strcpy(baVal,"retry");break;
    case 5:  strcpy(baVal,"ignore");break;
    case 6:  strcpy(baVal,"yes");break;
    case 7:  strcpy(baVal,"no");break;
    default: strcpy(baVal,"unknown");break;
    }
    SrcSaveVariableValue(mpScrManager,"SysVarMsgBoxBtnName", baVal);
    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPortPipe(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    bool fOpen;
    PipeConsoleHost *pPipeHost;

    asMsg.printf("Associate Logical Port %d with %s %s",
                                         pScriptPC->port_pipe.nPort,
                                                 pScriptPC->port_pipe.pAppPathNameOpts,
                                                     pScriptPC->port_pipe.pAppCurrentFolder);
    frmMain->DisplayUserMessage(asMsg.c_str());
    StopAllModuleTimers();

    if( pScriptPC->port_pipe.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->port_pipe.nPort];

        if(pModule->mpHost)
        {
            delete pModule->mpHost;
            pModule->mpHost=NULL;
        }
        pPipeHost=CreatePipeConsoleHost(this,
                                        pModule->mTimer->Tag,
                                        pScriptPC->port_pipe.pAppPathNameOpts,
                                        pScriptPC->port_pipe.pAppCurrentFolder);
        pModule->mpHost=pPipeHost;
        pModule->mnComPort=-1;

        if(pModule->mpHost==NULL)
        {
            asMsg.printf("Host Creation Error:: %s %s",
                                                pScriptPC->port_pipe.pAppPathNameOpts,
                                                   pScriptPC->port_pipe.pAppCurrentFolder);
            OnFail(asMsg.c_str(),-33,false);
            return;
        }

        TBaseCommParms commParms={0,0,0,0,true,true,0,0};
        fOpen = pModule->mpHost->Open(0,(UBaseOpenParms *)&commParms);

        if( ! fOpen )
        {
            asMsg.printf("Pipe failed to open:: %s %s",
                                                pScriptPC->port_pipe.pAppPathNameOpts,
                                                    pScriptPC->port_pipe.pAppCurrentFolder);
            OnFail(asMsg.c_str(),-34,false);
        }
        else
        {
            if(mfShowTrace)
            {
                AnsiString asCaption = pModule->mpHost->GetHostType();
                asCaption += " ";
                asCaption += pModule->mpHost->GetLogicalPort();
                pModule->mpFrmTrace->Caption = asCaption;
                pModule->mpFrmTrace->Top  = 0;
                pModule->mpFrmTrace->Left = mTraceLeft;
                mTraceLeft += 250;
                pModule->mpFrmTrace->Show();
            }
            TriggerNextAction(__LINE__);
        }
    }
    else
    {
        asMsg.printf("Logical Port %d cannot be specified",pScriptPC->port_pipe.nPort);
        OnFail(asMsg.c_str(),-37,false);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionPrint(TScriptAction *pScriptPC,bool fEscaped)
{
    frmMain->DisplayPrintMessage(pScriptPC->print.pPrnMsg,fEscaped);
    TriggerNextAction(__LINE__);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TRegression::LaunchExecuteApp(char *pCommandLine, int nFlags)
{
    return LaunchExecuteAppEx(pCommandLine, 0, false, (nFlags & 0x00000002), (nFlags & 0x00000001), 0);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TRegression::LaunchExecuteApp(char *pCommandLine, char *pFolder, bool fWait, bool fHide, bool fMinimize)
{
    return LaunchExecuteAppEx(pCommandLine, pFolder, fWait, fHide, fMinimize, 0);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TRegression::LaunchExecuteAppEx(char *pCommandLine, char *pFolder, bool fWait, bool fHide, bool fMinimize, DWORD *dwRetCode)
{
    bool fRetVal = false;
    STARTUPINFO si;
    PROCESS_INFORMATION  sAppPi;
    DWORD dwExitCode;

    if( pCommandLine)
    {
        memset(&si,0,sizeof(si));
        si.cb = sizeof(si);
        //Deal with hidden/minimised options
        if(fHide)
        {
            /*
                    #define SW_HIDE             0
                    #define SW_SHOWNORMAL       1
                    #define SW_NORMAL           1
                    #define SW_SHOWMINIMIZED    2
                    #define SW_SHOWMAXIMIZED    3
                    #define SW_MAXIMIZE         3
                    #define SW_SHOWNOACTIVATE   4
                    #define SW_SHOW             5
                    #define SW_MINIMIZE         6
                    #define SW_SHOWMINNOACTIVE  7
                    #define SW_SHOWNA           8
                    #define SW_RESTORE          9
                    #define SW_SHOWDEFAULT      10
            */
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE;
        }
        else if (fMinimize)
        {
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_MINIMIZE;
        }
        memset(&sAppPi,0,sizeof(sAppPi));

        //Run process
        if( CreateProcess(  NULL,
                            pCommandLine,
                            NULL,NULL,false,0,NULL,NULL,
                            &si,&sAppPi ))
        {
            fRetVal = true;
            if( fWait )
            {
                //Wait for application to exit
                GetExitCodeProcess(sAppPi.hProcess, &dwExitCode);
                while(dwExitCode == STILL_ACTIVE)
                {
                    Application->ProcessMessages();
                    Sleep(10);
                    GetExitCodeProcess(sAppPi.hProcess, &dwExitCode);
                }
            }
        }
    }

    if (dwRetCode)
    {
        //Return the exit code
        *dwRetCode = dwExitCode;
    }
    return fRetVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionExecute(TScriptAction *pScriptPC)
{
    bool fLaunched = LaunchExecuteApp(pScriptPC->execute.pCmdLine,0);
    if(fLaunched)
    {
        TriggerNextAction(__LINE__);
    }
    else
    {
        AnsiString asFile=pScriptPC->existFile.pFilename;
        AnsiString asMsg="Application Failed to Launch <";
        asMsg += asFile;
        asMsg += ">";
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionExecuteEx(TScriptAction *pScriptPC)
{
    bool fLaunched = LaunchExecuteApp(pScriptPC->executeEx.pCmdLine,pScriptPC->executeEx.nFlags);
    if(fLaunched)
    {
        TriggerNextAction(__LINE__);
    }
    else
    {
        AnsiString asFile=pScriptPC->existFile.pFilename;
        AnsiString asMsg="Application Failed to Launch <";
        asMsg += asFile;
        asMsg += ">";
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionExecuteWait(TScriptAction *pScriptPC)
{
    bool fLaunched = LaunchExecuteApp(pScriptPC->execute.pCmdLine,".\\",true,false,false);
    if(fLaunched)
    {
        TriggerNextAction(__LINE__);
    }
    else
    {
        AnsiString asFile=pScriptPC->existFile.pFilename;
        AnsiString asMsg="Application Failed to Launch <";
        asMsg += asFile;
        asMsg += ">";
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionExecuteWaitRet(TScriptAction *pScriptPC)
{
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->executeWaitRet.pParmB);
    DWORD dwRetCode;
    bool fLaunched = LaunchExecuteAppEx(pScriptPC->execute.pCmdLine,".\\",true,false,false, &dwRetCode);
    if(fLaunched)
    {
        SrcSaveVariableValueInteger(pVariable, dwRetCode);
        TriggerNextAction(__LINE__);
    }
    else
    {
        AnsiString asFile=pScriptPC->existFile.pFilename;
        AnsiString asMsg="Application Failed to Launch <";
        asMsg += asFile;
        asMsg += ">";
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionExecuteWaitEx(TScriptAction *pScriptPC)
{
    TScriptVariable *pVariable;
    bool fLaunched;
    DWORD dwRetCode;

    if(pScriptPC->execute.pOptVarName)
    {
        pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->execute.pOptVarName);
        fLaunched = LaunchExecuteAppEx(pScriptPC->execute.pCmdLine,
                                       ".\\",
                                       true,
                                       (pScriptPC->executeEx.nFlags & 0x00000001),
                                       (pScriptPC->executeEx.nFlags & 0x00000002),
                                       &dwRetCode);
    }
    else
    {
        pVariable = NULL;
        fLaunched = LaunchExecuteApp(pScriptPC->execute.pCmdLine,
                                     ".\\",
                                     true,
                                     (pScriptPC->executeEx.nFlags & 0x00000001),
                                     (pScriptPC->executeEx.nFlags & 0x00000002));
    }
    if(fLaunched)
    {
        if(pVariable)
        {
            SrcSaveVariableValueInteger(pVariable, dwRetCode);
        }
        TriggerNextAction(__LINE__);
    }
    else
    {
        AnsiString asFile=pScriptPC->execute.pCmdLine;
        AnsiString asMsg="Application Failed to Launch <";
        asMsg += asFile;
        asMsg += ">";
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int TRegression::ScaleTimeout(int nIntVal)
{
    if( mTimeScalarPerc != 100 )
    {
        long long nInt = nIntVal * mTimeScalarPerc;
        nInt /= 100;
        if(nInt > 0x7FFFFFFF )
        {
            nInt = 0x7FFFFFFF;
        }
        else if(nInt < 1 )
        {
            //do not allow timeout to be 0
            nInt = 1;
        }
        nIntVal = (int)nInt;
    }
    return nIntVal;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionDelay(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    int nIntVal = atoi(pScriptPC->delay.pDelay);

    asMsg.sprintf("Delay %dms <%s>",ScaleTimeout(nIntVal),pScriptPC->delay.pDelay);
    frmMain->DisplayPrintMessage(asMsg.c_str(),false);

    if(nIntVal>0)
    {
        StartDelay(nIntVal, true);
    }
    else
    {
        TriggerNextAction(__LINE__);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionDelayAbs(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    int nIntVal = atoi(pScriptPC->delay.pDelay);

    asMsg.sprintf("Delay %dms <%s>",nIntVal,pScriptPC->delay.pDelay);
    frmMain->DisplayPrintMessage(asMsg.c_str(),false);

    if(nIntVal>0)
    {
        StartDelay(nIntVal,false);
    }
    else
    {
        TriggerNextAction(__LINE__);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionDefTimeout(TScriptAction *pScriptPC)
{
    int nIntVal = atoi(pScriptPC->defTimeout.pTimeout);
    if(nIntVal>0)
    {
        mDefaultTimeoutMs = nIntVal;
        TriggerNextAction(__LINE__);
    }
    else
    {
        mDefaultTimeoutMs = DEFAULT_TIMEOUT_MS;
        OnFail("Zero Default Timeout is not allowed");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionTimeScalar(TScriptAction *pScriptPC)
{
    int nIntVal = atoi(pScriptPC->timeScalar.pPerc);
    if(nIntVal>0)
    {
        mTimeScalarPerc = nIntVal;
        TriggerNextAction(__LINE__);
    }
    else
    {
        mTimeScalarPerc = DEFAULT_TIMESCALAR_PERC;
        OnFail("Zero or negative Time Scalar is not allowed");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionTitle(TScriptAction *pScriptPC)
{
    /* remember the location of this title just in case we need to */
    /* retry on the event of a failure */
    mpScrManager->mpCurrentTitle = mpScrManager->mpPC;
    /* and also take a snapshot of the top of the include filename stack */
    mpScrManager->mnIncFilenameStackTopAtTitle = mnIncFilenameStackTop;

    frmMain->DisplayPrintMessage(pScriptPC->title.pTestTitle,false);
    if(mpCurrentTestTitle)
    {
        MiscMemFree(mpCurrentTestTitle);
        mpCurrentTestTitle=NULL;
    }
    mpCurrentTestTitle = (char *)MiscMemAlloc(strlen(pScriptPC->title.pTestTitle)+1);
    if(mpCurrentTestTitle)
    {
        strcpy(mpCurrentTestTitle,pScriptPC->title.pTestTitle);
        frmMain->StatusBarTestNumId(mpCurrentTestTitle);
    }

    /* reset the fail count for this test */
    mpScrManager->mTitleFailCount = 0;

    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionSubTitle(TScriptAction *pScriptPC)
{
    frmMain->DisplayPrintMessage(pScriptPC->title.pTestTitle,false);
    if(mpCurrentTestTitle)
    {
        MiscMemFree(mpCurrentTestTitle);
        mpCurrentTestTitle=NULL;
    }
    mpCurrentTestTitle = (char *)MiscMemAlloc(strlen(pScriptPC->title.pTestTitle)+1);
    if(mpCurrentTestTitle)
    {
        strcpy(mpCurrentTestTitle,pScriptPC->title.pTestTitle);
    }
    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFail(TScriptAction *pScriptPC)
{
    OnFail(pScriptPC->fail.pFailMsg);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionModemStat(TScriptAction *pScriptPC)
{
    char baStat[8];
    AnsiString asMsg;

    if( pScriptPC->modemStat.nPort < gnMaximumLogicalPorts)
    {
        StopAllModuleTimers();
        TModule *pModule = &mpModule[pScriptPC->modemStat.nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            strcpy(baStat,"OFF");
            switch(pScriptPC->modemStat.nType)
            {
            case MDMSTAT_CTS:
                if(pModule->mpHost->GetCTS())strcpy(baStat,"ON");
                break;
            case MDMSTAT_DSR:
                if(pModule->mpHost->GetDSR())strcpy(baStat,"ON");
                break;
            case MDMSTAT_DCD:
                if(pModule->mpHost->GetDCD())strcpy(baStat,"ON");
                break;
            case MDMSTAT_RI:
                if(pModule->mpHost->GetRI())strcpy(baStat,"ON");
                break;
            default:
                strcpy(baStat,"ERR");
                break;
            }

            int nCurValue = (strcmpi(baStat,pScriptPC->modemStat.pExpectedState)==0) ? 1 : 0 ;
            UpdateConditionFlags(nCurValue);
            TriggerNextAction(__LINE__);
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->modemStat.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->modemStat.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionModemSet(TScriptAction *pScriptPC)
{
    AnsiString asMsg;
    bool fNewState;
    bool fCanAction=false;

    if( pScriptPC->modemSet.nPort < gnMaximumLogicalPorts)
    {
        StopAllModuleTimers();
        TModule *pModule = &mpModule[pScriptPC->modemSet.nPort];
        if(pModule->mpHost && pModule->mpHost->IsOpen())
        {
            fNewState = false;
            if( strcmpi(pScriptPC->modemSet.pNewState,"ON")==0 )
            {
                fNewState = true;
                fCanAction = true;
            }
            else if( strcmpi(pScriptPC->modemSet.pNewState,"OFF")==0 )
            {
                fNewState = false;
                fCanAction = true;
            }
            if(fCanAction)
            {
                switch(pScriptPC->modemStat.nType)
                {
                case MDMLINE_DTR:
                    pModule->mpHost->SetDTR(fNewState);
                    TriggerNextAction(__LINE__);
                    break;
                case MDMLINE_RTS:
                    pModule->mpHost->SetRTS(fNewState);
                    TriggerNextAction(__LINE__);
                    break;
                default:
                    OnFail("Illegal Modem action");
                    break;
                }
            }
            else
            {
                asMsg.printf("Port %d: Requested illegal Modem state = %s",pScriptPC->modemSet.nPort,pScriptPC->modemSet.pNewState);
                OnFail(asMsg.c_str());
            }
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->modemSet.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->modemSet.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFlushRx(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    asMsg.printf("Port %d: Flush RX",pScriptPC->flushRx.nPort);
    frmMain->DisplayUserMessage(asMsg.c_str());

    if( pScriptPC->flushRx.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->flushRx.nPort];

        if(pModule->mpHost && pModule->mpHost->IsOpen() )
        {
            pModule->mpHost->FlushRxBuffer();
            TriggerNextAction(__LINE__);
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->flushRx.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->flushRx.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionFlushTx(TScriptAction *pScriptPC)
{
    AnsiString asMsg;

    asMsg.printf("Port %d: Flush TX",pScriptPC->flushTx.nPort);
    frmMain->DisplayUserMessage(asMsg.c_str());

    if( pScriptPC->flushTx.nPort < gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[pScriptPC->flushTx.nPort];

        if(pModule->mpHost && pModule->mpHost->IsOpen() )
        {
            pModule->mpHost->FlushTxBuffer();
            TriggerNextAction(__LINE__);
        }
        else
        {
            asMsg.printf("Port %d is not open",pScriptPC->flushTx.nPort);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        asMsg.printf("Port %d is not specified",pScriptPC->flushTx.nPort);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionLabel(TScriptAction *pScriptPC)
{
    TriggerNextAction(__LINE__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionJump(TScriptAction *pScriptPC)
{
    switch(pScriptPC->jump.eJmpCond)
    {
    case JMP_ALWAYS:
        mpScrManager->mpPC = (TScriptAction *)&mpScrManager->mpCode[pScriptPC->jump.nAbsIndex];
        TriggerNextAction(__LINE__);
        break;
    case JMP_Z:
        if( mConditionFlags.z )
        {
            mpScrManager->mpPC = (TScriptAction *)&mpScrManager->mpCode[pScriptPC->jump.nAbsIndex];
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_NZ:
        if( ! mConditionFlags.z )
        {
            mpScrManager->mpPC = (TScriptAction *)&mpScrManager->mpCode[pScriptPC->jump.nAbsIndex];
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_LT:
        if( mConditionFlags.lt )
        {
            mpScrManager->mpPC = (TScriptAction *)&mpScrManager->mpCode[pScriptPC->jump.nAbsIndex];
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_LE:
        if( mConditionFlags.le )
        {
            mpScrManager->mpPC = (TScriptAction *)&mpScrManager->mpCode[pScriptPC->jump.nAbsIndex];
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_GT:
        if( mConditionFlags.gt )
        {
            mpScrManager->mpPC = (TScriptAction *)&mpScrManager->mpCode[pScriptPC->jump.nAbsIndex];
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_GE:
        if( mConditionFlags.ge )
        {
            mpScrManager->mpPC = (TScriptAction *)&mpScrManager->mpCode[pScriptPC->jump.nAbsIndex];
        }
        TriggerNextAction(__LINE__);
        break;
    default:
        OnFail("Runtime: Illegal Jump Condition Type");
        break;
    }
}

void TRegression::DoSkip()
{
    //Skips to next test.
    //Does not guard against end of applications
    AnsiString tmpstr;
    tmpstr = " *** Skipped test (";
    tmpstr += mpCurrentTestTitle;
    tmpstr += ") *** ";
    frmMain->DisplayPrintMessage(tmpstr.c_str(), false);
    frmMain->LogToPlainFailFile(tmpstr);
    AdvanceToNextTest();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionIf(TScriptAction *pScriptPC)
{
    switch(pScriptPC->ifcond.eJmpCond)
    {
    case JMP_Z:
        if( ! mConditionFlags.z )
        {
            mpScrManager->mpPC++;
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_NZ:
        if( mConditionFlags.z )
        {
            mpScrManager->mpPC++;
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_LT:
        if( ! mConditionFlags.lt )
        {
            mpScrManager->mpPC++;
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_LE:
        if( ! mConditionFlags.le )
        {
            mpScrManager->mpPC++;
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_GT:
        if( ! mConditionFlags.gt )
        {
            mpScrManager->mpPC++;
        }
        TriggerNextAction(__LINE__);
        break;
    case JMP_GE:
        if( ! mConditionFlags.ge )
        {
            mpScrManager->mpPC++;
        }
        TriggerNextAction(__LINE__);
        break;
    default:
        OnFail("Runtime: Illegal IF Condition Type");
        break;
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionEnd(TScriptAction *pScriptPC)
{
    StopAllModuleTimers();
    OnScriptEnd();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::RunActionExit(TScriptAction *pScriptPC)
{
    StopAllModuleTimers();
    OnScriptExit();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceNextOpcode(void)
{
    AnsiString asMsg;
    bool fOkA;
    bool fOkB;
    TScriptAction *pScriptPC;
    int nErrorOnVarMissing;

    GetVariableValueAsInteger("SysVarFailOnVarMissing", &nErrorOnVarMissing, 0);

    fOkA = nErrorOnVarMissing ? true : false ; /* variable substitution error will abort script */
    fOkB = nErrorOnVarMissing ? true : false ; /* variable substitution error will abort script */
    pScriptPC = SrcCopyNextScriptAction(mpScrManager,fOkA,fOkB);

    //cache the current script line number
    mnCurrentActionLineNum =  pScriptPC->generic.nLineNum;

#if defined(SPDBG_NEXTOPCODE)
    asMsg.printf("?Mv2Nxt (%i) (%i)\n", fOkA, fOkB);
    frmMain->DisplayUserMessage(asMsg.c_str());
#endif

    if(fOkA)
    {
        fOkA = SrcUnescapeString(pScriptPC->generic.pParmA);
        if(fOkA)
        {
            if(fOkB)
            {
                fOkB = SrcUnescapeString(pScriptPC->generic.pParmB);
                if(fOkB)
                {
                    frmMain->StatusBarProgramCounter(mpScrManager->mpPC - mpScrManager->mpCode);

                    switch(pScriptPC->generic.eAction)
                    {
                    case ACTION_SEND:       RunActionSend(pScriptPC); break;
                    case ACTION_BRKON:      RunActionBreakOn(pScriptPC); break;
                    case ACTION_BRKOFF:     RunActionBreakOff(pScriptPC); break;
                    case ACTION_SETVAR:     RunActionSetVar(pScriptPC);break;
                    case ACTION_SETVARX:    RunActionSetVar(pScriptPC);break;
                    case ACTION_MULTIINDEX: RunActionMultiIndex(pScriptPC);break;
                    case ACTION_SAVERESP:   RunActionSaveResp(pScriptPC);break;
                    case ACTION_PORTINFO:   RunActionPortInfo(pScriptPC);break;
                    case ACTION_FILTERMOVE: RunActionFilterMove(pScriptPC);break;
                    case ACTION_STRDEHEXIZE:   RunActionStrDeHexize(pScriptPC);break;
                    case ACTION_STRING:     RunActionString(pScriptPC);break;
                    case ACTION_STRCAT:     RunActionStrCat(pScriptPC);break;
                    case ACTION_GETCWD:     RunActionGetCwd(pScriptPC);break;
                    case ACTION_WAITRESP:   RunActionWaitResp(pScriptPC);break;
                    case ACTION_WAITRESPEX: RunActionWaitRespEx(pScriptPC);break;
                    case ACTION_WAITTIMEOUT: RunActionWaitTimeout(pScriptPC);break;
                    case ACTION_WAITUNTIL:  RunActionWaitUntil(pScriptPC);break;
                    case ACTION_WAITMULTI:  RunActionWaitMulti(pScriptPC);break;
                    case ACTION_WAITDATA:   RunActionWaitData(pScriptPC);break;
                    case ACTION_WAITCHANNEL:RunActionWaitChannel(pScriptPC);break;
                    case ACTION_WAITPACKET: RunActionWaitPacket(pScriptPC);break;
                    case ACTION_WAIT_TIEVENT: RunActionWaitTiEvent(pScriptPC);break;
                    case ACTION_PKTCOUNTGET:RunActionPktCountGet(pScriptPC);break;
                    case ACTION_PKTCOUNTSET:RunActionPktCountSet(pScriptPC);break;
                    case ACTION_CONST:      RunActionConst(pScriptPC);break;
                    case ACTION_CHECKVAR:   RunActionCheckVar(pScriptPC);break;
                    case ACTION_CONTAIN:    RunActionContain(pScriptPC,true,true);break;
                    case ACTION_NOTCONTAIN: RunActionContain(pScriptPC,false,true);break;
                    case ACTION_MATCHCOUNT: RunActionContain(pScriptPC,true,false);break;
                    case ACTION_SETCOMMS:   RunActionSetComms(pScriptPC);break;
                    case ACTION_SENDCMD:    RunActionSendCmd(pScriptPC);break;
                    case ACTION_SENDMPDATA: RunActionSendMpData(pScriptPC);break;
                    case ACTION_TXSTRING:   RunActionTxString(pScriptPC);break;
                    case ACTION_TXSTRINGEX: RunActionTxStringEx(pScriptPC);break;
                    case ACTION_TXUNICODE:  RunActionTxUnicode(pScriptPC);break;

                    case ACTION_PORT_UART:  RunActionPortUart(pScriptPC);break;
                    case ACTION_PORT_OC:    RunActionPortOC(pScriptPC);break;
                    case ACTION_PORT_MPOINT:RunActionPortMPoint(pScriptPC);break;
                    case ACTION_PORT_PIPE:  RunActionPortPipe(pScriptPC);break;
                    case ACTION_PORT_CLOSE: RunActionPortClose(pScriptPC);break;

                    case ACTION_MESSAGEBOX: RunActionMessageBox(pScriptPC);break;
                    case ACTION_TIMER:      RunActionTimer(pScriptPC);break;
                    case ACTION_EXECUTE:    RunActionExecute(pScriptPC);break;
                    case ACTION_EXECUTEEX:    RunActionExecuteEx(pScriptPC);break;
                    case ACTION_EXECUTEWAIT:RunActionExecuteWait(pScriptPC);break;
                    case ACTION_EXECUTEWAITEX:RunActionExecuteWaitEx(pScriptPC);break;
                    case ACTION_EXECUTEWAITRET:RunActionExecuteWaitRet(pScriptPC);break;
                    case ACTION_PRINT:      RunActionPrint(pScriptPC,false);break;
                    case ACTION_PRINTE:     RunActionPrint(pScriptPC,true);break;
                    case ACTION_EXIST_FILE: RunActionExistFile(pScriptPC);break;
                    case ACTION_EXIST_FOLDER: RunActionExistFolder(pScriptPC);break;
                    case ACTION_FILE_COMPARE: RunActionFileCompare(pScriptPC);break;
                    case ACTION_FILE_OPEN:  RunActionFileOpen(pScriptPC);break;
                    case ACTION_FILE_CLOSE: RunActionFileClose(pScriptPC);break;
                    case ACTION_FILE_READ:  RunActionFileRead(pScriptPC);break;
                    case ACTION_FILE_WRITE: RunActionFileWrite(pScriptPC);break;
                    case ACTION_DELAY:      RunActionDelay(pScriptPC);break;
                    case ACTION_DELAYABS:   RunActionDelayAbs(pScriptPC);break;
                    case ACTION_DEFTIMEOUT: RunActionDefTimeout(pScriptPC);break;
                    case ACTION_TIMESCALAR: RunActionTimeScalar(pScriptPC);break;
                    case ACTION_TITLE:      RunActionTitle(pScriptPC);break;
                    case ACTION_SUBTITLE:   RunActionSubTitle(pScriptPC);break;
                    case ACTION_FAIL :      RunActionFail(pScriptPC);break;
                    case ACTION_MDMSTAT:    RunActionModemStat(pScriptPC);break;
                    case ACTION_MDMSET:     RunActionModemSet(pScriptPC);break;
                    case ACTION_FLUSHRX:    RunActionFlushRx(pScriptPC); break;
                    case ACTION_FLUSHTX:    RunActionFlushTx(pScriptPC); break;
                    case ACTION_LABEL:      RunActionLabel(pScriptPC); break;
                    case ACTION_ALU:        RunActionDecArithmetic(pScriptPC);break;
                    case ACTION_HEXALU:     RunActionHexArithmetic(pScriptPC);break;
                    case ACTION_JUMP:       RunActionJump(pScriptPC);break;
                    case ACTION_IF:         RunActionIf(pScriptPC);break;
                    case ACTION_CONSOLEUPDATE:  RunActionConsoleUpdate(pScriptPC);break;
                    case ACTION_TRACE:      RunActionTrace(pScriptPC);break;
                    case ACTION_TRACEWIN:   RunActionTraceWin(pScriptPC);break;
                    case ACTION_END:        RunActionEnd(pScriptPC);break;
                    case ACTION_EXIT:       RunActionExit(pScriptPC);break;
                    case ACTION_SPRINTINT:  RunActionSprintInt(pScriptPC);break;
                    case ACTION_SPRINTHEX:  RunActionSprintHex(pScriptPC);break;
                    case ACTION_HEX2DEC:    RunActionHex2Dec(pScriptPC);break;
                    case ACTION_DEC2HEX:    RunActionDec2Hex(pScriptPC);break;
                    case ACTION_MID:        RunActionMid(pScriptPC);break;
                    case ACTION_INRANGE:    RunActionInRange(pScriptPC); break;
                    case ACTION_INCLUDE_PUSH:  RunActionIncludePush(pScriptPC); break;
                    case ACTION_INCLUDE_POP:   RunActionIncludePop(pScriptPC); break;
                    default:                RunActionUnknown(pScriptPC);break;
                    }
                }
                else
                {
                    asMsg.printf("Runtime Unescape Err: %s",pScriptPC->generic.pParmB);
                    OnFail(asMsg.c_str());
                }
            }
            else
            {
                //asMsg.printf("Runtime Var Subst Err: %s",pScriptPC->generic.pParmB);
                asMsg.printf("Runtime variable missing: %s",mpScrManager->mMissingVarnameB);
                OnFail(asMsg.c_str());
            }
        }
        else
        {
            asMsg.printf("Runtime Unescape Err: %s",pScriptPC->generic.pParmA);
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        //asMsg.printf("Runtime Var Subst Err: %s",pScriptPC->generic.pParmA);
        asMsg.printf("Runtime variable missing: %s",mpScrManager->mMissingVarnameA);
        OnFail(asMsg.c_str());
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::OnUnexpectedResponse(char *pResp, char *pExpectedResp)
{
    frmMain->DisplayUserMessage("FAIL: Unexpected response");
    frmMain->DisplayUserMessage("GOT");
    frmMain->DisplayUserMessage("----------------------------------");
    frmMain->DisplayUserMessage(pResp);
    frmMain->DisplayUserMessage("----------------------------------");
    frmMain->DisplayUserMessage("EXPECTED");
    frmMain->DisplayUserMessage("----------------------------------");
    frmMain->DisplayUserMessage(pExpectedResp);
    frmMain->DisplayUserMessage("----------------------------------");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void  TRegression::OnModuleTimerExpire(int nModuleId)
{
    if( !((mpScrManager->mCurrentAction.generic.eAction == ACTION_DELAY)
          ||
          (mpScrManager->mCurrentAction.generic.eAction == ACTION_DELAYABS)) )
    {
        if( (nModuleId >= 0) && (nModuleId < gnMaximumLogicalPorts))
        {
            TracePrint("Timer for Module %d expired",nModuleId);

            if(mpScrManager->mCurrentAction.generic.eAction == ACTION_SEND)
            {
                if( mpScrManager->mCurrentAction.send.nPortB >= 0)
                {
                    char *pFullResp = GetFullResponse(mpScrManager->mCurrentAction.send.nPortB);
                    DisplayResponse(mpScrManager->mCurrentAction.send.nPortB,pFullResp);
                }
            }

            if( nModuleId == mpScrManager->mCurrentAction.generic.nParmA )
            {
                if( mpScrManager->mCurrentAction.generic.eOutcome == OUTCOME_MOD_TIMEOUT )
                {
                    TriggerNextAction(__LINE__);
                }
                else if( mpScrManager->mCurrentAction.generic.eAction == ACTION_WAITPACKET)
                {
                    mConditionFlags.z = false;
                    AnsiString asMsg;
                    asMsg.printf("Module %d timeout : waiting for a packet",nModuleId);
                    OnFail(asMsg.c_str());
                }
                else if( mpScrManager->mCurrentAction.generic.eAction == ACTION_WAIT_TIEVENT)
                {
                    mConditionFlags.z = false;
                    AnsiString asMsg;
                    asMsg.printf("Module %d timeout : waiting for TIEVENT match <%s>",nModuleId, mpScrManager->mCurrentAction.waitTiEvent.pMatch);
                    OnFail(asMsg.c_str());
                }
                else if( mpScrManager->mCurrentAction.generic.eAction == ACTION_WAITTIMEOUT)
                {
                    if(mpScrManager->mCurrentAction.waitTimeout.nPort == nModuleId)
                    {
                        TModule *pModule = &mpModule[nModuleId];
                        TScriptVariable *pVariable = NULL;
                        if( mpScrManager->mCurrentAction.waitTimeout.pVarName )
                        {
                            pVariable = SrcGetScriptVariable(mpScrManager, mpScrManager->mCurrentAction.waitTimeout.pVarName);
                        }
                        if(pVariable)
                        {
                            char *pFullResp = pModule->mpHost->GetFullResponse();
                            int nFullRespLen = strlen(pFullResp);
                            SaveBlockToVariable(pFullResp, 0, nFullRespLen, pVariable);
                        }
                        TriggerNextAction(__LINE__);
                    }
                }
                else if( mpScrManager->mCurrentAction.generic.eAction == ACTION_WAITUNTIL)
                {
                    if(mpScrManager->mCurrentAction.waitTimeout.nPort == nModuleId)
                    {
                        mConditionFlags.z = false;
                        TriggerNextAction(__LINE__);
                    }
                }
                else
                {   //fail test
                    AnsiString asMsg;
                    asMsg.printf("Module %d timeout",nModuleId);
                    OnFail(asMsg.c_str());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtOkResponse(int nModule)
{
    char *pFullResp;

    if(mpScrManager->mCurrentAction.generic.eOutcome == OUTCOME_OK)
    {
        switch(mpScrManager->mCurrentAction.generic.eAction)
        {
        case ACTION_SEND:
            if(mpScrManager->mCurrentAction.send.nPortB == nModule)
            {
                TriggerNextAction(__LINE__);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtErrorResponse(int nModule,int nError)
{
    char *pFullResp;

    if( mpScrManager->mCurrentAction.generic.eOutcome == OUTCOME_ERROR )
    {
        switch(mpScrManager->mCurrentAction.generic.eAction)
        {
        case ACTION_SEND:
            if(mpScrManager->mCurrentAction.send.nPortB == nModule)
            {
                TriggerNextAction(__LINE__);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtConnect(int nModule,char *pResp)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtNoCarrier(int nModule)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtCrLf(int nModule,char *pResp)
{

    //do not delete the following or we will leak
    MiscMemFree(pResp);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TScriptVariable * TRegression::SavePacketInVariable(TModule *pModule, TScriptAction *pScriptPC,TPacketType pktType)
{
    AnsiString asMsg,asTmp;
    int offset,endOffset,valLen;
    TScriptVariable *pVariable = SrcGetScriptVariable(mpScrManager, pScriptPC->waitPacket.pVarName);
    if(pVariable)
    {
        if( pVariable->pValue )
        {
            MiscMemFree(pVariable->pValue);
            pVariable->pValue=NULL;
        }
        if( SavePacket(pModule->mpHost, pVariable,pktType) )
        {
            /* packet has been dropped from the underlying buffer */
            char ch;
            asTmp="";
            {
#define MAX_PRINT_LEN           112
                switch(pktType)
                {
                case TPACKET_TYPE_HCI_LE_EVENT:
                    valLen = strlen(pVariable->pValue) ;
                    asMsg.printf("TiEvent: %d<",pScriptPC->waitTiEvent.nPort);
                    offset=0;endOffset=offset+6;

                    ch = pVariable->pValue[endOffset];
                    pVariable->pValue[endOffset] = 0;
                    asTmp += &pVariable->pValue[offset];
#if !defined(DEBUG_TIEVENTS)
                    asTmp += " ";
#endif
                    pVariable->pValue[endOffset] = ch;
                    if( valLen >= 10 )
                    {
                        /* the 4 hex digit event type  -- LSB then MSB */
                        offset=6;endOffset=offset+4;
                        ch = pVariable->pValue[endOffset];
                        pVariable->pValue[endOffset] = 0;
                        asTmp += &pVariable->pValue[offset];
#if !defined(DEBUG_TIEVENTS)
                        asTmp += " ";
#endif
                        pVariable->pValue[endOffset] = ch;
                    }
                    if( valLen > MAX_PRINT_LEN )
                    {
                        ch = pVariable->pValue[ MAX_PRINT_LEN ];
                        pVariable->pValue[ MAX_PRINT_LEN ] = 0;
                        asTmp += &pVariable->pValue[endOffset];
                        asTmp += "...";
                        pVariable->pValue[ MAX_PRINT_LEN ] = ch;
                    }
                    else
                    {
                        asTmp += &pVariable->pValue[endOffset];
                    }
                    break;

                case TPACKET_TYPE_MPOINT:
                default:
                    asMsg.printf("Packet: %d<",pScriptPC->waitPacket.nPort);
                    if( strlen( pVariable->pValue ) > MAX_PRINT_LEN )
                    {
                        char ch = pVariable->pValue[ MAX_PRINT_LEN ];
                        pVariable->pValue[ MAX_PRINT_LEN ] = 0;
                        asTmp += pVariable->pValue;
                        asTmp += "...";
                        pVariable->pValue[ MAX_PRINT_LEN ] = ch;
                    }
                    else
                    {
                        asTmp = pVariable->pValue;
                    }
                    break;
                }
#undef MAX_PRINT_LEN
                asMsg += asTmp;
                frmMain->DisplayUserMessage(asMsg.c_str());
                return pVariable;
            }
        }
        else
        {
            asMsg.printf("WAITPACKET failed to save in variable");
            OnFail(asMsg.c_str());
        }
    }
    else
    {
        OnFail("Runtime: Unknown Variable");
    }
    return pVariable;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TRegression::OnTiEvent(TModule *pModule,TScriptAction *pScriptPC)
{
    TScriptVariable *pVariable;

    mConditionFlags.z = true;
    pVariable = SavePacketInVariable(pModule,pScriptPC,TPACKET_TYPE_HCI_LE_EVENT);
    /* check if match string available */
#if defined(SPDBG_TIEVENTS)
    AnsiString asMsg = "";
#endif

    if( strlen(pScriptPC->waitTiEvent.pMatch)==0 )
    {
#if defined(SPDBG_TIEVENTS)
        asMsg.printf("?Strlen=0\n");
        frmMain->DisplayUserMessage(asMsg.c_str());
#endif
        return true;
    }
    else
    {
        if(  pVariable )
        {
#if defined(SPDBG_TIEVENTS)
            asMsg.printf("?IsVar\n");
            frmMain->DisplayUserMessage(asMsg.c_str());
#endif
            if (MiscContainsPattern(pVariable->pValue, pScriptPC->waitTiEvent.pMatch) )
            {
#if defined(SPDBG_TIEVENTS)
                asMsg.printf("?GotStr (%s) (%s)\n", pVariable->pValue, pScriptPC->waitTiEvent.pMatch);
                frmMain->DisplayUserMessage(asMsg.c_str());
#endif
                return true;
            }
            /* ELSE will wait for a timeout or until one turns up */
#if defined(SPDBG_TIEVENTS)
            else
            {
                asMsg.printf("?NotGotStr (%s) (%s)\n", pVariable->pValue, pScriptPC->waitTiEvent.pMatch);
                frmMain->DisplayUserMessage(asMsg.c_str());
            }
#endif
        }
#if defined(SPDBG_TIEVENTS)
        else
        {
            asMsg.printf("?IsNotVar\n");
            frmMain->DisplayUserMessage(asMsg.c_str());
        }
#endif
    }
    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtTiEvent(int nModule,char *pFullResp)
{
    TScriptVariable *pVariable;
    if(mpScrManager->mCurrentAction.generic.eOutcome == OUTCOME_HCI_LE_EVENT)
    {
        TScriptAction *pScriptPC = &mpScrManager->mCurrentAction;
        switch(pScriptPC->generic.eAction)
        {
        case ACTION_WAIT_TIEVENT:
            if(pScriptPC->waitTiEvent.nPort == nModule)
            {
                TModule *pModule = &mpModule[nModule];
                while( pModule->mpHost->IsPacketPresent(TPACKET_TYPE_HCI_LE_EVENT) )
                {
                    if( OnTiEvent(pModule,pScriptPC) )   //will drop the packet
                    {
#if defined(SPDBG_TIEVENTS)
                        AnsiString asMsg = "";
                        asMsg.printf("?TrigNextB\n");
                        frmMain->DisplayUserMessage(asMsg.c_str());
#endif
                        StopModuleTimer(nModule);
                        ServiceNextOpcode();
                        return;
                    }
                }
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtPacket(int nModule,char *pFullResp)
{
    if(mpScrManager->mCurrentAction.generic.eOutcome == OUTCOME_PACKET)
    {
        TScriptAction *pScriptPC = &mpScrManager->mCurrentAction;
        switch(pScriptPC->generic.eAction)
        {
        case ACTION_WAITPACKET:
            if(pScriptPC->waitPacket.nPort == nModule)
            {
                mConditionFlags.z = true;
                StopModuleTimer(nModule);
                SavePacketInVariable(&mpModule[nModule],pScriptPC,TPACKET_TYPE_MPOINT);
                TriggerNextAction(__LINE__);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtPattern(int nModule,char *pFullResp)
{
    if(mpScrManager->mCurrentAction.generic.eOutcome == OUTCOME_PATTERN)
    {
        switch(mpScrManager->mCurrentAction.generic.eAction)
        {
        case ACTION_SEND:
            if(mpScrManager->mCurrentAction.send.nPortB == nModule)
            {
                DisplayResponse(nModule,pFullResp);
                StopModuleTimer(nModule);
                TriggerNextAction(__LINE__);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TRegression::ServiceEvtPartialPattern(int nModule,char *pFullResp)
{
    if(mpScrManager->mCurrentAction.generic.eOutcome == OUTCOME_PATTERN)
    {
        switch(mpScrManager->mCurrentAction.generic.eAction)
        {
        case ACTION_WAITDATA:
            if(mpScrManager->mCurrentAction.waitData.nPort == nModule)
            {
                DisplayResponse(nModule,pFullResp);
                StopModuleTimer(nModule);
                TriggerNextAction(__LINE__);
            }
            break;

        case ACTION_WAITCHANNEL:
            if(mpScrManager->mCurrentAction.waitChannel.nPort == nModule)
            {
                StopModuleTimer(nModule);
                TriggerNextAction(__LINE__);
            }
            break;

        case ACTION_WAITRESP:
        case ACTION_WAITMULTI:
            if(mpScrManager->mCurrentAction.waitResp.nPort == nModule)
            {
                DisplayResponse(nModule,pFullResp);
                StopModuleTimer(nModule);
                TriggerNextAction(__LINE__);
            }
            break;

        case ACTION_WAITRESPEX:
        case ACTION_WAITTIMEOUT:
        case ACTION_WAITUNTIL:
            if(mpScrManager->mCurrentAction.waitResp.nPort == nModule)
            {
                TModule *pModule = &mpModule[nModule];
                if( mpWaitRespExCleanExpResp )
                {
                    char *pFound = MiscContainsPattern(pFullResp,mpWaitRespExCleanExpResp);
                    if(pFound)
                    {
                        TScriptVariable *pVariable = NULL;
                        if( mpScrManager->mCurrentAction.waitRespEx.pVarName )
                        {
                            pVariable = SrcGetScriptVariable(mpScrManager, mpScrManager->mCurrentAction.waitRespEx.pVarName);
                        }
                        //int nFullRespLen = strlen(pFullResp);
                        int nToDelete = (pFound - pFullResp) + strlen(mpWaitRespExCleanExpResp);
                        DisplayResponse(nModule,pFullResp);
                        SaveBlockToVariable(pFullResp, 0, nToDelete, pVariable);
                        pModule->mpHost->DropFromResponse(nToDelete);
                    }
                }
                StopModuleTimer(nModule);
                if(mpScrManager->mCurrentAction.generic.eAction==ACTION_WAITRESPEX)
                {
                    TriggerNextAction(__LINE__);
                }
                else if(mpScrManager->mCurrentAction.generic.eAction==ACTION_WAITTIMEOUT)
                {
                    //fail test
                    AnsiString asMsg;
                    asMsg.printf("Module %d did not timeout, got matching string",nModule);
                    OnFail(asMsg.c_str());
                }
                else // if(mpScrManager->mCurrentAction.generic.eAction==ACTION_WAITUNTIL)
                {
                    mConditionFlags.z = true;
                    TriggerNextAction(__LINE__);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool TRegression::ServiceDefault(tagMSG &Msg)
{
    WIZMNGR_EVENT evt = (WIZMNGR_EVENT)Msg.message;

    bool fRetVal = false;

#if 0
    if( (evt>=WIZMNGR_FIRST) && (evt<=WIZMNGR_LAST) )
    {
        TracePrint("EVENT = %s",GetEventNameString(evt));
    }
#else
    evt=evt;
#endif

    return fRetVal;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall TRegression::ServiceTestStateMachine(tagMSG &Msg)
{
    bool fRetVal = true;
    WIZMNGR_EVENT evt = (WIZMNGR_EVENT)Msg.message;

    if( (evt>=WIZMNGR_FIRST) && (evt<=WIZMNGR_LAST) && mfTrace)
    {
        TracePrint("EVENT = %s",GetEventNameString(evt));
    }

    switch(evt)
    {
    case WIZMNGR_INIT:
        ServiceEvtInit();
        break;
    case WIZMNGR_NEXTOPCODE:
        ServiceNextOpcode();
        break;
    case WIZMNGR_RESPONSE_OK:
        ServiceEvtOkResponse(Msg.wParam);
        break;
    case WIZMNGR_RESPONSE_ERROR:
        ServiceEvtErrorResponse(Msg.wParam,Msg.lParam);
        break;
    case WIZMNGR_RESPONSE_CONNECT:
        ServiceEvtConnect(Msg.wParam,(char *)Msg.lParam);
        break;
    case WIZMNGR_RESPONSE_NOCARRIER:
        ServiceEvtNoCarrier(Msg.wParam);
        break;
    case WIZMNGR_RESPONSE_CRLF:
        ServiceEvtCrLf(Msg.wParam,(char *)Msg.lParam);
        break;
    case WIZMNGR_RESPONSE_PATTERN:
        ServiceEvtPattern(Msg.wParam,(char *)Msg.lParam);
        break;
    case WIZMNGR_RESPONSE_PARTIAL:
        ServiceEvtPartialPattern(Msg.wParam,(char *)Msg.lParam);
        break;
    case WIZMNGR_RESPONSE_PACKET:
        ServiceEvtPacket(Msg.wParam,(char *)Msg.lParam);
        break;
    case WIZMNGR_RESPONSE_TIEVENT:
        ServiceEvtTiEvent(Msg.wParam,(char *)Msg.lParam);
        break;
    default:
        fRetVal =   ServiceDefault(Msg);
        break;
    }

    return fRetVal;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TRegression::UpdateModemDisplay(int nId, bool fOpen, bool fCTS, bool fDSR, bool fRI, bool fDCD)
{
    frmMain->UpdateModemDisplay(nId, fOpen, fCTS, fDSR, fRI, fDCD);
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TRegression::CloseAllModules(void)
{
    if(mpModule != NULL)
    {
        for(int i=0;i<gnMaximumLogicalPorts;i++)
        {
                if(mpModule[i].mpHost)
                {
                    delete mpModule[i].mpHost;
                    mpModule[i].mpHost = NULL;
                }
                mpModule[i].mTimer->Enabled = false;
        }
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TRegression::CloseAllFileHandles(void)
{
    for(int i=0;i<MAX_FILE_HANDLES;i++)
    {
        if(mpFileHandle[i])
        {
            fclose(mpFileHandle[i]);
            mpFileHandle[i]=NULL;
        }
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TRegression::ModulesCreate(int nModules)
{
    if( nModules < gnMaximumLogicalPorts )
    {
        gnMaximumLogicalPorts=gnMaximumLogicalPorts;
    }
    else if( nModules > MAX_LOGICAL_PORTS_LIMIT )
    {
        gnMaximumLogicalPorts=MAX_LOGICAL_PORTS_LIMIT;
    }
    else
    {
        gnMaximumLogicalPorts=nModules;
    }
    ModulesDelete();
    mpModule=(TModule *)MiscMemAlloc(sizeof(TModule)*gnMaximumLogicalPorts);
    if( mpModule )
    {
        for(int i=0;i<gnMaximumLogicalPorts;i++)
        {
            mpModule[i].mnIndex                  = i;

            mpModule[i].mpHost                   = CreateEmptyHost(this,i);
            mpModule[i].mTimer                   = new TTimer(NULL);
            mpModule[i].mTimer->Enabled          = false;
            mpModule[i].mTimer->OnTimer          = tmrOnModuleTimer;
            mpModule[i].mTimer->Tag              = i;

            mpModule[i].mnComPort                = -1;
            mpModule[i].mCommParms.mnBaudrate    = 9600;
            mpModule[i].mCommParms.mnDataBits    = 8;
            mpModule[i].mCommParms.mnStopBits    = 1;
            mpModule[i].mCommParms.mnParity      = 0;
            mpModule[i].mCommParms.mnPostOpenWaitMs  = 0;
            mpModule[i].mCommParms.mnPostCloseWaitMs = 0;

            mpModule[i].mpFrmTrace               = new TfrmTrace(frmMain);
            mpModule[i].mpFrmTrace->mPortNum     = i;
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TRegression::ModulesDelete(void)
{
    if( mpModule )
    {
        for(int i=0;i<gnMaximumLogicalPorts;i++)
        {
            if( mpModule[i].mpHost )
            {
                delete mpModule[i].mpHost;
            }
            if( mpModule[i].mTimer )
            {
                mpModule[i].mTimer->Enabled=false;
                delete mpModule[i].mTimer;
            }
            if( mpModule[i].mpFrmTrace )
            {
                delete mpModule[i].mpFrmTrace;
            }
        }
        MiscMemFree(mpModule);
        mpModule=NULL;
    }
}


/******************************************************************************/
/******************************************************************************/
// Global Functions or Public Members
/******************************************************************************/
/******************************************************************************/

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
TRegression::TRegression()
{
    //Application Event handler
    mOldAppOnMessageHandler  = Application->OnMessage;
    mMsgQueHandle           = Application->Handle;
    Application->OnMessage  = AppMessageHandler;

    mpScrManager            = SrcCreateScriptManager(MAX_COMPILED_SCRIPT_LINES,
                                                     MAX_COMPILED_SCRIPT_VARIABLES);

    mfTrace                 = false;
    mfQuit                  = false;
    mfPauseReq              = false;
    mfPaused                = false;
    mpCurrentTestTitle       = NULL;
    mfShowTrace             = false;
    mfAllowSkip             = false;
    mMaxMemoLines           = DEFAULT_MAIN_MEMO_LINES;
    mTraceLeft              = 0;
    mMaxRetryPerTitleTest   = 1;
    mTotalRetryCounts       = 0;
    mTimeScalarPerc         = 100;
    mfConsoleUpdate         = true;
    mfLoggerUpdate          = false;

    UpdateConditionFlags(0);

    mDefaultTimeoutMs = DEFAULT_TIMEOUT_MS;

    for(int i=0;i<MAX_FILE_HANDLES;i++)
    {
        mpFileHandle[i]=NULL;
    }

    mpModule=NULL;

    for(int i=0;i<MAX_COMMANDLINE_ARGS;i++)
    {
        mArgv[i]="";
    }
    mArgc=0;
    for(int i=0;i<MAX_COMMANDLINE_VARS;i++)
    {
        mArgVarName[i]="";
        mArgVarValue[i]="";
    }
    mArgVarCount=0;

    mScriptDelayTimer          = new TTimer(NULL);
    mScriptDelayTimer->Enabled = false;
    mScriptDelayTimer->OnTimer = tmrScriptTimer;

    for(int i=0;i<=MAX_FILE_NEST_DEPTH;i++)
    {
        //Ensure include filenames stack is empty 
        mbaIncFilename[i][0]=0;
    }
    mnIncFilenameStackTop=-1;

    mExpansionBuf = (char *)MiscMemAlloc(0x10000);
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
__fastcall TRegression::~TRegression()
{
    CloseAllModules();

    CloseAllFileHandles();

    if(mpCurrentTestTitle)
    {
        MiscMemFree(mpCurrentTestTitle);
        mpCurrentTestTitle=NULL;
    }

    if(mExpansionBuf)MiscMemFree(mExpansionBuf);
    mExpansionBuf=NULL;

    SrcDeleteScriptManager(mpScrManager);
    mpScrManager=NULL;
}

//=============================================================================
//=============================================================================
void TRegression::OnInit(void)
{
    ModulesCreate(gnMaximumLogicalPorts);
}

//=============================================================================
//=============================================================================
bool TRegression::ReadCommandLineOptions(void)
{
    bool fOptionsOK = true;
    bool fThisOk;

    if( _argc > 1 )
    {
        for(int i=1;i<_argc;i++)
        {
            fThisOk    = InterpretCmdOption(_argv[i]);
            fOptionsOK = fOptionsOK && fThisOk;
        }
    }
    else
    {
        fOptionsOK = false;
    }

    return fOptionsOK;
}


//=============================================================================
//=============================================================================
unsigned int TRegression::Ati3VersionStrToInt(AnsiString &asVersion)
{
    //Version is in the form "x.y.zzz optional characters"
    //If 2 '.'s are not found then returns 0x0000000
    //If it contains other than decimal characters before the z's then returns 0x000000
    //Otherwise the return value is 0xXXYYZZZZ

    //xx yy and zzzz are decimal letters

#define MAX_ATI3_VERSION_STR_LEN    16

    unsigned int nRetVal = 0;
    AnsiString asVerCopy = asVersion;
    char baVersion[MAX_ATI3_VERSION_STR_LEN+1];
    char *pDot,*pStart;
    unsigned int nXX,nYY,nZZ;

    //make a local copy of the version string
    strncpy(baVersion,asVerCopy.c_str(),MAX_ATI3_VERSION_STR_LEN);
    baVersion[MAX_ATI3_VERSION_STR_LEN]=0;

    //first find the first space character if it exists
    char *pSpace = strstr(baVersion," ");
    if( pSpace )*pSpace = 0; //terminate the string at the space
    if(IsValidAti3Version(baVersion) )
    {   //String is in correct format
        //Now find the first '.'
        pStart = baVersion;
        pDot = strstr(pStart,".");
        if(pDot)
        {
            *pDot++ = 0; //terminate and go beyond
            nXX = atoi(pStart);

            pStart = pDot;
            pDot = strstr(pStart,".");
            if(pDot)
            {
                *pDot++ = 0; //terminate and go beyond
                nYY = atoi(pStart);
                nZZ = atoi(pDot);

                nRetVal = ((nXX<<24)&0xFF000000) |
                          ((nYY<<16)&0x00FF0000) |
                          ( nZZ     &0x0000FFFF)   ;
            }
        }
    }


    return nRetVal;
}

//=============================================================================
//=============================================================================
void TRegression::PauseScript(void)
{
    mfPauseReq = true;
}

//=============================================================================
//=============================================================================
void TRegression::ResumeScript(void)
{
    mfPauseReq = false;
    if(mfPaused)
    {
        mfPaused = false;
        ThrowStateMachineEvent(WIZMNGR_NEXTOPCODE,0);
    }
}

//=============================================================================
//=============================================================================
void TRegression::CopyAsToCharArray(const AnsiString &as, char *pDest, int nDestLen, bool fAddBackSlash)
{
    if(nDestLen)
    {
        int nLen = as.Length();
        char *pSrc = as.c_str();

        if( nLen > nDestLen ) nLen = nDestLen;

        memcpy(pDest,pSrc,nLen);
        pDest += nLen;
        *pDest=0;
        if(fAddBackSlash)
        {
            pDest--;
            if( *pDest != '\\' )
            {
                pDest++;
                *pDest++ = '\\';
                *pDest=0;
            }
        }
    }
}

//=============================================================================
//=============================================================================
AnsiString TRegression::GetFullScriptFileName(void)
{
    AnsiString asFolder = GetScriptFolder();
    AnsiString asFilename = GetScriptFileName();
    AnsiString asRet = asFolder + asFilename;
    return asRet;
}

//=============================================================================
//=============================================================================
void TRegression::SetScriptFileName(const AnsiString &as)
{
    /* commented out in 1.9.28
    //AnsiString asFile = MiscExtractFilenameOnly(as);
    */
    AnsiString asFile = as;
    CopyAsToCharArray(asFile,mpScrManager->mbaScriptFileName,MAX_FILNAME_LEN, false);
}

//=============================================================================
//=============================================================================
void TRegression::SetCurrentWorkingFolder(const AnsiString &asPath)
{
    /* commented out in 1.9.28
    //AnsiString asPath = MiscExtractFilenameOnly(as);
    */
    SetCurrentDir(asPath);
}

//=============================================================================
//=============================================================================
void TRegression::SetLogfileFolder(const AnsiString &asPath)
{
    AnsiString asFile = asPath;
    CopyAsToCharArray(asFile,mpScrManager->mbaLogFileFolder,MAX_FILNAME_LEN, false);
}

//=============================================================================
//=============================================================================
AnsiString TRegression::GetOtherIncludeFolder(int nIndex)
{
    AnsiString asRet = "";
    if(nIndex < MAX_OTHER_INCLUDES)
    {
        asRet = mpScrManager->mbaOtherIncludeFolder[nIndex];
    }
    return asRet;
}

//=============================================================================
//=============================================================================
void TRegression::SetOtherIncludeFolder(int nIndex, const AnsiString &as)
{
    if(nIndex < MAX_OTHER_INCLUDES)
    {
        CopyAsToCharArray(as,mpScrManager->mbaOtherIncludeFolder[nIndex],(MAX_FILNAME_LEN-1), true);
    }
}

//=============================================================================
//=============================================================================
void TRegression::TraceTx(int nLogicalPort, char *pTx)
{
    if(nLogicalPort<gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nLogicalPort];
        if(pModule)
        {
            if(pModule->mpFrmTrace)
            {
                pModule->mpFrmTrace->OnTransmit(pTx);
            }
        }
    }
}

//=============================================================================
//=============================================================================
void TRegression::TraceUnicodeTx(int nLogicalPort, unsigned char *pTx, int nLen)
{
    if(nLogicalPort<gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nLogicalPort];
        if(pModule)
        {
            if(pModule->mpFrmTrace)
            {
                pModule->mpFrmTrace->OnTransmitUnicode(pTx,nLen);
            }
        }
    }
}

//=============================================================================
//=============================================================================
void TRegression::TraceNewLine(int nLogicalPort)
{
    if(nLogicalPort<gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nLogicalPort];
        if(pModule)
        {
            if(pModule->mpFrmTrace)
            {
                pModule->mpFrmTrace->ForceNewLine();
            }
        }
    }
}

//=============================================================================
//=============================================================================
void TRegression::TraceRx(int nLogicalPort, char *pRx)
{
    if(nLogicalPort<gnMaximumLogicalPorts)
    {
        TModule *pModule = &mpModule[nLogicalPort];
        if(pModule)
        {
            if(pModule->mpFrmTrace)
            {
                pModule->mpFrmTrace->OnReceive(pRx);
            }
        }
    }
}


/******************************************************************************/
/******************************************************************************/
// Global Functions Non Class Members
/******************************************************************************/
/******************************************************************************/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ThrowStateMachineEvent(WIZMNGR_EVENT evt, void *pEvtData, int nModuleId)
{
    ::PostMessage(Application->Handle, (UINT)evt, (WPARAM)nModuleId, (LPARAM)pEvtData);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ThrowStateMachineEvent(WIZMNGR_EVENT evt, unsigned int nEvtData, int nModuleId)
{
    ::PostMessage(Application->Handle, (UINT)evt, (WPARAM)nModuleId, (LPARAM)nEvtData);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ThrowStateMachineEvent(WIZMNGR_EVENT evt, int nEvtData, int nModuleId)
{
    ::PostMessage(Application->Handle, (UINT)evt, (WPARAM)nModuleId, (LPARAM)nEvtData);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void ThrowStateMachineEvent(WIZMNGR_EVENT evt, int nModuleId)
{
    ::PostMessage(Application->Handle, (UINT)evt, (WPARAM)nModuleId, 0);
}

/******************************************************************************/
// END OF FILE
/******************************************************************************/




