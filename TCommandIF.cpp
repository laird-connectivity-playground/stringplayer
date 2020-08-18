/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      TCommandIF.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include "TCommandIF.h"
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
void CCommandIF::ClosePipes( void )
{
    if ( m_bPipesCreated == FALSE ) return;

    CloseHandle(m_asynch_pi.hProcess);
    CloseHandle(m_asynch_pi.hThread);

    CloseHandle(m_asynch_hReadPipeOut);
    CloseHandle(m_asynch_hWritePipeOut);
    CloseHandle(m_asynch_hReadPipeIn);
    CloseHandle(m_asynch_hWritePipeIn);
    CloseHandle(m_asynch_hReadPipeErr);
    CloseHandle(m_asynch_hWritePipeErr);
    m_bPipesCreated = FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CCommandIF::InitConsoleApp( const char * appname, const char * apppath, const char * options )
{
    AnsiString strTargetDir = apppath;
    AnsiString strFullFilePath = apppath;
    //strFullFilePath += "\\";
    strFullFilePath += appname;

    //does file exist?
    if( !FileExists(strFullFilePath) )
    {
        return FALSE;
    }

    if( strlen(options) > 0 )
    {
        strFullFilePath += " ";
        strFullFilePath += options;
    }

    // Child input(stdin) & output(stdout) pipes
    // Set the bInheritHandle flag so pipe handles are inherited.
    ZeroMemory( &m_asynch_sa, sizeof(SECURITY_ATTRIBUTES));
    m_asynch_sa.bInheritHandle = TRUE;
    m_asynch_sa.lpSecurityDescriptor = NULL;
    m_asynch_sa.nLength = sizeof(SECURITY_ATTRIBUTES);


    BOOL    bPipe1, bPipe2, bPipe3;
    bPipe1 = CreatePipe( &m_asynch_hReadPipeOut, &m_asynch_hWritePipeOut, &m_asynch_sa, 0 );
    bPipe2 = CreatePipe( &m_asynch_hReadPipeErr, &m_asynch_hWritePipeErr, &m_asynch_sa, 0 );
    bPipe3 = CreatePipe( &m_asynch_hReadPipeIn, &m_asynch_hWritePipeIn, &m_asynch_sa, 0 );

    if (( bPipe1 == TRUE ) &&( bPipe2 == TRUE ) &&( bPipe3 == TRUE ))
    {
        m_bPipesCreated = TRUE;
    }
    else
    {
        return FALSE;
    }

    ZeroMemory( &m_asynch_si, sizeof( STARTUPINFO ));
    m_asynch_si.cb = sizeof( STARTUPINFO );
    m_asynch_si.hStdOutput = m_asynch_hWritePipeOut;
    m_asynch_si.hStdError = m_asynch_hWritePipeErr;
    m_asynch_si.hStdInput = m_asynch_hReadPipeIn;
    m_asynch_si.dwFlags = STARTF_USESTDHANDLES + STARTF_USESHOWWINDOW;
    //m_asynch_si.wShowWindow = SW_HIDE;
    m_asynch_si.wShowWindow = SW_SHOWDEFAULT;

    if( CreateProcess(
        NULL,
        strFullFilePath.c_str(),     // Command line.
        &m_asynch_sa,
        &m_asynch_sa,
        true,
        NORMAL_PRIORITY_CLASS,
        NULL,
        strTargetDir.c_str(),
        &m_asynch_si,
        &m_asynch_pi ) )
    {
        ::WaitForInputIdle( m_asynch_pi.hProcess, 60000 ); //wait 60 seconds?

        m_dwLenDataFromOutputPipe = m_dwLenDataFromErrorPipe = 0;
        m_bThreadActive = TRUE;
        unsigned threadId;
        BeginThread(NULL,0x8000,PipeMonitorThread, this, 0,threadId);
    }
    else
    {
        ClosePipes();
        return FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CCommandIF::ProcessResponse( AnsiString resp )
{
    Write( resp.c_str(), resp.Length() );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CCommandIF::ClosePipeAsynch( void )
{
    m_bEndThread = TRUE;

    while(  m_bThreadActive == TRUE )
    {
        Sleep( 100 );
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int _fastcall CCommandIF::PipeMonitorThread( LPVOID pParam )
{
    CCommandIF* pObjmrb = (CCommandIF*)pParam;

    Sleep( 1000 );

    DWORD dwRet;
    while( 1 )
    {
        if ( pObjmrb->m_bEndThread == TRUE )
            break;

        dwRet = WaitForSingleObject( pObjmrb->m_asynch_pi.hProcess, 10 );
        if ( dwRet == WAIT_TIMEOUT )
        {

            DWORD dwBytesReadOut, dwBytesInOutPipe, dwBytesLeftInOutPipe;
            DWORD dwBytesReadErr, dwBytesInErrPipe, dwBytesLeftInErrPipe;

            if ( !PeekNamedPipe(pObjmrb->m_asynch_hReadPipeOut,NULL,NULL,NULL, &dwBytesInOutPipe, &dwBytesLeftInOutPipe))
                return FALSE;
            if ( !PeekNamedPipe(pObjmrb->m_asynch_hReadPipeErr,NULL,NULL,NULL, &dwBytesInErrPipe, &dwBytesLeftInErrPipe))
                return FALSE;

            // Read from pipes that are the standard output and error for child process.
            if ( dwBytesInOutPipe )
            {
                DWORD nToRead = nMaxOpBuff - 1;
                if(dwBytesInOutPipe <= nToRead)nToRead = dwBytesInOutPipe;

                ReadFile(
                    pObjmrb->m_asynch_hReadPipeOut,
                    pObjmrb->m_pDataFromOutputPipe, // + pObjmrb->m_dwLenDataFromOutputPipe,
                    //min( dwBytesInOutPipe, nMaxOpBuff - 1 ),
                    nToRead,
                    &dwBytesReadOut, 0);

                pObjmrb->m_pDataFromOutputPipe[ dwBytesReadOut ] = 0;

                AnsiString str="\n";
                str += pObjmrb->m_pDataFromOutputPipe;

                pObjmrb->ProcessResponse( str );

                ZeroMemory( pObjmrb->m_pDataFromOutputPipe, dwBytesInOutPipe );
            }
            if ( dwBytesInErrPipe )
            {

                ReadFile(
                    pObjmrb->m_asynch_hReadPipeOut,
                    pObjmrb->m_pDataFromErrorPipe + pObjmrb->m_dwLenDataFromErrorPipe,
                    dwBytesInErrPipe,
                    &dwBytesReadErr, 0);
                pObjmrb->m_dwLenDataFromOutputPipe += dwBytesInErrPipe;

            }
        }
        else if ( dwRet == WAIT_OBJECT_0 )
        {
            //This indiactes successful termination of the process.
            //An odd way to demonstrate success?!

            break;
        }
        else if (dwRet == WAIT_ABANDONED )
        {
            return 0;
        }
    }
    pObjmrb->ClosePipes();
    pObjmrb->m_bThreadActive = FALSE;
    return 0;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CCommandIF::OnDataReceived( void )
{
    ::PostMessage(Application->Handle,mnWinMsgId,(WPARAM)PIPE_EVENT_TYPE_ONDATA,(LPARAM)mnParentHandle);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CCommandIF::OnReady( void )
{
    ::PostMessage(Application->Handle,mnWinMsgId,(WPARAM)PIPE_EVENT_TYPE_ONREADY,(LPARAM)mnParentHandle);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CCommandIF::OnInitialised( void )
{
    ::PostMessage(Application->Handle,mnWinMsgId,(WPARAM)PIPE_EVENT_TYPE_INITIALISE,(LPARAM)mnParentHandle);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CCommandIF::OnWindowsMessage( WPARAM wparam )
{
    if( mpCallBack )
    {
        switch(wparam)
        {
        case PIPE_EVENT_TYPE_INITIALISE:
            mpCallBack->OnInitialise();
            break;
        case PIPE_EVENT_TYPE_ONDATA:
            mpCallBack->OnData();
            break;
        case PIPE_EVENT_TYPE_ONREADY:
            mpCallBack->OnReady();
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
CCommandIF::CCommandIF(CCommandIFCallback *pCallBack, int nCacheSize, int nParentHandle, int nWinMsgId)
{
    m_bPipesCreated         = FALSE;
    m_bThreadActive         = FALSE;
    m_bEndThread            = FALSE;

    m_pDataFromOutputPipe = (char * ) malloc( nMaxOpBuff );
    m_pDataFromErrorPipe = (char * ) malloc( 1024 );
    ZeroMemory( m_pDataFromOutputPipe, nMaxOpBuff );
    ZeroMemory( m_pDataFromErrorPipe, 1024 );

    mnHead                  = 0;
    mnTail                  = 0;
    if(nCacheSize < MIN_COMMAND_RESPONSE_CACHE_SIZE)nCacheSize = MIN_COMMAND_RESPONSE_CACHE_SIZE;
    mnCacheSize             = nCacheSize;
    mpRespCache             = (char *)malloc(mnCacheSize);
    mnWinMsgId              = nWinMsgId;
    mnParentHandle          = nParentHandle;
    mpCallBack              = pCallBack;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
CCommandIF::~CCommandIF()
{
    //Also terminate the process
    TerminateProcess(m_asynch_pi.hProcess, 0);

    if ( TRUE == m_bThreadActive )
    {
        ClosePipeAsynch();
    }

    ClosePipes();

    free( m_pDataFromOutputPipe );
    free( m_pDataFromErrorPipe );
    free( mpRespCache );

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CCommandIF::Initialise( const char *filename, const char *path, const char * options)
{
    if ( m_bPipesCreated        == TRUE ) return FALSE;
    if ( m_bThreadActive        == TRUE ) return FALSE;
    if ( m_pDataFromOutputPipe  == NULL ) return FALSE;
    if ( m_pDataFromErrorPipe   == NULL ) return FALSE;
    if ( mpRespCache            == NULL ) return FALSE;

    return InitConsoleApp( filename, path, options );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CCommandIF::Send( char *pSrc, DWORD nSrcLen )
{
    if (( m_bThreadActive == FALSE ) || ( m_bPipesCreated == FALSE ))
        return FALSE;

    DWORD dwBytesWrote;
    BOOL    bWriteSucceeded;

    bWriteSucceeded = WriteFile(
            m_asynch_hWritePipeIn,
            pSrc,
            nSrcLen,
            &dwBytesWrote, NULL);

    return bWriteSucceeded;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CCommandIF::Send( AnsiString strCmd )
{
    strCmd += "\n";
    return Send( strCmd.c_str(), strCmd.Length() );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CCommandIF::Write( char * data , int nBytes)
{
    int nRetVal;
    int nSpace;
    int nTail = mnTail;
    int nFree = mnCacheSize - (((mnHead - nTail)+mnCacheSize)%mnCacheSize);
    if( nBytes > nFree)nBytes = nFree;
    nRetVal = nBytes;

    while(nFree && nBytes)
    {
        if(mnHead >= nTail)
        {
            nSpace = mnCacheSize - mnHead;
        }
        else
        {
            nSpace = nTail - mnHead - 1;
        }

        if( nSpace > nBytes)nSpace=nBytes;
        memcpy( &mpRespCache[mnHead],data,nSpace );
        mnHead += nSpace;
        mnHead %= mnCacheSize;
        data   += nSpace;
        nBytes -= nSpace;

        nFree = mnCacheSize - (((mnHead - nTail)+mnCacheSize)%mnCacheSize);
    }

    OnDataReceived();
    return nRetVal;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CCommandIF::Read( char * data, int nMaxBytes )
{
    int nRetVal = nMaxBytes;
    int nHead = mnHead;
    int nUsed = ((nHead - mnTail)+mnCacheSize)%mnCacheSize;
    if( nUsed < nRetVal ) nRetVal = nUsed;
    if( nRetVal )
    {
        if( nHead >= mnTail )
        {
            memcpy(data,&mpRespCache[mnTail],nRetVal);
        }
        else
        {
            int nTop = mnCacheSize - mnTail;
            memcpy(data,&mpRespCache[mnTail],nTop);
            memcpy(data,&mpRespCache[0],(nRetVal-nTop));
        }
        mnTail += nRetVal;
        mnTail %= mnCacheSize;
    }
    return nRetVal;
}


/******************************************************************************/
// END OF FILE
/******************************************************************************/


