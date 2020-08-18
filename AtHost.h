/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      ATHOST.H
**
*******************************************************************************/

#if !defined(_ATHOST_H)     /* prevent multiple inclusions */
#define _ATHOST_H

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "project.h"

/******************************************************************************/
// Defines
/******************************************************************************/

#define ATHANDLE_NULL                       NULL

#define ATHOST_MASK_ROUTINE_PROCESS_RXDATA      0x0001

/******************************************************************************/
// Macros
/******************************************************************************/

/******************************************************************************/
// Enum Typedefs
/******************************************************************************/

typedef enum
{
    ATRESPONSE_NONE                         = 0x0000
   ,ATRESPONSE_OK                           = 0x0001
   ,ATRESPONSE_ERROR                        = 0x0002
   ,ATRESPONSE_CONNECT                      = 0x0004
   ,ATRESPONSE_NO_CARRIER                   = 0x0008
   ,ATRESPONSE_PATTERN                      = 0x0010
   ,ATRESPONSE_PARTIAL_PATTERN              = 0x0020
   ,ATRESPONSE_TIMEOUT                      = 0x2000
   ,ATRESPONSE_UNKNOWN                      = 0x4000
   ,ATRESPONSE_NOTOPEN                      = 0x8000

   ,ATRESPONSE_OK_ERROR                     = (ATRESPONSE_OK | ATRESPONSE_ERROR)
   ,ATRESPONSE_ALL                          = 0x7FFF
}
ATRESPONSE;

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

typedef unsigned short (*ATHOSTCALLBACK_TRANSMIT)(void *, unsigned char *,unsigned short);
typedef void           (*ATHOSTCALLBACK_ONCRLF)(void *, char *);
typedef void           (*ATHOSTCALLBACK_ONASYNCRESP)(void *, ATRESPONSE, char *);
typedef bool           (*ATHOSTCALLBACK_ONBLOCKWAIT)(void *);
typedef void           (*ATHOSTCALLBACK_ONRESP_OVERFLOW)(void *);
typedef unsigned int   (*ATHOSTCALLBACK_GET_TICK_COUNT)(void *);

typedef void 		    * ATHANDLE;

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
    void                           *pContext;
    unsigned short                  busyMask;  //see ATHOST_MASK_ROUTINE_??? defines
    unsigned short                  nMaxRespLen;
    ATHOSTCALLBACK_TRANSMIT         Transmit;
    ATHOSTCALLBACK_ONCRLF           OnCrLf;
    ATHOSTCALLBACK_ONASYNCRESP      OnAsyncResp;
    ATHOSTCALLBACK_ONBLOCKWAIT      OnWait;
    ATHOSTCALLBACK_ONRESP_OVERFLOW  OnOverFlow;
    ATHOSTCALLBACK_GET_TICK_COUNT   GetTickCount;
    unsigned char                  *pRespBuf;
    unsigned short                  nRespLen;
    unsigned short                  nLastCrLfEnd;
    unsigned short                  nRespMask;
    unsigned short                  nRxResponseType;
    unsigned char                  *pPattern;
    unsigned short                  nPatternLen;
    bool                            fFullString;
}
ATHOST;

/******************************************************************************/
// Union definitions
/******************************************************************************/

/******************************************************************************/
// Global Functions (API etc) exported for other modules
/******************************************************************************/
ATHANDLE 	AtHostCreateChannel( void *pContext,
                                 unsigned short nMaxRespLen,
                          		 ATHOSTCALLBACK_TRANSMIT OnTransmit,
                          	 	 ATHOSTCALLBACK_ONCRLF OnCrlf,
                          		 ATHOSTCALLBACK_ONASYNCRESP OnAsyncResp,
                          		 ATHOSTCALLBACK_ONBLOCKWAIT OnWait,
                          		 ATHOSTCALLBACK_ONRESP_OVERFLOW OnOverFlow,
                                 ATHOSTCALLBACK_GET_TICK_COUNT GetTickCount);

void            AtHostFlushRespBuffer(ATHANDLE handle);
void 			AtHostSendCommand(ATHANDLE handle,char *pCmd);
ATRESPONSE      AtHostSendCommandAndWait(ATHANDLE handle,char *pCmd,ATRESPONSE respMask, unsigned int nTimeoutMs);
void 			AtHostSendString(ATHANDLE handle,char *pCmd);
void 			AtHostSendBlock(ATHANDLE handle,unsigned char *pTxBlock, unsigned short nLen);
unsigned short 	AtHostProcessRxData(ATHANDLE handle,unsigned char *pRxBlock, unsigned short nLen);
unsigned short 	AtHostProcessHexRxData(ATHANDLE handle,unsigned char *pRxBlock, unsigned short nLen);
void            AtHostDiscardToFirstCrLf(ATHANDLE handle);
char           *AtHostGetFullResponse(ATHANDLE handle);
unsigned short  AtHostGetFullResponseLength(ATHANDLE handle);
char *          AtHostSetRxPattern(ATHANDLE handle,char *pPattern,unsigned short nPatternLen, bool fFullStr);
void            AtHostDiscardOldestBlock(ATHANDLE handle, int nToDelete);

#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/
