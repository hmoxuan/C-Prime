/** \file

$Id$

Copyright (c) 2016 Philips Lighting Holding B.V.
All Rights Reserved.

This source code and any compilation or derivative thereof is the proprietary
information of Philips Lighting Holding B.V. and is confidential in nature.
Under no circumstances is this software to be combined with any
Open Source Software in any way or placed under an Open Source License
of any type without the express written permission of Philips Lighting Holding B.V.
*/

/******************************************************************************
* EXTERNAL INCLUDE FILES
******************************************************************************/

// Package includes
#include "cwpnode_Local.h"

// other includes
#include "testframework.h"
#include "fta.h"
#include "spiflash_wrapper.h"
#include "fcc_Wrapper.h"

#include "lcp_variant.h"
#include "lcp_types.h"
#include "lcp_macros.h"

#include "cwpnode_CommandHandler.h"
#include "zigbeestack-api.h"

/******************************************************************************
* TESTFRAMEWORK CONFIGURATION DECLARATIONS
******************************************************************************/

/*!
 * @brief Testframework Execution configuration
 */
const testframework_ExecuteConfig_t testframework_ExecuteConfig =
{
    .stackSize = 300,
    .taskPriority = (uint16_t)osa_TaskPrio_Low,
    .queueSize = 2,
};

/*!
 * @brief TestFramwork wrapper list
 */
const testframework_Wrapper_t* testframework_wrapperList[] =
{
    &g_fta_commandHandler,
    &spiflash_Wrapper,
    &fcc_Wrapper,
	&l_c4m_cfgCommandHandler,
	&l_c4m_conCommandHandler,
	&l_c4m_zclCommandHandler,
	&l_c4m_thCommandHandler,
#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY)
	&l_c4m_bleCommandHandler,
#endif
    #if defined(BUILD_TEST_AID)
	&l_c4m_testAidCommandHandler,
	#endif

    // Mandatory end of the wrapper list marker
    TESTFRAMEWORK_WRAPPERLIST_END
};
