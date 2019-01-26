/** \file

$Id: sensic_CommandHandler.h 22140 2015-04-16 09:31:21Z marlon.van.den.berg@philips.com $

Copyright (c) 2011 Koninklijke Philips N.V.
All Rights Reserved.

This source code and any compilation or derivative thereof is the proprietary
information of Koninklijke Philips N.V. and is confidential in nature.
Under no circumstances is this software to be combined with any
Open Source Software in any way or placed under an Open Source License
of any type without the express written permission of Koninklijke Philips N.V.
*/

#ifndef __CWP_COMMANDHANDLER_H
#define __CWP_COMMANDHANDLER_H


/******************************************************************************
* INCLUDE FILES
******************************************************************************/

#include "testframework.h"

/******************************************************************************
* C++ DECLARATION WRAPPER
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* EXPORTED MACROS AND DEFINITIONS
******************************************************************************/

/******************************************************************************
* EXPORTED TYPES
******************************************************************************/

/******************************************************************************
* EXPORTED CONSTANTS
******************************************************************************/

/******************************************************************************
* EXPORTED VARIABLES
******************************************************************************/


/*!
 * @brief The command handler instance for the sensic testframework related commands.
 */
extern const testframework_Wrapper_t  l_c4m_cfgCommandHandler;
extern const testframework_Wrapper_t  l_c4m_conCommandHandler;
extern const testframework_Wrapper_t  l_c4m_zclCommandHandler;
extern const testframework_Wrapper_t  l_c4m_thCommandHandler;
extern const testframework_Wrapper_t  l_c4m_bleCommandHandler;
extern const testframework_Wrapper_t  l_c4m_testAidCommandHandler;


/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/

/******************************************************************************
* END OF C++ DECLARATION WRAPPER
******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __C4M_COMMANDHANDLER_H
