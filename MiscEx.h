/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Module:      MISCEX.H
**
*******************************************************************************/

#if !defined(_MISCEX_H)     /* prevent multiple inclusions */
#define _MISCEX_H

/******************************************************************************/
// Include Files
/******************************************************************************/

/******************************************************************************/
// Defines
/******************************************************************************/

/******************************************************************************/
// Macros
/******************************************************************************/

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/

/******************************************************************************/
// Enum Typedefs
/******************************************************************************/

typedef enum
{
    OUTCOME_NONE                = 0
   ,OUTCOME_OK
   ,OUTCOME_ERROR
   ,OUTCOME_MOD_TIMEOUT
   ,OUTCOME_SCR_TIMEOUT
   ,OUTCOME_PATTERN
   ,OUTCOME_PACKET
   ,OUTCOME_HCI_LE_EVENT
   ,OUTCOME_DONE               = 0xFFFE

   ,OUTCOME_UNKNOWN            = 0xFFFF
}
EExpectedOutcome;

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
// Global Functions (API etc) exported for other modules
/******************************************************************************/
char *MiscOutcomeString(EExpectedOutcome eOutcome);

#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/
