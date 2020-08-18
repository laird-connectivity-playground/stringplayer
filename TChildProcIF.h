/******************************************************************************
**              Copyright (C) 2018 Laird Connectivity
**
** Module:      TChildProcIF.H
**
*******************************************************************************/

#if !defined(_TCHILDPROCIF_H)     /* prevent multiple inclusions */
#define _TCHILDPROCIF_H

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

#include "TPipeCache.h"

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
    PIPE_EVENT_TYPE_ONDATA      = 0
}
PIPE_EVENT_TYPE;

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

/******************************************************************************/
// Forward declaration of Class, Struct & Unions
/******************************************************************************/

/******************************************************************************/
// Struct & Class definitions
/******************************************************************************/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CChildProcCallback
{
private:

public:
    CChildProcCallback() {};
    virtual __fastcall ~CChildProcCallback() {};

    virtual void OnRxData( void ) = 0;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CChildProc
{
public: //methods
    CChildProc(CChildProcCallback *pCallBack, int nChildOutCacheSize, int nParentHandle, int nWinMsgId, int nLogicalPort);
    virtual ~CChildProc();

    BOOL        Launch(const char *pFullPathAppNameAndOptions, const char *pCurrentDir);
    BOOL        Send( char *pSrc, DWORD nSrcLen);

    int         Read( char * data, int nMaxBytes );
    void        Flush( void );
    void        OnWindowsMessage( WPARAM wparam );

private: //methods

    int         CreateAndLaunchAppProcess( const char *pFullPathAppNameAndOptions, const char *pCurrentDir, bool bWaitforChildToIdle  );
    void        ClosePipes( void );
    void        KillPipeThreads( void );
    void        PostMessage(PIPE_EVENT_TYPE evtype);
    static int  _fastcall PipeMonitorThreadRd( LPVOID pParam );

public :  //data


private:  //data
    volatile BOOL           m_bPipesCreated;
    SECURITY_ATTRIBUTES     m_asynch_sa;
    HANDLE                  m_hChildStd_OUT_Rd;
    HANDLE                  m_hChildStd_OUT_Wr;
    HANDLE                  m_hChildStd_IN_Rd;
    HANDLE                  m_hChildStd_IN_Wr;
    PROCESS_INFORMATION     m_piProcInfo;
    STARTUPINFO             m_siStartInfo;

    volatile unsigned       m_nThreadRdId;
    volatile BOOL           m_bThreadRdActive;
    volatile BOOL           m_bThreadRdTerminate;

    CChildProcCallback    * mpCallBack;
    CPipeCache            * mpChildOutCache;
    int                     mnWinMsgId;
    int                     mnParentHandle;
    int                     mnLogicalPort;

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


