/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Module:      TCommandIF.H
**
*******************************************************************************/

#if !defined(_TCOMMANDIF_H)     /* prevent multiple inclusions */
#define _TCOMMANDIF_H

/******************************************************************************/
// Include Files
/******************************************************************************/
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <assert.h>

/******************************************************************************/
// Defines
/******************************************************************************/
#define MIN_COMMAND_RESPONSE_CACHE_SIZE    0x2000

/******************************************************************************/
// Macros
/******************************************************************************/

/******************************************************************************/
// Enum Typedefs
/******************************************************************************/

typedef enum
{
    PIPE_EVENT_TYPE_INITIALISE      = 0
   ,PIPE_EVENT_TYPE_ONDATA
   ,PIPE_EVENT_TYPE_ONREADY
}
PIPE_EVENT_TYPE;

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

/******************************************************************************/
// Forward declaration of Class, Struct & Unions
/******************************************************************************/

/******************************************************************************/
// Class definitions
/******************************************************************************/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CCommandIFCallback
{
private:

public:
    CCommandIFCallback() {};
    virtual __fastcall ~CCommandIFCallback() {};

    virtual void OnInitialise( void ) = 0;
    virtual void OnData( void ) = 0;
    virtual void OnReady( void ) = 0;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CCommandIF
{
public: //methods
    CCommandIF(CCommandIFCallback *pCallBack, int nCacheSize, int nParentHandle, int nWinMsgId);
    virtual ~CCommandIF();

    BOOL        Initialise( const char *filename, const char *path, const char * options );
    BOOL        Send( char *pSrc, DWORD nSrcLen);
    BOOL        Send( AnsiString strCmd);

    int         Read( char * data, int nMaxBytes );
    void        Flush( void ) {mnTail = mnHead;};
    void        OnWindowsMessage( WPARAM wparam );

private: //methods

    BOOL        InitConsoleApp( const char * appname, const char * apppath, const char * options );
    void        ClosePipes( void );
    void        ClosePipeAsynch( void );
    static int _fastcall PipeMonitorThread( LPVOID pParam );
    void        ProcessResponse( AnsiString resp );

    void        OnDataReceived( void );
    void        OnReady( void );
    void        OnInitialised( void );
    int         Write( char * data, int nBytes );

public :  //data


private:  //data
    volatile BOOL           m_bPipesCreated;
    SECURITY_ATTRIBUTES     m_asynch_sa;
    HANDLE                  m_asynch_hReadPipeOut,  m_asynch_hReadPipeIn,  m_asynch_hReadPipeErr;    // read handles
    HANDLE                  m_asynch_hWritePipeOut, m_asynch_hWritePipeIn, m_asynch_hWritePipeErr;   // write handles
    volatile BOOL           m_bThreadActive;
    volatile BOOL           m_bEndThread;
    PROCESS_INFORMATION     m_asynch_pi;
    STARTUPINFO             m_asynch_si;

    char                  * m_pDataFromOutputPipe;
    char                  * m_pDataFromErrorPipe;
    DWORD                   m_dwLenDataFromOutputPipe;
    DWORD                   m_dwLenDataFromErrorPipe;


    CCommandIFCallback    * mpCallBack;
    char                  * mpRespCache;
    int                     mnCacheSize;
    int                     mnHead;
    int                     mnTail;
    int                     mnWinMsgId;
    int                     mnParentHandle;

};

/******************************************************************************/
// Struct definitions
/******************************************************************************/

/******************************************************************************/
// Union definitions
/******************************************************************************/

/******************************************************************************/
// Global Functions (API etc) exported for other modules
/******************************************************************************/

#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/


