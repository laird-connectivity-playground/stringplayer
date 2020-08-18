/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Upgrade Class
**
** Module:      TREGRESSION.H
**
*******************************************************************************/

#if !defined(_TREGRESSION_H)     /* prevent multiple inclusions */
#define _TREGRESSION_H

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
#include "Misc.h"
#include "BaseHost.h"
#include "TScriptSource.h"
#include "TFrmTrace.h"
#include "TFrmAggLogger.h"

/******************************************************************************/
// Defines
/******************************************************************************/

#define MAX_LOGICAL_PORTS                  16
#define MAX_LOGICAL_PORTS_LIMIT            255
#define MAX_FILE_HANDLES                   8

#define MAX_COMMANDLINE_ARGS               16
#define MAX_COMMANDLINE_VARS               16

#define WM_BONDMNGR_EVT                    (WM_USER + 100)
#define WM_PIPECONSOLE_EVT                 (WM_USER + 300)

#define STR_MAGIC_CMD                       "!<BISM>!"

#define RESPONSE_OK                         0
#define RESPONSE_ERROR                      1
#define RESPONSE_TIMEOUT                    2

#define STOP_STATE_TIMER                    0
#define LEAVE_STATE_TIMER                   -1

#define MAX_INT_VALUE_STRING_SIZE           16

#define MIN_MAIN_MEMO_LINES                 (100)
#define SMALLLOG_MAIN_MEMO_LINES            (500)
#define DEFAULT_MAIN_MEMO_LINES             (750)
#define MAX_MAIN_MEMO_LINES                 (10000)

#define MAX_INCFILEPATHNAME_LEN             (512)

/******************************************************************************/
// Macros
/******************************************************************************/

/******************************************************************************/
// Enum Typedefs
/******************************************************************************/

typedef enum
{
    WIZMNGR_INIT                        = WM_BONDMNGR_EVT
   ,WIZMNGR_NEXTOPCODE
   ,WIZMNGR_RESPONSE_OK
   ,WIZMNGR_RESPONSE_ERROR
   ,WIZMNGR_RESPONSE_CRLF
   ,WIZMNGR_RESPONSE_CONNECT
   ,WIZMNGR_RESPONSE_NOCARRIER
   ,WIZMNGR_RESPONSE_PATTERN
   ,WIZMNGR_RESPONSE_PARTIAL
   ,WIZMNGR_RESPONSE_PACKET
   ,WIZMNGR_RESPONSE_TIEVENT

/*
   ,WIZMNGR_
*/
   ,WIZMNGR_LAST
   ,WIZMNGR_FIRST                       = WM_BONDMNGR_EVT

}
WIZMNGR_EVENT;

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

/******************************************************************************/
// Forward declaration of Class, Struct & Unions
/******************************************************************************/
class TfrmMessage;
class TBaseHost;

/******************************************************************************/
// Struct definitions
/******************************************************************************/

typedef struct
{
    int                  mnBaudrate;
    int                  mnDataBits;
    int                  mnStopBits;
    int                  mnParity;
    int                  mnPostOpenWaitMs;
    int                  mnPostCloseWaitMs;
}
COMPARMS;

typedef struct
{
    int                  mnIndex;
    int                  mnComPort;
    COMPARMS             mCommParms;
    TBaseHost           *mpHost;
    TTimer              *mTimer;
    TfrmTrace           *mpFrmTrace;
}
TModule;

typedef struct
{
    bool    z;
    bool    lt;
    bool    le;
    bool    gt;
    bool    ge;
}
TConditionFlags;

/******************************************************************************/
// Class definitions
/******************************************************************************/

class TRegression
{
//Data
private:

//Data
protected:

//Data
public:
    TMessageEvent        mOldAppOnMessageHandler;
    HWND                 mMsgQueHandle;
    bool                 mfTrace;
    bool                 mfQuit;
    bool                 mfPauseReq;
    bool                 mfPaused;
    unsigned int         mTickCount;
    int                  mMaxRetryPerTitleTest;
    int                  mTotalRetryCounts;
    int                  mTimeScalarPerc;

    TModule             *mpModule;
    FILE                *mpFileHandle[MAX_FILE_HANDLES];
    TScriptManager      *mpScrManager;
    int                  mDefaultTimeoutMs;
    TTimer              *mScriptDelayTimer;
    char                *mpCurrentTestTitle;
    TConditionFlags      mConditionFlags;
    char                *mExpansionBuf;

    unsigned int         mStartTickCount;
    bool                 mfShowTrace;
    int                  mTraceLeft;
    unsigned short       mTriggerSrc;
    unsigned short       mTriggerFifo[17];
    AnsiString           mArgv[MAX_COMMANDLINE_ARGS];
    int                  mArgc;
    char                *mpWaitRespExCleanExpResp;
    AnsiString           mArgVarName[MAX_COMMANDLINE_VARS];
    AnsiString           mArgVarValue[MAX_COMMANDLINE_VARS];
    int                  mArgVarCount;
    bool                 mfAllowSkip;
    int                  mMaxMemoLines;
    char                 mbaIncFilename[MAX_FILE_NEST_DEPTH+1][MAX_INCFILEPATHNAME_LEN+1];
    uint32               mnaIncludeLineNum[MAX_FILE_NEST_DEPTH+1];
    int                  mnIncFilenameStackTop;
    uint32               mnCurrentActionLineNum;
    bool                 mfConsoleUpdate;
    bool                 mfLoggerUpdate;


//Functions
private:
    bool InterpretCmdOption(char *pOption);
    bool AnsiStringToInt(const AnsiString asInt, int *pRetVal);
    void CloseAllModules(void);
    void CloseAllFileHandles(void);
    void ModulesCreate(int nModules);    //see gnMaximumLogicalPorts = MAX_MODULES
    void ModulesDelete(void);

//Functions
protected:

//Functions
public:
    TRegression();
    virtual __fastcall ~TRegression();

    void OnInit(void);
    bool ReadCommandLineOptions(void);
    unsigned int Ati3VersionStrToInt(AnsiString &asVersion);
    bool IsValidAti3Version(char *pVer);
    void __fastcall AppMessageHandler(tagMSG &Msg, bool &Handled);
    void __fastcall tmrOnModuleTimer(TObject *Sender);
    void __fastcall tmrScriptTimer(TObject *Sender);
    void  OnModuleTimerExpire(int nModuleId);
    void StartModuleTimer(TModule *pModule, int nTimeoutMs, bool fApplyScalar);
    void StopModuleTimer(TModule *pModule);
    void StopModuleTimer(unsigned int nModule);
    void StopAllModuleTimers(void);
    char *GetEventNameString(WIZMNGR_EVENT evt);
    void ResetTickCountImage(void);
    unsigned int ElapsedTickCount(bool fReset);
    void DisplayTicks(bool fReset);
    char * SetRxPattern(TModule *pModule,char *pPattern,unsigned short nPatternLen,int nSearchType);
    char * SetRxPattern(TModule *pModule,char *pPattern);
    char * SetPartialRxPattern(TModule *pModule,char *pPattern);
    char * GetFullResponse(int nModule);
    bool ParseCommsString(char *pCommParm, TBaseCommParms *pCommParms);
    bool SaveResp(unsigned int nModule, TScriptVariable *pVariable);
    bool SaveBlockToVariable(char *pSrc, int nOffset, int nLen, TScriptVariable *pVariable);
    bool SavePacket(TBaseHost *pHost, TScriptVariable *pVariable,TPacketType pktType);
    void StartDelay(int nTimeoutMs, bool fApplyScalar);
    void AdvanceToNextTest(void);
    void RetreatToStartOfThisTest(void);
    TScriptVariable *SavePacketInVariable(TModule *pModule, TScriptAction *pScriptPC,TPacketType pktType);
    int ScaleTimeout(int nIntVal);

    void CopyAsToCharArray(const AnsiString &as, char *pDest, int nDestLen, bool fAddBackSlash);

    AnsiString GetScriptFileName(void) { return mpScrManager->mbaScriptFileName; };
    void SetScriptFileName(const AnsiString &as);
    void SetCurrentWorkingFolder(const AnsiString &as);
    AnsiString GetFullScriptFileName(void);
    void SetLogfileFolder(const AnsiString &asPath);

    AnsiString GetScriptFolder(void) { return mpScrManager->mbaScriptFolder; };
    AnsiString GetLogFileFolder(void) { return mpScrManager->mbaLogFileFolder; };
    void SetScriptFolder(const AnsiString &as) { CopyAsToCharArray(as,mpScrManager->mbaScriptFolder,MAX_FILNAME_LEN, true);};

    AnsiString GetAppFolder(void) { return mpScrManager->mbaAppFolder; };
    void SetAppFolder(const AnsiString &as) { CopyAsToCharArray(as,mpScrManager->mbaAppFolder,(MAX_FILNAME_LEN-1), true);};

    AnsiString GetOtherIncludeFolder(int nIndex);
    void SetOtherIncludeFolder(int nIndex, const AnsiString &as);

    void DisplayResponse(unsigned int nModule,char *pResp);
    void DisplayChannelResponse(unsigned int nModule, int channel, char *pResp);

    bool IsValidBluetoothAddress(char *pBdAddr);
    bool IsPortOpen(int nModuleId);
    void TriggerNextAction(int src);
    void OnScriptEnd(void);
    void OnScriptExit(void);
    void OnScriptStart(void);
    void OnFail(char *pFailMsg, int nExitCode=-1000, bool fNextTest=true);
    void OnUnexpectedResponse(char *pResp, char *pExpectedResp);
    void UpdateConditionFlags(int nCurValue);
    ESCRERROR AddSystemVariables(void);
    void PauseScript(void);
    void ResumeScript(void);
    void UpdateModemDisplay(int nId, bool fOpen, bool fCTS, bool fDSR, bool fRI, bool fDCD);

    bool LaunchExecuteApp(char *pCommandLine, int nFlags);
    bool LaunchExecuteApp(char *pCommandLine, char *pFolder, bool fWait, bool fHide, bool fMinimize);
    bool LaunchExecuteAppEx(char *pCommandLine, char *pFolder, bool fWait, bool fHide, bool fMinimize, DWORD *dwRetCode);

    bool GetVariableValueAsInteger(char *pVarName, int *pIntVal, int nDefVal);
    bool GetPortInfo(unsigned int nModule, TScriptVariable *pVariable, int nArg);

    void TraceTx(int nLogicalPort, char *pTx);
    void TraceUnicodeTx(int nLogicalPort, unsigned char *pTx, int nLen);
    void TraceNewLine(int nLogicalPort);
    void TraceRx(int nLogicalPort, char *pRx);

    void IncludePush(TActionIncludePush *pIncludePush);
    void IncludePop(TActionIncludePop *pIncludePop);

    bool __fastcall ServiceTestStateMachine(tagMSG &Msg);
    void ServiceEvtInit(void);
    void ServiceNextOpcode(void);
    void ServiceEvtOkResponse(int nModule);
    void ServiceEvtNoCarrier(int nModule);
    void ServiceEvtConnect(int nModule,char *pResp);
    void ServiceEvtErrorResponse(int nModule,int nError);
    void ServiceEvtCrLf(int nModule,char *pResp);
    void ServiceEvtPattern(int nModule,char *pFullResp);
    void ServiceEvtPartialPattern(int nModule,char *pFullResp);
    void ServiceEvtPacket(int nModule,char *pFullResp);
    void ServiceEvtTiEvent(int nModule,char *pFullResp);
    bool ServiceDefault(tagMSG &Msg);
    void RunActionArithmetic(TScriptAction *pScriptPC, bool fHexMode);
    bool OnTiEvent(TModule *pModule,TScriptAction *pScriptPC);

    //run actions
    void RunActionUnknown(TScriptAction *pScriptPC);
    void RunActionSend(TScriptAction *pScriptPC);
    void RunActionBreakOn(TScriptAction *pScriptPC);
    void RunActionBreakOff(TScriptAction *pScriptPC);
    void RunActionSetVar(TScriptAction *pScriptPC);
    void RunActionConst(TScriptAction *pScriptPC);
    void RunActionCheckVar(TScriptAction *pScriptPC);
    void RunActionContain(TScriptAction *pScriptPC, bool fPresent, bool fFailAllow);
    void RunActionSaveResp(TScriptAction *pScriptPC);
    void RunActionPortInfo(TScriptAction *pScriptPC);
    void RunActionMultiIndex(TScriptAction *pScriptPC);
    void RunActionString(TScriptAction *pScriptPC);
    void RunActionFilterMove(TScriptAction *pScriptPC);
    void RunActionStrDeHexize(TScriptAction *pScriptPC);
    void RunActionWaitResp(TScriptAction *pScriptPC);
    void RunActionWaitRespEx(TScriptAction *pScriptPC);
    void RunActionWaitTimeout(TScriptAction *pScriptPC);
    void RunActionWaitUntil(TScriptAction *pScriptPC);
    void RunActionWaitMulti(TScriptAction *pScriptPC);
    void RunActionWaitData(TScriptAction *pScriptPC);
    void RunActionWaitChannel(TScriptAction *pScriptPC);
    void RunActionWaitPacket(TScriptAction *pScriptPC);
    void RunActionWaitTiEvent(TScriptAction *pScriptPC);
    void RunActionSetComms(TScriptAction *pScriptPC);
    void RunActionPrint(TScriptAction *pScriptPC,bool fEscaped);
    void RunActionDelay(TScriptAction *pScriptPC);
    void RunActionDelayAbs(TScriptAction *pScriptPC);
    void RunActionDefTimeout(TScriptAction *pScriptPC);
    void RunActionTitle(TScriptAction *pScriptPC);
    void RunActionSubTitle(TScriptAction *pScriptPC);
    void RunActionFail(TScriptAction *pScriptPC);
    void RunActionModemStat(TScriptAction *pScriptPC);
    void RunActionModemSet(TScriptAction *pScriptPC);
    void RunActionEnd(TScriptAction *pScriptPC);
    void RunActionExit(TScriptAction *pScriptPC);
    void RunActionFlushRx(TScriptAction *pScriptPC);
    void RunActionFlushTx(TScriptAction *pScriptPC);
    void RunActionLabel(TScriptAction *pScriptPC);
    void RunActionDecArithmetic(TScriptAction *pScriptPC);
    void RunActionHexArithmetic(TScriptAction *pScriptPC);
    void RunActionJump(TScriptAction *pScriptPC);
    void DoSkip();
    void RunActionIf(TScriptAction *pScriptPC);
    void RunActionPortUart(TScriptAction *pScriptPC);
    void RunActionPortClose(TScriptAction *pScriptPC);
    void RunActionPortMPoint(TScriptAction *pScriptPC);
    void RunActionPortOC(TScriptAction *pScriptPC);
    void RunActionMessageBox(TScriptAction *pScriptPC);
    void RunActionPortPipe(TScriptAction *pScriptPC);
    void RunActionSendCmd(TScriptAction *pScriptPC);
    void RunActionSendMpData(TScriptAction *pScriptPC);
    void RunActionTxString(TScriptAction *pScriptPC);
    void RunActionTxStringEx(TScriptAction *pScriptPC);
    void RunActionTxUnicode(TScriptAction *pScriptPC);
    void RunActionExecute(TScriptAction *pScriptPC);
    void RunActionExecuteEx(TScriptAction *pScriptPC);
    void RunActionExecuteWait(TScriptAction *pScriptPC);
    void RunActionExecuteWaitRet(TScriptAction *pScriptPC);
    void RunActionExecuteWaitEx(TScriptAction *pScriptPC);
    void RunActionExistFile(TScriptAction *pScriptPC);
    void RunActionExistFolder(TScriptAction *pScriptPC);
    void RunActionFileCompare(TScriptAction *pScriptPC);
    void RunActionConsoleUpdate(TScriptAction *pScriptPC);
    void RunActionTrace(TScriptAction *pScriptPC);
    void RunActionTraceWin(TScriptAction *pScriptPC);
    void RunActionStrCat(TScriptAction *pScriptPC);
    void RunActionGetCwd(TScriptAction *pScriptPC);
    void RunActionFileOpen(TScriptAction *pScriptPC);
    void RunActionFileClose(TScriptAction *pScriptPC);
    void RunActionFileRead(TScriptAction *pScriptPC);
    void RunActionFileWrite(TScriptAction *pScriptPC);
    void RunActionTimer(TScriptAction *pScriptPC);
    void RunActionSprintInt(TScriptAction *pScriptPC);
    void RunActionSprintHex(TScriptAction *pScriptPC);
    void RunActionHex2Dec(TScriptAction *pScriptPC);
    void RunActionDec2Hex(TScriptAction *pScriptPC);
    void RunActionPktCountGet(TScriptAction *pScriptPC);
    void RunActionPktCountSet(TScriptAction *pScriptPC);
    void RunActionPktCountResp(TScriptAction *pScriptPC);
    void RunActionMid(TScriptAction *pScriptPC);
    void RunActionTimeScalar(TScriptAction *pScriptPC);
    void RunActionInRange(TScriptAction *pScriptPC);
    void RunActionIncludePush(TScriptAction *pScriptPC);
    void RunActionIncludePop(TScriptAction *pScriptPC);

};


/******************************************************************************/
// Union definitions
/******************************************************************************/

/******************************************************************************/
// Global Functions (API etc) exported for other modules
/******************************************************************************/
void ThrowStateMachineEvent(UINT nMsg, WPARAM wParam, LPARAM lParam );
void ThrowStateMachineEvent(WIZMNGR_EVENT evt, void *pEvtData, int nModuleId);
void ThrowStateMachineEvent(WIZMNGR_EVENT evt, unsigned int nEvtData, int nModuleId);
void ThrowStateMachineEvent(WIZMNGR_EVENT evt, int nEvtData, int nModuleId);
void ThrowStateMachineEvent(WIZMNGR_EVENT evt,int nModuleId);

extern int gnMaximumLogicalPorts;

#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/
