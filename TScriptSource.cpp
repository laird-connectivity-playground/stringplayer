/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Upgrade Class
**
** Module:      TSCRIPTSOURCE.CPP
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

#include "TScriptSource.h"
#include "TMainForm.h"

/******************************************************************************/
// Local Defines
/******************************************************************************/

#define MAX_SCRIPT_LINE_LENGTH          4096
#define MAX_HEXIZE_STRINGLEN            256

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

char gbaHexizedStr[((MAX_HEXIZE_STRINGLEN+2)*2)];

/******************************************************************************/
// External Function Declarations
/******************************************************************************/

/******************************************************************************/
// Local Forward Function Declarations
/******************************************************************************/
static ESCRERROR SrcCompileFile(char *pFilename, uint32 nLineNum, TScriptManager *pScrMngr );

/******************************************************************************/
/******************************************************************************/
// Local Functions or Private Members
/******************************************************************************/
/******************************************************************************/


//=============================================================================
//=============================================================================
static EScriptAction SrcConvertToEAction(char *pToken)
{
    EScriptAction eAction = ACTION_UNKNOWN;

    if( strcmpi(pToken,"END") == 0 )
    {
        eAction = ACTION_END;
    }
    else if( strcmpi(pToken,"EXIT") == 0 )
    {
        eAction = ACTION_EXIT;
    }
    else if( strcmpi(pToken,"CONSOLEUPDATE") == 0 )
    {
        eAction = ACTION_CONSOLEUPDATE;
    }
    else if( strcmpi(pToken,"TRACE") == 0 )
    {
        eAction = ACTION_TRACE;
    }
    else if( strcmpi(pToken,"TRACEWIN") == 0 )
    {
        eAction = ACTION_TRACEWIN;
    }
    else if( strcmpi(pToken,"SEND") == 0 )
    {
        eAction = ACTION_SEND;
    }
    else if( strcmpi(pToken,"INRANGE") == 0 )
    {
        eAction = ACTION_INRANGE;
    }
    else if( strcmpi(pToken,"SENDWAIT") == 0 )
    {
        eAction = ACTION_SEND;
    }
    else if( strcmpi(pToken,"SENDWAITOK") == 0 )
    {
        eAction = ACTION_SEND;
    }
    else if( strcmpi(pToken,"SENDWAITERR") == 0 )
    {
        eAction = ACTION_SEND;
    }
    else if( strcmpi(pToken,"BREAKON") == 0 )
    {
        eAction = ACTION_BRKON;
    }
    else if( strcmpi(pToken,"BREAKOFF") == 0 )
    {
        eAction = ACTION_BRKOFF;
    }
    else if( strcmpi(pToken,"DECLARE") == 0 )
    {
        eAction = ACTION_DECLARE;
    }
    else if( strcmpi(pToken,"CHECK") == 0 )
    {
        eAction = ACTION_CHECKVAR;
    }
    else if( strcmpi(pToken,"CONTAIN") == 0 )
    {
        eAction = ACTION_CONTAIN;
    }
    else if( strcmpi(pToken,"NOTCONTAIN") == 0 )
    {
        eAction = ACTION_NOTCONTAIN;
    }
    else if( strcmpi(pToken,"MATCHCOUNT") == 0 )
    {
        eAction = ACTION_MATCHCOUNT;
    }
    else if( strcmpi(pToken,"SETVAR") == 0 )
    {
        eAction = ACTION_SETVAR;
    }
    else if( strcmpi(pToken,"SPRINTINT") == 0 )
    {
        eAction = ACTION_SPRINTINT;
    }
    else if( strcmpi(pToken,"SPRINTHEX") == 0 )
    {
        eAction = ACTION_SPRINTHEX;
    }
    else if( strcmpi(pToken,"MID$") == 0 )
    {
        eAction = ACTION_MID;
    }
    else if( strcmpi(pToken,"HEX2DEC") == 0 )
    {
        eAction = ACTION_HEX2DEC;
    }
    else if( strcmpi(pToken,"DEC2HEX") == 0 )
    {
        eAction = ACTION_DEC2HEX;
    }
    else if( strcmpi(pToken,"SET") == 0 )
    {
        eAction = ACTION_SETVAR;
    }
    else if( strcmpi(pToken,"SETX") == 0 )
    {
        eAction = ACTION_SETVARX;
    }
    else if( strcmpi(pToken,"STRCAT") == 0 )
    {
        eAction = ACTION_STRCAT;
    }
    else if( strcmpi(pToken,"GETCWD") == 0 )
    {
        eAction = ACTION_GETCWD;
    }
    else if( strcmpi(pToken,"CONST") == 0 )
    {
        eAction = ACTION_CONST;
    }
    else if( strcmpi(pToken,"MULTIINDEX") == 0 )
    {
        eAction = ACTION_MULTIINDEX;
    }
    else if( strcmpi(pToken,"PORTINFO") == 0 )
    {
        eAction = ACTION_PORTINFO;
    }
    else if( strcmpi(pToken,"SAVERESP") == 0 )
    {
        eAction = ACTION_SAVERESP;
    }
    else if( strcmpi(pToken,"SAVERESPNE") == 0 )
    {
        eAction = ACTION_SAVERESP;
    }
    else if( strcmpi(pToken,"FILTERMOVE") == 0 )
    {
        eAction = ACTION_FILTERMOVE;
    }
    else if( strcmpi(pToken,"STRDEHEXIZE") == 0 )
    {
        eAction = ACTION_STRDEHEXIZE;
    }
    else if( strcmpi(pToken,"STRLEN") == 0 )
    {
        eAction = ACTION_STRING;
    }
    else if( strcmpi(pToken,"STRCMP") == 0 )
    {
        eAction = ACTION_STRING;
    }
    else if( strcmpi(pToken,"STRCMPI") == 0 )
    {
        eAction = ACTION_STRING;
    }
    else if( strcmpi(pToken,"STRSTR") == 0 )
    {
        eAction = ACTION_STRING;
    }
    else if( strcmpi(pToken,"STRSTRI") == 0 )
    {
        eAction = ACTION_STRING;
    }
    else if( strcmpi(pToken,"WAITRESP") == 0 )
    {
        eAction = ACTION_WAITRESP;
    }
    else if( strcmpi(pToken,"WAITRESPEX") == 0 )
    {
        eAction = ACTION_WAITRESPEX;
    }
    else if( strcmpi(pToken,"WAITTIMEOUT") == 0 )
    {
        eAction = ACTION_WAITTIMEOUT;
    }
    else if( strcmpi(pToken,"WAITUNTIL") == 0 )
    {
        eAction = ACTION_WAITUNTIL;
    }
    else if( strcmpi(pToken,"WAITMULTI") == 0 )
    {
        eAction = ACTION_WAITMULTI;
    }
    else if( strcmpi(pToken,"WAITDATA") == 0 )
    {
        eAction = ACTION_WAITDATA;
    }
    else if( strcmpi(pToken,"WAITCHANNEL") == 0 )
    {
        eAction = ACTION_WAITCHANNEL;
    }
    else if( strcmpi(pToken,"WAITPACKET") == 0 )
    {
        eAction = ACTION_WAITPACKET;
    }
    else if( strcmpi(pToken,"WAITTIEVENT") == 0 )
    {
        eAction = ACTION_WAIT_TIEVENT;
    }
    else if( strcmpi(pToken,"SETCOMMS") == 0 )
    {
        eAction = ACTION_SETCOMMS;
    }
    else if( strcmpi(pToken,"TXSTRING") == 0 )
    {
        eAction = ACTION_TXSTRING;
    }
    else if( strcmpi(pToken,"TXSTRINGEX") == 0 )
    {
        eAction = ACTION_TXSTRINGEX;
    }
    else if( strcmpi(pToken,"SENDCMD") == 0 )
    {
        eAction = ACTION_SENDCMD;
    }
    else if( strcmpi(pToken,"SENDMPDATA") == 0 )
    {
        eAction = ACTION_SENDMPDATA;
    }
    else if( strcmpi(pToken,"TXUNICODE") == 0 )
    {
        eAction = ACTION_TXUNICODE;
    }
    else if( strcmpi(pToken,"PORT") == 0 )
    {
        eAction = ACTION_PORT_UART;
    }
    else if( strcmpi(pToken,"PORT_UART") == 0 )
    {
        eAction = ACTION_PORT_UART;
    }
    else if( strcmpi(pToken,"PORT_CLOSE") == 0 )
    {
        eAction = ACTION_PORT_CLOSE;
    }
    else if( strcmpi(pToken,"PORT_MPOINT") == 0 )
    {
        eAction = ACTION_PORT_MPOINT;
    }
    else if( strcmpi(pToken,"PORT_HEXBIN") == 0 )
    {
        eAction = ACTION_PORT_MPOINT;
    }
    else if( strcmpi(pToken,"PORT_OC") == 0 )
    {
        eAction = ACTION_PORT_OC;
    }
    else if( strcmpi(pToken,"MESSAGEBOX") == 0 )
    {
        eAction = ACTION_MESSAGEBOX;
    }
    else if( strcmpi(pToken,"TIMER") == 0 )
    {
        eAction = ACTION_TIMER;
    }
    else if( strcmpi(pToken,"PORT_PIPE") == 0 )
    {
        eAction = ACTION_PORT_PIPE;
    }
    else if( strcmpi(pToken,"EXECUTE") == 0 )
    {
        eAction = ACTION_EXECUTE;
    }
    else if( strcmpi(pToken,"EXECUTEWAITRET") == 0 )
    {
        eAction = ACTION_EXECUTEWAITRET;
    }
    else if( strcmpi(pToken,"EXECUTEEX") == 0 )
    {
        eAction = ACTION_EXECUTEEX;
    }
    else if( strcmpi(pToken,"EXECUTEWAIT") == 0 )
    {
        eAction = ACTION_EXECUTEWAIT;
    }
    else if( strcmpi(pToken,"EXECUTEWAITEX") == 0 )
    {
        eAction = ACTION_EXECUTEWAITEX;
    }
    else if( strcmpi(pToken,"FILEEXISTS") == 0 )
    {
        eAction = ACTION_EXIST_FILE;
    }
    else if( strcmpi(pToken,"FOLDEREXISTS") == 0 )
    {
        eAction = ACTION_EXIST_FOLDER;
    }
    else if( strcmpi(pToken,"FILECOMPARE") == 0 )
    {
        eAction = ACTION_FILE_COMPARE;
    }
    else if( strcmpi(pToken,"FILEOPEN") == 0 )
    {
        eAction = ACTION_FILE_OPEN;
    }
    else if( strcmpi(pToken,"FILECLOSE") == 0 )
    {
        eAction = ACTION_FILE_CLOSE;
    }
    else if( strcmpi(pToken,"FILEREAD") == 0 )
    {
        eAction = ACTION_FILE_READ;
    }
    else if( strcmpi(pToken,"FILEWRITE") == 0 )
    {
        eAction = ACTION_FILE_WRITE;
    }
    else if( strcmpi(pToken,"PRINT") == 0 )
    {
        eAction = ACTION_PRINT;
    }
    else if( strcmpi(pToken,"PRINTE") == 0 )
    {
        eAction = ACTION_PRINTE;
    }
    else if( strcmpi(pToken,"DELAY") == 0 )
    {
        eAction = ACTION_DELAY;
    }
    else if( strcmpi(pToken,"DELAYABS") == 0 )
    {
        eAction = ACTION_DELAYABS;
    }
    else if( strcmpi(pToken,"DEFTIMEOUT") == 0 )
    {
        eAction = ACTION_DEFTIMEOUT;
    }
    else if( strcmpi(pToken,"TIMESCALAR") == 0 )
    {
        eAction = ACTION_TIMESCALAR;
    }
    else if( strcmpi(pToken,"TITLE") == 0 )
    {
        eAction = ACTION_TITLE;
    }
    else if( strcmpi(pToken,"SUBTITLE") == 0 )
    {
        eAction = ACTION_SUBTITLE;
    }
    else if( strcmpi(pToken,"FAIL") == 0 )
    {
        eAction = ACTION_FAIL;
    }
    else if( strcmpi(pToken,"CHECKCTS") == 0 )
    {
        eAction = ACTION_MDMSTAT;
    }
    else if( strcmpi(pToken,"CHECKDCD") == 0 )
    {
        eAction = ACTION_MDMSTAT;
    }
    else if( strcmpi(pToken,"CHECKDSR") == 0 )
    {
        eAction = ACTION_MDMSTAT;
    }
    else if( strcmpi(pToken,"CHECKRI") == 0 )
    {
        eAction = ACTION_MDMSTAT;
    }
    else if( strcmpi(pToken,"SETDTR") == 0 )
    {
        eAction = ACTION_MDMSET;
    }
    else if( strcmpi(pToken,"SETRTS") == 0 )
    {
        eAction = ACTION_MDMSET;
    }
    else if( strcmpi(pToken,"FLUSHRX") == 0 )
    {
        eAction = ACTION_FLUSHRX;
    }
    else if( strcmpi(pToken,"FLUSHTX") == 0 )
    {
        eAction = ACTION_FLUSHTX;
    }
    else if( strcmpi(pToken,"LABEL") == 0 )
    {
        eAction = ACTION_LABEL;
    }
    else if( strcmpi(pToken,"ABS") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"ADD") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"MUL") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"DIV") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"SUB") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"TST") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"MOD") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"AND") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"OR") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"XOR") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"ROL") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"ROR") == 0 )
    {
        eAction = ACTION_ALU;
    }
    else if( strcmpi(pToken,"HEXADD") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXMUL") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXDIV") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXSUB") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXTST") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXMOD") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXAND") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXOR") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXXOR") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXROL") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"HEXROR") == 0 )
    {
        eAction = ACTION_HEXALU;
    }
    else if( strcmpi(pToken,"JUMP") == 0 )
    {
        eAction = ACTION_JUMP;
    }
    else if( strcmpi(pToken,"JUMPZ") == 0 )
    {
        eAction = ACTION_JUMP;
    }
    else if( strcmpi(pToken,"JUMPNZ") == 0 )
    {
        eAction = ACTION_JUMP;
    }
    else if( strcmpi(pToken,"JUMPLT") == 0 )
    {
        eAction = ACTION_JUMP;
    }
    else if( strcmpi(pToken,"JUMPLE") == 0 )
    {
        eAction = ACTION_JUMP;
    }
    else if( strcmpi(pToken,"JUMPGT") == 0 )
    {
        eAction = ACTION_JUMP;
    }
    else if( strcmpi(pToken,"JUMPGE") == 0 )
    {
        eAction = ACTION_JUMP;
    }
    else if( strcmpi(pToken,"IFZ") == 0 )
    {
        eAction = ACTION_IF;
    }
    else if( strcmpi(pToken,"IFNZ") == 0 )
    {
        eAction = ACTION_IF;
    }
    else if( strcmpi(pToken,"IFLT") == 0 )
    {
        eAction = ACTION_IF;
    }
    else if( strcmpi(pToken,"IFLE") == 0 )
    {
        eAction = ACTION_IF;
    }
    else if( strcmpi(pToken,"IFGT") == 0 )
    {
        eAction = ACTION_IF;
    }
    else if( strcmpi(pToken,"IFGE") == 0 )
    {
        eAction = ACTION_IF;
    }
    else if( strcmpi(pToken,"PKTCOUNTGET") == 0 )
    {
        eAction = ACTION_PKTCOUNTGET;
    }
    else if( strcmpi(pToken,"PKTCOUNTSET") == 0 )
    {
        eAction = ACTION_PKTCOUNTSET;
    }
    else if( strcmpi(pToken,"PKTCOUNTRESP") == 0 )
    {
        eAction = ACTION_PKTCOUNTRESP;
    }

/*
    else if( strcmpi(pToken,"XXX") == 0 )
    {
        eAction = ACTION_;
    }
*/

    return eAction;
}

//=============================================================================
//=============================================================================
static EExpectedOutcome SrcConvertToEOutcome(char *pToken)
{
    EExpectedOutcome eOutcome = OUTCOME_UNKNOWN;

    if( strcmpi(pToken,"NONE") == 0 )
    {
        eOutcome = OUTCOME_NONE;
    }
    else if( strcmpi(pToken,"OK") == 0 )
    {
        eOutcome = OUTCOME_OK;
    }
    else if( strcmpi(pToken,"ERROR") == 0 )
    {
        eOutcome = OUTCOME_ERROR;
    }
    else if( strcmpi(pToken,"MOD_TIMEOUT") == 0 )
    {
        eOutcome = OUTCOME_MOD_TIMEOUT;
    }
    else if( strcmpi(pToken,"SCR_TIMEOUT") == 0 )
    {
        eOutcome = OUTCOME_SCR_TIMEOUT;
    }
    else if( strcmpi(pToken,"PATTERN") == 0 )
    {
        eOutcome = OUTCOME_PATTERN;
    }
    else if( strcmpi(pToken,"PACKET") == 0 )
    {
        eOutcome = OUTCOME_PACKET;
    }
    else if( strcmpi(pToken,"HCI_LE_EVENT") == 0 )
    {
        eOutcome = OUTCOME_HCI_LE_EVENT;
    }

/*
    else if( strcmpi(pToken,"XXX") == 0 )
    {
        eOutcome = OUTCOME_;
    }
*/

    return eOutcome;
}

//=============================================================================
//=============================================================================
static int SrcConvertToInteger(char *pToken, ESCRERROR &nError)
{
    nError = SCRERROR_SYNTAX;
    int nRetVal = 0;
    if( (pToken[0]=='-') || (pToken[0]=='+') || isdigit(pToken[0]) )
    {
        nRetVal = atoi(pToken);
        nError = SCRERROR_NONE;
    }
    return nRetVal;
}

//=============================================================================
//=============================================================================
static char *SrcAdvanceToNonWhiteSpace(char *pStr)
{
    while(*pStr)
    {
        if(*pStr > ' ')break;
        pStr++;
    }
    return pStr;
}

//=============================================================================
//=============================================================================
static int SrcConvertStrToInteger(char *pToken, ESCRERROR &nError)
{
    nError = SCRERROR_SYNTAX;
    long nRetVal = 0;
    char *pPtr;
    int base=10;
    bool fPos=true;
    char *pStart=pToken;
    int nTknLen=strlen( pToken );

    pToken=SrcAdvanceToNonWhiteSpace(pToken);

    if(pToken[0]=='+')
    {
        fPos=true;
        pToken++;
        if( !isdigit(pToken[0]) )
        {
            nError = SCRERROR_NOT_DECIMAL_NUMBER;
            return 0;
        }
    }
    else if(pToken[0]=='-')
    {
        fPos=false;
        pToken++;
        if( !isdigit(pToken[0]) )
        {
            nError =  SCRERROR_NOT_DECIMAL_NUMBER;
            return 0;
        }
    }
    else if( (pToken[0]=='0') && (pToken[1]=='x') )
    {
        base=16;
        pToken+=2;
        if( !isxdigit(pToken[0]) )
        {
            nError =  SCRERROR_NOT_HEX_NUMBER;
            return 0;
        }
    }
    else if( (pToken[0]=='0') && (pToken[1]=='b') )
    {
        base=2;
        pToken+=2;
        if( ! (isdigit(pToken[0]) && (pToken[0]<'2')) )
        {
            nError = SCRERROR_NOT_BINARY_NUMBER;
            return 0;
        }
    }
    else if( (pToken[0]=='0') && (pToken[1]=='o') )
    {
        base=8;
        pToken+=2;
        if( ! (isdigit(pToken[0]) && (pToken[0]<'8')) )
        {
            nError = SCRERROR_NOT_OCTAL_NUMBER;
            return 0;
        }
    }

    nRetVal = strtol(pToken,&pPtr,base);
    if( (pPtr-pStart) == nTknLen )
    {
        nError = SCRERROR_NONE;
    }
    else
    {
        nError = SCRERROR_NUMBER_INVALID_CHR;
    }

    return nRetVal;
}

//=============================================================================
//=============================================================================
static bool SrcValidateForUnescapeSequence(char *pStr)
{
    //returns new length
    bool fRetVal = true;
    char *pSrc = pStr;

    if(pStr)
    {
        while(*pSrc)
        {
            if( pSrc[0] == '\\' )
            {
                if( pSrc[1]=='r')
                {
                    pSrc += 2;
                }
                else if(pSrc[1]=='n')
                {
                    pSrc += 2;
                }
                else if(pSrc[1]=='b')
                {
                    pSrc += 2;
                }
                else if(pSrc[1]=='\\')
                {
                    pSrc += 2;
                }
                else if(pSrc[1]=='%')
                {
                    pSrc += 2;
                }
                else if(pSrc[1]=='[')
                {
                    pSrc += 2;
                }
                else if(pSrc[1]==']')
                {
                    pSrc += 2;
                }
                else if(isxdigit(pSrc[1]) && isxdigit(pSrc[2]) )
                {
                    pSrc += 3;
                }
                else
                {
                    fRetVal = false;
                    break;
                }
            }
            else
            {
                pSrc++;
            }
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
static char *SrcCopyStringToNewBlock(char *pToken, int nTokenLen, ESCRERROR &nError)
{
    char *pRetVal = NULL;
    nError = SCRERROR_SYNTAX;
    if( nTokenLen >= 2 )
    {
        nError = SCRERROR_OUTOFMEM;
        int nLen = nTokenLen-2;
        pRetVal = (char *)MiscMemAlloc(nLen+1);
        if(pRetVal)
        {
            nError = SCRERROR_NONE;
            if(nLen)
            {
                strncpy(pRetVal,&pToken[1],nLen);
            }
            pRetVal[nLen]=0;
            if(!SrcValidateForUnescapeSequence(pRetVal))
            {
                nError=SCRERROR_INCOMPLETE_ESCAPE;
            }
        }
    }
    return pRetVal;
}

//=============================================================================
//=============================================================================
static char *SrcCopyUnquotedStringToNewBlock(char *pToken, int nTokenLen, ESCRERROR &nError)
{
    char *pRetVal = NULL;
    nError = SCRERROR_SYNTAX;
    if( nTokenLen )
    {
        nError = SCRERROR_OUTOFMEM;
        pRetVal = (char *)MiscMemAlloc(nTokenLen+1);
        if(pRetVal)
        {
            nError = SCRERROR_NONE;
            strncpy(pRetVal,pToken,nTokenLen);
            pRetVal[nTokenLen]=0;
            if(!SrcValidateForUnescapeSequence(pRetVal))
            {
                nError=SCRERROR_INCOMPLETE_ESCAPE;
            }
        }
    }
    return pRetVal;
}

//=============================================================================
//=============================================================================
static char *SrcCopyUnquotedStringToNewBlockNoUnEscape(char *pToken, int nTokenLen, ESCRERROR &nError)
{
    char *pRetVal = NULL;
    nError = SCRERROR_SYNTAX;
    if( nTokenLen )
    {
        nError = SCRERROR_OUTOFMEM;
        pRetVal = (char *)MiscMemAlloc(nTokenLen+1);
        if(pRetVal)
        {
            nError = SCRERROR_NONE;
            strncpy(pRetVal,pToken,nTokenLen);
            pRetVal[nTokenLen]=0;
        }
    }
    return pRetVal;
}

//=============================================================================
//=============================================================================
static char *SrcCopyLabelToNewBlock(char *pToken, int nTokenLen, char *pSuffix, ESCRERROR &nError)
{
    char *pRetVal = NULL;
    nError = SCRERROR_SYNTAX;
    if( nTokenLen > 0 )
    {
        nError = SCRERROR_OUTOFMEM;
        pRetVal = (char *)MiscMemAlloc(nTokenLen+1+strlen(pSuffix));
        if(pRetVal)
        {
            memcpy(pRetVal,pToken,nTokenLen);
            strcpy(&pRetVal[nTokenLen],pSuffix);
            nError = SCRERROR_NONE;
        }
    }
    return pRetVal;
}

//=============================================================================
//=============================================================================
static uint32 SrcDetectTokenLength(char *pLine, ESCRERROR &nError)
{
    uint32 nRetVal=0;
    bool fInQuotes=false;

    nError = SCRERROR_NONE;

    while(*pLine)
    {
        if( ! fInQuotes )
        {   //outside "
            if( (unsigned char)pLine[0] <= ' ')
            {
                break;
            }
            else if( (pLine[0] == '"' ) && (nRetVal==0) )
            {
                fInQuotes=true;
            }
        }
        else
        {   //inside "
            if( pLine[0] == '"' )
            {
                if( pLine[1] != '"' )
                {
                    fInQuotes=false;
                    nRetVal++;
                    break;
                }
            }

        }

        pLine++;
        nRetVal++;
    }

    if(fInQuotes)nError = SCRERROR_UNTERM_STRING;

    return nRetVal;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcIsValidateToken(char *pToken)
{
    return SCRERROR_NONE;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcIsTokenDecimalNumber(char *pToken,bool fSigned)
{
    //look for even number of '%' characters which are used to
    //embed replacement strings
    ESCRERROR nError = SCRERROR_NONE;

    if(fSigned)
    {
        if ( (*pToken=='-') || (*pToken=='+') )
        {
            pToken++;
        }
    }

    while(*pToken)
    {
        if( !isdigit(*pToken) )
        {
            nError = SCRERROR_NOT_DECIMAL_NUMBER;
        }
        pToken++;
    }

    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcIsTokenLogicValue(char *pToken)
{
    //look for either "true" or "false"
    ESCRERROR nError = SCRERROR_INV_LOGIC_VALUE;

    if( (strcmpi(pToken,"true")==0) || (strcmpi(pToken,"false")==0) )
    {
        nError = SCRERROR_NONE;
    }

    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcIsTokenEnumValue(char *pToken)
{
    //look for either "true" or "false"  or "clear"
    ESCRERROR nError = SCRERROR_INV_LOGIC_VALUE;

    if( (strcmpi(pToken,"true")==0) ||
        (strcmpi(pToken,"false")==0) ||
        (strcmpi(pToken,"clear")==0)
       )
    {
        nError = SCRERROR_NONE;
    }

    return nError;
}

//=============================================================================
//=============================================================================
static int SrcConvertLogicToNumber(char *pToken)
{
    int nRetVal = 0;
    if(strcmpi(pToken,"true")==0)
    {
        nRetVal = 1;
    }
    return nRetVal;
}

//=============================================================================
//=============================================================================
static int SrcConvertEnumToNumber(char *pToken)
{
    int nRetVal = 0;
    if(strcmpi(pToken,"true")==0)
    {
        nRetVal = 1;
    }
    else if(strcmpi(pToken,"false")==0)
    {
        nRetVal = 0;
    }
    else if(strcmpi(pToken,"clear")==0)
    {
        nRetVal = 2;
    }
    return nRetVal;
}


//=============================================================================
//=============================================================================
static char *SrcExtractToken(char *pLine, char *pToken, uint32 &nTokenLen, ESCRERROR &nError)
{
    pLine = SrcAdvanceToNonWhiteSpace(pLine);
    nTokenLen = SrcDetectTokenLength(pLine,nError);
    if(nError == SCRERROR_NONE)
    {
        if(nTokenLen > MAX_SCRIPT_LINE_LENGTH)nTokenLen=MAX_SCRIPT_LINE_LENGTH;
        memcpy(pToken,pLine,nTokenLen);
        pToken[nTokenLen]=0;
        pLine += nTokenLen;
        nError = SrcIsValidateToken(pToken);
    }
    return pLine;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCheckRestOfLineIsComment(char *pLine)
{
    //check that the rest of the line is a comment
    pLine  = SrcAdvanceToNonWhiteSpace(pLine);
    if( *pLine)
    {
        if( !((pLine[0]=='/') && (pLine[1]=='/')) )return SCRERROR_UNEXPECTED_SOURCE;
    }
    return SCRERROR_NONE;
}

//=============================================================================
//=============================================================================
void SrcHexize(char *pDest, const char *pSrc, int nStrLen)
{
    while( nStrLen && *pSrc )
    {
        sprintf( pDest, "%02X", *pSrc);
        pDest += 2;
        nStrLen--;
        pSrc++;
    }
    *pDest=0;
}

//=============================================================================
//=============================================================================
static char *SrcGetVarValue(TScriptManager *pScrMngr,char *pVarName)
{
    //char *pRetVal = "$VAR_UNKNOWN$";
    char *pRetVal = NULL;
    if( pVarName[0] == '%' )
    {
        pVarName++;
        int nStrLen = strlen(pVarName);
        if( nStrLen <= MAX_HEXIZE_STRINGLEN )
        {
            SrcHexize(gbaHexizedStr,pVarName,nStrLen);
            pRetVal = gbaHexizedStr;
        }
    }
    else
    {
        TScriptVariable *pVariables = SrcGetScriptVariable(pScrMngr, pVarName);
        if( pVariables )
        {
            pRetVal = pVariables->pValue;
        }
    }
    return pRetVal;
}
//=============================================================================
// Returns the number of \ characters preceding pSrc.
//=============================================================================
static int SrcPrecedingBackslashCount(char *pSrc, char *pStart)
{
    int nRetVal=0;
    pSrc--;
    while(pSrc >= pStart)
    {
        if( *pSrc == '\\' )
        {
            nRetVal++;
        }
        else
        {
            break;
        }
        pSrc--;
    }

    return nRetVal;
}
//=============================================================================
//=============================================================================
static char *SrcNextBracketChar(char *pSrc)
{
    char *pStr = pSrc;
    while(*pStr)
    {
        if( (*pStr=='[') || (*pStr==']') )
        {
            int nBackslashCount=SrcPrecedingBackslashCount(pStr, pSrc);
            if( (nBackslashCount& 0x01) == 0x00 )
            {   //Even so this one is NOT escaped
                break;
            }
        }
        pStr++;
    }
    return pStr;
}

//=============================================================================
//=============================================================================
static char *SrcNextOpenBracketChar(char *pSrc)
{
    char *pStr = pSrc;
    int nBackslashCount;
    while(*pStr)
    {
        if(*pStr=='[')
        {
            nBackslashCount=SrcPrecedingBackslashCount(pStr, pSrc);
            if( (nBackslashCount&0x01) == 0x00 )
            {   //Even so this one is NOT escaped
                break;
            }
        }
        pStr++;
    }
    return pStr;
}

//=============================================================================
// Replace [name] by the content of that variable.
// \[ is ignored.
//=============================================================================
static bool SrcBracketedSubstitution(TScriptManager *pScrMngr, char *pParm, bool fErrNoValue)
{
    bool fRetVal = true; //ok
    char *pTempBuf  = pScrMngr->mpSubstBuffer;
    char *pVarStart,*pVarEnd;

    pVarStart = SrcNextOpenBracketChar(pParm);
    while(*pVarStart)
    {
        char *pNextBracket = SrcNextBracketChar(pVarStart+1);
        if( *pNextBracket == '[' )
        {
            fRetVal = SrcBracketedSubstitution(pScrMngr,pVarStart+1,fErrNoValue);
        }
        if(fRetVal)
        {
            pVarEnd = strchr(pVarStart,']');
            if(pVarEnd)
            {
                strcpy(pTempBuf,(pVarEnd+1)); //copy all from ] onwards for later concatenation
                *pVarEnd = 0;
                char *pVarValue = SrcGetVarValue(pScrMngr,(pVarStart+1));
                if( pVarValue )
                {
                    //variable has been found so substitute
                    strcpy(pVarStart,pVarValue);
                }
                else
                {
                    //variable NOT found, so abort if flagged to do so
                    strncpy(pScrMngr->mMissingVarname,pVarStart+1,MAX_VARNAME_LEN);
                    pScrMngr->mMissingVarname[MAX_VARNAME_LEN]=0;
                    *pVarStart=0;
                    if(fErrNoValue)
                    {
                        fRetVal=false;
                        break;
                    }
                }
                strcat(pVarStart,pTempBuf);  //append what was beyond the ] character
            }
            else
            {
                fRetVal=false;
                break;
            }
        }
        pVarStart = SrcNextOpenBracketChar(pParm);
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
static bool SrcVariableSubstitution(TScriptManager *pScrMngr, char *pSrcParm, char *pDstParm, bool fErrNoValue)
{
#if defined(BRACKETED_SUBSTITUTION)

    //Innermost "[xxx]" is replaced by "yyy..yyy" where yyy is the value of the string xxx
    strcpy(pDstParm,pSrcParm);
    return SrcBracketedSubstitution(pScrMngr, pDstParm, fErrNoValue);

#else
    //"%xxx%" is replaced by "yyy..yyy" where yyy is the value of the string xxx
    //"%%" is replaced by ""

    bool    fRetVal = true; //ok
    int     nFragLen;
    int     nVarNameSize;
    char    *pPerc=NULL;
    char    *pTermPerc=NULL;
    char    *pSrc  = pScrMngr->mpSubstBuffer;
    char    *pDest;

    strcpy(pSrc,pSrcParm);

    do
    {
        pDest = pDstParm;
        pPerc = strchr(pSrc,'%');
        while(pPerc)
        {
            nFragLen      = pPerc - pSrc;
            pTermPerc     = strchr(pPerc+1,'%');
            if(pTermPerc)
            {
                nVarNameSize  = pTermPerc - pPerc;
                if(nVarNameSize>1)
                {   //%xx..xx%
                    memcpy(pDest,pSrc,nFragLen);
                    pDest += nFragLen;

                    char *pVarName  = pPerc+1;
                    *pTermPerc      = 0;
                    char *pVarValue = SrcGetVarValue(pScrMngr,pVarName);
                    if( pVarValue )
                    {
                        int nVarValueSize = strlen(pVarValue);
                        memcpy(pDest,pVarValue,nVarValueSize);
                        pDest+=nVarValueSize;
                    }
                    else
                    {
                        if(fErrNoValue)
                        {
                            fRetVal=false;
                            break;
                        }
                    }
                    pSrc = pTermPerc+1;
                }
                else
                {   //%%
                    memcpy(pDest,pSrc,nFragLen);
                    pDest += nFragLen;
                    pSrc  += (nFragLen+2);
                }
                *pDest = 0;
                strcpy(pScrMngr->mpSubstBuffer,pSrc);
                pSrc  = pScrMngr->mpSubstBuffer;
                pPerc = strchr(pSrc,'%');
            }
            else
            {
                fRetVal=false;
                break;
            }
        }

        if( fRetVal)
        {
            strcpy(pDest,pSrc);

            //now check if further substitution required
            pPerc = strchr(pDstParm,'%') ;
            if(pPerc)
            {
                pSrc  = pScrMngr->mpSubstBuffer;
                strcpy(pSrc,pDstParm);
            }
        }
    }
    while(pPerc && fRetVal);
    return fRetVal;
#endif
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcVariableSubstitution(TScriptManager *pScrMngr, char *pSrcParm, bool fErrNoValue)
{
#if defined(BRACKETED_SUBSTITUTION)

    bool fOk = SrcBracketedSubstitution(pScrMngr, pSrcParm, fErrNoValue);
    return (fOk) ? SCRERROR_NONE : SCRERROR_CANNOT_RESOLVE_VAR;

#else
    ESCRERROR nError = SCRERROR_OUTOFMEM;
    char *pTempBuf = (char *)MiscMemAlloc(pScrMngr->mCurActParmSize);
    if(pTempBuf)
    {
        nError = SCRERROR_NONE;
        if( SrcVariableSubstitution(pScrMngr, pSrcParm, pTempBuf, true) )
        {
            strcpy(pSrcParm,pTempBuf);
        }
        else
        {
            nError = SCRERROR_CANNOT_RESOLVE_VAR;
        }
        MiscMemFree(pTempBuf);
    }
    return nError;
#endif
}
//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEND(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //END

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEXIT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //END

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static bool SrcIsValidVarCharacter(char ch)
{
    if(ch=='_')return true;
    if(ch<'0')return false;
    if( (ch>=':')&&(ch<='?')  )return false;
    if( (ch>='[')&&(ch<=0x60) )return false;
    if(  ch>='{'              )return false;

    //otherwise ok
    return true;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcIsValidVarName(char *pToken)
{
    ESCRERROR nError = SCRERROR_ILLEGAL_VARNAME;
    int nLen = strlen(pToken);
    if(nLen>0)
    {
        if( !isdigit(pToken[0]) && SrcIsValidVarCharacter(pToken[0]) )
        {
            pToken++;
            nError = SCRERROR_NONE;
            while(*pToken)
            {
                if( ! SrcIsValidVarCharacter(pToken[0]) )
                {
                    nError = SCRERROR_ILLEGAL_VARNAME;
                    break;
                }
                pToken++;
            }
        }
    }

    return nError;
}

//=============================================================================
//=============================================================================
static bool SrcVarNameExists(TScriptManager *pScrMngr, char *pToken)
{
    return SrcGetScriptVariable(pScrMngr,pToken) ? true : false;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileWAITPACKET(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //WAITPACKET timeoutMs portnum varname

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_PACKET;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileWAITTIEVENT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //WAITPACKET timeoutMs portnum varname

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_HCI_LE_EVENT;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"matchstring"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}


//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSEND(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SEND timeout porta "txstr" portb "rspStr"

    ESCRERROR nError;
    uint32    nTokenLen;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"txstr"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //portb
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmB = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmB >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"rspStr"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}


//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileINRANGE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //INRANGE perc "targetvalue" "value"       //perc 0..100

    ESCRERROR nError;
    uint32    nTokenLen;

    //perc
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if((unsigned int)scriptObj.nParmA > 100)return SCRERROR_INVALID_VALUE;

    //"targetvalue"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSENDWAIT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SENDAIT timeout porta "txstr"

    ESCRERROR nError;
    uint32    nTokenLen;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"txstr"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileCompositeSEND(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;

    if( strcmpi(pToken,"SEND") == 0 )
    {
        scriptObj.eOutcome   = OUTCOME_PATTERN;
        nError = SrcCompileSEND(pScrMngr,pLine,scriptObj,pToken);
    }
    else if( strcmpi(pToken,"SENDWAIT") == 0 )
    {
        scriptObj.eOutcome   = OUTCOME_MOD_TIMEOUT;
        nError = SrcCompileSENDWAIT(pScrMngr,pLine,scriptObj,pToken);
    }
    else if( strcmpi(pToken,"SENDWAITOK") == 0 )
    {
        scriptObj.eOutcome   = OUTCOME_OK;
        nError = SrcCompileSENDWAIT(pScrMngr,pLine,scriptObj,pToken);
        scriptObj.nParmB = scriptObj.nParmA;
    }
    else if( strcmpi(pToken,"SENDWAITERR") == 0 )
    {
        scriptObj.eOutcome   = OUTCOME_ERROR;
        nError = SrcCompileSENDWAIT(pScrMngr,pLine,scriptObj,pToken);
        scriptObj.nParmB = scriptObj.nParmA;
    }
    else
    {
        nError =  SCRERROR_UNKNOWN_ACTION;
    }

    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileWAITRESP(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //WAITRESP timeout porta "respstr"

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_PATTERN;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"respstr"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileWAITRESPEX_TIMEOUT_UNTIL(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //WAITRESPEX timeout porta "respstr" optional_varname

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_PATTERN;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"respstr"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //optional_varname
    nError = SrcCheckRestOfLineIsComment(pLine);
    if(nError == SCRERROR_NONE)
    {
        //optional variable not specified
        scriptObj.pParmB = NULL;
        return nError;
    }
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmB = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileWAITMULTI(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //WAITMULTI timeout porta "str0|str1|str2..."

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_PATTERN;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"str0|str1|str2..."
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileWAITDATA(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //WAITDATA timeout porta channel "respstr"

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_PATTERN;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //channel
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmB = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if((unsigned int)scriptObj.nParmB == 0)return SCRERROR_INVALID_CHANNEL;
    if((unsigned int)scriptObj.nParmB >= 256)return SCRERROR_INVALID_CHANNEL;

    //"respstr"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}
//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileWAITCHANNEL(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    return SrcCompileWAITDATA(pScrMngr,pLine,scriptObj,pToken);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileBRKON_OFF(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_MOD_TIMEOUT;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //nTimeout
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if( pToken[0] != '"')
    {
        if( strstr(pToken,"[") == 0 )
        {
            nError = SrcIsTokenDecimalNumber(pToken,true);
            if(nError != SCRERROR_NONE)return nError;
            scriptObj.nTimeoutMs = SrcConvertToInteger(pToken,nError);
            if(nError != SCRERROR_NONE)return nError;
            scriptObj.pParmA = NULL;
        }
        else
        {
            scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);
            if(nError != SCRERROR_NONE)return nError;
            scriptObj.nTimeoutMs = 0;
        }
    }
    else
    {
        scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
        if(nError != SCRERROR_NONE)return nError;
        scriptObj.nTimeoutMs = 0;
    }

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFLUSHRX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSETCOMMS(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"commparm"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePORT_UART(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"comport"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //"commparm"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePORT_MPOINT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    return SrcCompilePORT_UART(pScrMngr,pLine,scriptObj,pToken);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePORT_OC(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    return SrcCompilePORT_UART(pScrMngr,pLine,scriptObj,pToken);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileMESSAGEBOX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"text"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //"caption"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //buttons
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePORT_CLOSE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePORT_PIPE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"pAppPathNameOpts"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //"pAppCurrentFolder"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEXECUTE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"appname optional args"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEXECUTEEX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    // flags "appname optional args"

    //flags
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"appname optional args"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEXECUTEWAIT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"appname optional args"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEXECUTEWAITRET(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"appname optional args"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //varname for exit code
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmB = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);
    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEXECUTEWAITEX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //EXECUTEWAITEX <nFlags> <appname and optional paramters>  optional_varname

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    // flags "appname optional args"

    //flags
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"appname optional args"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //optional_varname
    nError = SrcCheckRestOfLineIsComment(pLine);
    if(nError == SCRERROR_NONE)
    {
        //optional variable not specified
        scriptObj.pParmB = NULL;
        return nError;
    }
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmB = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);
    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileTRACE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //true/false
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenEnumValue(pToken);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertEnumToNumber(pToken);

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileCONSOLEUPDATE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //true/false
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenLogicValue(pToken);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertLogicToNumber(pToken);

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEXISTFILE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //true/false
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenLogicValue(pToken);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertLogicToNumber(pToken);

    //"appname optional args"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileEXISTFOLDER(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //true/false
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenLogicValue(pToken);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertLogicToNumber(pToken);

    //"appname optional args"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFILECOMPARE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //true/false
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenLogicValue(pToken);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertLogicToNumber(pToken);

    //"filenameA"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //"filenameB"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePRINT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"print_message_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcIsValidLabelName(char *pToken)
{
    ESCRERROR nError = SCRERROR_ILLEGAL_LABELNAME;
    int nLen = strlen(pToken);
    if(nLen>0)
    {
        if( !isdigit(pToken[0]) && SrcIsValidVarCharacter(pToken[0]) )
        {
            pToken++;
            nError = SCRERROR_NONE;
            while(*pToken)
            {
                if( ! SrcIsValidVarCharacter(pToken[0]) )
                {
                    nError = SCRERROR_ILLEGAL_VARNAME;
                    break;
                }
                pToken++;
            }
        }
    }

    return nError;
}

//=============================================================================
//=============================================================================
static bool SrcLabelNameExists(TScriptManager *pScrMngr,uint32 nStartIndex, char *pLabel,uint32 &nActionPc)
{
    bool fRetVal = false;
    uint32 nIndex = nStartIndex; //pScrMngr->mnActionFileStart;
    while( nIndex < pScrMngr->mnActionCount)
    {
        if(pScrMngr->mpCode[nIndex].generic.eAction == ACTION_LABEL)
        {
            if(strcmpi(pScrMngr->mpCode[nIndex].label.pLabel,pLabel)==0)
            {
                fRetVal = true;
                nActionPc = nIndex;
                break;
            }
        }
        nIndex++;
    }

    return fRetVal;
}

//=============================================================================
//=============================================================================
static TScriptAction *SrcFixJumpLabels(TScriptManager *pScrMngr)
{
    TScriptAction *pRetVal = NULL;
    uint32  nIndex = 0;
    uint32  nLabelPos;

    while( nIndex < pScrMngr->mnActionCount)
    {
        if(pScrMngr->mpCode[nIndex].generic.eAction == ACTION_JUMP)
        {
            if( SrcLabelNameExists(pScrMngr,0,pScrMngr->mpCode[nIndex].generic.pParmA,nLabelPos) )
            {
                pScrMngr->mpCode[nIndex].generic.nParmB = nLabelPos;
            }
            else
            {
                pRetVal = &pScrMngr->mpCode[nIndex];
                break;
            }
        }
        nIndex++;
    }

    return pRetVal;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileLABEL(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;
    uint32    nLabelPos;
    char baBuf[16];

    scriptObj.eOutcome   = OUTCOME_NONE;
    sprintf(baBuf,"_%06d",pScrMngr->mLabelSuffix);

    //"label_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidLabelName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.pParmA = SrcCopyLabelToNewBlock(pToken, nTokenLen, baBuf, nError);
    if(nError != SCRERROR_NONE)return nError;
    if( SrcLabelNameExists(pScrMngr,pScrMngr->mnActionFileStart,scriptObj.pParmA,nLabelPos) )return SCRERROR_MULTIPLE_LABEL;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileJUMP(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;
    uint32    nLabelPos;
    char baBuf[16];

    scriptObj.eOutcome   = OUTCOME_NONE;
    sprintf(baBuf,"_%06d",pScrMngr->mLabelSuffix);

    if( strcmpi(pToken,"JUMP")==0)
    {
        scriptObj.nParmA = JMP_ALWAYS;
    }
    else if( strcmpi(pToken,"JUMPZ")==0)
    {
        scriptObj.nParmA = JMP_Z;
    }
    else if( strcmpi(pToken,"JUMPNZ")==0)
    {
        scriptObj.nParmA = JMP_NZ;
    }
    else if( strcmpi(pToken,"JUMPLT")==0)
    {
        scriptObj.nParmA = JMP_LT;
    }
    else if( strcmpi(pToken,"JUMPLE")==0)
    {
        scriptObj.nParmA = JMP_LE;
    }
    else if( strcmpi(pToken,"JUMPGT")==0)
    {
        scriptObj.nParmA = JMP_GT;
    }
    else if( strcmpi(pToken,"JUMPGE")==0)
    {
        scriptObj.nParmA = JMP_GE;
    }
    else
        return SCRERROR_UNKNOWN_ACTION;

    //"label_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidLabelName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.pParmA = SrcCopyLabelToNewBlock(pToken, nTokenLen, baBuf, nError);
    if(nError != SCRERROR_NONE)return nError;
#if 0
    if( ! SrcLabelNameExists(pScrMngr,pScrMngr->mnActionFileStart,scriptObj.pParmA,nLabelPos) )return SCRERROR_UNKNOWN_LABELNAME;
    scriptObj.nParmB = nLabelPos;
#else
    scriptObj.nParmB = 0;
#endif

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileIF(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    if( strcmpi(pToken,"IFZ")==0)
    {
        scriptObj.nParmA = JMP_Z;
    }
    else if( strcmpi(pToken,"IFNZ")==0)
    {
        scriptObj.nParmA = JMP_NZ;
    }
    else if( strcmpi(pToken,"IFLT")==0)
    {
        scriptObj.nParmA = JMP_LT;
    }
    else if( strcmpi(pToken,"IFLE")==0)
    {
        scriptObj.nParmA = JMP_LE;
    }
    else if( strcmpi(pToken,"IFGT")==0)
    {
        scriptObj.nParmA = JMP_GT;
    }
    else if( strcmpi(pToken,"IFGE")==0)
    {
        scriptObj.nParmA = JMP_GE;
    }
    else
        return SCRERROR_UNKNOWN_ACTION;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileTITLE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"test_message_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFAIL(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"test_message_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileDELAY(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_SCR_TIMEOUT;

    //"delay_value_as_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileDELAYABS(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_SCR_TIMEOUT;

    //"delay_value_as_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileDEFTIMEOUT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"default_alue_as_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileTIMESCALAR(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //"default_alue_as_string"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileMDMSTAT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    if( strcmpi(pToken,"CHECKCTS")==0)
    {
        scriptObj.nParmB = MDMSTAT_CTS;
    }
    else if( strcmpi(pToken,"CHECKDSR")==0)
    {
        scriptObj.nParmB = MDMSTAT_DSR;
    }
    else if( strcmpi(pToken,"CHECKDCD")==0)
    {
        scriptObj.nParmB = MDMSTAT_DCD;
    }
    else if( strcmpi(pToken,"CHECKRI")==0)
    {
        scriptObj.nParmB = MDMSTAT_RI;
    }
    else
    {
        return SCRERROR_UNKNOWN_ACTION;
    }

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"expected_state"  must be "ON" or "OFF"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileMDMSET(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    if( strcmpi(pToken,"SETDTR")==0)
    {
        scriptObj.nParmB = MDMLINE_DTR;
    }
    else if( strcmpi(pToken,"SETRTS")==0)
    {
        scriptObj.nParmB = MDMLINE_RTS;
    }
    else
    {
        return SCRERROR_UNKNOWN_ACTION;
    }

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"desired_state"  must be "ON" or "OFF"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcAddVariable(TScriptManager *pScrMngr, EVARTYPE varType, char *pToken, uint32 nTokenLen)
{
    ESCRERROR nError=SCRERROR_OUTOFMEM;
    TScriptVariable *pVariable = &pScrMngr->mpVariables[pScrMngr->mnVarCount];

    pVariable->pName = (char *)MiscMemAlloc(nTokenLen+1);
    if(pVariable->pName)
    {
        memcpy(pVariable->pName,pToken,nTokenLen);
        pVariable->pName[nTokenLen]=0;
        pVariable->pValue=NULL;
        pVariable->eVarType = varType;
        pScrMngr->mnVarCount++;
        nError = SCRERROR_NONE;
    }

    if( pScrMngr->mnVarCount >= pScrMngr->mnMaxVarSize )
    {
        nError=SCRERROR_OUTOFMEM;
        int nNewSize      = pScrMngr->mnMaxVarSize + 1024;
        int nNewBlockSize = nNewSize * sizeof(TScriptVariable);
        TScriptVariable *pNewVarBlock = (TScriptVariable *)MiscMemAlloc( nNewBlockSize );
        if(pNewVarBlock)
        {
            memset(pNewVarBlock,0,nNewBlockSize);
            int nOldSize      = pScrMngr->mnMaxVarSize * sizeof(TScriptVariable);
            memcpy(pNewVarBlock,pScrMngr->mpVariables,nOldSize);
            MiscMemFree(pScrMngr->mpVariables);
            pScrMngr->mpVariables  = pNewVarBlock;
            pScrMngr->mnMaxVarSize = nNewSize;
            nError = SCRERROR_NONE;
        }
    }

    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileDECLARE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //DECLARE varname

    ESCRERROR nError;
    uint32    nTokenLen;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )
    {
        nError = SrcAddVariable(pScrMngr,VARTYPE_STRING,pToken,nTokenLen);
    }
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileCONST(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //CONST varname portnum

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )
    {
        nError = SrcAddVariable(pScrMngr,VARTYPE_CONST,pToken,nTokenLen);
        pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    }
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    if( strchr(pToken,'%') != 0)return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //copy value into variable struct
    if( pVariable->pValue )MiscMemFree(pVariable->pValue);
    int nSrcSize = strlen(scriptObj.pParmB);
    pVariable->pValue = (char *)MiscMemAlloc(nSrcSize+1);
    if( pVariable->pValue )
    {
        memcpy(pVariable->pValue,scriptObj.pParmB,nSrcSize);
        pVariable->pValue[nSrcSize]=0;
    }
    else
    {
        return SCRERROR_OUTOFMEM;
    }

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSPRINTINT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SPRINTINT varname "decimal-integer-value"

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSPRINTHEX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SPRINTHEX varname "hexadecimal-integer-value"

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileMID(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //MID varname "string" offset length

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //offset
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
//    if((int)scriptObj.nParmA < 0)return SCRERROR_INVALID_VALUE;

    //length
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,true);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmB = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if((int)scriptObj.nParmB <= 0)return SCRERROR_INVALID_VALUE;



    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileHEX2DEC(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //HEX2DEC varname "hex-integer-value"

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileDEC2HEX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //DEC2HEX varname "dec-integer-value"

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSETVAR(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SETVAR varname "string"

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //copy value into variable struct
    if( pVariable->pValue )MiscMemFree(pVariable->pValue);
    int nSrcSize = strlen(scriptObj.pParmB);
    pVariable->pValue = (char *)MiscMemAlloc(nSrcSize+1);
    if( pVariable->pValue )
    {
        memcpy(pVariable->pValue,scriptObj.pParmB,nSrcSize);
        pVariable->pValue[nSrcSize]=0;
    }
    else
    {
        return SCRERROR_OUTOFMEM;
    }

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSETVARX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SETVAR varname "string"

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )
    {
        nError = SrcAddVariable(pScrMngr,VARTYPE_STRING,pToken,nTokenLen);
    }
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //copy value into variable struct
    if( pVariable->pValue )MiscMemFree(pVariable->pValue);
    int nSrcSize = strlen(scriptObj.pParmB);
    pVariable->pValue = (char *)MiscMemAlloc(nSrcSize+1);
    if( pVariable->pValue )
    {
        memcpy(pVariable->pValue,scriptObj.pParmB,nSrcSize);
        pVariable->pValue[nSrcSize]=0;
    }
    else
    {
        return SCRERROR_OUTOFMEM;
    }

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static int SrcTimerActionToNumber(char *pToken)
{
    int nRetVal = TIMER_ILLEGAL;

    if(strcmpi(pToken,"start")==0)
    {
        nRetVal = TIMER_START;
    }
    else if(strcmpi(pToken,"elapse")==0)
    {
        nRetVal = TIMER_ELAPSED;
    }
    else if(strcmpi(pToken,"expired")==0)
    {
        nRetVal = TIMER_EXPIRED;
    }

    return nRetVal;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcIsTokenTimerValue(char *pToken)
{
    //look for either "true" or "false"
    ESCRERROR nError = SCRERROR_INV_LOGIC_VALUE;

    if( (strcmpi(pToken,"start")==0) ||
        (strcmpi(pToken,"elapse")==0) ||
        (strcmpi(pToken,"expired")==0) )
    {
        nError = SCRERROR_NONE;
    }

    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileTIMER(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //TIMER START   varname
    //TIMER ELAPSE  varnameTmr varnameVal
    //TIMER EXPIRED varname "string"

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //start!elapsed!expired
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenTimerValue(pToken);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcTimerActionToNumber(pToken);

    //varname1
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    if(scriptObj.nParmA == TIMER_ELAPSED )
    {
        //varname2
        pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
        if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
        if(nError != SCRERROR_NONE)return nError;
        if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
        nError = SrcIsValidVarName(pToken);
        if(nError != SCRERROR_NONE)return nError;
        if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
        scriptObj.pParmB = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);
    }
    else if(scriptObj.nParmA == TIMER_EXPIRED )
    {
        //"value"
        pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
        if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
        if(nError != SCRERROR_NONE)return nError;
        if(pToken[0]!='"')return SCRERROR_NOT_STRING;
        scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
        if(nError != SCRERROR_NONE)return nError;
    }

    return SrcCheckRestOfLineIsComment(pLine);
}
//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSTRCAT(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //STRCAT varname "string"

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileGETCWD(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //GETCWD varname

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    if( pVariable->eVarType == VARTYPE_CONST )return SCRERROR_VAR_IS_CONST;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileCHECKVAR(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SETVAR varname portnum

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileTXSTRING(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"string to transmit"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    scriptObj.nParmB = 1;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileTXSTRINGEX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"string to transmit"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //"number of times"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSENDCMD(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //"string to send"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSENDMPDATA(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //channel
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmB = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if((unsigned int)scriptObj.nParmB == 0  )return SCRERROR_INVALID_CHANNEL;
    if((unsigned int)scriptObj.nParmB >= 256)return SCRERROR_INVALID_CHANNEL;

    //"string to send"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileARITHMETIC(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //KEYNAME varname "valstr"

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    if( strcmpi(pToken,"ADD")==0)
    {
        scriptObj.nParmA = ALU_ADD;
    }
    else if( strcmpi(pToken,"ABS")==0)
    {
        scriptObj.nParmA = ALU_ABS;
    }
    else if( strcmpi(pToken,"MUL")==0)
    {
        scriptObj.nParmA = ALU_MUL;
    }
    else if( strcmpi(pToken,"DIV")==0)
    {
        scriptObj.nParmA = ALU_DIV;
    }
    else if( strcmpi(pToken,"SUB")==0)
    {
        scriptObj.nParmA = ALU_SUB;
    }
    else if( strcmpi(pToken,"TST")==0)
    {
        scriptObj.nParmA = ALU_TST;
    }
    else if( strcmpi(pToken,"MOD")==0)
    {
        scriptObj.nParmA = ALU_MOD;
    }
    else if( strcmpi(pToken,"AND")==0)
    {
        scriptObj.nParmA = ALU_AND;
    }
    else if( strcmpi(pToken,"OR")==0)
    {
        scriptObj.nParmA = ALU_OR;
    }
    else if( strcmpi(pToken,"XOR")==0)
    {
        scriptObj.nParmA = ALU_XOR;
    }
    else if( strcmpi(pToken,"ROR")==0)
    {
        scriptObj.nParmA = ALU_ROR;
    }
    else if( strcmpi(pToken,"ROL")==0)
    {
        scriptObj.nParmA = ALU_ROL;
    }
    else if( strcmpi(pToken,"HEXADD")==0)
    {
        scriptObj.nParmA = ALU_HEXADD;
    }
    else if( strcmpi(pToken,"HEXMUL")==0)
    {
        scriptObj.nParmA = ALU_HEXMUL;
    }
    else if( strcmpi(pToken,"HEXDIV")==0)
    {
        scriptObj.nParmA = ALU_HEXDIV;
    }
    else if( strcmpi(pToken,"HEXSUB")==0)
    {
        scriptObj.nParmA = ALU_HEXSUB;
    }
    else if( strcmpi(pToken,"HEXTST")==0)
    {
        scriptObj.nParmA = ALU_HEXTST;
    }
    else if( strcmpi(pToken,"HEXMOD")==0)
    {
        scriptObj.nParmA = ALU_HEXMOD;
    }
    else if( strcmpi(pToken,"HEXAND")==0)
    {
        scriptObj.nParmA = ALU_HEXAND;
    }
    else if( strcmpi(pToken,"HEXOR")==0)
    {
        scriptObj.nParmA = ALU_HEXOR;
    }
    else if( strcmpi(pToken,"HEXXOR")==0)
    {
        scriptObj.nParmA = ALU_HEXXOR;
    }
    else if( strcmpi(pToken,"HEXROR")==0)
    {
        scriptObj.nParmA = ALU_HEXROR;
    }
    else if( strcmpi(pToken,"HEXROL")==0)
    {
        scriptObj.nParmA = ALU_HEXROL;
    }
    else
    {
        return SCRERROR_UNKNOWN_ACTION;
    }


    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //"value"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSAVERESP(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SAVERESP varname port

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    if( strcmpi(pToken,"SAVERESP")==0)
    {
        scriptObj.nParmB = 1;
    }
    else
    {
        scriptObj.nParmB = 0;
    }

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePORTINFO(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //PORTINFO varname port cmdId

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //port
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //cmdid
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmB = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileMULTIINDEX(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //MULTIINDEX varname port

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePKTCOUNTGET(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SAVERESP varname port

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePKTCOUNTSET(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SAVERESP varname port

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompilePKTCOUNTRESP(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //SAVERESP varname port

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //porta
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if(scriptObj.nParmA >= gnMaximumLogicalPorts)return SCRERROR_INVALID_PORT;

    //varname
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFILTERMOVE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //FILTERMOVE mask varname1 varname2

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    scriptObj.nParmB = 0; //no logic inversion

    //mask
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if( strcmpi(pToken,"NOT")==0)
    {
        scriptObj.nParmB = 1; //no logic inversion
        pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    }
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    //nError = SrcIsTokenDecimalNumber(pToken,false);
    //if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertStrToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //varname1
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //varname2
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmB = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSTRDEHEXIZE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //STRDEHEXIZE flags varname1 varname2

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //flags
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    //nError = SrcIsTokenDecimalNumber(pToken,false);
    //if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertStrToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;

    //varname1
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //varname2
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmB = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}



//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileSTRING(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //STRxxx varname1 varname2

    ESCRERROR nError;
    uint32    nTokenLen;

    scriptObj.eOutcome   = OUTCOME_NONE;

    if( strcmpi(pToken,"STRLEN")==0)
    {
        scriptObj.nParmA = STR_LEN;
    }
    else if( strcmpi(pToken,"STRCMP")==0)
    {
        scriptObj.nParmA = STR_CMP;
    }
    else if( strcmpi(pToken,"STRCMPI")==0)
    {
        scriptObj.nParmA = STR_CMPI;
    }
    else if( strcmpi(pToken,"STRSTR")==0)
    {
        scriptObj.nParmA = STR_STR;
    }
    else if( strcmpi(pToken,"STRSTRI")==0)
    {
        scriptObj.nParmA = STR_STRI;
    }
    else
    {
        return SCRERROR_UNKNOWN_ACTION;
    }

    //varname1
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //varname2
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    if( ! SrcVarNameExists(pScrMngr,pToken) )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmB = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFILEOPEN(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //FILEOPEN handle "READ|WRITE|APPEND" "filename"

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //handle
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if((unsigned int)scriptObj.nParmA >= MAX_FILE_HANDLES)return SCRERROR_INVALID_FILE_HANDLE;

    //Permissions "READ" or "WRITE" or "APPEND"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmB = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    //"filename"
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    scriptObj.pParmA = SrcCopyStringToNewBlock(pToken, nTokenLen, nError);
    if(nError != SCRERROR_NONE)return nError;

    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFILECLOSE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //FILECLOSE handle

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //handle
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if((unsigned int)scriptObj.nParmA >= MAX_FILE_HANDLES)return SCRERROR_INVALID_FILE_HANDLE;


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFILEREAD(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //FILEREAD handle varData varLen

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //handle
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if((unsigned int)scriptObj.nParmA >= MAX_FILE_HANDLES)return SCRERROR_INVALID_FILE_HANDLE;

    //varData
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);

    //varLen
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmB = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFILEWRITE(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    //FILEWRITE handle varData

    ESCRERROR nError;
    uint32    nTokenLen;
    TScriptVariable *pVariable;

    scriptObj.eOutcome   = OUTCOME_NONE;

    //handle
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;
    nError = SrcIsTokenDecimalNumber(pToken,false);
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.nParmA = SrcConvertToInteger(pToken,nError);
    if(nError != SCRERROR_NONE)return nError;
    if((unsigned int)scriptObj.nParmA >= MAX_FILE_HANDLES)return SCRERROR_INVALID_FILE_HANDLE;

    //varData
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_UNEXPECTED_STRING;
    nError = SrcIsValidVarName(pToken);
    if(nError != SCRERROR_NONE)return nError;
    pVariable = SrcGetScriptVariable(pScrMngr, pToken);
    if( pVariable == NULL )return SCRERROR_UNKNOWN_VAR;
    scriptObj.pParmA = SrcCopyUnquotedStringToNewBlock(pToken, nTokenLen, nError);


    return SrcCheckRestOfLineIsComment(pLine);
}


#if 0
//=============================================================================
//=============================================================================
static ESCRERROR SrcCompile(TScriptManager *pScrMngr,char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    .............

    return SrcCheckRestOfLineIsComment(pLine);
}
#endif

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileToObject(TScriptManager *pScrMngr, char *pLine, TAutoScript &scriptObj, char *pToken)
{
    ESCRERROR nError;
    uint32    nTokenLen;

    //eAction
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    scriptObj.eAction = SrcConvertToEAction(pToken);
    if(scriptObj.eAction == ACTION_UNKNOWN)return SCRERROR_UNKNOWN_ACTION;

    switch(scriptObj.eAction)
    {
    case ACTION_END:
        nError = SrcCompileEND(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_EXIT:
        nError = SrcCompileEXIT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_TRACE:
    case ACTION_TRACEWIN:
        nError = SrcCompileTRACE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_CONSOLEUPDATE:
        nError = SrcCompileCONSOLEUPDATE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SEND:
        nError = SrcCompileCompositeSEND(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_INRANGE:
        nError = SrcCompileINRANGE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_BRKON:
    case ACTION_BRKOFF:
        nError = SrcCompileBRKON_OFF(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_DECLARE:
        nError = SrcCompileDECLARE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_DELAY:
        nError = SrcCompileDELAY(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_DELAYABS:
        nError = SrcCompileDELAYABS(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_DEFTIMEOUT:
        nError = SrcCompileDEFTIMEOUT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_TIMESCALAR:
        nError = SrcCompileTIMESCALAR(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SETVAR:
        nError = SrcCompileSETVAR(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SETVARX:
        nError = SrcCompileSETVARX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SPRINTINT:
        nError = SrcCompileSPRINTINT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SPRINTHEX:
        nError = SrcCompileSPRINTHEX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_MID:
        nError = SrcCompileMID(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_HEX2DEC:
        nError = SrcCompileHEX2DEC(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_DEC2HEX:
        nError = SrcCompileDEC2HEX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_STRCAT:
        nError = SrcCompileSTRCAT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_GETCWD:
        nError = SrcCompileGETCWD(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_CHECKVAR:
    case ACTION_CONTAIN:
    case ACTION_NOTCONTAIN:
    case ACTION_MATCHCOUNT:
        nError = SrcCompileCHECKVAR(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_CONST:
        nError = SrcCompileCONST(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SETCOMMS:
        nError = SrcCompileSETCOMMS(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PORT_UART:
        nError = SrcCompilePORT_UART(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PORT_MPOINT:
        nError = SrcCompilePORT_MPOINT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PORT_OC:
        nError = SrcCompilePORT_OC(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_MESSAGEBOX:
        nError = SrcCompileMESSAGEBOX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PORT_CLOSE:
        nError = SrcCompilePORT_CLOSE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_TIMER:
        nError = SrcCompileTIMER(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PORT_PIPE:
        nError = SrcCompilePORT_PIPE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_EXECUTE:
        nError = SrcCompileEXECUTE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_EXECUTEEX:
        nError = SrcCompileEXECUTEEX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_EXECUTEWAIT:
        nError = SrcCompileEXECUTEWAIT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_EXECUTEWAITRET:
        nError = SrcCompileEXECUTEWAITRET(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_EXECUTEWAITEX:
        nError = SrcCompileEXECUTEWAITEX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_EXIST_FILE:
        nError = SrcCompileEXISTFILE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_EXIST_FOLDER:
        nError = SrcCompileEXISTFOLDER(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_FILE_COMPARE:
        nError = SrcCompileFILECOMPARE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_FILE_CLOSE:
        nError = SrcCompileFILECLOSE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_FILE_OPEN:
        nError = SrcCompileFILEOPEN(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_FILE_READ:
        nError = SrcCompileFILEREAD(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_FILE_WRITE:
        nError = SrcCompileFILEWRITE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PRINT:
    case ACTION_PRINTE:
        nError = SrcCompilePRINT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_MDMSTAT:
        nError = SrcCompileMDMSTAT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_MDMSET:
        nError = SrcCompileMDMSET(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_TITLE:
    case ACTION_SUBTITLE:
        nError = SrcCompileTITLE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_FAIL:
        nError = SrcCompileFAIL(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_WAITMULTI:
        nError = SrcCompileWAITMULTI(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_MULTIINDEX:
        nError = SrcCompileMULTIINDEX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SAVERESP:
        nError = SrcCompileSAVERESP(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PORTINFO:
        nError = SrcCompilePORTINFO(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PKTCOUNTGET:
        nError = SrcCompilePKTCOUNTGET(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PKTCOUNTSET:
        nError = SrcCompilePKTCOUNTSET(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_PKTCOUNTRESP:
        nError = SrcCompilePKTCOUNTRESP(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_STRING:
        nError = SrcCompileSTRING(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_FILTERMOVE:
        nError = SrcCompileFILTERMOVE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_STRDEHEXIZE:
        nError = SrcCompileSTRDEHEXIZE(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_WAITRESP:
        nError = SrcCompileWAITRESP(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_WAITRESPEX:
    case ACTION_WAITTIMEOUT:
    case ACTION_WAITUNTIL:
        nError = SrcCompileWAITRESPEX_TIMEOUT_UNTIL(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_WAITDATA:
        nError = SrcCompileWAITDATA(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_WAITCHANNEL:
        nError = SrcCompileWAITCHANNEL(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_WAITPACKET:
        nError = SrcCompileWAITPACKET(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_WAIT_TIEVENT:
        nError = SrcCompileWAITTIEVENT(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_FLUSHRX:
    case ACTION_FLUSHTX:
        nError = SrcCompileFLUSHRX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_LABEL:
        nError = SrcCompileLABEL(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_ALU:
    case ACTION_HEXALU:
        nError = SrcCompileARITHMETIC(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_JUMP:
        nError = SrcCompileJUMP(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_IF:
        nError = SrcCompileIF(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_TXSTRING:
    case ACTION_TXUNICODE:
        nError = SrcCompileTXSTRING(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_TXSTRINGEX:
        nError = SrcCompileTXSTRINGEX(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SENDCMD:
        nError = SrcCompileSENDCMD(pScrMngr,pLine,scriptObj,pToken);
        break;

    case ACTION_SENDMPDATA:
        nError = SrcCompileSENDMPDATA(pScrMngr,pLine,scriptObj,pToken);
        break;

    default:
        nError = SCRERROR_UNKNOWN_ACTION;
        break;
    }

    return nError;
}

//=============================================================================
//=============================================================================
static bool SrcOtherIncludeSave(TScriptManager *pScrMngr, int nIndex, int nMaxLen, const char *pToken)
{
    bool fRetVal = false;
    if(nIndex<MAX_OTHER_INCLUDES)
    {
        char *pDest = pScrMngr->mbaOtherIncludeFolder[nIndex];
        int nSrcLen = strlen(pToken);
        if(nMaxLen > (nSrcLen+2))
        {
            *pDest=0;
            if(nSrcLen)
            {
                memcpy(pDest,pToken,nSrcLen);
                pDest += nSrcLen;
                if( pDest[-1] != '\\' )
                {
                    pDest--;
                    *pDest++ = '\\';
                }
                *pDest++ = 0;
            }
            fRetVal=true;
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
static AnsiString SrcFileExists(TScriptManager *pScrMngr,const char *pToken, char chDelim)
{
    AnsiString asToken = pToken;
    AnsiString asFolder;

    //if pToken starts with "\" or "." or "D:" then check without prepending folder
    if( (pToken[0] == '\\' ) || (pToken[0] == '.' ) || (pToken[1] == ':' ) )
    {
        asFolder = asToken;
        if ( FileExists(asFolder) )
        {
            return asFolder;
        }
    }

    //check if exists in script folder
    if( chDelim == '"' )
    {
        asFolder = pScrMngr->mbaScriptFolder;
        asFolder += asToken;
        if ( FileExists(asFolder) )
        {
            return asFolder;
        }
    }

    //check if exists in other include folder
    bool fExists;
    for(int i=0;i<MAX_OTHER_INCLUDES;i++)
    {
        asFolder = pScrMngr->mbaOtherIncludeFolder[i];
        if(asFolder.Length() > 0 )
        {
            asFolder += asToken;
            if ( FileExists(asFolder) )
            {
                return asFolder;
            }
        }
    }

    //check if exists in app folder
    asFolder = pScrMngr->mbaAppFolder;
    asFolder += asToken;
    if ( FileExists(asFolder) )
    {
        return asFolder;
    }

    // does not exist
    asFolder="";
    return asFolder;
}

//=============================================================================
//=============================================================================
static bool SrcCheckNameWithNumSuffix(char *pToken,
                                      char *pName,
                                      int nLowNum,
                                      int nHiNum,
                                      int *pNum)
{
    char baName[128+16];
    int i;
    int nNameLen = strlen(pName);
    if( nNameLen >= 128 )
    {
        return false;
    }
    strcpy(baName,pName);

    for(int i=nLowNum;i<=nHiNum;i++)
    {
        sprintf(&baName[nNameLen],"%d",i);
        if( strcmpi(pToken,baName)==0)
        {
            *pNum = i;
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static bool LaunchExecuteApp(char *pCommandLine, char *pFolder, bool fWait,bool fHide)
{
    bool fRetVal = false;
    static STARTUPINFO si;
    static PROCESS_INFORMATION  sAppPi;
    DWORD dwExitCode;

    if( pCommandLine)
    {
        memset(&si,0,sizeof(si));
        si.cb = sizeof(si);
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
        memset(&sAppPi,0,sizeof(sAppPi));

        if( CreateProcess(  NULL,
                            pCommandLine,
                            NULL,
                            NULL,
                            false,
                            0,
                            NULL,
                            pFolder,
                            &si,
                            &sAppPi ))
        {
            fRetVal = true;
            if( fWait )
            {
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

    return fRetVal;
}
//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileDirectiveExecute(char *pLine,
                                            TScriptManager *pScrMngr,
                                            char *pToken,
                                            char *pFolder,
                                            bool fWait,
                                            bool fHide)
{
    ESCRERROR nError = SCRERROR_ILLEGAL_DIRECTIVE;
    uint32    nTokenLen;
    int nValue;
#define  MaxExecuteMsgSize     (1024)
    static char baMsg[MaxExecuteMsgSize+1];

    //Get string which is the app to launch
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]!='"')return SCRERROR_NOT_STRING;
    pToken[nTokenLen-1]=0;
    pToken++;
    nTokenLen-=2;
    nError = SrcVariableSubstitution(pScrMngr, pToken, true);
    if(nError != SCRERROR_NONE)return nError;

    //Get string which is the default folder for the launched app
    if(pFolder)
    {
        pLine = SrcExtractToken(pLine,pFolder,nTokenLen,nError);
        if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
        if(nError != SCRERROR_NONE)return nError;
        if(pFolder[0]!='"')return SCRERROR_NOT_STRING;
        pFolder[nTokenLen-1]=0;
        pFolder++;
        nTokenLen-=2;
        nError = SrcVariableSubstitution(pScrMngr, pFolder, true);
        if(nError != SCRERROR_NONE)return nError;
    }

    nError = SrcCheckRestOfLineIsComment(pLine);
    if( nError == SCRERROR_NONE)
    {   //execute the command at pLine and spin until the app exists
        snprintf(baMsg,MaxExecuteMsgSize,"#execute \"%s\" \"%s\"",pToken,pFolder);
        frmMain->DisplayUserMessage(baMsg,COMPILE_COLOR);
        LaunchExecuteApp(pToken,pFolder,fWait,fHide);
    }

    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileDirective(char *pLine, uint32 nLineNum, TScriptManager *pScrMngr, char *pToken)
{
    ESCRERROR nError = SCRERROR_ILLEGAL_DIRECTIVE;
    uint32    nTokenLen;
    int nValue;

/*  VALID DIRECTIVES ARE:-

    #include "filename"
    #includeFolder0 "foldername"
    #includeFolder1 "foldername"
    ...
    #includeFolderN "foldername"   where N = (MAX_OTHER_INCLUDES-1)

*/

    //Directive name
    pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
    if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
    if(nError != SCRERROR_NONE)return nError;
    if(pToken[0]=='"')return SCRERROR_ILLEGAL_DIRECTIVE;

    if( strcmpi(pToken,"include")==0)
    {
        //Get filename
        pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
        if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
        if(nError != SCRERROR_NONE)return nError;
        if( !((pToken[0]=='"') || (pToken[0]=='<')) )return SCRERROR_NOT_STRING;
        pToken[nTokenLen-1]=0;
        /* this substituition was added at v1.7.0 */
        nError = SrcVariableSubstitution(pScrMngr, pToken, true);
        if(nError != SCRERROR_NONE)return nError;

        if( SrcCheckRestOfLineIsComment(pLine) == SCRERROR_NONE)
        {   //compile the new file code
            AnsiString asSrcFile = SrcFileExists(pScrMngr,&pToken[1], pToken[0]);
            if(asSrcFile != "")
            {
                nError = SrcCompileFile(asSrcFile.c_str(), nLineNum, pScrMngr );
                if(nError == SCRERROR_FILE_NEST_TOODEEP)
                {
                    if(pScrMngr->mFileNestCount==0)
                    {
                        frmMain->DisplayUserMessage("Compile Error: File nesting too deep");
                    }
                }
            }
            else
            {
                frmMain->DisplayUserMessage("Compile Error: File nesting too deep");
                nError = SCRERROR_FILE_UNKNOWN;
            }
        }
    }
    else if(SrcCheckNameWithNumSuffix(pToken,
                                      "includeFolder",
                                      0,
                                      (MAX_OTHER_INCLUDES-1),
                                      &nValue) )
    {
        //Get foldername
        pLine = SrcExtractToken(pLine,pToken,nTokenLen,nError);
        if(nTokenLen==0)return SCRERROR_INCOMPLETE_LINE;
        if(nError != SCRERROR_NONE)return nError;
        if(pToken[0]!='"')return SCRERROR_NOT_STRING;
        pToken[nTokenLen-1]=0;

        if( SrcCheckRestOfLineIsComment(pLine) == SCRERROR_NONE)
        {   //update the other folder variable
            if( ! SrcOtherIncludeSave(pScrMngr, nValue, (MAX_FILNAME_LEN-2), &pToken[1]) )
            {
                frmMain->DisplayUserMessage("Compile Error: Invalid Other Include Folder Name");
                nError = SCRERROR_INVALID_FOLDER;
            }
        }
    }
    else if( strcmpi(pToken,"execute")==0)
    {
        char *pFolder   = (char *)MiscMemAlloc(MAX_SCRIPT_LINE_LENGTH+1);
        if(pFolder)
        {
            nError = SrcCompileDirectiveExecute(pLine,pScrMngr,pToken,pFolder,false,false);
            MiscMemFree(pFolder);
        }
        else
        {
            nError = SCRERROR_OUTOFMEM;
        }
    }
    else if( strcmpi(pToken,"executewait")==0)
    {
        char *pFolder   = (char *)MiscMemAlloc(MAX_SCRIPT_LINE_LENGTH+1);
        if(pFolder)
        {
            nError = SrcCompileDirectiveExecute(pLine,pScrMngr,pToken,pFolder,true,false);
            MiscMemFree(pFolder);
        }
        else
        {
            nError = SCRERROR_OUTOFMEM;
        }
    }
    else if( strcmpi(pToken,"executehide")==0)
    {
        char *pFolder   = (char *)MiscMemAlloc(MAX_SCRIPT_LINE_LENGTH+1);
        if(pFolder)
        {
            nError = SrcCompileDirectiveExecute(pLine,pScrMngr,pToken,pFolder,false,true);
            MiscMemFree(pFolder);
        }
        else
        {
            nError = SCRERROR_OUTOFMEM;
        }
    }
    else if( strcmpi(pToken,"executewaithide")==0)
    {
        char *pFolder   = (char *)MiscMemAlloc(MAX_SCRIPT_LINE_LENGTH+1);
        if(pFolder)
        {
            nError = SrcCompileDirectiveExecute(pLine,pScrMngr,pToken,pFolder,true,true);
            MiscMemFree(pFolder);
        }
        else
        {
            nError = SCRERROR_OUTOFMEM;
        }
    }
    else
    {
        nError = SCRERROR_ILLEGAL_DIRECTIVE;
    }

    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR ScrResizeCodeSpace(TScriptManager *pScrMngr, uint32 nAdditionSpace)
{
    ESCRERROR nError = SCRERROR_INVALID_MNGR;
    uint32 nBlockSize;
    if(pScrMngr)
    {
        if( nAdditionSpace < 1024 )nAdditionSpace=1024;
        uint32 nNewSize = pScrMngr->mnMaxCodeSize + nAdditionSpace;
        nError = SCRERROR_OUTOFMEM;
        nBlockSize = nNewSize * sizeof(TScriptAction);
        TScriptAction *pNewCodeBlock = (TScriptAction *)MiscMemAlloc(nBlockSize) ;
        if(pNewCodeBlock)
        {
            memset(pNewCodeBlock,0,nBlockSize);
            memcpy(pNewCodeBlock,pScrMngr->mpCode,(pScrMngr->mnMaxCodeSize * sizeof(TScriptAction)));
            MiscMemFree(pScrMngr->mpCode);
            pScrMngr->mpCode = pNewCodeBlock;
            pScrMngr->mnMaxCodeSize = nNewSize;
            nError = SCRERROR_NONE;
        }
    }
    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR  SrcAddScriptObjectToCodeFile(TScriptManager *pScrMngr,TAutoScript &scriptObj)
{
    ESCRERROR nError = SCRERROR_INVALID_MNGR;
    if(pScrMngr)
    {
        nError = SCRERROR_NONE;
        if( pScrMngr->mnActionCount >= pScrMngr->mnMaxCodeSize )
        {
            ScrResizeCodeSpace(pScrMngr, (1024*4));
        }
        if( pScrMngr->mnActionCount >= pScrMngr->mnMaxCodeSize )
        {
            nError = SCRERROR_OUTOFMEM;
        }
        else
        {
            pScrMngr->mpCode[pScrMngr->mnActionCount++].generic=scriptObj;
        }
    }
    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileLine(char *pLine, uint32 nLineNum, TScriptManager *pScrMngr, char *pToken)
{
    ESCRERROR nError = SCRERROR_NONE;

    pLine  = SrcAdvanceToNonWhiteSpace(pLine);
    TAutoScript scriptObject;

    pScrMngr->meLastAction = ACTION_UNKNOWN;
    if(pLine)
    {
        if( strlen(pLine) > 0 )
        {
            //check for comment and ignore the rest if it is
            if( !((pLine[0]=='/') && (pLine[1]=='/')) )
            {   //not a comment
                if( pLine[0] == '#' )
                {
                    pLine++;
                    nError = SrcCompileDirective(pLine, nLineNum, pScrMngr, pToken);
                }
                else
                {
                    scriptObject.eAction    = ACTION_UNKNOWN;
                    scriptObject.eOutcome   = OUTCOME_UNKNOWN;
                    scriptObject.nTimeoutMs = -1;
                    scriptObject.nLineNum   = nLineNum;
                    scriptObject.nParmA     = -1;
                    scriptObject.pParmA     = NULL;
                    scriptObject.nParmB     = -1;
                    scriptObject.pParmB     = NULL;

                    nError  = SrcCompileToObject(pScrMngr,pLine, scriptObject, pToken);
                    pScrMngr->meLastAction = scriptObject.eAction;

                    if(nError == SCRERROR_NONE)
                    {
                        if((scriptObject.eAction&ACTION_NOCODE_MASK)== ACTION_NOCODE_MASK)
                        {
                            if(scriptObject.pParmA)MiscMemFree(scriptObject.pParmA);
                            if(scriptObject.pParmB)MiscMemFree(scriptObject.pParmB);
                        }
                        else
                        {
                            SrcAddScriptObjectToCodeFile(pScrMngr,scriptObject);
                        }
                    }
                    else
                    {
                        if(scriptObject.pParmA)MiscMemFree(scriptObject.pParmA);
                        if(scriptObject.pParmB)MiscMemFree(scriptObject.pParmB);
                    }
                }
            }
        }
    }
    //else empty line so mark it as no_err and return

    return nError;
}

//=============================================================================
//=============================================================================
static ESCRERROR SrcCompileFile(char *pFilename, uint32 nLineNum, TScriptManager *pScrMngr )
{
    ESCRERROR nError = SCRERROR_FILE_UNKNOWN;
    uint32 nLineCount=  0;
    int nCurLabelSuffix;
    AnsiString asMsg;
    TAutoScript scriptObject;

    if(++pScrMngr->mFileNestCount <= MAX_FILE_NEST_DEPTH)
    {
        FILE *fp = fopen(pFilename,"rt");
        nCurLabelSuffix = pScrMngr->mLabelSuffix;
        pScrMngr->mFileIncCount++;
        pScrMngr->mLabelSuffix = pScrMngr->mFileIncCount;
        pScrMngr->mnActionFileStart = pScrMngr->mnActionCount;
        if(fp)
        {
            //Add an IncludePush action
            scriptObject.eAction    = ACTION_INCLUDE_PUSH;
            scriptObject.eOutcome   = OUTCOME_UNKNOWN;
            scriptObject.nTimeoutMs = -1;
            scriptObject.nLineNum   = nLineNum;
            scriptObject.nParmB     = -1;
            scriptObject.pParmB     = NULL;
            scriptObject.nParmA     = strlen(pFilename);
            scriptObject.pParmA     = SrcCopyUnquotedStringToNewBlockNoUnEscape(pFilename, scriptObject.nParmA, nError);
            if(nError != SCRERROR_NONE)return nError;
            //all ok, so add the action item to the compiled output
            SrcAddScriptObjectToCodeFile(pScrMngr,scriptObject);

            //process the file
            nError = SCRERROR_OUTOFMEM;
            char *pLineBuf = (char *)MiscMemAlloc(MAX_SCRIPT_LINE_LENGTH+1);
            char *pToken   = (char *)MiscMemAlloc(MAX_SCRIPT_LINE_LENGTH+1);
            if(pLineBuf && pToken)
            {
                nLineCount++;
                while( fgets(pLineBuf, MAX_SCRIPT_LINE_LENGTH, fp) != NULL )
                {
                    /*
                    When running a large test, the compilation can take
                    a long time.
                    Sometimes you want to abort. This allows it
                    */
                    if( ProcessApplicationMessages() )
                    {
                        Application->Terminate();
                        exit(1234);
                    }

                    if( strstr(pLineBuf,"//BREAKPOINT"))
                    {
                        pLineBuf[0]='/';
                    }
                    nError = SrcCompileLine(pLineBuf, nLineCount, pScrMngr, pToken);
                    if( nError!=SCRERROR_NONE)
                    {
                        if(  (nError!=SCRERROR_FILE_NEST_TOODEEP) ||
                             ((nError==SCRERROR_FILE_NEST_TOODEEP) && (pScrMngr->mFileNestCount==1) ) )
                        {
                            asMsg.printf("Script File Compile Error = %d (%s)",(int)nError, SrcGetErrorText(nError));
                            frmMain->DisplayUserMessage(asMsg.c_str());
                            frmMain->DisplayUserMessage("");
                            asMsg.printf("File: %s",pFilename);
                            frmMain->DisplayUserMessage(asMsg.c_str());
                            asMsg.printf("Line: %d ",nLineCount);
                            frmMain->DisplayUserMessage(asMsg.c_str());
                            frmMain->DisplayUserMessage(pLineBuf);
                            frmMain->DisplayUserMessage("Regression test cannot be performed");
                        }
                        break;
                    }
                    else if( pScrMngr->meLastAction == ACTION_END)
                    {
                        //stop compiling
                        break;
                    }
                    nLineCount++;
                }
                if( (nError == SCRERROR_NONE) && (pScrMngr->mnActionCount==0) )
                {
                    nError = SCRERROR_FILE_EMPTY;
                }
            }
                
            //Add an IncludePop action
            scriptObject.eAction    = ACTION_INCLUDE_POP;
            scriptObject.eOutcome   = OUTCOME_UNKNOWN;
            scriptObject.nTimeoutMs = -1;
            scriptObject.nLineNum   = 0;
            scriptObject.nParmA     = -1;
            scriptObject.pParmA     = NULL;
            scriptObject.nParmB     = -1;
            scriptObject.pParmB     = NULL;
            //all ok, so add the action item to the compiled output
            SrcAddScriptObjectToCodeFile(pScrMngr,scriptObject);

            if(pLineBuf)MiscMemFree(pLineBuf);
            if(pToken)MiscMemFree(pToken);

            fclose(fp);
        }
        else
        {
            asMsg.printf("Script File (%s) missing",pFilename);
            frmMain->DisplayUserMessage(asMsg.c_str());
        }
        pScrMngr->mLabelSuffix = nCurLabelSuffix;
    }
    else
        nError = SCRERROR_FILE_NEST_TOODEEP;

    pScrMngr->mFileNestCount--;

    return nError;
}

/******************************************************************************/
/******************************************************************************/
// Global Functions Non Class Members
/******************************************************************************/
/******************************************************************************/

//=============================================================================
//=============================================================================
char *SrcGetErrorText(ESCRERROR nError)
{
    char *pRetVal;
    switch(nError)
    {
    case SCRERROR_NONE:                 pRetVal = "ERROR_NONE";break;
    case SCRERROR_FILE_UNKNOWN:         pRetVal = "ERROR_FILE_UNKNOWN";break;
    case SCRERROR_SYNTAX:               pRetVal = "ERROR_SYNTAX";break;
    case SCRERROR_OUTOFMEM:             pRetVal = "ERROR_OUTOFMEM";break;
    case SCRERROR_UNTERM_STRING:        pRetVal = "ERROR_UNTERM_STRING";break;
    case SCRERROR_UNKNOWN_ACTION:       pRetVal = "ERROR_UNKNOWN_ACTION";break;
    case SCRERROR_UNKNOWN_OUTCOME:      pRetVal = "ERROR_UNKNOWN_OUTCOME";break;
    case SCRERROR_TIMEOUT_SYNTAX:       pRetVal = "ERROR_TIMEOUT_SYNTAX";break;
    case SCRERROR_INCOMPLETE_LINE:      pRetVal = "ERROR_INCOMPLETE_LINE";break;
    case SCRERROR_NOT_STRING:           pRetVal = "ERROR_NOT_STRING";break;
    case SCRERROR_UNEXPECTED_SOURCE:    pRetVal = "ERROR_UNEXPECTED_SOURCE";break;
    case SCRERROR_INVALID_MNGR:         pRetVal = "ERROR_INVALID_MNGR";break;
    case SCRERROR_NOT_DECIMAL_NUMBER:   pRetVal = "ERROR_NOT_DECIMAL_NUMBER";break;
    case SCRERROR_ILLEGAL_DIRECTIVE:    pRetVal = "ERROR_ILLEGAL_DIRECTIVE";break;
    case SCRERROR_FILE_EMPTY:           pRetVal = "ERROR_FILE_EMPTY";break;
    case SCRERROR_UNEXPECTED_STRING:    pRetVal = "ERROR_UNEXPECTED_STRING";break;
    case SCRERROR_ILLEGAL_VARNAME:      pRetVal = "ERROR_ILLEGAL_VARNAME";break;
    case SCRERROR_UNKNOWN_VAR:          pRetVal = "ERROR_UNKNOWN_VAR";break;
    case SCRERROR_FILE_NEST_TOODEEP:    pRetVal = "ERROR_FILE_NEST_TOODEEP";break;
    case SCRERROR_ILLEGAL_LABELNAME:    pRetVal = "ERROR_ILLEGAL_LABELNAME";break;
    case SCRERROR_MULTIPLE_LABEL:       pRetVal = "ERROR_MULTIPLE_LABEL";break;
    case SCRERROR_UNKNOWN_LABELNAME:    pRetVal = "ERROR_UNKNOWN_LABELNAME";break;
    case SCRERROR_INCOMPLETE_ESCAPE:    pRetVal = "ERROR_INCOMPLETE_ESCAPE";break;
    case SCRERROR_INVALID_PORT:         pRetVal = "ERROR_INVALID_PORT";break;
    case SCRERROR_INVALID_CHANNEL:      pRetVal = "ERROR_INVALID_CHANNEL";break;
    case SCRERROR_INVALID_FILE_HANDLE:  pRetVal = "ERROR_INVALID_FILE_HANDLE";break;
    case SCRERROR_CANNOT_RESOLVE_VAR:   pRetVal = "ERROR_CANNOT_RESOLVE_VAR";break;
    case SCRERROR_VAR_IS_CONST:         pRetVal = "ERROR_VAR_IS_CONST";break;
    case SCRERROR_INVALID_FOLDER:       pRetVal = "ERROR_INVALID_FOLDER_NAME";break;
    case SCRERROR_INV_LOGIC_VALUE:      pRetVal = "ERROR_INVALID_LOGIC_VALUE";break;
    case SCRERROR_INVALID_VALUE:        pRetVal = "ERROR_INVALID_VALUE";break;
    case SCRERROR_NOT_HEX_NUMBER:       pRetVal = "ERROR_NOT_HEX_NUMBER";break;
    case SCRERROR_NOT_OCTAL_NUMBER:     pRetVal = "ERROR_NOT_OCTAL_NUMBER";break;
    case SCRERROR_NOT_BINARY_NUMBER:    pRetVal = "ERROR_NOT_BINARY_NUMBER";break;
    case SCRERROR_NUMBER_INVALID_CHR:   pRetVal = "ERROR_NUMBER_INVALID_CHR";break;

    default:                            pRetVal = "ERROR_UNKNOWN";break;
    }
    return pRetVal;
}

//=============================================================================
//=============================================================================
bool ScriptStructureCheck(void)
{
    bool fRetVal = false;
    int nSize = sizeof(TAutoScript);

    if( nSize == sizeof(TActionSend) )
    {
        fRetVal = true;
    }

    return fRetVal;
}

//=============================================================================
//=============================================================================
void SrcEscapeString(char *pString,AnsiString &asStr)
{
    unsigned char *pSrc = (unsigned char *)pString;
    asStr="";
    if(pSrc)
    {
        int nParmLen = strlen(pSrc);
        char baBuf[24];
        for(int j=0;j<nParmLen;j++)
        {
            MiscEscapeCharacter(*pSrc++, baBuf);
            asStr += baBuf;
        }
    }
}

//=============================================================================
//=============================================================================
int SrcEscapeString(char *pSrc,char *pDst, int nMaxDstLen)
{
    int nRetVal =0;
    if(pSrc && pDst)
    {
        char *pDestStart = pDst;
        int nExpandedLen;
        while(*pSrc && (nMaxDstLen >=4))
        {
            nExpandedLen = MiscEscapeCharacter(*pSrc++, pDst);
            nMaxDstLen -= nExpandedLen;
            pDst += nExpandedLen;
        }
        *pDst = 0;
        nRetVal = pDst - pDestStart;
    }
    return nRetVal;
}

//=============================================================================
//=============================================================================
int SrcEscapeStringNullAnd0x8X(char *pSrc, int nSrcLen, char *pDst, int nMaxDstLen)
{
    int nRetVal =0;
    if(pSrc && pDst && nSrcLen)
    {
        char *pDestStart = pDst;
        int nExpandedLen;
        while( nSrcLen-- && (nMaxDstLen >=4))
        {
            nExpandedLen = MiscEscapeCharacterNullAnd0x8X(*pSrc++, pDst);
            nMaxDstLen -= nExpandedLen;
            pDst += nExpandedLen;
        }
        *pDst = 0;
        nRetVal = pDst - pDestStart;
    }
    return nRetVal;
}

//=============================================================================
//=============================================================================
bool SrcUnescapeString(char *pStr)
{
    //returns new length
    bool fRetVal = true;
    char *pDst = pStr;
    char *pSrc = pStr;

    if(pStr)
    {
        char ch;
        while(*pSrc)
        {
            pSrc = MiscUnescapeCharacter(&ch, pSrc);
            *pDst++ = ch;;
        }
        *pDst=0;
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
bool SrcStripWhitespace(char *pStr)
{
    //returns new length
    bool fRetVal = true;
    char *pDst = pStr;
    char *pSrc = pStr;

    if(pStr)
    {
        char ch;
        while(*pSrc)
        {
            if( ((unsigned char)(*pSrc)) <=' ' )
            {
                pSrc++;
            }
            else if ( *pSrc >= 126 )
            {
                pSrc++;
            }
            else
            {
                *pDst++ = *pSrc++;
            }
        }
        *pDst=0;
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
TScriptManager * SrcCreateScriptManager(uint32 nInitCodeSize, uint32 nInitMaxVariables)
{
    uint32 nBlockSize;
    TScriptManager *pManager = (TScriptManager *)MiscMemAlloc(sizeof(TScriptManager));
    if(pManager)
    {
        pManager->mnMaxCodeSize = 0;
        pManager->mnActionCount = 0;
        pManager->mnMaxVarSize  = 0;
        pManager->mnVarCount = 0;
        pManager->mFileNestCount = 0;
        pManager->mTotalFailCount = 0;
        pManager->mTitleFailCount = 0;
        pManager->mLabelSuffix = 0;
        pManager->mFileIncCount = 0;
        pManager->mnIncFilenameStackTopAtTitle = -1;

        pManager->mbaScriptFileName[0] = 0;
        pManager->mbaLogFileFolder[0] = 0;
        pManager->mbaScriptFolder[0] = 0;
        pManager->mbaAppFolder[0]= 0;
        for(int i=0;i<MAX_OTHER_INCLUDES;i++)
        {
            pManager->mbaOtherIncludeFolder[i][0] = 0;
        }

        if( nInitCodeSize < (1024*4))nInitCodeSize=(1024*4);
        nBlockSize = nInitCodeSize * sizeof(TScriptAction);
        pManager->mpCode = (TScriptAction *)MiscMemAlloc( nBlockSize) ;

        if(pManager->mpCode)
        {
            memset(pManager->mpCode,0,nBlockSize);
            if( nInitMaxVariables < (1024*1))nInitMaxVariables=(1024*1);
            nBlockSize = nInitMaxVariables * sizeof(TScriptVariable);
            pManager->mpVariables = (TScriptVariable *)MiscMemAlloc( nBlockSize );
            if(pManager->mpVariables)
            {
                memset(pManager->mpVariables,0,nBlockSize);
                pManager->mnMaxCodeSize = nInitCodeSize;
                pManager->mnMaxVarSize  = nInitMaxVariables;

                pManager->mCurActParmSize = 0x10000;
                memset(&pManager->mCurrentAction,0,sizeof(pManager->mCurrentAction));
                pManager->mCurrentAction.generic.pParmA = (char *)MiscMemAlloc( pManager->mCurActParmSize );
                pManager->mCurrentAction.generic.pParmB = (char *)MiscMemAlloc( pManager->mCurActParmSize );
                pManager->mpSubstBuffer                 = (char *)MiscMemAlloc( pManager->mCurActParmSize );
                if( pManager->mCurrentAction.generic.pParmA &&
                    pManager->mCurrentAction.generic.pParmB &&
                    pManager->mpSubstBuffer                   )
                {
                    pManager->mCurrentAction.generic.pParmA[0]=0;
                    pManager->mCurrentAction.generic.pParmB[0]=0;
                }
                else
                {
                    if(pManager->mCurrentAction.generic.pParmA)
                    {
                        MiscMemFree(pManager->mCurrentAction.generic.pParmA);
                        pManager->mCurrentAction.generic.pParmA=NULL;
                    }
                    if(pManager->mCurrentAction.generic.pParmB)
                    {
                        MiscMemFree(pManager->mCurrentAction.generic.pParmB);
                        pManager->mCurrentAction.generic.pParmB=NULL;
                    }
                    if(pManager->mpSubstBuffer)
                    {
                        MiscMemFree(pManager->mpSubstBuffer);
                        pManager->mpSubstBuffer=NULL;
                    }
                    MiscMemFree(pManager->mpCode);
                    pManager->mpCode = NULL;
                    MiscMemFree(pManager);
                    pManager = NULL;
                }
            }
            else
            {
                MiscMemFree(pManager->mpCode);
                pManager->mpCode = NULL;
                MiscMemFree(pManager);
                pManager = NULL;
            }
        }
        else
        {
            MiscMemFree(pManager);
            pManager = NULL;
        }

        pManager->mpPC   = NULL;
    }
    return pManager;
}

//=============================================================================
//=============================================================================
void SrcClearScript(TScriptManager *pManager)
{
    uint32 nIndex;
    TScriptAction *pAction;
    TScriptVariable *pVar;
    if(pManager)
    {
        //delete all variables
        pVar = pManager->mpVariables;
        for(nIndex=0;nIndex<pManager->mnMaxVarSize;nIndex++)
        {
            if(pVar->pName)MiscMemFree(pVar->pName);
            pVar->pName=NULL;

            if(pVar->pValue)MiscMemFree(pVar->pValue);
            pVar->pValue=NULL;

            pVar++;
        }

        //Delete action code
        pAction = pManager->mpCode;
        for(nIndex=0;nIndex<pManager->mnActionCount;nIndex++)
        {
            if( pAction->generic.pParmA )MiscMemFree(pAction->generic.pParmA);
            pAction->generic.pParmA = NULL;

            if( pAction->generic.pParmB )MiscMemFree(pAction->generic.pParmB);
            pAction->generic.pParmB = NULL;

            pAction++;
        }

        pManager->mnActionCount = 0;
        pManager->mnVarCount = 0;
    }
}

//=============================================================================
//=============================================================================
void SrcDeleteScriptManager(TScriptManager *pManager)
{
    if(pManager)
    {
        //delete mem for current action string buffers
        if(pManager->mCurrentAction.generic.pParmA)
        {
            MiscMemFree(pManager->mCurrentAction.generic.pParmA);
            pManager->mCurrentAction.generic.pParmA=NULL;
        }
        if(pManager->mCurrentAction.generic.pParmB)
        {
            MiscMemFree(pManager->mCurrentAction.generic.pParmB);
            pManager->mCurrentAction.generic.pParmB=NULL;
        }

        SrcClearScript(pManager);

        MiscMemFree(pManager->mpVariables);
        pManager->mpVariables = NULL;

        MiscMemFree(pManager->mpCode);
        pManager->mpCode = NULL;

        MiscMemFree(pManager);
        pManager = NULL;
    }
}

//=============================================================================
//=============================================================================
bool SrcCopyAndExpandVariable(TScriptManager *pScrMngr, char *pSrc, char *pDst)
{
    bool fRetVal=false;

    if(pSrc && pDst )
    {
        fRetVal = SrcVariableSubstitution(pScrMngr, pSrc, pDst,false);
        if(fRetVal)
        {
            fRetVal = SrcUnescapeString(pDst);
        }
    }

    return fRetVal;
}

//=============================================================================
//=============================================================================
TScriptAction *SrcCopyNextScriptAction(TScriptManager *pScrMngr, bool &fSubstA,bool &fSubstB)
{
    TScriptAction *pSrc  = pScrMngr->mpPC++;
    TScriptAction *pDest = &pScrMngr->mCurrentAction;

    pDest->generic.eAction     = pSrc->generic.eAction;
    pDest->generic.eOutcome    = pSrc->generic.eOutcome;
    pDest->generic.nTimeoutMs  = pSrc->generic.nTimeoutMs;
    pDest->generic.nLineNum    = pSrc->generic.nLineNum;
    pDest->generic.nParmA      = pSrc->generic.nParmA;
    pDest->generic.nParmB      = pSrc->generic.nParmB;
    pDest->generic.pParmA[0]   = 0;
    pDest->generic.pParmB[0]   = 0;

    if(pSrc->generic.pParmA)
    {
        pScrMngr->mMissingVarname[0]=0;
        fSubstA = SrcVariableSubstitution(pScrMngr, pSrc->generic.pParmA, pDest->generic.pParmA,fSubstA);
        strcpy(pScrMngr->mMissingVarnameA,pScrMngr->mMissingVarname);
    }
    else
    {
        fSubstA = true;
    }
    if(pSrc->generic.pParmB)
    {
        pScrMngr->mMissingVarname[0]=0;
        fSubstB = SrcVariableSubstitution(pScrMngr, pSrc->generic.pParmB, pDest->generic.pParmB,fSubstB);
        strcpy(pScrMngr->mMissingVarnameB,pScrMngr->mMissingVarname);
    }
    else
    {
        fSubstB = true;
    }
    return pDest;
}

//=============================================================================
//=============================================================================
TScriptVariable * SrcGetScriptVariable(TScriptManager *pScrMngr, char *pToken)
{
    TScriptVariable *pRetVal = NULL;
    TScriptVariable *pVariables = pScrMngr->mpVariables;
    for(uint32 nIndex=0;nIndex<pScrMngr->mnVarCount;nIndex++)
    {
        if(strcmpi(pToken,pVariables->pName)==0)
        {
            pRetVal = pVariables;
            break;
        }
        pVariables++;
    }
    return pRetVal;
}

//=============================================================================
//=============================================================================
ESCRERROR SrcAddSystemVariable(TScriptManager *pScrMngr,char *pVarName,char *pVarValue)
{
    ESCRERROR nError=SCRERROR_ILLEGAL_VARNAME;
    int nVarNameLen  = strlen(pVarName);
    int nVarValueLen = strlen(pVarValue);

    if(nVarNameLen>0)
    {
        nError=SCRERROR_NONE;
        TScriptVariable *pVariable = SrcGetScriptVariable(pScrMngr,pVarName);
        if( pVariable == NULL )
        {
            nError = SrcAddVariable(pScrMngr,VARTYPE_STRING,pVarName,nVarNameLen);
            pVariable = SrcGetScriptVariable(pScrMngr,pVarName);
        }
        nError=SCRERROR_OUTOFMEM;
        if( pVariable)
        {
            if(nVarValueLen>0)
            {
                pVariable->pValue = (char *)MiscMemAlloc(nVarValueLen+1);
                if(pVariable->pValue)
                {
                    nError=SCRERROR_NONE;
                    strcpy(pVariable->pValue,pVarValue);
                }
            }
            else
            {
                nError=SCRERROR_NONE;
            }
        }
    }

    return nError;
}

//=============================================================================
#if 0
//=============================================================================
ESCRERROR SrcAddSystemArrayVariable(TScriptManager *pScrMngr,char *pVarName,char *pVarValue, int nArrLen)
{
    ESCRERROR nError=SCRERROR_ILLEGAL_VARNAME;
    int nVarNameLen  = strlen(pVarName);
    int nVarValueLen = strlen(pVarValue);

    if(nArrLen<=0)
    {
        nArrLen=1;
    }
    else if(nArrLen>MAX_SYS_VAR_ARRAY_LEN)
    {
        nArrLen=MAX_SYS_VAR_ARRAY_LEN;
    }

    if(nVarNameLen>0)
    {
        nError=SCRERROR_NONE;
        TScriptVariable *pVariable = SrcGetScriptVariable(pScrMngr,pVarName);
        if( pVariable == NULL )
        {
            while( nArrLen-- && (nError==SCRERROR_NONE))
            {
                nError = SrcAddVariable(pScrMngr,VARTYPE_STRING,pVarName,nVarNameLen);
                pVariable = SrcGetScriptVariable(pScrMngr,pVarName);
                nError=SCRERROR_OUTOFMEM;
                if( pVariable)
                {
                    if(nVarValueLen>0)
                    {
                        pVariable->pValue = (char *)MiscMemAlloc(nVarValueLen+1);
                        if(pVariable->pValue)
                        {
                            nError=SCRERROR_NONE;
                            strcpy(pVariable->pValue,pVarValue);
                        }
                    }
                }
            }
        }
    }

    return nError;
}
#endif
//=============================================================================
//=============================================================================
void SrcListScript(TScriptManager *pScrMngr)
{
    AnsiString asLine,asParmA,asParmB;
    TScriptAction *pScript = pScrMngr->mpCode;
    uint32  nActionCount   = pScrMngr->mnActionCount;

    for(uint32 i=0;i<nActionCount;i++)
    {
        SrcEscapeString(pScript->generic.pParmA,asParmA);
        SrcEscapeString(pScript->generic.pParmB,asParmB);
        asLine.printf("%d %d  %d  %d %s  %d %s",
                       pScript->generic.eAction,
                          pScript->generic.eOutcome,
                              pScript->generic.nTimeoutMs,
                                  pScript->generic.nParmA,
                                     asParmA.c_str(),
                                         pScript->generic.nParmB,
                                            asParmB.c_str()
                                     );

        frmMain->DisplayUserMessage(asLine.c_str());
        pScript++;
    }

}

//=============================================================================
//=============================================================================
ESCRERROR SrcCompileScriptFile(char *pFilename, TScriptManager *pScrMngr, AnsiString asLogPath )
{
    ESCRERROR nError;

    /* Display the current working folder as the first line in the display */
    AnsiString asCurFolder = GetCurrentDir();
    frmMain->DisplayUserMessage("++++++++++++++++++++++",COMPILE_COLOR);
    AnsiString asMsg = "Current Working Folder = " + asCurFolder;
    frmMain->DisplayUserMessage(asMsg.c_str(),COMPILE_COLOR);
    frmMain->DisplayUserMessage("++++++++++++++++++++++",COMPILE_COLOR);
    asMsg = pFilename;
    asMsg = "Script Filename = " + asMsg;
    frmMain->DisplayUserMessage(asMsg.c_str(),COMPILE_COLOR);
    frmMain->DisplayUserMessage("++++++++++++++++++++++",COMPILE_COLOR);
    asMsg = "Log Folder = " + asLogPath;
    frmMain->DisplayUserMessage(asMsg.c_str(),COMPILE_COLOR);
    frmMain->DisplayUserMessage("++++++++++++++++++++++",COMPILE_COLOR);
    asMsg = "Full LogFilePathname = " +  frmMain->masFullLogFilePath;
    frmMain->DisplayUserMessage(asMsg.c_str(),COMPILE_COLOR);
    if(frmMain->mfpLog==NULL)
    {
        frmMain->DisplayUserMessage("??? Could not open Logfile",COMPILE_COLOR);
    }
    frmMain->DisplayUserMessage("++++++++++++++++++++++",COMPILE_COLOR);

    frmMain->DisplayUserMessage("Compile Start",COMPILE_COLOR);
    nError = SrcCompileFile(pFilename,0,pScrMngr );
    frmMain->DisplayUserMessage("Compile End",COMPILE_COLOR);

    TScriptAction *pAction = SrcFixJumpLabels(pScrMngr);

    if(pAction)
    {
        nError = SCRERROR_UNKNOWN_LABELNAME;

        AnsiString asMsg;
        asMsg.printf("Script File Unresolved Jump Label = %s",pAction->jump.pLabelName);
        frmMain->DisplayUserMessage(asMsg.c_str());
    }

    return nError;
}

//=============================================================================
//=============================================================================
static int SrcCountEscapedDestLenEx(char *pNewValue, int nStrLen)
{
    int nDestLen=0;
    char *pSrc = pNewValue;

    //Count the number of backslashes in the source string
    while(nStrLen--)
    {
        nDestLen++;
        if(*pSrc == '\\')
        {
            nDestLen++;
        }
        else if(*pSrc == '[')
        {
            nDestLen+=2;
        }
        else if(*pSrc == ']')
        {
            nDestLen+=2;
        }
        pSrc++;
    }
    return nDestLen;
}

//=============================================================================
//=============================================================================
static int SrcCountEscapedDestLen(char *pNewValue)
{
    return SrcCountEscapedDestLenEx(pNewValue, strlen(pNewValue));
}

//=============================================================================
//=============================================================================
static void SrcCopyAndEscapeExpandEx(char *pDst, char *pSrc, int nSrcLen)
{
    while(nSrcLen--)
    {
        *pDst = *pSrc;
        if(*pSrc == '\\')
        {
            pDst++;
            *pDst = '\\';
        }
        else if(*pSrc == '[')
        {
            *pDst++ = '\\';
            *pDst++ = '5';
            *pDst   = 'b';
        }
        else if(*pSrc == ']')
        {
            *pDst++ = '\\';
            *pDst++ = '5';
            *pDst   = 'd';
        }
        pDst++;
        pSrc++;
    }
    *pDst++=0;
}

//=============================================================================
//=============================================================================
static void SrcCopyAndEscapeExpand(char *pDst, char *pSrc)
{
    SrcCopyAndEscapeExpandEx(pDst,pSrc,strlen(pSrc));
}

//=============================================================================
//=============================================================================
bool SrcSaveVariableValueInteger(TScriptVariable *pVariable, int nValue)
{
    bool fRetVal=false;
    if( pVariable )
    {
        if( pVariable->pValue )
        {
            int nCurLen = strlen(pVariable->pValue);
            if( nCurLen <= 15 )
            {
                MiscMemFree(pVariable->pValue);
                pVariable->pValue=NULL;
            }
        }
        if( pVariable->pValue == NULL )
        {
            pVariable->pValue = (char *)MiscMemAlloc(15+1);
        }

        if(pVariable->pValue)
        {
            sprintf(pVariable->pValue,"%d",nValue);
            fRetVal = true;
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
bool SrcSaveVariableValueEx(TScriptVariable *pVariable, char *pNewValue, int nValLen)
{
    bool fRetVal=false;
    if( pVariable )
    {
        int nDestLen;
        nDestLen=SrcCountEscapedDestLenEx(pNewValue,nValLen);

        if( pVariable->pValue )
        {
            int nCurLen = strlen(pVariable->pValue);
            if( nDestLen > nCurLen )
            {
                MiscMemFree(pVariable->pValue);
                pVariable->pValue=NULL;
            }
        }
        if( pVariable->pValue == NULL )
        {
            pVariable->pValue = (char *)MiscMemAlloc(nDestLen+1);
        }

        if(pVariable->pValue)
        {
            SrcCopyAndEscapeExpandEx(pVariable->pValue, pNewValue, nValLen );
            fRetVal = true;
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
bool SrcSaveVariableValue(TScriptVariable *pVariable, char *pNewValue)
{
    return SrcSaveVariableValueEx(pVariable,pNewValue,strlen(pNewValue));
}

//=============================================================================
//=============================================================================
bool SrcSaveVariableValueEx(TScriptManager *pScrMngr,char *pVarName, char *pNewValue, int nValLen)
{
    TScriptVariable *pVariable = SrcGetScriptVariable(pScrMngr,pVarName);
    if( pVariable == NULL )
    {
        return false;
    }
    return SrcSaveVariableValueEx(pVariable,pNewValue,strlen(pNewValue));
}

//=============================================================================
//=============================================================================
bool SrcSaveVariableValue(TScriptManager *pScrMngr,char *pVarName, char *pNewValue)
{
    return SrcSaveVariableValueEx(pScrMngr,pVarName,pNewValue,strlen(pNewValue));
}


//=============================================================================
//=============================================================================
bool SrcConCatVariableValue(TScriptVariable *pVariable, char *pNewValue)
{
    bool fRetVal=false;
    if( pVariable )
    {
        int nDestLen=SrcCountEscapedDestLen(pNewValue);

        char *pOrig = pVariable->pValue;
        int nOrigLen=0;
        int nNewLen;

        if( pOrig )
        {
            nOrigLen = strlen(pOrig);
        }

        nNewLen = nDestLen + nOrigLen;
        if( nNewLen )
        {
            pVariable->pValue = (char *)MiscMemAlloc(nNewLen+1);
            if(pVariable->pValue)
            {
                char *pDst = pVariable->pValue;
                if(nOrigLen)
                {
                    memcpy( pDst, pOrig , nOrigLen );
                    pDst += nOrigLen;
                }
                SrcCopyAndEscapeExpand(pDst, pNewValue);
                fRetVal = true;
            }
        }
        else
        {
            pVariable->pValue=NULL;
            fRetVal=true;
        }

        if( pOrig )
        {
            MiscMemFree(pOrig);
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
int SrcFilterMoveDestLen(char *pSrc, int nMask, bool fInvert)
{
    int nLen=0;
    if(pSrc)
    {
        while(*pSrc)
        {
            if(nMask & 0x0001) //octal digits
            {
                if( (*pSrc >= '0') && (*pSrc <= '7') )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0002) //decimal digits
            {
                if( (*pSrc >= '0') && (*pSrc <= '9') )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0004) //isxdigit
            {
                if( isxdigit(*pSrc) )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0008) //'A' to 'Z'
            {
                if( (*pSrc >= 'A') && (*pSrc <= 'Z') )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0010) //'a' to 'z'
            {
                if( (*pSrc >= 'a') && (*pSrc <= 'z') )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0020) //' '
            {
                if( *pSrc == ' ' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0040) //'.'
            {
                if( *pSrc == '.' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0080) //','
            {
                if( *pSrc == ',' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0100) //'('
            {
                if( *pSrc == '(' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0200) //')'
            {
                if( *pSrc == ')' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0400) //'{'
            {
                if( *pSrc == '{' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x0800) //'}'
            {
                if( *pSrc == '}' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x1000) //'['
            {
                if( *pSrc == '[' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x2000) //']'
            {
                if( *pSrc == ']' )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x4000) //ispunct
            {
                if( ispunct(*pSrc) )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x8000) // isspace
            {
                if( isspace(*pSrc) )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x10000) // iscntrl
            {
                if( iscntrl(*pSrc) )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            if(nMask & 0x800000) // >=0x80
            {
                if( (unsigned char)(*pSrc) >0x80 )
                {
                    if(fInvert)break;
                    nLen++;
                    pSrc++;
                    continue;
                }
            }
            //no match
            if( fInvert )
            {
                nLen++;
                pSrc++;
            }
            else
            {
                break;
            }
        }
    }
    return nLen;
}


//=============================================================================
//=============================================================================
bool SrcFilterMoveVariable(TScriptVariable *pVariableDst, TScriptVariable *pVariableSrc, int nMask, bool fInvert)
{
    bool fRetVal=false;
    if( pVariableDst && pVariableSrc )
    {
        MiscMemFree(pVariableDst->pValue);
        int nDestLen=SrcFilterMoveDestLen(pVariableSrc->pValue, nMask, fInvert);
        if( nDestLen )
        {
            pVariableDst->pValue = (char *)MiscMemAlloc(nDestLen+1);
            if(pVariableDst->pValue)
            {
                memcpy( pVariableDst->pValue, pVariableSrc->pValue , nDestLen );
                pVariableDst->pValue[nDestLen]=0;

                int nRemain = strlen(pVariableSrc->pValue)-nDestLen;
                memcpy( pVariableSrc->pValue, &pVariableSrc->pValue[nDestLen] , nRemain );
                pVariableSrc->pValue[nRemain]=0;

                fRetVal = true;
            }
        }
        else
        {
            pVariableDst->pValue=(char *)MiscMemAlloc(1);
            pVariableDst->pValue[0]=0;
            fRetVal=true;
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
bool SrcDeHexizeVariable(TScriptVariable *pVariableDst, TScriptVariable *pVariableSrc, int nFlags)
{
    bool fRetVal=false;
    if( pVariableDst && pVariableSrc )
    {
        MiscMemFree(pVariableDst->pValue);
        pVariableDst->pValue = MiscDeHexizeString(pVariableSrc->pValue);
        if( pVariableDst->pValue)
        {
            fRetVal=true;
        }
        else
        {
            pVariableDst->pValue=(char *)MiscMemAlloc(1);
            pVariableDst->pValue[0]=0;
        }
    }
    return fRetVal;
}


/******************************************************************************/
// END OF FILE
/******************************************************************************/


