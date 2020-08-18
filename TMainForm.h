/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     StringPlayer
**
** Module:      TMainForm.h
**
*******************************************************************************/
#ifndef TMainFormH
#define TMainFormH

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdPort.hpp"
#include "OoMisc.hpp"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <Graphics.hpp>

#include "TRegression.h"
#include "Misc.h"
#include <Dialogs.hpp>

/******************************************************************************/
/* Version History                                                            */
/******************************************************************************/

#define APP_VERSION     "v4.2.2"
#define TITLE_BAR_TEXT  "String Player (" APP_VERSION ")"

/*
[4.2.2]
     *  Bug Fix: On FAIL a retry or a advance to next test relocates to the
        nearest TITLE statement (backwards or forwards). In that case the
        #include filename stack get out of sync if that TITLE statement was
        beyond an INCLUDE_PUSH or INLCUDE_POP

[4.2.1]
     *  PORT_MPOINT/PORT_HEXBIN, eliminate race condition when a response is received
        while screen is updated and the next WAITRESPEX has not been processed after
        some data has been sent like SENDCMD or TXSTRING

[4.2.0]
     *  When console update is disabled, it is temporarily enabled when there is a FAIL
     *  When tracing is disabled, it is temporarily enabled when there is a FAIL
     *  When a script ends, if console update is disabled it is re-enabled.

[4.1.0]
     *  Fix bug arising from changes in 4.0.0 which meant that #includefolder
        statement was impacted

[4.0.0]
     *  When there is a FAIL, now the statement line number where it failed
        is printed to help quickly locate where it failed

[3.0.0]
     *  Changes in 2.7.1 were constant delays, in the version when the serial
        port is opened the delays for post open and close can be specified
        in the script

[2.7.1]
     *  After comport open/close there is unconditional sleep of 2000ms to see
        if TI dongle issue on Win10 is fixed.

[2.7.0]
     *  For very large tests it becomes gradually slower beacuse the main
        reMemo main console becomes HUGE.
        This version limits number of lines that is specified by new command
        line MAXLINES=N so that when that limit is reached,
        HYSTERESIS_MAIN_MEMO_LINES (500) oldest lines are deleted
        The default value of MAXLINES is now 2500

[2.6.8]
     *  Added WAITUNTIL command which does not generate a PASS/FAIL outcome
        but instead will set the Z flag a string match is found or clear the
        Z flag if there is a timeout

[2.6.6]
     *  WAITTIMEOUT command now display "NOT exp:" instead of "exp:"

[2.6.4]
     *  Added WAITTIMEOUT command

[2.6.2]
     *  Fixed FILTERMOVE for moving from one variable to another
     *  Added STRDEHEXIZE command

[2.6.0]
     *  Added FILTERMOVE for moving from one variable to another

[2.5.8]
     *  EXECUTEWAITEX now takes an optional extra last parameter which is a
        varible name - if it exists then it will return the exitcode

[2.5.2]
     * If launched with no parameters and then it decides to quit without
       selecting it was crashing with because a null pointer was being dereferenced
     * Dialog box to select script file to run also now displays only files with
       extensions .scr .sub .sbr

[2.5.0]
     * ensure a character with value 0x80 to 0xFF is not detected as whitespace

[2.4.0]
     * added a new  optional parameter to WaitRespEx which is a variable name
       and if it exists, then in addition to dropping the data up to and including
       the pattern match, it copies that dropped data to the varname specified
       so that it can be processed further if required.

[2.3.0]
     * Added command line argument MAXPORTS=n where n=16 to 255 and will default
       to 16 so that current behaviour happens

[2.2.1]
     * Error value is not correctly displayed rather than a very large number

[2.2.0]
     * PORT_PIPE added which works with iPython
     * [%sometext] now replaced by a string double the length of 'sometext'
       and for each letter in 'sometext' the ascii value in hex is returned
       E.g. [%ABC] is replaced by "414243" which are 6 characters and
       [%AB CD] will be 4142204344 - that is spaces are allowed

[2.1.9]
     * Added EXECUTEWAITRET function which is the same as EXECUTEWAIT but also
       returns the exit code of the executable to the 2nd variable

[2.1.8]
     * Bug Fix : For TiEvent packets were dropped twice

[2.1.7]
     * Appending null to buffers after dropping TIEVENT

[2.1.6]
     * Added time stamp to comms log - the time stamp is the modulus of the
       GetTickCount() and 100000 so that we get relative information between
       each line and 100000 means up to 100 seconds
     * increased the number of lines dumped to file on FAIL

[2.1.5]
     * Pulled Jamie's changes

[2.1.4]
     * WAITTIEVENT now increments program counter by throwing the nextopcode event
     * Cleanup

[2.1.3]
     * Cleanup of pattern matching code for WAITTIEVENT statement

[2.1.2]
     * Disabled buffering for title log file
     * Added option to enable clearing of the main window log (use command
       argument SMALLLOG)
     * Added option to skip current test (use command argument SKIP)

[2.1.1]
     * PORTINFO now takes ID of 2 which returns the leghth of fullresponse in
       ATHOST mode

[2.1.0]
     * Fix bug related to WAITRESPEX which was resulting in buffer overflow
       because the dropping of characters in AtHostDiscardOldestBlock() was
       faulty because null character was not being appended.

[2.0.4]
     * Added TOPLEFT Command line

[2.0.3]
     * Initial screen position change to 'designed'

[2.0.2]
     * Initial screen position change

[2.0.1]
     * TRACEWIN now takes the 'clear' argument to erase the reMemo

[2.0.0]
     * Added the name:value command line so that variable called name is
       created and initialised with value

[1.12.1]
     * When Console update is disabled the OFF message is not printed

[1.12.0]
     * The FAIL text log file can now be opened while the test is running
       to see what failed

[1.11.11]
     * Improve behaviour of WAITTIEVENT

[1.11.10]
     * Increase baudrate limit from 921600 to (921600 *16)

[1.11.9]
     * Added INRANGE statement

[1.11.8]
     * Added CONSOLEUPDATE statement

[1.11.6]
     * Overhaul of the OcHost onRxData routine

[1.11.4]
     * Implemented MPointMyHost::DropFromResponse(int nToDeleteHex) so that
       WAITRESPEX for the PORT_MPOINT actually does as per the user manual

[1.11.2]
    * Bug fix in OcHost::ExtractDataChannels

[1.11.0]
    * Added the PORT_OC statement for customer X sb testing
    * Added WAITCHANNEL statement for customer X sb testing

[1.10.2]
    * Added statements
        EXECUTEWAITEX

[1.10.0]
    * Added statements
        TIMESCALAR <percent_value>
        DELAYABS <timeoutMs>
    * DELAYABS now behaves as per old DELAY statement and DELAY now uses the
      timescalar value specified by TIMESCALAR to adjust the actual timeout.
    * The statements SENDxxx, WAITRESP, WAITRESPEX, WAITMULTI, WAITDATA, WAITPACKET, WAITTIEVENT
      all incorporate the time period scaling specified by the TIMESCALAR command

[1.9.73]
    * Changed the reMemo maxlength to 16384 characters

[1.9.72]
    * Display FAIL in statusbar now shows the retry count and the max retries
      per test that is allowed

[1.9.70]
    * Added command line argument 'maxretry=decimal_number' and enhanced
      runtime engine so that it will retry for decimal_number of times

[1.9.60]
    * Allow abort when compiling the test script

[1.9.50]
    *  Bug Fix : 9 Aug 2014
       At fast baudrates if the pattern happens multiple times before
       the event in the windows message loop is processed, we end up with
       multiple triggers

[1.9.45]
    * #include <filename> now forces a search through the include paths

[1.9.42]
    * More sanity checking to prevent exception

[1.9.40]
    * Fix bug that was creating an exception. To do with BT900 and
      too much data coming in at 115200

[1.9.30]
    * More debugging info and logfile name creation bugfix

[1.9.29]
    * More debugging info and logfile name creation bugfix

[1.9.28]
    * More debugging info and logfile name creation bugfix

[1.9.26]
    * Print the logpath in display area on startup

[1.9.25]
    * The SCRIPT= argument now accepts a path and filename

[1.9.24]
    * Display the current working folder as first line

[1.9.23]
    * LOGPATH was not working

[1.9.22]
    * If smartBasic app fails, then if QUIT was specified then app exits and
      better granularity of exitcodes. -1000 is catch all.

[1.9.21]
    * If smartBasic app fails, then if QUIT was specified then app exits

[1.9.20]
    * Added LOGPATH=xxx command line argument

[1.9.11]
    * Exit the app with the number of failures (I hope)

[1.9.10]
    * Added EXECUTEEX statement

[1.9.9]
    * Display the expected string in WAITRESPEX

[1.9.8]
    * Added ABS command

[1.9.7]
    * WAITRESPEX modified so that next opcode is not thrown but invoked
      immediately

[1.9.6]
    * Now more concise file is appended to so that failure patterns can be discerned

[1.9.5]
    * Fix issue with WAITTIEVENT statement -- where too many TIEVENTS
      came in. Now next Line in processed immediately

[1.9.4]
    * Fix issue with WAITTIEVENT statement

[1.9.3]
    * Added the WAITTIEVENT statement

[1.9.2]
    * Added the EXECUTEWAIT statement

[1.9.1]
    * Enhanced WAITRESPEX so that as matches are found all data up to and
      including the pattern are discarded from the low level resp buffer

[1.9.0]
    * Added the MESSAGEBOX statement
    * Added the system variable SysVarMsgBoxBtnVal
    * Added the system variable SysVarMsgBoxBtnName

[1.8.3]
    * Enhanced #EXECUTE and #EXECUTEWAIT directive so that default working directory
      for the launched app can be specified
    * added #EXECUTEHIDE and #EXECUTEWAITHIDE directives so that the window for the
      child process is hidden
    * Compilation progress is now displayed so that all the #execute lines are displayed

[1.8.2]
    * Added #EXECUTE directive
    * Added #EXECUTEWAIT directive
        -- both above calls createprocess() to launch an app.
        -- ExecuteWait waits for the app to exit before proceeding.

[1.8.1]
    * Added WAITRESPEX which is exactly the same as WAITREST
      but allows me to right the BLE module scripts so that future
      I can optimise the usage.

[1.8.0]
    * Trying to fix the "No more trigger slots" dialog box

[1.7.0]
    * #INCLUDEFOLDERn tokens such that n now is dependent on just MAX_OTHER_INCLUDES
      and no hard coding.
    * MAX_OTHER_INCLUDES increased from 4 to 10, hence  INCLUDEFOLDER0 to INCLUDEFOLDER9
    * in #include "string" the string allows embedding of [xx] so that a variable
      filename can be created

[1.6.7]
    * PORTINFO updated so that it can also return CTS and DSR toggle counts

[1.6.6]
    * The patternlen for multipoint was limited to 1024 instead of 32768

[1.6.5]
    * Allow APPEND parameter for FILEOPEN

[1.6.4]
    * PORTINFO updated for multipoint class

[1.6.3]
    * BugFix : SENDMPDATA was displaying the wrong length

[1.6.2]
    * Enhancement  : Can specify handshaking

[1.6.1]
    * Enhancement  : Changed RXCOUNT statement to PORTINFO

[1.6.0]
    * Enhancement  : RXCOUNT statement added

[1.5.0]
    * Enhancement  : Wildcard matching in AT mode too

[1.4.9]
    * Enhancement  : A time count is displayed in title bar

[1.4.8]
    * Enhancement  : FAIL reports are written to a cumulative log file (htm)
                     so that we can see a trend.

[1.4.7]
    * Added  : PORT_CLOSE statement

[1.4.6]
    * Added  : When a test fails, it also prints out the fail count

[1.4.5]
    * Added  : SPRINTHEX statement (similar to SPRINTINT)

[1.4.4]
    * Added  : More diagnostincs info when a test fails

[1.4.3]
    * Added  : WAITMULTI and MULTIINDEX statements

[1.4.2]
    * Added  : MID statement to extract a substr from a string and put into another variable

[1.4.1]
    * Added statements HEX2DEC and DEC2HEX

[1.4.0]
    * Added command line argument ARG=something of which there can be
      up to 16. They are copied into system variable SysVarArgN where
      N is a number from 0 to 15.

[1.3.6]
    * Added TST and HEXTST statments which are as per SUB and HEXSUB but
      the result is not stored back in the varaible

[1.3.5]
    * Added SysVarMatchCount system variable
    * Modified the CONTAIN statement so that it updates  SysVarMatchCount
    * Added MATCHCOUNT statement

[1.3.4]
    * Added SysVarFailOnVarMissing system variable

[1.3.3]
    * AGGLOG window now works when TRACE is false in the script

[1.3.2]
    * Changed statusbar so filename not displayed there.
    * Script filename is displayed on main widnow title bar
    * The trace window now seperates tx/rx lines in different lines

[1.3.1]
    * Still something wrong as am getting ghost responses

[1.3.0]
    * FlushRX was not flushing the HEX buffer, hence next command would
      think it has received a response.

[1.2.10]
    * Added WAITDATA which is very similar to WAITRESP but extracts
      multipoint data before comparing
    * Added SENDMPDATA which knows about format of MP data packet

[1.2.9]
    * Display message if main script file is missing
    * add the CWD to specify current working folder

[1.2.8]
    * WaitREsp now uses comparestring instead of strstr

[1.2.7]
    * Fix bug in CleanupHexString() where the null terminator was not being added

[1.2.6]
    * Fix race in WAITRESP

[1.2.5]
    * Add SENDCMD command which is like TXSTRING but flushes the rx buffer
      before sending the data

[1.2.4]
    * Add HEX arithmetic capability

[1.2.3]
    * WAITPACKET sets z flag when packet arrives and does not fail the test
      if the timeout occurs

[1.2.2]
    * Allow wildcard '*' matching

[1.2.1]
    * More optimisations on the mpoint object

[1.2.0]
    * changes arising from testing the mpoint port object

[1.1.7]
    * Hex Port refactored to use the new BaseUartHost class

[1.1.6]
    * Split AtMyHost into BaseUart and specialty code so that we can
      reuse code for future uart protocols

[1.1.5]
    * Added  : PORTHEX statement and the files HexMyHost.cpp/h

[1.1.4]
    * Added  : SPRINTINT statement

[1.1.3]
    * Added  : Aggregate comms window if AGGLOG command line is supplied
    * ReAdded: Program Icon

[1.1.2]
    * BugFix : IFNZ was working as if it was IFZ
    * Added  : STRSTR and STRSTRI statements
*/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


#define DEASSERTED_COLOR    clRed
#define ASSERTED_COLOR      clGreen
#define NOTOPEN_COLOR       clWhite

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
class TfrmMain : public TForm
{
__published:    // IDE-managed Components
    TToolBar *ToolBar1;
    TStatusBar *statusBar;
    TPanel *panel;
    TRichEdit *reMemo;
    TTimer *tmrOnStart;
    TButton *btnQuit;
    TOpenDialog *OpenDialog1;
    TButton *btnPause;
    TPanel *Panel2;
    TShape *shpCTS1;
    TLabel *Label1;
    TLabel *Label2;
    TShape *shpDSR1;
    TLabel *Label3;
    TShape *shpDCD1;
    TLabel *Label4;
    TShape *shpRI1;
    TLabel *Label5;
    TPanel *Panel1;
    TShape *shpCTS0;
    TLabel *Label6;
    TLabel *Label7;
    TShape *shpDSR0;
    TLabel *Label8;
    TShape *shpDCD0;
    TLabel *Label9;
    TShape *shpRI0;
    TLabel *Label10;
    TPanel *Panel3;
    TShape *shpCTS7;
    TLabel *Label11;
    TLabel *Label12;
    TShape *shpDSR7;
    TLabel *Label13;
    TShape *shpDCD7;
    TLabel *Label14;
    TShape *shpRI7;
    TLabel *Label15;
    TPanel *Panel4;
    TShape *shpCTS6;
    TLabel *Label16;
    TLabel *Label17;
    TShape *shpDSR6;
    TLabel *Label18;
    TShape *shpDCD6;
    TLabel *Label19;
    TShape *shpRI6;
    TLabel *Label20;
    TPanel *Panel5;
    TShape *shpCTS5;
    TLabel *Label21;
    TLabel *Label22;
    TShape *shpDSR5;
    TLabel *Label23;
    TShape *shpDCD5;
    TLabel *Label24;
    TShape *shpRI5;
    TLabel *Label25;
    TPanel *Panel6;
    TShape *shpCTS4;
    TLabel *Label26;
    TLabel *Label27;
    TShape *shpDSR4;
    TLabel *Label28;
    TShape *shpDCD4;
    TLabel *Label29;
    TShape *shpRI4;
    TLabel *Label30;
    TPanel *Panel7;
    TShape *shpCTS3;
    TLabel *Label31;
    TLabel *Label32;
    TShape *shpDSR3;
    TLabel *Label33;
    TShape *shpDCD3;
    TLabel *Label34;
    TShape *shpRI3;
    TLabel *Label35;
    TPanel *Panel8;
    TShape *shpCTS2;
    TLabel *Label36;
    TLabel *Label37;
    TShape *shpDSR2;
    TLabel *Label38;
    TShape *shpDCD2;
    TLabel *Label39;
    TShape *shpRI2;
    TLabel *Label40;
    TTimer *tmrUpTime;
        TButton *btnSkip;
    void __fastcall tmrOnStartTimer(TObject *Sender);
    void __fastcall btnQuitClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall btnPauseClick(TObject *Sender);
    void __fastcall tmrUpTimeTimer(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall btnSkipClick(TObject *Sender);

private:    // User declarations
    bool   fUpdateConsole;

public:     // User declarations
    __fastcall TfrmMain(TComponent* Owner);

    void ScrollToEnd(TRichEdit *pRichEdit);
    void LimitContent(TRichEdit *pRichEdit) ;
    void DisplayWriteLine(TColor color, AnsiString asLine);
    void DisplayUserMessage(char *pMsg);
    void DisplayUserMessage(char *pMsg,TColor color);
    void DisplayFailMessage(char *pMsg);
    void DisplayFailMessage(char *pMsg, bool fEscaped);
    void DisplayPrintMessage(char *pMsg, bool fEscaped);
    void DisplayTraceMessage(char *pMsg);
    void DisplayCommand(unsigned int nModule,char *pCmd);
    void DisplayCommand(unsigned int nModule,char *phdr, char *pData);
    void DisplayUnicodeCommand(unsigned int nModule,char *pCmd);
    void DisplayResponse(unsigned int nModule,char *pResp);
    void DisplayChannelResponse(unsigned int nModule,int channel, char *pResp);
    void OnAccept(void);
    void StatusText(AnsiString asMsg);

    void StatusBarProgramCounter(int nPc);
    void StatusBarScriptSize(int nSize);
    void StatusBarFailCount(int nFailCount, int nRetries, int maxRetries);
    void StatusBarTestNumId(AnsiString asTestId);
    void DisplayScriptFilename(AnsiString asFilename);
    void UpdateModemDisplay(int nId, bool fOpen, bool fCTS, bool fDSR, bool fRI, bool fDCD);
    void LogToFailFile(TColor color, AnsiString asLine);
    void LogToPlainFailFile(AnsiString asLine);
    void DumpToLogFile(FILE *fp, unsigned int nLines);

    void ConsoleUpdateEnable(void){fUpdateConsole=true;};
    void ConsoleUpdateDisable(void){fUpdateConsole=false;};

    //data
    TRegression    *mpRegression;
    bool            mfCloseApp;
    FILE           *mfpLog;
    FILE           *mfpFailLog;
    unsigned int   mUpTimeHours;
    unsigned int   mUpTimeMins;
    unsigned int   mUpTimeSecs;
    AnsiString     masWinTitle;
    bool           mfWinTitle;
    AnsiString     masFullLogFilePath;
    AnsiString     masFilePathFailTxtFile;

};

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
extern PACKAGE TfrmMain *frmMain;

bool ProcessApplicationMessages(void);

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#endif
