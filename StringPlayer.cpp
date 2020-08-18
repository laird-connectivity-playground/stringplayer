//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

int gnReturnCode=0;

USEFORM("TMainForm.cpp", frmMain);
USEFORM("TfrmMessage.cpp", frmMessage);
USEFORM("TfrmPort.cpp", frmPort);
USEFORM("TFrmTrace.cpp", frmTrace);
USEFORM("TFrmAggLogger.cpp", frmAggLogger);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->CreateForm(__classid(TfrmMain), &frmMain);
         Application->CreateForm(__classid(TfrmAggLogger), &frmAggLogger);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return gnReturnCode;
}
//---------------------------------------------------------------------------
