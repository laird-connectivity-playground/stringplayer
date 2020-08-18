/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle
**
** Module:      MISC.CPP
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
#include "MiscEx.h"

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

//=============================================================================
//=============================================================================
uint8 MiscConvertHexChr2Nibble(uint8 ch)
{
    if( (ch>='0') && (ch<='9') )
    {
        return (ch - '0');
    }
    else if( (ch>='A') && (ch<='F') )
    {
        return (ch - 'A' + 10);
    }
    else if( (ch>='a') && (ch<='f') )
    {
        return (ch - 'a' + 10);
    }
    return 0;
}

//=============================================================================
//=============================================================================
uint8 MiscConvert2HexChr2Binary(uint8 *pSrc)
{
    return (MiscConvertHexChr2Nibble(pSrc[0])<<4) + MiscConvertHexChr2Nibble(pSrc[1]);
}


//=============================================================================
// Converts the byte into a two character string - UNTERMINATED
//=============================================================================
uint8 MiscConvertNibbleToAscii(uint8 val)
{
    val &= 0x0F;

    if( val <= 9 )
    {
        val += '0';
    }
    else
    {
        val += ('A' - 10);
    }

    return val;
}

//=============================================================================
//=============================================================================
uint8 *MiscAdvanceToNonWhitespace(uint8 *pSrc)
{
    while( *pSrc )
    {
        if(*pSrc <= ' ')
        {
            pSrc++;
        }
        else
        {
            break;
        }
    }
    return pSrc;
}

//=============================================================================
//=============================================================================
uint8 *MiscAdvanceToNonHexDigit(uint8 *pSrc)
{
    while( *pSrc )
    {
        if( isxdigit(*pSrc))
        {
            pSrc++;
        }
        else
        {
            break;
        }
    }
    return pSrc;
}

//=============================================================================
//=============================================================================
bool MiscExtractBdAddr(uint8 *pSrc, uint8 *pBdAddr)
{
    bool fRetVal = false;
    uint8 *pEnd;
    if(pSrc && pBdAddr)
    {
        pSrc = MiscAdvanceToNonWhitespace(pSrc);
        pEnd = MiscAdvanceToNonHexDigit(pSrc);
        if( (pEnd - pSrc) == 12 )
        {
            pBdAddr[0] = MiscConvert2HexChr2Binary(pSrc); pSrc += 2;
            pBdAddr[1] = MiscConvert2HexChr2Binary(pSrc); pSrc += 2;
            pBdAddr[2] = MiscConvert2HexChr2Binary(pSrc); pSrc += 2;
            pBdAddr[3] = MiscConvert2HexChr2Binary(pSrc); pSrc += 2;
            pBdAddr[4] = MiscConvert2HexChr2Binary(pSrc); pSrc += 2;
            pBdAddr[5] = MiscConvert2HexChr2Binary(pSrc);
            fRetVal = true;
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
bool MiscExtractUuid(uint8 *pSrc, uint8 *pUuid)
{
    bool fRetVal = false;
    uint8 *pEnd;
    if(pSrc && pUuid)
    {
        pSrc = MiscAdvanceToNonWhitespace(pSrc);
        pEnd = MiscAdvanceToNonHexDigit(pSrc);
        if( (pEnd - pSrc) == 4 )
        {
            pUuid[0] = MiscConvert2HexChr2Binary(pSrc); pSrc += 2;
            pUuid[1] = MiscConvert2HexChr2Binary(pSrc);
            fRetVal = true;
        }
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
uint32 MiscReadHostU32(uint8 *pSrc)
{
    uint32 nRetVal;

    nRetVal = pSrc[0];
    nRetVal <<= 8;
    nRetVal |= pSrc[1];
    nRetVal <<= 8;
    nRetVal |= pSrc[2];
    nRetVal <<= 8;
    nRetVal |= pSrc[3];

    return nRetVal;
}

//=============================================================================
//=============================================================================
bool MiscConvertDecStrToU32(uint8 *pSrc,uint32 *pVal)
{
    uint8 *pTemp = MiscAdvanceToNonWhitespace(pSrc);
    int strLen = strlen(pTemp);
    if(strLen)
    {
        while(*pTemp)
        {
            if( ! isdigit(*pTemp) )return false;
            pTemp++;
        }
        *pVal = atoi(pSrc);
    }
    return false;
}

//=============================================================================
//=============================================================================
bool MiscConvertHexStrToU32(uint8 *pSrc,uint32 *pVal)
{
    uint8 *pTemp = MiscAdvanceToNonWhitespace(pSrc);
    int strLen = strlen(pTemp);
    if(strLen>=2)
    {
        if( *pTemp != '$')return false;
        pTemp++;
        uint8 *pStart = pTemp;
        while(*pTemp)
        {
            if( ! isxdigit(*pTemp) )return false;
            pTemp++;
        }
        sscanf(pStart,"%x",pVal);
    }
    return false;
}

//=============================================================================
//=============================================================================
char *MiscUuidNameString(uint16 nUuid)
{
    char *pRetVal="????";

    switch(nUuid)
    {
    case 0x1101 : pRetVal="Serial Port Profile";        break;
    case 0x1102 : pRetVal="LAP";                        break;
    case 0x1103 : pRetVal="Dialup Networking (DUN)";    break;
    case 0x1104 : pRetVal="IrMC Sync";                  break;
    case 0x1105 : pRetVal="OBEX Object Push";           break;
    case 0x1106 : pRetVal="OBEX File Transfer";         break;
    case 0x1107 : pRetVal="IrMC Sync Command";          break;
    case 0x1108 : pRetVal="Headset";                    break;
    case 0x1109 : pRetVal="Cordless Telephony";         break;
    case 0x1110 : pRetVal="Intercom";                   break;
    case 0x1111 : pRetVal="Fax";                        break;
    case 0x1112 : pRetVal="Audio Gateway";              break;
    case 0x1113 : pRetVal="WAP";                        break;
    case 0x1114 : pRetVal="WAP Client";                 break;
    }

    return pRetVal;
}

//=============================================================================
//=============================================================================
AnsiString  MiscGetTimeDateString(void)
{
    return FormatDateTime("dd-mmm-yyyy hh:nn", Now());
}

//=============================================================================
//=============================================================================
AnsiString  MiscGetElapsedTimeString(unsigned int nStartTick)
{
    unsigned int nTickNow = GetTickCount();
    unsigned int nElapsed = nTickNow - nStartTick;
    AnsiString asRet;

    if( nElapsed < 1000 )
    {
        asRet.printf("%d msec",nElapsed);
    }
    else
    {
        nElapsed /= 1000;
        if( nElapsed < 60 )
        {
            asRet.printf("%d sec",nElapsed);
        }
        else
        {
            unsigned int nMin = nElapsed/60;
            unsigned int nSec = nElapsed%60;
            if( nMin < 60 )
            {
                asRet.printf("%d min %d sec",nMin,nSec);
            }
            else
            {
                unsigned int nHour = nMin/60;
                nMin %= 60;
                asRet.printf("%d hour %d min %d sec",nHour,nMin,nSec);
            }
        }
    }

    return asRet;
}

//=============================================================================
//=============================================================================
FILE  *MiscOpenLogFile(AnsiString asFilename, bool fAppend, bool fTimeStamp, bool fHtmlFormatting)
{
    FILE *fp = NULL;

    if( fAppend )
    {
        fp = fopen( asFilename.c_str(), "at+");
        //Disable output buffering
        setvbuf(fp, NULL, _IONBF, 0);
    }
    else
    {
        fp = fopen( asFilename.c_str(), "wt+");
        //Disable output buffering
        setvbuf(fp, NULL, _IONBF, 0);
    }

    if(fp && fTimeStamp)
    {
        AnsiString asTemp = "Start: ";
        asTemp += MiscGetTimeDateString();
        if( fHtmlFormatting)
        {
            MiscWriteLogFile(fp, MESSAGE_COLOR, "**********************************************");
            MiscWriteLogFile(fp, MESSAGE_COLOR, asTemp);
            MiscWriteLogFile(fp, MESSAGE_COLOR, "**********************************************");
        }
        else
        {
            MiscWriteLogFilePlain(fp, "**********************************************");
            MiscWriteLogFilePlain(fp, asTemp);
            MiscWriteLogFilePlain(fp, "**********************************************");
        }
    }
    return fp;
}

//=============================================================================
//=============================================================================
void   MiscCloseLogFile(FILE *pFileHandle)
{
    if( pFileHandle )fclose(pFileHandle);
}

//=============================================================================
//=============================================================================
void   MiscWriteLogFile(FILE *pFileHandle, TColor color, AnsiString asLine)
{
    if( pFileHandle )
    {
        fputs("<TT><font color=",pFileHandle);

        switch(color)
        {
        case RSPPKT_COLOR:      fputs("blue>",pFileHandle);     break;
        case CMDPKT_COLOR:      fputs("red>",pFileHandle);      break;
        case FAIL_COLOR:        fputs("maroon>",pFileHandle);     break;
        case TRACE_COLOR:       fputs("gray>",pFileHandle);     break;
        case MESSAGE_COLOR:     fputs("green>",pFileHandle);     break;
        case LOG_COLOR:         fputs("maroon>",pFileHandle);     break;
        default:                fputs("black>",pFileHandle);    break;
        }
        char *pStr = asLine.c_str();
        while(*pStr)
        {
            if( *pStr == '<' )
            {
                fputs("&lt;",pFileHandle);
            }
            else if( *pStr == '>' )
            {
                fputs("&gt;",pFileHandle);
            }
            else if( *pStr == '&' )
            {
                fputs("&amp;",pFileHandle);
            }
            else if( *pStr == ' ' )
            {
                fputs("&nbsp;",pFileHandle);
            }
            else
            {
                fputc(*pStr,pFileHandle);
            }
            pStr++;
        }
        fprintf(pFileHandle,"<BR></font></TT>\r\n");
    }
}

//=============================================================================
//=============================================================================
void   MiscWriteLogFilePlain(FILE *pFileHandle, AnsiString asLine)
{
    if( pFileHandle )
    {
        char *pStr = asLine.c_str();
        fprintf(pFileHandle,"%s\r\n",pStr);
    }
}

//=============================================================================
//=============================================================================
bool MiscCopyLogFile(AnsiString asFileSrc, AnsiString asFileDest)
{
    bool fRetVal = false;
    FILE *fpSrc=NULL;
    FILE *fpDest=NULL;
    uint8 baBuffer[32];
    int nRead;

    fpSrc  = fopen( asFileSrc.c_str(), "rt+");
    fpDest = fopen( asFileDest.c_str(), "wt+");
    if( fpSrc && fpDest )
    {
        //Disable file buffering
        setvbuf(fpSrc, NULL, _IONBF, 0);
        setvbuf(fpDest, NULL, _IONBF, 0);

        nRead = fread(baBuffer, sizeof(uint8), sizeof(baBuffer), fpSrc);
        while(nRead)
        {
            fwrite(baBuffer, sizeof(uint8), nRead, fpDest);
            nRead = fread(baBuffer, sizeof(uint8), sizeof(baBuffer), fpSrc);
        }
        fRetVal = true;
    }

    if( fpSrc )
    {
        fclose(fpSrc);
    }
    if( fpDest )
    {
        fclose(fpDest);
    }
    return fRetVal;
}

//=============================================================================
//=============================================================================
uint16 MiscCalcBer(uint8 linkQual)
{
/*
    The HCI_Get_Link_Quality command returns a number that is directly
    equivalent to Bit Error Rate on the following scale…

    Value BER
    0xff 0.0000
    0xfe 0.0025
    0xfd 0.0050
    0xfc 0.0075
    ... ...
    0xd8 0.0975
    0xd7 0.1000 (Sensitivity BER Limit)
    ... ...

    The scale below 215 (0xd7) is not fully characterised since results
    in this region are not stable and often indicate a link is dropping
    more packets than it is receiving.
*/

    //This routine returns a value in the range 0 to 1000
    if( (unsigned int)linkQual > 255 ) linkQual = 255;
    unsigned int nDiff = 255 - linkQual;

    nDiff *= 25;

    return (uint16)nDiff;
}

//=============================================================================
//=============================================================================
void  *MiscMemAlloc(unsigned int nSize)
{
    if(nSize<MIN_MALLOC_BLOCK_SIZE)nSize = MIN_MALLOC_BLOCK_SIZE;
    return malloc(nSize);
}

//=============================================================================
//=============================================================================
void   MiscMemFree(void *pBlock)
{
    free(pBlock);
}


//=============================================================================
//=============================================================================
AnsiString  MiscExtractFolder(AnsiString asFullPath, AnsiString &asFilename)
{
    AnsiString asRet="";
    int nLen = asFullPath.Length();
    int ch;
    if(nLen)
    {
        while(nLen)
        {
            ch=asFullPath[nLen];
            if( (ch == '\\') || (ch == '/') )
            {
                break;
            }
            else
            {
                nLen--;
            }
        }

        if(nLen)
        {
            asRet = asFullPath.SubString(1, nLen);
            asFilename = asFullPath.SubString(nLen+1, asFullPath.Length());
        }
    }
    return asRet;
}

//=============================================================================
//=============================================================================
void  MiscAddBackSlashToFoldername(AnsiString &asFoldername)
{
    int len = asFoldername.Length();
    int ch;

    if(len)
    {
        ch = asFoldername[len];
        if(  !((ch=='\\')||(ch=='/'))  )
        {
            asFoldername += "\\";
        }
    }
}

//=============================================================================
//=============================================================================
AnsiString  MiscExtractFilenameOnly(AnsiString asFullFilename)
{
    AnsiString asRet="";
    asFullFilename = MiscRemoveExtention(asFullFilename); /* remove the extension */
    int nLen = asFullFilename.Length();
    int ch;
    if(nLen)
    {
        while(nLen)
        {
            ch = asFullFilename[nLen];
            if( (ch == '\\') || (ch == '/')  )
            {
                break;
            }
            else
            {
                nLen--;
            }
        }

        if(nLen)
        {
            asRet = asFullFilename.SubString((nLen+1), asFullFilename.Length());
        }
        else
        {
            asRet = asFullFilename;
        }
    }
    return asRet;
}


//=============================================================================
//=============================================================================
AnsiString  MiscRemoveExtention(AnsiString asFullFilename)
{
    AnsiString asRet="";
    int nLen = asFullFilename.Length();
    int ch;
    if(nLen)
    {
        while(nLen)
        {
            ch = asFullFilename[nLen];
            if( ch == '.' )
            {
                nLen--;
                break;
            }
            else
            {
                nLen--;
            }
        }

        if(nLen)
        {
            asRet = asFullFilename.SubString(1,nLen);
        }
        else
        {
            asRet = asFullFilename;
        }
    }
    return asRet;
}

//=============================================================================
//=============================================================================
uint8 MiscHexAsciiToBinary(char *pSrc)
{
    uint8 nRetVal=0;

    if( (pSrc[0]>='0') && (pSrc[0]<='9') )
    {
        nRetVal = (pSrc[0]-'0')<<4;
    }
    else if( (pSrc[0]>='A') && (pSrc[0]<='F') )
    {
        nRetVal = (pSrc[0]-'A'+10)<<4;
    }
    else if( (pSrc[0]>='a') && (pSrc[0]<='f') )
    {
        nRetVal = (pSrc[0]-'a'+10)<<4;
    }

    pSrc++;

    if( (pSrc[0]>='0') && (pSrc[0]<='9') )
    {
        nRetVal += (pSrc[0]-'0');
    }
    else if( (pSrc[0]>='A') && (pSrc[0]<='F') )
    {
        nRetVal += (pSrc[0]-'A'+10);
    }
    else if( (pSrc[0]>='a') && (pSrc[0]<='f') )
    {
        nRetVal += (pSrc[0]-'a'+10);
    }

    return nRetVal;
}

//------------------------------------------------------------------------------
// Will return NULL if malloc fails otherwise a pointer to a malloced pointer
// which the caller will need to free using MiscMemFree()
//------------------------------------------------------------------------------
char * MiscDeHexizeString(
    const unsigned char *pString //null terminated string
    )
{
    char *pRetVal=NULL;
    char *pDstStr;
    char ch;
    int nDstLen=0;
    int nMaxDstStrLen;

    //advance to non-white space character
    pString = MiscAdvanceToNonWhitespace((uint8 *)pString);
    int nSrcPos=0;
    int nStringLen=strlen( pString );
    //nMaxDstStrLen =  ((nStringLen+1)/2);
    nMaxDstStrLen =  nStringLen;  //because [ and ] are escaped so potentially if all then same length
    pRetVal= pDstStr =(char *)MiscMemAlloc(nMaxDstStrLen+1);
    if(pDstStr)
    {
        while(*pString && (nDstLen<nMaxDstStrLen) && (nSrcPos<nStringLen))
        {
            if( isxdigit(pString[0]) )
            {
                //the first digit is a hex digit
                if( isxdigit(pString[1]) )
                {
                    //the second digit is also a hex digit
                    ch = MiscConvert2HexChr2Binary((uint8 *)pString);
                    if( (ch == '[') || (ch==']') )
                    {
                        *pDstStr++= 0x5C; // the backslaxh chaarter
                        nDstLen++;
                    }
                    *pDstStr++=ch;
                    nDstLen++;
                    pString+=2;
                    nSrcPos+=2;
                    if(ch==0)
                    {
                        break;
                    }
                }
                else
                {
                    //the second digit is NOT a hex digit
                    nDstLen--;
                    break;
                }
            }
            else
            {
                //the first digit is NOT a hex digit and is NOT a whitespace either
                break;
            }
        }
        *pDstStr=0;
    }
    return pRetVal;
}



//=============================================================================
//=============================================================================
int MiscEscapeCharacter(unsigned char ch, char *pDest)
{
    int nExpandSize=1;
    //assume pDest is at least 4 bytes long
    if(ch == '\r')
    {
        strcpy(pDest,"\\r");
        nExpandSize=2;
    }
    else if(ch == '\n')
    {
        strcpy(pDest,"\\n");
        nExpandSize=2;
    }
    else if(ch == 0x08)
    {
        strcpy(pDest,"\\b");
        nExpandSize=2;
    }
    else if(ch == '\\')
    {
        strcpy(pDest,"\\\\");
        nExpandSize=2;
    }
    else if(ch == '[')
    {
        strcpy(pDest,"\\[");
        nExpandSize=2;
    }
    else if(ch == ']')
    {
        strcpy(pDest,"\\]");
        nExpandSize=2;
    }
    else if((ch < ' ') || (ch>=127) )
    {
        sprintf(pDest,"\\%02X",ch);
        nExpandSize=3;
    }
    else
    {
        pDest[0] = ch;
        pDest[1] = 0;
        nExpandSize=1;
    }
    return nExpandSize;
}

//=============================================================================
//=============================================================================
int MiscEscapeCharacterNullAnd0x8X(unsigned char ch, char *pDest)
{
    int nExpandSize=1;
    //assume pDest is at least 4 bytes long

    if((ch == 0) || (ch>=127) )
    {
        sprintf(pDest,"\\%02X",ch);
        nExpandSize=3;
    }
    else
    {
        pDest[0] = ch;
        pDest[1] = 0;
        nExpandSize=1;
    }
    return nExpandSize;
}

//=============================================================================
//=============================================================================
char *MiscUnescapeCharacter(char *pCh, char *pSrc)
{
    *pCh=0;
    if( pSrc[0] == '\\' )
    {
        if( pSrc[1]=='r')
        {
            *pCh = '\r';
            pSrc += 2;
        }
        else if(pSrc[1]=='n')
        {
            *pCh = '\n';
            pSrc += 2;
        }
        else if(pSrc[1]=='b')
        {
            *pCh = 0x08;
            pSrc += 2;
        }
        else if(pSrc[1]=='[')
        {
            *pCh = '[';
            pSrc += 2;
        }
        else if(pSrc[1]==']')
        {
            *pCh = ']';
            pSrc += 2;
        }
        else if(pSrc[1]=='\\')
        {
            *pCh = '\\';
            pSrc += 2;
        }
        else if(isxdigit(pSrc[1]) && isxdigit(pSrc[2]) )
        {
            *pCh = MiscHexAsciiToBinary(&pSrc[1]);
            pSrc += 3;
        }
        else
        {
            *pCh = *pSrc++;
        }
    }
    else
    {
        *pCh = *pSrc++;
    }
    return pSrc;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static bool MiscStrContainsFrag(char *pStr, char *pSubStr, bool fCaseSensitive)
{
    char ch1,ch2;

    for(;;)
    {
        ch1 = fCaseSensitive ? *pStr    : toupper(*pStr);
        ch2 = fCaseSensitive ? *pSubStr : toupper(*pSubStr);

        if( ch2 == 0 )
        {
            //end of substring
            return true;
        }
        if( ch1 == 0 )
        {
            //substring too long
            return false;
        }
        if( ch1 != ch2 )
        {
            //no substring match
            return false;
        }
        pStr++;
        pSubStr++;
    }
}


//------------------------------------------------------------------------------
// Returns true if the substring is contained in str
//------------------------------------------------------------------------------
bool MiscStrContains(char *pStr, char *pSubStr, bool fCaseSensitive)
{
    int nStrLen  = strlen(pStr);
    int nSubStrLen = strlen(pSubStr);

    if( nSubStrLen == 0 )
    {
        //sub string is empty so treat it as contained in the str
        return true;
    }
    while( nStrLen >= nSubStrLen )
    {
        if( MiscStrContainsFrag(pStr, pSubStr, fCaseSensitive) )
        {
            return true;
        }
        pStr++;
        nStrLen--;
    }
    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char *MiscOutcomeString(EExpectedOutcome eOutcome)
{
    switch(eOutcome)
    {
    case OUTCOME_NONE:          return "OUTCOME_NONE";
    case OUTCOME_OK:            return "OUTCOME_OK";
    case OUTCOME_ERROR:         return "OUTCOME_ERROR";
    case OUTCOME_MOD_TIMEOUT:   return "OUTCOME_MOD_TIMEOUT";
    case OUTCOME_SCR_TIMEOUT:   return "OUTCOME_SCR_TIMEOUT";
    case OUTCOME_PATTERN:       return "OUTCOME_PATTERN";
    case OUTCOME_PACKET:        return "OUTCOME_PACKET";
    case OUTCOME_HCI_LE_EVENT:  return "OUTCOME_HCI_LE_EVENT";
    default:
        break;
    }
    return "OUTCOME_???";
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool MiscCompareStrings(char *pString,char *pPattern,int len)
{
    char ch;
    while(len--)
    {
        if( (ch = *pPattern++) != *pString++ )
        {
            switch(ch)
            {
            case '*':
                break;
            default:
                return false;
            }
        }
    }
    return true;
}
//------------------------------------------------------------------------------
// returns -1 if not found. Otherwise index for pattern that was found
//------------------------------------------------------------------------------
int MiscCompareStringsMulti(char *pString,MiscMultiPattern *pMultiPattern, int nMultiPatterns)
{
    int nIndex;
    for(nIndex=0;nIndex<nMultiPatterns;nIndex++)
    {
        if( MiscCompareStrings(pString,(char *)pMultiPattern->mpPattern ,pMultiPattern->mPatternLen) )
        {
            return nIndex;
        }
        pMultiPattern++;
    }
    return -1;
}

//------------------------------------------------------------------------------
// returns -1 if not found. Otherwise index for pattern that was found
//------------------------------------------------------------------------------
int MiscContainsPatternMulti(char *pString,int nStringLen, MiscMultiPattern *pMultiPattern, int nMultiPatterns)
{
    int nIndex;
    for(nIndex=0;nIndex<nMultiPatterns;nIndex++)
    {
        if( MiscContainsPattern(pString,nStringLen, (char *)pMultiPattern->mpPattern ,pMultiPattern->mPatternLen) )
        {
            return nIndex;
        }
        pMultiPattern++;
    }
    return -1;
}

//------------------------------------------------------------------------------
// returns NULL if not found. Otherwise pointer to position
//------------------------------------------------------------------------------
char * MiscContainsPattern(char *pString,int nStringLen, char *pPattern,int nPatternlen)
{
    while(nStringLen-- >= nPatternlen)
    {
        if(MiscCompareStrings(pString,pPattern,nPatternlen))
        {
            return pString;
        }
        pString++;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// returns NULL if not found. Otherwise pointer to position
//------------------------------------------------------------------------------
char * MiscContainsPattern(char *pString,char *pPattern)
{
    int nStringLen  = strlen(pString);
    int nPatternlen = strlen(pPattern);

    return   MiscContainsPattern(pString,nStringLen,pPattern,nPatternlen);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char *MiscMpExtractData(char *pHexFullResp, unsigned int nChannel, char *pDestData, int nDestDataLen)
{
    char *pDstBuf = pDestData;
    int nState=0;
    unsigned char nLen;
    unsigned char nChan;
    int nCount=0;
    int nSrcLen;

    if( pHexFullResp == NULL)
    {
        return NULL;
    }

    nDestDataLen--; /* reserve space for null terminator */
    *pDstBuf=0;

    nSrcLen = strlen(pHexFullResp) & 0xFFFFFFFE;
    while(nSrcLen)
    {
        switch(nState)
        {
        case 0: /* waiting for len */
            nLen = MiscConvert2HexChr2Binary(pHexFullResp);
            if(nLen--)
            {
                nState = 1;
            }
            break;

        case 1: /* waiting for channel */
            nChan = MiscConvert2HexChr2Binary(pHexFullResp);
            if(--nLen==0)
            {
                nState = 0;
            }
            else
            {
                if(nChan==0)
                {
                    nState = 3;
                }
                else
                {
                    nState = 2;
                }
            }
            break;

        case 2: /* waiting for data */
            if(((unsigned char)nChannel== nChan) && (nCount<nDestDataLen))
            {
                *pDstBuf++ = pHexFullResp[0];
                *pDstBuf++ = pHexFullResp[1];
                nCount += 2;
            }
            if(--nLen == 0)
            {
                nState = 0;
            }
            break;

        case 3: /* waiting for packet */
            if(--nLen == 0)
            {
                nState = 0;
            }
            break;
        }
        pHexFullResp+=2;
        nSrcLen -= 2;
    }
    *pDstBuf=0;

    return pDestData;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned char *MiscAdvanceToNextCharacter(unsigned char *pSrc, unsigned char ch)
{
    while( *pSrc )
    {
        if(*pSrc == ch)
        {
            return pSrc;
        }
        pSrc++;
    }
    return NULL;
}


/******************************************************************************/
// END OF FILE
/******************************************************************************/

