/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:     Intelligent Serial Dongle - Host AT Processor
**
** Module:      HEXMYHOST.CPP
**
*******************************************************************************/

/******************************************************************************/
// CONDITIONAL COMPILE DEFINES
/******************************************************************************/

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "Target.h"

#include "HexMyHost.h"
#include "TRegression.h"
#include "Misc.h"
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
int TracePrint(char *pFmt,...);
bool ProcessApplicationMessages(void);

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

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
HexMyHost::HexMyHost(TRegression *pRegression, unsigned int nLogicalPort) :
    TBaseUartHost(pRegression,nLogicalPort)
{
    int i;

    mnBinaryBlockLen = MAX_BINARY_BLOCK_LEN;

    mnMaxRespCount  = MAXREADHEXBLOCKLEN;
    mnRespCount   = 0;

    mnHexMaxRespCount  = MAXREADHEXBLOCKLEN*2;
    mnHexRespCount   = 0;

    mbaHexResponse[0]=0;

    for(i=0;i<=255;i++)
    {
        sprintf(mbaHex[i],"%02X",i);
    }
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
__fastcall HexMyHost::~HexMyHost()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned short HexMyHost::DecodeStringToBinary(
    const unsigned char *pString, //null terminated string
    int nStringLen,
    unsigned char *pDstStr,
    unsigned short nMaxDstStrLen
    )
{
    unsigned short nDstLen=0;
    const unsigned char *pStrEnd = &pString[nStringLen];

    //advance to non-white space character
    pString = MiscAdvanceToNonWhitespace((uint8 *)pString);
    while(*pString && (nDstLen<nMaxDstStrLen) && (pString<pStrEnd))
    {
        if( isxdigit(pString[0]) )
        {
            //the first digit is a hex digit
            if( isxdigit(pString[1]) )
            {
                //the second digit is also a hex digit
                *pDstStr++ = MiscConvert2HexChr2Binary((uint8 *)pString);
                pString+=2;
                nDstLen++;
            }
            else
            {
                //the second digit is NOT a hex digit so treat the
                //first hex digit as a byte
                *pDstStr++ = *pString++;
                nDstLen++;
            }
        }
        else if( (pString[0]=='/')&& (pString[1]=='/'))
        {
            //treat the rest of the string as a comment
            break;
        }
        else
        {
            //the first digit is NOT a hex digit and is NOT a whitespace either
            if(*pString == '\"')
            {
                // so copy everthing as is until a closing "
                pString++;
                while(*pString && (*pString!='\"')&& (nDstLen<nMaxDstStrLen) && (pString<pStrEnd))
                {
                    *pDstStr++ = *pString++;
                    nDstLen++;
                }
                if(*pString == '\"')pString++;
            }
            else
            {
                //the first digit is not a " either so just copy
                *pDstStr++ = *pString++;
                nDstLen++;
            }
        }
        //advance to non-white space character
        pString = MiscAdvanceToNonWhitespace((uint8 *)pString);
    }

    return nDstLen;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned short  //returns bytes absorbed
HexMyHost::EncodeBinaryToHex(
    const unsigned char *pBinStr,
    unsigned short nBinStrLen,
    unsigned char *pDstStr,
    unsigned short nMaxDstStrLen
    )
{
    unsigned char *pDst = pDstStr;
    int nCount = nBinStrLen;

    while(nMaxDstStrLen && nCount)
    {
        memcpy(pDst,mbaHex[*pBinStr++],2);
        pDst      += 2;
        nMaxDstStrLen -= 2;
        nCount--;
    }
    *pDst=0;

    return (nBinStrLen-nCount);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned short HexMyHost::CleanupHexString(
    const unsigned char *pString, //null terminated string
    int nStringLen,
    unsigned char *pDstStr,
    unsigned short nMaxDstStrLen
    )
{
    unsigned short nDstLen=0;
    const unsigned char *pStrEnd;

    /*
    July 2017:
    Updated this routine so that it can accept pPattern==NULL
    */
    if( (pString==NULL) || (nStringLen==0) )
    {
        if(  pDstStr )
        {
            pDstStr[0]=0;
        }
        return 0;
    }

    pStrEnd = &pString[nStringLen];

    //advance to non-white space character
    pString = MiscAdvanceToNonWhitespace((uint8 *)pString);
    while(*pString && (nDstLen<nMaxDstStrLen) && (pString<pStrEnd))
    {
        if( isxdigit(pString[0]) )
        {
            //the first digit is a hex digit
            if( isxdigit(pString[1]) )
            {
                //the second digit is also a hex digit
                *pDstStr++ = toupper(*pString++);
                *pDstStr++ = toupper(*pString++);
                nDstLen+=2;
            }
            else
            {
                //the second digit is NOT a hex digit so treat the
                //first hex digit as a byte
                *pDstStr++ = *pString++;
                nDstLen++;
            }
        }
        else if( (pString[0]=='/')&& (pString[1]=='/'))
        {
            //treat the rest of the string as a comment
            break;
        }
        else
        {
            //the first digit is NOT a hex digit and is NOT a whitespace either
            if(*pString == '\"')
            {
                // so copy everthing as is until a closing "
                pString++;
                while(*pString && (*pString!='\"')&& (nDstLen<nMaxDstStrLen) && (pString<pStrEnd))
                {
                    *pDstStr++ = *pString++;
                    nDstLen++;
                }
                if(*pString == '\"')pString++;
            }
            else
            {
                //the first digit is not a " either so just copy
                *pDstStr++ = *pString++;
                nDstLen++;
            }
        }
        //advance to non-white space character
        pString = MiscAdvanceToNonWhitespace((uint8 *)pString);
    }
    /* Terminate the string */
    *pDstStr++ = 0;

    return nDstLen;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void  HexMyHost::OnOpen(void)
{
}

//=============================================================================
//=============================================================================
unsigned short HexMyHost::TransmitBlock(unsigned char *pSendBlock,unsigned short nBlockLen)
{
    return __TransmitBlock(pSendBlock,nBlockLen);
}

//=============================================================================
//=============================================================================
char * HexMyHost::GetFullResponse(void)
{
    return mbaHexResponse;
}

//=============================================================================
//=============================================================================
void HexMyHost::OnFlushRxBuffer(bool fPostFlush)
{
}

//=============================================================================
//=============================================================================
void HexMyHost::OnFlushTxBuffer(bool fPostFlush)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool HexMyHost::IsPacketPresent(TPacketType pktType)
{
    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char * HexMyHost::GetPacket(int *pPktLen,TPacketType pktType)
{
    *pPktLen=0;
    return NULL;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int HexMyHost::DropPacket(TPacketType pktType)
{
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void HexMyHost::FlushRxBuffer(void)
{
    mnHexRespCount = mnRespCount = 0;
    mbaHexResponse[0]= 0;
    TBaseUartHost::FlushRxBuffer();
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void HexMyHost::FlushTxBuffer(void)
{
    TBaseUartHost::FlushTxBuffer();
}

/******************************************************************************/
// END OF FILE
/******************************************************************************/


