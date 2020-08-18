/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Project:		Contains defines which are visible by every module in this
**              project
**
** Module:		PROJECT.H
**
*******************************************************************************/

#if !defined(_PROJECT_H)     /* prevent multiple inclusions */
#define _PROJECT_H

/******************************************************************************/
// Include Files
/******************************************************************************/
#include "basetype.h"

/******************************************************************************/
// Compile time defines for DEBUG builds only
/******************************************************************************/
#if defined(_DEBUG)

#define ENABLE_TRACEPRINT
#define ENABLE_WIZSTATE_DISPLAY

#endif
/******************************************************************************/
// Compile time defines for RELEASE builds only
/******************************************************************************/

#define BRACKETED_SUBSTITUTION

/******************************************************************************/
// Defines
/******************************************************************************/

/******************************************************************************/
// Macros
/******************************************************************************/

#if defined(_DEBUG)
#define DEVMESSAGE(_x_)		DevMessage(__FILE__,__LINE__,_x_)
#else
#define DEVMESSAGE(_x_)		/* */
#endif

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

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

/******************************************************************************/
// Union definitions
/******************************************************************************/

/******************************************************************************/
// Global function decleration
/******************************************************************************/
int TracePrint(char *pFmt,...);

#if defined(_DEBUG)
void DevMessage(char *pFile, int nLine, char *pMsg);
#endif


#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/
