/******************************************************************************
**              Copyright (C) 2019 Laird Connectivity
**
** Module:		BASETYPE.H
**
** Description: This header file is common to all C/C++ projects developed
**              by the software team at TDK Systems Europe Ltd. It is included
**              in the project specific header file called PROJECT.H
**
** References:  See document titled:
**              Software Development: Methodology Standard
**
** Creator:     Mahendra Tailor
**
** Date:        October 1999
**
** Notes:       This header file forces a common look and feel fo all software
**              projects undertaken by the software team at TDK
**
**              If a third party header file (e.g WINDOWS.H) is not defining
**              BYTE, WORD or DWORD, then the following defines MUST be set via
**              the command line respectively: DEF_BYTE, DEF_WORD, DEF_DWORD
*******************************************************************************/

#if !defined(BASETYPE_H)     /* prevent multiple inclusions */
#define BASETYPE_H


/******************************************************************************/
// Validation checks
/******************************************************************************/


/******************************************************************************/
// Include Files
/******************************************************************************/

// This file MUST NEVER include a file, as this is invoked at the top of the
// include file hierarchy.

/******************************************************************************/
// Defines
/******************************************************************************/

#define CONST               const
#define STATIC              static

/******************************************************************************/
// Macros
/******************************************************************************/
#define sizeofarray(x)      ( sizeof(x) / sizeof(x[0]) )

/******************************************************************************/
// Simple (non struct/union) Typedefs
/******************************************************************************/


//====================================================
// The following allow for compatibility with third
// party libraries
//====================================================
typedef int                         INT;
typedef char                        CHAR;
typedef float                       FLOAT;
typedef double                      DOUBLE;
#if defined(__cplusplus)
//    typedef bool                    BOOL;
#else
    #define BOOL                    unsigned char
    #define bool                    unsigned char
#endif

#if !defined(VOID)
    #define VOID                    void
#endif

//====================================================
// Flag variables
//====================================================
#if defined(__cplusplus) 
//    typedef bool                    BOOLEAN;
#else
    #define BOOLEAN                 unsigned char
#endif


//====================================================
// 8 bit variables
//====================================================
typedef signed char                 I8;
typedef signed char                 S8;
typedef unsigned char               UI8;
typedef unsigned char               U8;
typedef unsigned char               UCHAR;
typedef volatile char               VOLATILE_I8;
typedef volatile unsigned char      VOLATILE_UI8;

#if defined(DEF_BYTE)
    typedef unsigned char           BYTE;
#endif
//====================================================
// 16 bit variables
//====================================================
typedef signed short                I16;
typedef signed short                S16;
typedef unsigned short              UI16;
typedef unsigned short              U16;
typedef volatile short              VOLATILE_I16;
typedef volatile unsigned short     VOLATILE_UI16;

#if defined(DEF_WORD)
    typedef unsigned short          WORD;
#endif
//====================================================
// 32 bit variables
//====================================================
typedef signed long                 I32;
typedef signed long                 S32;
typedef unsigned long               UI32;
typedef unsigned long               U32;
typedef volatile long               VOLATILE_I32;
typedef volatile unsigned long      VOLATILE_UI32;
typedef float                       F32;
typedef volatile float              VOLATILE_F32;

#if defined(DEF_DWORD)
    typedef unsigned long           DWORD;
#endif
//====================================================
// 64 bit variables
//====================================================
typedef double                      F64;
typedef volatile double             VOLATILE_F64;
// Comment the following four lines if the compiler
// does not provide 64bit integers
typedef __int64                     I64;
typedef unsigned __int64            UI64;
typedef volatile __int64            VOLATILE_I64;
typedef volatile unsigned __int64   VOLATILE_UI64;

//====================================================
// Pointer variables
//====================================================
typedef void*                       PVOID;
typedef void**                      PPVOID;

//The following are to be used to access memory mapped io ports
typedef volatile unsigned char      PORT8;
typedef volatile unsigned short     PORT16;
typedef volatile unsigned long      PORT32;



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
// Global Functions (API etc) exported for other modules
/******************************************************************************/



#endif /* Prevention of multiple inclusion */
/******************************************************************************/
// END OF FILE
/******************************************************************************/
