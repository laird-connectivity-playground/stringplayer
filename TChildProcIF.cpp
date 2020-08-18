/******************************************************************************
**              Copyright (C) 2018 Laird Connectivity
**
** Module:      TChildProcIF.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include "TChildProcIF.h"
/******************************************************************************/
// Local Defines
/******************************************************************************/

#define notUSE_PEEK_NAMED_PIPE

#define  CACHE_READ_BLOCK_LEN                 128

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
const int nMaxOpBuff = 1024;


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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CChildProc::ClosePipes( void )
{
    if ( m_bPipesCreated == FALSE ) return;

    CloseHandle(m_hChildStd_IN_Rd);
    CloseHandle(m_hChildStd_IN_Wr);
    CloseHandle(m_hChildStd_OUT_Rd);
    CloseHandle(m_hChildStd_OUT_Wr);
    
    m_bPipesCreated = FALSE;
}

//------------------------------------------------------------------------------
// returns 0 for success, other values are just to check which path failed to
// help with diagnostics and for documentation refer to this source of the function
//------------------------------------------------------------------------------
int CChildProc::CreateAndLaunchAppProcess( const char *pFullPathAppNameAndOptions, const char *pCurrentDir, bool bWaitforChildToIdle )
{
    AnsiString strCurrentDir = pCurrentDir;
    AnsiString strFullFilePathOpt = pFullPathAppNameAndOptions;
    AnsiString strChildProcTitle;

    // Set the bInheritHandle flag so pipe handles are inherited.
    ZeroMemory( &m_asynch_sa, sizeof(SECURITY_ATTRIBUTES));
    m_asynch_sa.bInheritHandle = TRUE;
    m_asynch_sa.lpSecurityDescriptor = NULL;
    m_asynch_sa.nLength = sizeof(SECURITY_ATTRIBUTES);

    // Create a pipe for the child process's STDOUT (will also be used for STDERR)
    if( ! CreatePipe( &m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &m_asynch_sa, 0 ) )
    {
        return 2;
    }
    //Ensure the read handle to the pipe for STDOUT is not inherited.
    if( ! SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
    {
        return 3;
    }

    // Create a pipe for the child process's STDIN
    if( ! CreatePipe( &m_hChildStd_IN_Rd, &m_hChildStd_IN_Wr, &m_asynch_sa, 0 ) )
    {
        return 4;
    }
    //Ensure the read handle to the pipe for STDIN is not inherited.
    if( ! SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
    {
        return 5;
    }

    //pipes have been created
    m_bPipesCreated = TRUE;

    //Now ready to create the child process
    ZeroMemory( &m_siStartInfo, sizeof( STARTUPINFO ));
    m_siStartInfo.cb = sizeof( STARTUPINFO );
    m_siStartInfo.hStdError = m_hChildStd_OUT_Wr;
    m_siStartInfo.hStdOutput = m_hChildStd_OUT_Wr;
    m_siStartInfo.hStdInput = m_hChildStd_IN_Rd;
    m_siStartInfo.dwFlags = STARTF_USESTDHANDLES + STARTF_USESHOWWINDOW;
    m_siStartInfo.wShowWindow = SW_SHOWMINIMIZED;

    strChildProcTitle = "StringPlayer Port-Pipe #";
    strChildProcTitle += mnLogicalPort;
    strChildProcTitle += ": ";
    strChildProcTitle += strFullFilePathOpt;
    m_siStartInfo.lpTitle =  strChildProcTitle.c_str();

    if( CreateProcess(NULL,
        strFullFilePathOpt.c_str(),    // Command line.
        NULL,                       // process security attributes
        NULL,                       // primary thread security attributes
        true,                       // handles are inherited
        NORMAL_PRIORITY_CLASS,      // creation flags
        NULL,                       // use parent's environment
        strCurrentDir.c_str(),       // parent's current directory
        &m_siStartInfo,             // STARTUPINFO pointer
        &m_piProcInfo ) )           // receives PROCESS_INFORMATION
    {
        if(bWaitforChildToIdle)
        {
            ::WaitForInputIdle( m_piProcInfo.hProcess, 60000 ); //wait 60 seconds?
        }

        //Start the thread to monitor data from child process
        m_bThreadRdTerminate  = FALSE;
        m_bThreadRdActive     = TRUE;
        unsigned threadId;
        BeginThread(NULL,0x8000,PipeMonitorThreadRd, this, 0, threadId);
        m_nThreadRdId =  threadId;
    }
    else
    {
        ClosePipes();
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CChildProc::KillPipeThreads( void )
{
    DWORD dwBytesWrote;
    char ch='*';

    //---------------------------------------------------
    // Safely kill the read thread
    //---------------------------------------------------
    m_bThreadRdTerminate = TRUE;
    /*
    We use ReadFile in the thread which blocks on reading the pipe.
    so we write into that pipe to unblock
    */
    if(m_bThreadRdActive)
    {
        //write any single byte to unblock the thread
        WriteFile(m_hChildStd_OUT_Wr, &ch, 1,&dwBytesWrote, NULL);
    }
    //wait for the thread to die
    while(  m_bThreadRdActive == TRUE )
    {
        Sleep( 100 );
    }

    
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CChildProc::PostMessage(PIPE_EVENT_TYPE evtype)
{
    ::PostMessage(Application->Handle,
                  mnWinMsgId,
                  (WPARAM)evtype,
                  (LPARAM)mnParentHandle);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int _fastcall CChildProc::PipeMonitorThreadRd( LPVOID pParam )
{
    CChildProc* pChildProc = (CChildProc*)pParam;
    char baTempBuf[CACHE_READ_BLOCK_LEN];

    Sleep( 250 );

    DWORD dwRet;
    while( ! pChildProc->m_bThreadRdTerminate )
    {
        if( pChildProc->mpChildOutCache->GetSpace() > 0 )
        {
            DWORD nReadLen=0;
            ReadFile( pChildProc->m_hChildStd_OUT_Rd, baTempBuf, 1, &nReadLen, NULL);
            if( nReadLen > 0)
            {
                pChildProc->mpChildOutCache->Write(baTempBuf,nReadLen);
            }
            /* inform the main thread that data has arrived */
            pChildProc->PostMessage(PIPE_EVENT_TYPE_ONDATA);
        }
    }
    pChildProc->ClosePipes();
    pChildProc->m_bThreadRdActive = FALSE;
    return 0;
}

//---------------------------------------------------------------------------
// Used to ensure that pipe caches are synced from the main thread
//---------------------------------------------------------------------------
void CChildProc::OnWindowsMessage( WPARAM wparam )
{
    if( mpCallBack )
    {
        switch(wparam)
        {
        case PIPE_EVENT_TYPE_ONDATA:
            mpCallBack->OnRxData();  //will end up calling PipeConsoleHost::OnData
            break;
        }
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
CChildProc::CChildProc(CChildProcCallback *pCallBack, int nChildOutCacheSize, int nParentHandle, int nWinMsgId, int nLogicalPort)
{
    m_bPipesCreated         = FALSE;
    m_bThreadRdActive       = FALSE;
    m_bThreadRdTerminate    = FALSE;

    if(nChildOutCacheSize < MIN_COMMAND_RESPONSE_CACHE_SIZE)
    {
        nChildOutCacheSize = MIN_COMMAND_RESPONSE_CACHE_SIZE;
    }
    mpChildOutCache = new CPipeCache(nChildOutCacheSize);
    mnWinMsgId              = nWinMsgId;
    mnParentHandle          = nParentHandle;
    mpCallBack              = pCallBack;
    mnLogicalPort           = nLogicalPort;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
CChildProc::~CChildProc()
{
    //Also terminate the process
    TerminateProcess(m_piProcInfo.hProcess, 0);
    KillPipeThreads();
    ClosePipes();

    if(mpChildOutCache) delete mpChildOutCache;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CChildProc::Launch( const char *pFullPathAppNameAndOptions, const char *pCurrentDir)
{
    int nLaunchRet;

    if ( m_bPipesCreated        == TRUE ) return FALSE;
    if ( m_bThreadRdActive      == TRUE ) return FALSE;
    if ( mpChildOutCache        == NULL ) return FALSE;

    nLaunchRet =  CreateAndLaunchAppProcess( pFullPathAppNameAndOptions, pCurrentDir, true );
    return (nLaunchRet == 0) ? true : false;
}

//------------------------------------------------------------------------------
// Used to send data to the child process
//------------------------------------------------------------------------------
BOOL CChildProc::Send( char *pSrc, DWORD nSrcLen )
{
    DWORD dwBytesWrote;

    if(m_bThreadRdActive == FALSE )
    {
        return FALSE;
    }

    if( m_bPipesCreated == FALSE )
    {
        return FALSE;
    }

    return WriteFile(m_hChildStd_IN_Wr,
                     pSrc,
                     nSrcLen,
                     &dwBytesWrote,
                     NULL);
}

//---------------------------------------------------------------------------
// called to read data that has arrived from the child process
//---------------------------------------------------------------------------
int CChildProc::Read( char * data, int nMaxBytes )
{
    return mpChildOutCache->Read(data,nMaxBytes);
}

//---------------------------------------------------------------------------
// called to read data that has arrived from the child process
//---------------------------------------------------------------------------
void CChildProc::Flush( void )
{
    return mpChildOutCache->Flush();
}


/******************************************************************************/
// END OF FILE
/******************************************************************************/


