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
* LOCAL INCLUDE FILES
******************************************************************************/

#include "testframework_Local.h"
#include "testframework.h"

/******************************************************************************
* EXTERNAL INCLUDE FILES
******************************************************************************/

#include <string.h>

#include "osa.h"
#include "evdisp.h"

#include "lcp_assert.h"
#include "lcp_variant.h"
#include "lcp_types.h"

/******************************************************************************
* LOCAL MACROS AND CONSTANTS
******************************************************************************/

const char l_testframework_PkgId[] = "TestFramework";

/******************************************************************************
* LOCAL TYPES
******************************************************************************/

#ifdef ASSERT
/*!
 * @brief Application states
 */
typedef enum testframework_AppState_t
{
    testframework_AppState_Unknown,
    testframework_AppState_Initialized,
    testframework_AppState_Ready
} testframework_AppState_t;
#endif

/*! The queue events */
typedef enum testframework_QueueEntryEvent_t
{
    testframework_QueueEntryEvent_CommandReceived,  //!< Indicates that a command has been received
    testframework_ForceEnumSize = 65536
} testframework_QueueEntryEvent_t;

/*! Queue entry structure */
typedef struct testframework_QueueEntry_t
{
    testframework_QueueEntryEvent_t  event;  //!< Contains the event
} testframework_QueueEntry_t;

/******************************************************************************
* LOCAL VARIABLES
******************************************************************************/

#ifdef ASSERT
static testframework_AppState_t s_appState = testframework_AppState_Unknown;
#endif

/*!
 * @brief The scip instance
 */
static Scip_t* s_pScip;

/*!
 * @brief The main task handle
 */
static osa_TaskHandle_t  s_mainTask;

/*!
 * @brief The command queue handle
 */
static osa_QueueHandle_t  s_commandQueue;

/*!
 * @brief The mutex for scip access
 */
static osa_MutexHandle_t s_scipMutex;

/*!
 * @brief The callback to be executed if a command is unrecognized
 */
static testframework_UnrecognizedCommandCb_t s_cbCommandUnrecognized = NULL;

/******************************************************************************
* LOCAL FUNCTION DECLARATIONS
******************************************************************************/

/*!
 * @brief Scip callback function
 * @param [in] pScip  The scip instance that fired the callback
 */
static void ScipCallBack( Scip_t *pScip );

/*!
 * @brief Processes a received command
 * @param [in] pScip  The scip instance that fired the callback
 */
static void ProcessReceivedCommand( Scip_t* pScip );

/******************************************************************************
* OVERLOADABLE FUNCTIONS
******************************************************************************/

/*!
 * @brief Called for each received message
 * @param [in] pScip  The scip instance
 */
#pragma weak testframework_MessageReceivedHook
void testframework_MessageReceivedHook( Scip_t* pScip );

/******************************************************************************
* STATE MACHINE
******************************************************************************/

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/

static void ScipCallBack( Scip_t *pScip )
{
    testframework_QueueEntry_t  entry;

    (void)pScip;

    entry.event = testframework_QueueEntryEvent_CommandReceived;

    osa_QueueSendToFrontCritical( s_commandQueue, &entry, 0 );
}

TASK_HANDLE(testframework_process)
{
    testframework_QueueEntry_t  entry;

    (void)data;

    TASK_MAIN_LOOP_BEGIN
    {
        // Wait 'till there is an event in the command queue
        TASK_WAIT_QUEUE_EVENT(s_commandQueue, (void*)&entry, OSA_MAX_TICKS);

        switch (entry.event)
        {
        case testframework_QueueEntryEvent_CommandReceived:
            ProcessReceivedCommand( s_pScip );
            break;

        default:
            break;
        }
    }
    TASK_MAIN_LOOP_END
}

static void ProcessReceivedCommand( Scip_t* pScip )
{
    if (scip_GetCommand( pScip ) == scip_Err_Ok)
    {
        int nrOfEntities = (int)scip_GetNrOfEntities( pScip );

        testframework_MessageReceivedHook( pScip );

        if (nrOfEntities > 0)
        {
            char* wrapperId  = scip_GetEntity( pScip, 0 );

            if (nrOfEntities > 1)
            {
                char* commandStr = scip_GetEntity( pScip, 1 );

                if ((wrapperId != NULL) && (commandStr != NULL))
                {
                    const testframework_Wrapper_t** pWrapper;
                    bool_t  endReached = false;

                    for (pWrapper = testframework_wrapperList; !endReached; pWrapper++) //lint !e441
                    {
                        if (memcmp( wrapperId, (*pWrapper)->wrapperId, (*pWrapper)->lenWrapperId ) == 0)
                        {
                            // Wrapper found

                            const testframework_Command_t* pCommand;

                            for (pCommand = (*pWrapper)->commandList;
                                 (pCommand != NULL) && (pCommand->command != NULL);
                                 pCommand++)
                            {
                                if (strcmp( commandStr, pCommand->command ) == 0)
                                {
                                    // Command found

                                    // Execute handler
                                    pCommand->handler();

                                    // We are done, no need to loop any further
                                    return;
                                }
                            }

                            // Wrapper found, though not the command
                            if ((*pWrapper)->unrecognizedCommand != NULL)
                            {
                                (*pWrapper)->unrecognizedCommand();

                                // We are done
                                return;
                            }
                        }

                        endReached = MAKE_BOOL( *pWrapper == TESTFRAMEWORK_WRAPPERLIST_END );
                    }
                }
            }
            else
            {
#if defined (CWP_NODE)
                log_Printfbasic("SYS,Error,Incorrect format");
#elif defined (CWP_DONGLE)
                logcom_Printfbasic("SYS,Error,Incorrect format");
#endif
            }

            // Pass the command to whom is interested
            if (s_cbCommandUnrecognized != NULL)
            {
                s_cbCommandUnrecognized();
            }
        }
    }
}

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/

void testframework_Init( void )
{
    // When no stacksize is specified, use the one as configured in ExecuteConfig struct
    testframework_Init2( testframework_ExecuteConfig.stackSize);
}

void testframework_Init2( uint32_t stackSize )
{
    const testframework_Wrapper_t** pWrapper;
    bool_t  endReached = false;

    #ifdef ASSERT
    assert( s_appState == testframework_AppState_Unknown );
    #endif

    s_mainTask = TASK_CREATE_WRAPPER(testframework_process,
                                 "TestFWorkT",
                                 stackSize,
                                 NULL,
                                 (osa_TaskPrio_t)testframework_ExecuteConfig.taskPriority );

    s_commandQueue = osa_QueueCreate( "TestFWorkQ", 10, sizeof( testframework_QueueEntry_t ) );

    for (pWrapper = testframework_wrapperList; !endReached; pWrapper++) //lint !e441
    {
        if ((*pWrapper)->init != NULL)
        {
            (*pWrapper)->init();
        }

        endReached = MAKE_BOOL( *pWrapper == TESTFRAMEWORK_WRAPPERLIST_END );
    };

    /* Create the mutex for scip access */
    s_scipMutex = osa_MutexCreate();

    #ifdef ASSERT
    s_appState = testframework_AppState_Initialized;
    #endif
}

uint32_t testframework_GetStackHighWaterMark( void )
{
    uint32_t taskHighWaterMark = 0;
    if (s_mainTask)
    {
        taskHighWaterMark = GetStackHighWaterMarkForTask(s_mainTask);
    }
    return taskHighWaterMark;
}

void testframework_TurnOn( Scip_t* pScip )
{
    const testframework_Wrapper_t** pWrapper;
    bool_t  endReached = false;

    #ifdef ASSERT
    assert( s_appState == testframework_AppState_Initialized );
    #endif

    assert_LogOnFail( evdisp_IsOnEvDispTask(), "testframework_TurnOn not called from EvDisp task!" );

    s_pScip = pScip;

    scip_Subscribe( s_pScip, ScipCallBack );

    for (pWrapper = testframework_wrapperList; !endReached; pWrapper++) //lint !e441
    {
        if ((*pWrapper)->turnOn != NULL)
        {
            (*pWrapper)->turnOn( pScip );
        }

        endReached = MAKE_BOOL( *pWrapper == TESTFRAMEWORK_WRAPPERLIST_END );
    };

    osa_TaskStart( s_mainTask );

    #ifdef ASSERT
    s_appState = testframework_AppState_Ready;
    #endif
}

osa_MutexHandle_t testframework_getScipMutex(void)
{
    return s_scipMutex;
}

bool_t testframework_ExecuteModuleCommand( int32_t cmdIndex, const testframework_Command_t* moduleCommandTable )
{
    if (scip_GetNrOfEntities(s_pScip) > cmdIndex)
    {
        char* commandStr = scip_GetEntity( s_pScip, cmdIndex );

        if (commandStr != NULL)
        {
            const testframework_Command_t* pCommand;

            for (pCommand = moduleCommandTable;
                 (pCommand != NULL) && (pCommand->command != NULL);
                 pCommand++)
            {
                if (strcmp( commandStr, pCommand->command ) == 0)
                {
                    // Command found

                    // Execute handler
                    pCommand->handler();

                    // We are done, no need to loop any further
                    return true;
                }
            }
        }
    }

    return false;
}

void testframework_SubscribeToUnrecognizedCommand( testframework_UnrecognizedCommandCb_t cb )
{
    s_cbCommandUnrecognized = cb;
}

inline Scip_t* testframework_GetScipInstance( void )
{
    return s_pScip;
}
