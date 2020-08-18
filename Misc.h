/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Module:      MISC.H
**
*******************************************************************************/

#if !defined(_MISC_H)     /* prevent multiple inclusions */
#define _MISC_H

/******************************************************************************/
// Include Files
/******************************************************************************/
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include "AdPort.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <assert.h>

#include <stdlib.h>
#include <string.h>

/******************************************************************************/
// Defines
/******************************************************************************/

#define RSPPKT_COLOR      clBlue
#define CMDPKT_COLOR      clRed
#define MESSAGE_COLOR     clMaroon
#define TRACE_COLOR       clGray
#define PRINT_COLOR       clGreen
#define FAIL_COLOR        clPurple
#define LOG_COLOR         clNavy
#define COMPILE_COLOR     clNavy	

#define MIN_MALLOC_BLOCK_SIZE       16

/******************************************************************************/
// Macros
/******************************************************************************/

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

typedef unsigned char       uint8;
typedef unsigned long       uint32;
typedef unsigned short      uint16;

/******************************************************************************/
// Enum Typedefs
/******************************************************************************/

/******************************************************************************/
// Forward declaration of Class, Struct & Unions
/******************************************************************************/

/******************************************************************************/
// Class definitions
/******************************************************************************/

/******************************************************************************/
// Struct definitions
/******************************************************************************/

typedef struct
{
    unsigned char *mpPattern;
    int            mPatternLen;
}
MiscMultiPattern;


/******************************************************************************/
// Union definitions
/******************************************************************************/

/******************************************************************************/
// Global Functions (API etc) exported for other modules
/******************************************************************************/
uint8  MiscConvertHexChr2Nibble(uint8 ch);
uint8  MiscConvert2HexChr2Binary(uint8 *pSrc);
uint8  MiscConvertNibbleToAscii(uint8 val);
uint8 *MiscAdvanceToNonWhitespace(uint8 *pSrc);
uint8 *MiscAdvanceToNonHexDigit(uint8 *pSrc);
bool   MiscExtractBdAddr(uint8 *pSrc, uint8 *pBdAddr);
bool   MiscExtractUuid(uint8 *pSrc, uint8 *pUuid);
uint32 MiscReadHostU32(uint8 *pSrc);
bool   MiscConvertDecStrToU32(uint8 *pSrc,uint32 *pVal);
bool   MiscConvertHexStrToU32(uint8 *pSrc,uint32 *pVal);
char  *MiscUuidNameString(uint16 nUuid);
uint16 MiscCalcBer(uint8 linkQual);

FILE  *MiscOpenLogFile(AnsiString asFilename, bool fAppend, bool fTimeStamp, bool fHtmlFormatting);
void   MiscCloseLogFile(FILE *pFileHandle);
void   MiscWriteLogFile(FILE *pFileHandle, TColor color, AnsiString asLine);
void   MiscWriteLogFilePlain(FILE *pFileHandle, AnsiString asLine);
bool   MiscCopyLogFile(AnsiString asFileSrc, AnsiString asFileDest);

AnsiString  MiscGetTimeDateString(void);
AnsiString  MiscGetElapsedTimeString(unsigned int nStartTick);

AnsiString  MiscExtractFolder(AnsiString asFullPath, AnsiString &asFilename);
AnsiString  MiscExtractFilenameOnly(AnsiString asFilename);
AnsiString  MiscRemoveExtention(AnsiString asFullFilename);
void  MiscAddBackSlashToFoldername(AnsiString &asFoldername);

uint8 MiscHexAsciiToBinary(char *pSrc);
int  MiscEscapeCharacter(unsigned char ch, char *pDest);
int  MiscEscapeCharacterNullAnd0x8X(unsigned char ch, char *pDest);
char *MiscUnescapeCharacter(char *pCh, char *pSrc);

bool MiscCompareStrings(char *pString,char *pPattern,int len);
char * MiscContainsPattern(char *pString,int nStringLen, char *pPattern,int nPatternlen);
char * MiscContainsPattern(char *pString,char *pPattern);
int MiscContainsPatternMulti(char *pString,int nStringLen, MiscMultiPattern *pMultiPattern, int nMultiPatterns);
int MiscCompareStringsMulti(char *pString,MiscMultiPattern *pMultiPattern, int nMultiPatterns);

void  *MiscMemAlloc(unsigned int nSize);
void   MiscMemFree(void *pBlock);

bool  MiscStrContains(char *pStr, char *pSubStr, bool fCaseSensitive);
char *MiscMpExtractData(char *pHexFullResp, unsigned int nChannel, char *pDestData, int nDestDataLen);
unsigned char *MiscAdvanceToNextCharacter(unsigned char *pSrc, unsigned char ch);

char * MiscDeHexizeString(const unsigned char *pString);


#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/
