/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Upgrade Class
**
** Module:      TSCRIPTSOURCE.H
**
*******************************************************************************/

#if !defined(_TSCRIPTSOURCE_H)     /* prevent multiple inclusions */
#define _TSCRIPTSOURCE_H

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
#include "MiscEx.h"

/******************************************************************************/
// Defines
/******************************************************************************/

#define USE_DEFAULT_TIMEOUT                 -1
#define DEFAULT_TIMEOUT_MS                   1000
#define DEFAULT_TIMESCALAR_PERC              100

#define MAX_FILNAME_LEN                      512
#define MAX_OTHER_INCLUDES                   10

#define MAX_SYS_VAR_ARRAY_LEN                128

#define MAX_VARNAME_LEN                      64

#define MAX_FILE_NEST_DEPTH                  8

/******************************************************************************/
// Macros
/******************************************************************************/

/******************************************************************************/
// Enum Typedefs
/******************************************************************************/

typedef enum
{
    ACTION_END                 = 0
   ,ACTION_EXIT
   ,ACTION_TITLE
   ,ACTION_SUBTITLE
   ,ACTION_FAIL
   ,ACTION_SEND
   ,ACTION_BRKON
   ,ACTION_BRKOFF
   ,ACTION_DELAY
   ,ACTION_DELAYABS
   ,ACTION_SETVAR
   ,ACTION_SETVARX
   ,ACTION_STRCAT
   ,ACTION_GETCWD
   ,ACTION_CONST
   ,ACTION_CHECKVAR
   ,ACTION_CONTAIN
   ,ACTION_NOTCONTAIN
   ,ACTION_MATCHCOUNT
   ,ACTION_SETCOMMS
   ,ACTION_PRINT
   ,ACTION_PRINTE
   ,ACTION_PORTINFO
   ,ACTION_SAVERESP
   ,ACTION_WAITRESP
   ,ACTION_WAITRESPEX
   ,ACTION_WAITTIMEOUT
   ,ACTION_WAITUNTIL
   ,ACTION_WAITMULTI
   ,ACTION_MULTIINDEX
   ,ACTION_WAITPACKET
   ,ACTION_WAIT_TIEVENT
   ,ACTION_WAITDATA
   ,ACTION_WAITCHANNEL
   ,ACTION_MDMSTAT
   ,ACTION_MDMSET
   ,ACTION_FLUSHRX
   ,ACTION_FLUSHTX
   ,ACTION_LABEL
   ,ACTION_ALU
   ,ACTION_HEXALU
   ,ACTION_JUMP
   ,ACTION_IF
   ,ACTION_PORT_UART
   ,ACTION_PORT_CLOSE
   ,ACTION_PORT_MPOINT
   ,ACTION_PORT_OC
   ,ACTION_DEFTIMEOUT
   ,ACTION_TIMESCALAR
   ,ACTION_SENDCMD
   ,ACTION_SENDMPDATA
   ,ACTION_TXSTRING
   ,ACTION_TXSTRINGEX
   ,ACTION_TXUNICODE
   ,ACTION_STRING
   ,ACTION_EXECUTE
   ,ACTION_EXECUTEWAIT
   ,ACTION_EXECUTEWAITRET
   ,ACTION_EXECUTEEX
   ,ACTION_EXECUTEWAITEX
   ,ACTION_CONSOLEUPDATE
   ,ACTION_TRACE
   ,ACTION_TRACEWIN
   ,ACTION_EXIST_FILE
   ,ACTION_EXIST_FOLDER
   ,ACTION_FILE_COMPARE
   ,ACTION_FILE_OPEN
   ,ACTION_FILE_CLOSE
   ,ACTION_FILE_READ
   ,ACTION_FILE_WRITE
   ,ACTION_TIMER
   ,ACTION_PORT_PIPE
   ,ACTION_SPRINTINT
   ,ACTION_SPRINTHEX
   ,ACTION_HEX2DEC
   ,ACTION_DEC2HEX
   ,ACTION_PKTCOUNTGET
   ,ACTION_PKTCOUNTSET
   ,ACTION_PKTCOUNTRESP
   ,ACTION_MID
   ,ACTION_MESSAGEBOX
   ,ACTION_INRANGE
   ,ACTION_FILTERMOVE
   ,ACTION_STRDEHEXIZE
   ,ACTION_INCLUDE_PUSH
   ,ACTION_INCLUDE_POP

   //All the following actions do NOT produce any action code
   ,ACTION_NOCODE_MASK         = 0x8000
   ,ACTION_DECLARE

   ,ACTION_UNKNOWN             = 0xFFFF
}
EScriptAction;

typedef enum
{
    SCRERROR_NONE                    = 0
   ,SCRERROR_FILE_UNKNOWN
   ,SCRERROR_SYNTAX
   ,SCRERROR_OUTOFMEM
   ,SCRERROR_UNTERM_STRING
   ,SCRERROR_UNKNOWN_ACTION
   ,SCRERROR_UNKNOWN_OUTCOME
   ,SCRERROR_TIMEOUT_SYNTAX
   ,SCRERROR_INCOMPLETE_LINE
   ,SCRERROR_NOT_STRING
   ,SCRERROR_UNEXPECTED_SOURCE
   ,SCRERROR_INVALID_MNGR
   ,SCRERROR_NOT_DECIMAL_NUMBER
   ,SCRERROR_ILLEGAL_DIRECTIVE
   ,SCRERROR_FILE_EMPTY
   ,SCRERROR_UNEXPECTED_STRING
   ,SCRERROR_ILLEGAL_VARNAME
   ,SCRERROR_UNKNOWN_VAR
   ,SCRERROR_FILE_NEST_TOODEEP
   ,SCRERROR_ILLEGAL_LABELNAME
   ,SCRERROR_MULTIPLE_LABEL
   ,SCRERROR_UNKNOWN_LABELNAME
   ,SCRERROR_INCOMPLETE_ESCAPE
   ,SCRERROR_INVALID_PORT
   ,SCRERROR_INVALID_CHANNEL
   ,SCRERROR_INVALID_FILE_HANDLE
   ,SCRERROR_CANNOT_RESOLVE_VAR
   ,SCRERROR_VAR_IS_CONST
   ,SCRERROR_INVALID_FOLDER
   ,SCRERROR_INV_LOGIC_VALUE
   ,SCRERROR_INVALID_VALUE
   ,SCRERROR_NOT_HEX_NUMBER
   ,SCRERROR_NOT_OCTAL_NUMBER
   ,SCRERROR_NOT_BINARY_NUMBER
   ,SCRERROR_NUMBER_INVALID_CHR


/*
    SCRERROR_
*/

}
ESCRERROR;

typedef enum
{
    MDMSTAT_CTS         = 0
   ,MDMSTAT_DSR
   ,MDMSTAT_DCD
   ,MDMSTAT_RI

   ,MDMLINE_DTR
   ,MDMLINE_RTS

}
MODEM_LINE;

typedef enum
{
    JMP_Z               = 0
   ,JMP_NZ
   ,JMP_LT
   ,JMP_LE
   ,JMP_GT
   ,JMP_GE
   ,JMP_ALWAYS
}
JUMP_COND;

typedef enum
{
    VARTYPE_CONST           = 0
   ,VARTYPE_STRING
}
EVARTYPE;

typedef enum
{
    ALU_ADD                 = 0
   ,ALU_MUL
   ,ALU_DIV
   ,ALU_SUB
   ,ALU_MOD
   ,ALU_AND
   ,ALU_OR
   ,ALU_XOR
   ,ALU_ROL
   ,ALU_ROR
   ,ALU_TST
   ,ALU_ABS

   ,ALU_HEXADD
   ,ALU_HEXMUL
   ,ALU_HEXDIV
   ,ALU_HEXSUB
   ,ALU_HEXMOD
   ,ALU_HEXAND
   ,ALU_HEXOR
   ,ALU_HEXXOR
   ,ALU_HEXROL
   ,ALU_HEXROR
   ,ALU_HEXTST

}
ALU_TYPE;

typedef enum
{
    STR_LEN                 = 0
   ,STR_CMP
   ,STR_CMPI
   ,STR_STR
   ,STR_STRI
}
STRING_FUNC;

typedef enum
{
    TIMER_START             = 0
   ,TIMER_ELAPSED
   ,TIMER_EXPIRED

   ,TIMER_ILLEGAL
}
TIMER_TYPE;


/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

/******************************************************************************/
// Forward declaration of Class, Struct & Unions
/******************************************************************************/

/******************************************************************************/
// Struct definitions
/******************************************************************************/

typedef struct
{
    EVARTYPE                    eVarType;
    char                       *pName;
    char                       *pValue;
}
TScriptVariable;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    int                         nTimeoutMs; //-1 == default
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pParmA;
    int                         nParmB;
    char                       *pParmB;
}
TAutoScript;

//The following MUST be the same size as TAutoScript
typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nNameLen;
    char                       *pFilename;
    int                         nPortB;
    char                       *pParmB;
}
TActionIncludePush;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pParmA;
    int                         nPortB;
    char                       *pParmB;
}
TActionIncludePop;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPortA;
    char                       *pSendStr;
    int                         nPortB;
    char                       *pRespStr;
}
TActionSend;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPerc;
    char                       *pTargetValue;
    int                         nPortB;
    char                       *pValue;
}
TActionInRange;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    int                         nTimeoutMs; //-1 == default
    uint32                      nLineNum;  //line number in script source

    int                         nPortA;

    char                       *pTimeoutMs;
    int                         nParmB;
    char                       *pParmB;
}
TAutoBreakOn;

typedef TAutoBreakOn             TAutoBreakOff;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    int                         nTimeoutMs; //-1 == default
    uint32                      nLineNum;  //line number in script source

    int                         nPort;

    char                       *pParmA;
    int                         nParmB;
    char                       *pParmB;
}
TAutoFlushRx;
typedef TAutoFlushRx TAutoFlushTx;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPortA;
    char                       *pVarName;
    int                         nPortB;
    char                       *pVarValue;
}
TActionSetVar;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPortA;
    char                       *pVarName;
    int                         nPortB;
    char                       *pVarValue;
}
TActionStrCat;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPortA;
    char                       *pVarName;
    int                         nPortB;
    char                       *pParmB;
}
TActionGetCwd;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nAluType;
    char                       *pVarName;
    int                         nPortB;
    char                       *pVarValue;
}
TActionAlu;

typedef  TActionSetVar TActionCheckVar;
typedef  TActionSetVar TActionSprintInt;
typedef  TActionSetVar TActionSprintHex;
typedef  TActionSetVar TActionHex2Dec;
typedef  TActionSetVar TActionDec2Hex;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pCommParm;

    int                         nPortB;
    char                       *pVarValue;
}
TActionSetComms;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pTxStr;

    int                         nParmB;
    char                       *pRepeatCount;
}
TActionTxString;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         offset;
    char                       *pVarName;

    int                         length;
    char                       *pSrcString;
}
TActionMid;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pTxStr;

    int                         nChannel;
    char                       *pVarValue;
}
TActionSendMpData;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nMask;
    char                       *pVarnameDst;

    int                         fInvert;
    char                       *pVarnameSrc;
}
TActionFilterMove;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nFlags;
    char                       *pVarnameDst;

    int                         nPortB;
    char                       *pVarnameSrc;
}
TActionStrDehexize;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pRespStr;

    int                         nPortB;
    char                       *pVarValue;
}
TActionWaitResp;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pRespStr;

    int                         nPortB;
    char                       *pVarName;
}
TActionWaitRespEx;

typedef TActionWaitRespEx       TActionWaitTimeout;
typedef TActionWaitRespEx       TActionWaitUntil;


typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pRespStr;

    int                         nPortB;
    char                       *pVarValue;
}
TActionWaitMulti;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pRespStr;

    int                         nChannel;
    char                       *pVarValue;
}
TActionWaitData;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pRespStr;

    int                         nChannel;
    char                       *pVarValue;
}
TActionWaitChannel;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pVarName;

    int                         nParmB;
    char                       *pParmB;
}
TActionWaitPacket;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pVarName;

    int                         nParmB;
    char                       *pMatch;
}
TActionWaitTiEvent;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nButtons;
    char                       *pText;

    int                         nSpare;
    char                       *pCaption;
}
TActionMessageBox;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pComPort;

    int                         nPortB;
    char                       *pComParm;
}
TActionPort;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pAppPathNameOpts;

    int                         nPortB;
    char                       *pAppCurrentFolder;
}
TActionPortPipe;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pPrnMsg;

    int                         nParmB;
    char                       *pParmB;
}
TActionPrint;

typedef TActionPrint            TActionPrintE;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nEnable;
    char                       *pParmA;

    int                         nParmB;
    char                       *pParmB;
}
TActionTrace;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nEnable;
    char                       *pParmA;

    int                         nParmB;
    char                       *pParmB;
}
TActionConsoleUpdate;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nLogic;
    char                       *pFilename;

    int                         nParmB;
    char                       *pParmB;
}
TActionExistFile;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nLogic;
    char                       *pFilenameA;

    int                         nParmB;
    char                       *pFilenameB;
}
TActionFileCompare;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nLogic;
    char                       *pFoldername;

    int                         nParmB;
    char                       *pParmB;
}
TActionExistFolder;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pTestTitle;

    int                         nParmB;
    char                       *pParmB;
}
TActionTitle;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pFailMsg;

    int                         nParmB;
    char                       *pParmB;
}
TActionFail;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pVarName;

    int                         fEcho;
    char                       *pParmB;
}
TActionSaveResp;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pVarName;

    int                         nParmB;
    char                       *pParmB;
}
TActionRxCount;
typedef TActionRxCount  TActionRxDataCount;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pVarName;

    int                         nParmB;
    char                       *pParmB;
}
TActionMultiIndex;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pVarName;

    int                         nParmB;
    char                       *pParmB;
}
TActionPktCountGet;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pVarName;

    int                         nParmB;
    char                       *pParmB;
}
TActionPktCountSet;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pCmdLine;

    int                         nParmB;
    char                       *pOptVarName;
}
TActionExecute;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pCmdLine;

    int                         nParmB;
    char                       *pParmB;
}
TActionExecuteWaitRet;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nFlags;
    char                       *pCmdLine;

    int                         nParmB;
    char                       *pParmB;
}
TActionExecuteEx;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pVarName;

    int                         nParmB;
    char                       *pParmB;
}
TActionPktCountResp;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nFuncType;
    char                       *pVarName1;

    int                         nParmB;
    char                       *pVarName2;
}
TActionString;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pDelay;

    int                         nParmB;
    char                       *pParmB;
}
TActionDelay;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pTimeout;

    int                         nParmB;
    char                       *pParmB;
}
TActionDefTimeout;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pPerc;

    int                         nParmB;
    char                       *pParmB;
}
TActionTimeScalar;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pExpectedState;  //"ON" or "OFF"

    int                         nType;
    char                       *pParmB;
}
TActionModemStat;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nPort;
    char                       *pNewState;  //"ON" or "OFF"

    int                         nType;
    char                       *pParmB;
}
TActionModemSet;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nParmA;
    char                       *pLabel;

    int                         nParmB;
    char                       *pParmB;
}
TActionLabel;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    int                         nTimeoutMs; //-1 == default
    uint32                      nLineNum;  //line number in script source

    int                         eJmpCond;
    char                       *pLabelName;
    int                         nAbsIndex;
    char                       *pParmB;
}
TAutoJumpCond;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    int                         nTimeoutMs; //-1 == default
    uint32                      nLineNum;  //line number in script source

    int                         eJmpCond;
    char                       *pParmA;
    int                         nParmB;
    char                       *pParmB;
}
TAutoIfCond;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nHandle;
    char                       *pFilename;

    int                         nReadWrite;
    char                       *pPermissions;
}
TActionFileOpen;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nHandle;
    char                       *pParmA;

    int                         nParmB;
    char                       *pParmB;
}
TActionFileClose;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nHandle;
    char                       *pVarData;

    int                         nParmB;
    char                       *pVarLen;
}
TActionFileRead;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nHandle;
    char                       *pVarData;

    int                         nParmB;
    char                       *pParmB;
}
TActionFileWrite;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nType;
    char                       *pVarTimer;

    int                         nParmB;
    char                       *pParmB;
}
TActionTimerStart;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nType;
    char                       *pVarTimer;

    int                         nParmB;
    char                       *pVarElapsed;
}
TActionTimerElapse;

typedef struct
{
    EScriptAction               eAction;
    EExpectedOutcome            eOutcome;
    uint32                      nTimeout;
    uint32                      nLineNum;  //line number in script source

    int                         nType;
    char                       *pVarTimer;

    int                         nParmB;
    char                       *pExpTime;
}
TActionTimerExpired;


typedef union
{
    TAutoScript                 generic;
    TActionSend                 send;
    TAutoBreakOn                breakOn;
    TAutoBreakOff               breakOff;
    TActionSetVar               setVar;
    TActionMultiIndex           multiIndex;
    TActionSetComms             setComms;
    TActionPrint                print;
    TActionPrintE               printE;
    TActionSaveResp             saveResp;
    TActionRxCount              rxCount;
    TActionRxDataCount          rxDataCount;
    TActionDelay                delay;
    TActionDefTimeout           defTimeout;
    TActionTitle                title;
    TActionFail                 fail;
    TActionModemStat            modemStat;
    TActionModemSet             modemSet;
    TAutoFlushRx                flushRx;
    TAutoFlushTx                flushTx;
    TActionLabel                label;
    TActionAlu                  alu;
    TAutoJumpCond               jump;
    TAutoIfCond                 ifcond;
    TActionPort                 port;
    TActionPortPipe             port_pipe;
    TActionTxString             txString;
    TActionSendMpData           sendMpData;
    TActionCheckVar             checkVar;
    TActionSprintInt            sprintInt;
    TActionSprintHex            sprintHex;
    TActionHex2Dec              hex2Dec;
    TActionDec2Hex              dec2Hex;
    TActionFilterMove           filterMove;
    TActionStrDehexize          strDeHexize;
    TActionWaitResp             waitResp;
    TActionWaitRespEx           waitRespEx;
    TActionWaitTimeout          waitTimeout;
    TActionWaitUntil            waitUntil;
    TActionWaitMulti            waitMulti;
    TActionWaitPacket           waitPacket;
    TActionWaitTiEvent          waitTiEvent;
    TActionWaitData             waitData;
    TActionWaitChannel          waitChannel;
    TActionString               string;
    TActionExecute              execute;
    TActionExecuteEx            executeEx;
    TActionExecuteWaitRet       executeWaitRet;
    TActionExistFile            existFile;
    TActionExistFolder          existFolder;
    TActionFileCompare          fileCompare;
    TActionConsoleUpdate        consoleUpdate;
    TActionTrace                trace;
    TActionStrCat               strCat;
    TActionGetCwd               getCwd;
    TActionFileOpen             fileOpen;
    TActionFileClose            fileClose;
    TActionFileRead             fileRead;
    TActionFileWrite            fileWrite;
    TAutoScript                 timer;
    TActionTimerStart           timerStart;
    TActionTimerElapse          timerElapse;
    TActionTimerExpired         timerExpired;
    TActionPktCountGet          pktCountGet;
    TActionPktCountSet          pktCountSet;
    TActionPktCountResp         pktCountResp;
    TActionMid                  mid;
    TActionMessageBox           messageBox;
    TActionTimeScalar           timeScalar;
    TActionInRange              inRange;
    TActionIncludePush          includePush;
    TActionIncludePop           includePop;
}
TScriptAction;

typedef struct
{
    TScriptAction       *mpCode;
    uint32               mnMaxCodeSize;
    uint32               mnActionCount;
    uint32               mnActionFileStart;

    TScriptAction       *mpPC;
    TScriptAction        mCurrentAction;
    TScriptAction       *mpCurrentTitle;  //so that we can retry if we want
    int                  mnIncFilenameStackTopAtTitle; //required to reset inc file stack on a retry
    uint32               mCurActParmSize;
    char                *mpSubstBuffer;

    char                 mbaScriptFileName[MAX_FILNAME_LEN+1];
    char                 mbaScriptFolder[(MAX_FILNAME_LEN)+1];
    char                 mbaAppFolder[(MAX_FILNAME_LEN)+1];
    char                 mbaOtherIncludeFolder[MAX_OTHER_INCLUDES][(MAX_FILNAME_LEN)+1];
    char                 mbaLogFileFolder[(MAX_FILNAME_LEN)+1];

    TScriptVariable     *mpVariables;
    uint32               mnMaxVarSize;
    uint32               mnVarCount;

    int                  mFileNestCount;
    int                  mTotalFailCount;
    int                  mTitleFailCount;
    int                  mLabelSuffix;
    int                  mFileIncCount;

    EScriptAction        meLastAction;

    char                 mMissingVarname[MAX_VARNAME_LEN+16];
    char                 mMissingVarnameA[MAX_VARNAME_LEN+16];
    char                 mMissingVarnameB[MAX_VARNAME_LEN+16];
}
TScriptManager;

/******************************************************************************/
// Class definitions
/******************************************************************************/

/******************************************************************************/
// Union definitions
/******************************************************************************/

/******************************************************************************/
// Global Functions (API etc) exported for other modules
/******************************************************************************/
TScriptManager * SrcCreateScriptManager(uint32 nInitCodeSize, uint32 nInitMaxVariables);
void SrcClearScript(TScriptManager *pManager);
void SrcDeleteScriptManager(TScriptManager *pManager);
ESCRERROR   SrcCompileScriptFile(char *pFilename, TScriptManager *pScrMngr , AnsiString asLogPath );
TScriptAction *SrcCopyNextScriptAction(TScriptManager *pScrMngr, bool &fSubstA,bool &fSubstB);
char *SrcGetErrorText(ESCRERROR nError);
TScriptVariable * SrcGetScriptVariable(TScriptManager *pScrMngr, char *pToken);
void SrcListScript(TScriptManager *pScrMngr);
void SrcEscapeString(char *pSrc,AnsiString &asStr);
int  SrcEscapeString(char *pSrc,char *pDst, int nMaxDstLen);
int  SrcEscapeStringNullAnd0x8X(char *pSrc, int nSrcLen, char *pDst, int nMaxDstLen);
bool SrcUnescapeString(char *pStr);
bool SrcStripWhitespace(char *pStr);
ESCRERROR SrcAddSystemVariable(TScriptManager *pScrMngr,char *pVarName,char *pVarValue);
ESCRERROR SrcAddSystemArrayVariable(TScriptManager *pScrMngr,char *pVarName,char *pVarValue, int nArrLen);
bool SrcCopyAndExpandVariable(TScriptManager *pScrMngr, char *pSrc, char *pDst);
bool SrcSaveVariableValueEx(TScriptVariable *pVariable, char *pNewValue, int nValLen);
bool SrcSaveVariableValue(TScriptVariable *pVariable, char *pNewValue);
bool SrcSaveVariableValueEx(TScriptManager *pScrMngr,char *pVarName, char *pNewValue, int nValLen);
bool SrcSaveVariableValue(TScriptManager *pScrMngr,char *pVarName, char *pNewValue);
bool SrcConCatVariableValue(TScriptVariable *pVariable, char *pNewValue);
bool SrcSaveVariableValueInteger(TScriptVariable *pVariable, int nValue);
bool SrcFilterMoveVariable(TScriptVariable *pVariableDst, TScriptVariable *pVariableSrc, int nMask, bool fInvert);
bool SrcDeHexizeVariable(TScriptVariable *pVariableDst, TScriptVariable *pVariableSrc, int nMask);

#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/


