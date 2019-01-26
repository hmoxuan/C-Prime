/** \file

$Id$

Copyright (c) 2018 Signify
All Rights Reserved.

This source code and any compilation or derivative thereof is the proprietary
information of Signify and is confidential in nature.
Under no circumstances is this software to be combined with any
Open Source Software in any way or placed under an Open Source License
of any type without the express written permission of Signify
*/

/******************************************************************************
* LOCAL INCLUDE FILES
******************************************************************************/

#include "cwpnode_Local.h"
#include "cwpnode.h"
#include "cwpnode_NormalMode.h"
#include "cwpnode_FtaMode.h"
#include "cwpnode_IrqTest.h"
#include "cwpnode_ZGBTask.h"
#include "cwpnode_Events.h"

#if defined(_DEBUG)
#include "cwpnode_DebugConsole.h"
#endif

/******************************************************************************
* EXTERNAL INCLUDE FILES
******************************************************************************/

// osa includes
#include "osa.h"
#include "osa_Init.h"

#include "vardiv_DriveDefs.h"
#include "filedefinition.h"

// util includes
#include "console.h"
#include "console_Init.h"
#include "evdisp.h"
#include "evdisp_Init.h"
#include "log.h"
#include "buildinfo.h"
#include "testframework.h"
#include "appuartframework.h"
#include "fta.h"
#include "dali.h"
#include "fileio.h"
#include "fileio_Init.h"
#include "fileio_Cfg.h"
#ifdef HASEEPROM
#include "memorybank.h"
#include "memorybank_Init.h"
#endif
#include "random_Init.h"
#include "random.h"
#include "datetime.h"
#include "fcc_Wrapper.h"
#include "burningtime_Init.h"
#include "burningtime.h"
#include "unitconv_PercLog.h"

// lsb includes
#include "lsb_Init.h"
#include "lsb_switchserver.h"

// hw includes
#include "hal.h"
#include "board.h"
#include "board_Init.h"
#include "adc_Init.h"
#include "gpio_Init.h"
#include "stim_Init.h"
#include "usart_Init.h"
#include "spiflash_Init.h"
#include "internalflash_Init.h"
#include "spiflashdisk.h"

// map includes
#include "map.h"
#include "map_Init.h"
#include "map_Cfg.h"

// svc includes
#include "scip.h"
#include "scip_Init.h"
#include "softwareupdate.h"
#include "softwareupdate_Init.h"
#include "security.h"

// dali vertical includes
#include "dali_drv.h"
#include "dali_ftaserver.h"
#include "dali_ftaserver_Init.h"

// for GBL creation
#include "application_properties.h"

// other includes
#include <string.h>

#include "zigbee_groupsserver_Cfg.h"

#include "zigbee_lightcontrolserver.h"
#include "zigbee_groupsserver.h"
#include "zigbee_feedbackserver.h"
#include "zigbee_scenesserver.h"
#include "zigbee_timerserver.h"
#include "zigbee_basicserver_Init.h"
#include "zigbee_zgpcommissioningplserver.h"
#include "zigbee_zgpsensorserver.h"

#include "zigbee_commissioningattrserver.h"
#include "zigbee_otaserver_Init.h"
#include "zigbee_otaserver.h"
#include "zigbee_groupsserver_Init.h"
#include "zigbee_scenesserver_Init.h"
//#include "lsb_lightactuatearbitratebridge_Init.h"
//#include "lsb_lightactuatearbitratebridge.h"
//#include "lsb_lightactuateflowcontrolbridge_Init.h"
//#include "lsb_lightactuateflowcontrolbridge.h"
#include "identify_feedbackbridge.h"
#include "lsb_powermeter_Init.h"
#include "zigbee_lcfsm_cluster.h"
#include "zigbee_timerserver.h"
#include "zigbee_timerserver_Init.h"
#include "zigbee_zgpsensorserver.h"

#ifdef LEDPWMDRV_NEW
#include "ledpwmdrv.h"
#include "ledpwmdrv_Init.h"
#else
#include "leddrv.h"
#include "leddrv_Init.h"
#endif

#include "cwpnode_Persistency.h"
#ifdef HASEEPROM
#include "vardiv_memorybank.h"
#endif
#include "zigbee_ballastconfigurationserver_Cfg.h"
#include "zigbee_ballastconfigurationserver.h"
#include "zigbee_zgp2zcl.h"

#include "zigbee_lcddr_cluster.h"
#include "zigbee_lcddr_cluster_Cfg.h"

#include "zigbee_illummeasurementclient.h"
#include "zigbee_illummeasurementserver.h"
#include "zigbee_reportingserver_Init.h"

#include "lc_DayLightCalibration.h"
#include "lc_Ddr.h"
#include "lc_ddr_Cfg.h"

#include "energymetering.h"
#include "peri_occserver.h"

#include "ctimer.h"

#include "softwareupdate_Cfg.h"
#if defined(DALI_MG12_ENABLED)
#include "xmodem_server.h"
#endif

#include "cwpnode_common.h"

#include "dsvc_lightcontrol_if.h"


// zigbee stack head files
#include "zigbeestack-task-api.h"
#include "zigbeestack-ota-api.h"
#include "zigbeestack-api.h"
#include "dsvcq_cmdQ.h"
#include "dsvcq_evtQ.h"
#if defined (CWP_COMBOzb) && defined (CWP_NODE)
#include "blestack-api.h"
#endif

/******************************************************************************
* LOCAL MACROS AND CONSTANTS
******************************************************************************/

/*!
 * @brief Package id
 */
const char l_cwpnode_PkgId[] = "CWPNode";

/*!
 * @brief Maximum number of arguments to an FTA command
 */
#define MAX_CONSOLE_SCIP_ARGC   (4 + 5) // Added 2 for deprecated Section,Command

/*!
 * @brief Size of command buffer
 */
#define SCIP_BUF_SIZE (256)  //(128)

/*!
 * @brief Buffer size for console write operations
 */
#define CONSOLE_WBUF_SIZE (1024)

/*!
 * @brief Buffer size for console read operations
 */
#define CONSOLE_RBUF_SIZE (2 * SCIP_BUF_SIZE)

/*!
 * @brief Project heap size
 */
//#define HEAP_SIZE ( 16 * 512 + CONSOLE_WBUF_SIZE + CONSOLE_RBUF_SIZE + (2 * SCIP_BUF_SIZE) )
#define HEAP_SIZE ( 106 * 512 + (2 * CONSOLE_WBUF_SIZE) + (2 * CONSOLE_RBUF_SIZE) + (2 * SCIP_BUF_SIZE) + (OTA_RESPONSE_WIDTH * OTA_RESPONSE_DEPTH)+ (ZCL_COMMAND_SEND_WIDTH * ZCL_COMMAND_SEND_DEPTH) )

/*!
 * @brief The number of event dispatcher timers used by this project (without those used by the included packages)
 */
#define CWPNODE_NUM_EVDISP_SUBSCRIPTIONS  (\
    /* Begin PROJECT subscribers */ \
    CWPNODE_EVENTS_NUM_EVDISP_SUBSCRIPTIONS +\
    CWPNODE_FTAMODE_NUM_EVDISP_SUBSCRIPTIONS +\
    CWPNODE_NORMALMODE_NUM_EVDISP_SUBSCRIPTIONS +\
    /* zigbee_reportingserver */ 1 +\
    /* End PROJECT subscribers */ \
    1 \
)

/*!
 * @brief Maximum number of event dispatcher subscriptions
 *
 * Adjust this number to fit best for the project.
 * Best fit equals the global number of calls to @ref evdisp_Subscribe
 * within the project.
 *
 */
#ifdef HASEEPROM
#define MAX_EVDISP_SUBSCRIPTIONS \
( \
    CWPNODE_NUM_EVDISP_SUBSCRIPTIONS +\
    /* Begin subscribers */ \
    LSB_NUM_EVDISP_SUBSCRIPTIONS +\
    FILEIO_NUM_EVDISP_SUBSCRIPTIONS +\
    MEMORYBANK_NUM_EVDISP_SUBSCRIPTIONS +\
    FTA_NUM_EVDISP_SUBSCRIPTIONS +\
    TESTFRAMEWORK_NUM_EVDISP_SUBSCRIPTIONS +\
    /* DALI_FTASERVER_NUM_EVDISP_SUBSCRIPTIONS + */ \
    LIGHTCONTROLSERVER_NUM_EVDISP_SUBSCRIPTIONS +\
    /* LSB_LIGHTACTUATEFLOWCONTROLBRIDGE_NUM_EVDISP_SUBSCRIPTIONS + */ \
    ZIGBEE_FEEDBACKSERVER_NUM_EVDISP_SUBSCRIPTIONS +\
    DATETIME_NUM_EVDISP_SUBSCRIPTIONS +\
    ZIGBEE_GROUPSSERVER_EVDISP_SUBSCRIPTIONS +\
    ZIGBEE_SCENESSERVER_EVDISP_SUBSCRIPTIONS+\
    ZIGBEE_OTASERVER_EVDISP_SUBSCRIPTIONS+\
    ZIGBEE_ZGL2ZCL_EVDISP_SUBSCRIPTIONS+\
    ZIGBEE_ILLUMMEASUREMENTCLIENT_NUM_EVDISP_SUBSCRIPTIONS+\
    /*zigbee_commissioningattrserver*/1+\
    /*zigbee_timerserver*/ 1 +\
    /*lc_ddr*/ 1 +\
    /*switchserver*/1+\
    /*unknown*/4+\
    /*occserver */ZIGBEE_ZGPSENSORSERVER_NUM_EVDISP_SUBSCRIPTIONS+\
    /* zigbee_illummeasurementclient */ 1 +\
    /* lc_dc */ 1 +\
    /* lc_ddr */ 2 +\
    /* End subscribers */ \
    0 \
)

#else
#define MAX_EVDISP_SUBSCRIPTIONS \
( \
    CWPNODE_NUM_EVDISP_SUBSCRIPTIONS +\
    /* Begin subscribers */ \
    LSB_NUM_EVDISP_SUBSCRIPTIONS +\
    FILEIO_NUM_EVDISP_SUBSCRIPTIONS +\
    FTA_NUM_EVDISP_SUBSCRIPTIONS +\
    TESTFRAMEWORK_NUM_EVDISP_SUBSCRIPTIONS +\
    /* DALI_FTASERVER_NUM_EVDISP_SUBSCRIPTIONS + */ \
    LIGHTCONTROLSERVER_NUM_EVDISP_SUBSCRIPTIONS +\
    /* LSB_LIGHTACTUATEFLOWCONTROLBRIDGE_NUM_EVDISP_SUBSCRIPTIONS + */ \
    ZIGBEE_FEEDBACKSERVER_NUM_EVDISP_SUBSCRIPTIONS +\
    DATETIME_NUM_EVDISP_SUBSCRIPTIONS +\
    ZIGBEE_GROUPSSERVER_EVDISP_SUBSCRIPTIONS +\
    ZIGBEE_SCENESSERVER_EVDISP_SUBSCRIPTIONS+\
    ZIGBEE_OTASERVER_EVDISP_SUBSCRIPTIONS+\
    ZIGBEE_ZGL2ZCL_EVDISP_SUBSCRIPTIONS+\
    ZIGBEE_ILLUMMEASUREMENTCLIENT_NUM_EVDISP_SUBSCRIPTIONS+\
    /*zigbee_timerserver*/ 1 +\
    /*lc_ddr*/ 1 +\
    /*switchserver*/1+\
    /*unknown*/4+\
    /*occserver */ZIGBEE_ZGPSENSORSERVER_NUM_EVDISP_SUBSCRIPTIONS+\
    /* zigbee_illummeasurementclient */ 1 +\
    /* lc_dc */ 1 +\
    /* lc_ddr */ 2 +\
    /* End subscribers */ \
    0 \
)
#endif

/*!
 * @brief The number of event dispatcher timers used by this project (without those used by the included packages)
 */
#define CWPNODE_NUM_EVDISP_TIMERS  (\
    /* Begin PROJECT timers */ \
    CWPNODE_FTAMODE_NUM_EVDISP_TIMERS +\
    CWPNODE_NORMALMODE_NUM_EVDISP_TIMERS +\
    CWPNODE_EVENTS_NUM_EVDISP_TIMERS +\
    ZIGBEE_GROUPSSERVER_NUM_EVDISP_TIMERS +\
    ZIGBEE_SCENESSERVER_NUM_EVDISP_TIMERS+\
    ZIGBEE_OTASERVER_NUM_EVDISP_TIMERS+\
    ZIGBEE_REPORTINGSERVER_NUM_EVDISP_TIMERS  +\
    ZIGBEE_COMMISSIONINGATTRSERVER_NUM_EVDISP_TIMERS +\
    /* End PROJECT timers */ \
    0 \
)

/*!
 * @brief Maximum number of event dispatcher timers
 *
 * Add the [PACKAGENAME]_NUM_EVDISP_TIMERS of each included package to this list.
 *
 */
#ifdef HASEEPROM
#define MAX_EVDISP_TIMERS (\
    /* This PROJECT */ \
    CWPNODE_NUM_EVDISP_TIMERS +\
    /* Begin package timers */ \
    TESTFRAMEWORK_NUM_EVDISP_TIMERS +\
    LSB_NUM_EVDISP_TIMERS +\
    FILEIO_NUM_EVDISP_TIMERS +\
    MEMORYBANK_NUM_EVDISP_TIMERS +\
    FTA_NUM_EVDISP_TIMERS +\
    /* DALI_FTASERVER_NUM_EVDISP_TIMERS + */ \
    /* LSB_LIGHTACTUATEFLOWCONTROLBRIDGE_NUM_EVDISP_TIMERS + */ \
    FEEDBACKBRIDGE_NUM_EVDISP_TIMERS +\
    ZIGBEE_FEEDBACKSERVER_NUM_EVDISP_TIMERS +\
    DATETIME_NUM_EVDISP_TIMERS +\
    ZIGBEE_ZGPSENSORSERVER_NUM_EVDISP_TIMERS +\
    ZIGBEE_ZGP2ZCL_NUM_EVDISP_TIMERS +\
    ZIGBEE_TIMERSERVER_NUM_EVDISP_TIMERS +\
    ZIGBEE_LCFSM_CLUSTER_NUM_EVDISP_TIMERS +\
    ZIGBEE_ILLUMMEASUREMENTCLIENT_NUM_EVDISP_TIMERS+\
    LC_DDR_NUM_EVDISP_TIMERS+\
    LC_DC_NUM_EVDISP_TIMERS+\
    /*unknown*/2+\
    /* End package timers */ \
    0 )

#else
#define MAX_EVDISP_TIMERS (\
    /* This PROJECT */ \
    CWPNODE_NUM_EVDISP_TIMERS +\
    /* Begin package timers */ \
    TESTFRAMEWORK_NUM_EVDISP_TIMERS +\
    LSB_NUM_EVDISP_TIMERS +\
    FILEIO_NUM_EVDISP_TIMERS +\
    FTA_NUM_EVDISP_TIMERS +\
    /* DALI_FTASERVER_NUM_EVDISP_TIMERS + */ \
    /* LSB_LIGHTACTUATEFLOWCONTROLBRIDGE_NUM_EVDISP_TIMERS + */ \
    FEEDBACKBRIDGE_NUM_EVDISP_TIMERS +\
    ZIGBEE_FEEDBACKSERVER_NUM_EVDISP_TIMERS +\
    DATETIME_NUM_EVDISP_TIMERS +\
    ZIGBEE_ZGPSENSORSERVER_NUM_EVDISP_TIMERS +\
    ZIGBEE_ZGP2ZCL_NUM_EVDISP_TIMERS +\
    ZIGBEE_TIMERSERVER_NUM_EVDISP_TIMERS +\
    ZIGBEE_LCFSM_CLUSTER_NUM_EVDISP_TIMERS +\
    ZIGBEE_ILLUMMEASUREMENTCLIENT_NUM_EVDISP_TIMERS+\
    LC_DDR_NUM_EVDISP_TIMERS+\
    LC_DC_NUM_EVDISP_TIMERS+\
    /*unknown*/2+\
    /* End package timers */ \
    0 )
#endif
// Ensure that MAX_EVDISP_SUBSCRIPTIONS is not zero (or else we get a build error)
// and not 1 (or else we get a run-time assert
#if MAX_EVDISP_SUBSCRIPTIONS == 0
#undef MAX_EVDISP_SUBSCRIPTIONS
#define MAX_EVDISP_SUBSCRIPTIONS (1)
#else
#if MAX_EVDISP_SUBSCRIPTIONS == 1
#undef MAX_EVDISP_SUBSCRIPTIONS
#define MAX_EVDISP_SUBSCRIPTIONS (2)
#endif
#endif

// Ensure that MAX_EVDISP_TIMERS is not zero (or else we get a build error)
#if MAX_EVDISP_TIMERS == 0
#undef MAX_EVDISP_TIMERS
#define MAX_EVDISP_TIMERS (1)
#endif

// Maximum speed to be used by the uart port
#ifndef UART_MAX_SPEED
    #define UART_MAX_SPEED (115200)
#endif

/**
 * the length of the string to determine if we use a production or test OKB
 */
#define OKB_VERIFICATION_STRING_LENGTH 16

#if defined(DALI_MG12_ENABLED)
#define JG_VERSION_ALIGNED     0
#endif

/**
 * the string that is to be encrypted to determine if we have a test okb or production okb
 */
static const char testOkbVerificationString[OKB_VERIFICATION_STRING_LENGTH] = "DevOKB=OK!123456";
/**
 * the result of encrypting testOkbVerificationString using a test OKB
 */
static const uint8_t encryptedString[OKB_VERIFICATION_STRING_LENGTH] = {0x75, 0xB8, 0x74, 0x0D, 0xBA, 0x9C, 0xE1, 0xAA, 0x1A, 0xA6, 0x2B, 0xBD, 0x0F, 0x9c, 0x51, 0x9F };

/*!
 * @brief Variable for /ref BurningTime to store its data in.
 */
static uint32_t s_burningminutes[1];

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/
extern zigbee_zgp2zcl_Config_t l_cwpnode_zigbee_zgp2zcl_config;

extern void ledpwmdrv_Enable(bool_t bOn);
extern void emberAfRegisterJoinCompleteCallback(void (*cb)(void));
extern bool emberStackIsUp(void);
extern void zigbee_meteringserver_SaveWork(void);
extern void cwpnode_BurningTime_SaveWork(void);

extern void zigbee_lightcontrolserver_CalAOC(const uint8_t *pu8MaxAocPercentage, const uint8_t *pu8MaxAocCurrentMA,
                                                   const uint8_t *pu8MinPhyPercentage, const uint8_t *pu8MinPhyCurrentMA,
                                                   const uint8_t *pu8MinAocCurrentMA, uint16_t u16AocCurrentMA);

extern void zigbee_otaserver_RegisterCompleteCallback(void (*cb)(void));

extern void ledlightactuateserver_Init( void );

extern void ledlightactuateserver_TurnOn( LED_DIMCURVE_T aDimCurve );

/******************************************************************************
* LOCAL TYPES
******************************************************************************/
typedef struct cwpnode_ZigBeeAddress_t
{
	uint8_t  IeeeAddress[8];
	uint16_t ShortAddress;
	bool_t	 ShortAddressValid;
	bool_t	 IeeeAddressValid;
} cwpnode_ZigBeeAddress_t;

static cwpnode_ZigBeeAddress_t s_myZigBeeAddress =
{
	.IeeeAddress  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	.ShortAddress = 0xFFFF,
};

#if defined(DALI_MG12_ENABLED)
/* copy from sns441 for compiling */
static int jg_ver_info = JG_VERSION_ALIGNED;
#endif

/******************************************************************************
* LOCAL FUNCTION DECLARATIONS
******************************************************************************/
#ifdef HASEEPROM
/*!
 * @brief read memory bank for fcc test function
 */
static void fccmemorybank_Read( void );
#endif

/*!
 * @brief Project initialisation function
 */
static void cwpnode_Init( void );

/*!
 * @brief Project start-up function
 */
static void cwpnode_TurnOn( void );

// {{{ fail safe mode
enum{
    E_STATUS_OK = 0,
    E_STATUS_NOK = 1,
};
#define FAIL_SAFE_COUNT_THRESHOLD_SHIFT       5
#define FAIL_SAFE_COUNT_THRESHOLD   (1<<FAIL_SAFE_COUNT_THRESHOLD_SHIFT) //32 //25
#define FAIL_SAFE_POWER_THRESHOLD   1500 //uint: mW
static volatile bool_t s_bIsFailSafeMode = false;

//}}}

void LoadSwitchedOnOffCb(bool_t bOn)
{
    cwpnode_eventLoadSwitched_t eventSwitched;
    eventSwitched.bOn = bOn;

    ledpwmdrv_Enable(bOn);
    cwpnode_PostPvtEvent(cwpnode_PvtEvent_LoadSwitched, &eventSwitched);

    s_bIsFailSafeMode = bOn;
}

static ledpwmdrv_Config_t s_ledpwmdrvConfig =
{
      .dithCallOut        = NULL,
      .pCompModTable      = NULL,
      .useDoubleBuffering = false,
      .pwmCarrier         = 16000,
      .loadSwitchedOnCb   = LoadSwitchedOnOffCb,
      .phyMinPerc         = BOARDDIV_LED_PWM_PHYS_MIN_PERC,
      .phyMaxPerc         = BOARDDIV_LED_PWM_PHYS_MAX_PERC,
};

typedef void (*pfnLogPower_t)(uint32_t u32Power);
static pfnLogPower_t s_pfnLogPower = NULL;
/******************************************************************************
* LOCAL VARIABLES
******************************************************************************/

/*!
 * @brief Project heap
 */
static char s_Heap[HEAP_SIZE];

/*!
 * @brief Heap configuration
 */
static const osa_HeapConfig_t s_heapConfig =
{
    s_Heap,     // The address of the heap space to manage
    HEAP_SIZE,  // The size of the heap space
    0,          // Minimum size to split
    0,          // Minimum size after split
    false       // Do not split
};

/*!
 * @brief Standard I/O definitions
 */
static stdio_Device_t s_out =
{
    NULL, & console_iSeqCharIO, & console_iDevLocking
};

static stdio_Device_t s_in =
{
    NULL, & console_iSeqCharIO, & console_iDevLocking
};

static stdio_Device_t s_out_AppUart =
{
    NULL, & console_iSeqCharIO, & console_iDevLocking
};

static stdio_Device_t s_in_AppUart =
{
    NULL, & console_iSeqCharIO, & console_iDevLocking
};

/*!
 * @brief Scip instance used by the console command interface
 */
static Scip_t* s_pConsoleScip;
static Scip_t* s_pConsoleScip_AppUart;

/*!
 * @brief Event dispatcher subscription table
 */
static evdisp_Subscription_t s_evdispSubscriptions[MAX_EVDISP_SUBSCRIPTIONS];

/*!
 * @brief Event dispatcher timer table
 */
static evdisp_Timer_t  s_evdispTimers[MAX_EVDISP_TIMERS ? MAX_EVDISP_TIMERS : 1];

/*!
 * @brief Event dispatcher configuration
 */
static evdisp_Config_t s_evdispConfig =
{
    /* subscriptionTable     = */ s_evdispSubscriptions,
    /* timerTable            = */ s_evdispTimers,
    /* pipelineBuffer        = */ NULL,
    /* subscriptionTableSize = */ MAX_EVDISP_SUBSCRIPTIONS,
    /* timerTableSize        = */ MAX_EVDISP_TIMERS,
    /* pipelineBufferSize    = */ 0,
    /* pipEventDataSize      = */ 0,
    /* fxnEventDataSize      = */ 0,
    /* cusEventDataSize      = */ 0,
    /* staEventDataSize      = */ 0,
    /* additionalStackSize   = */ 32,
    /* eventQueueSize        = */ 16,
    /* cycleTime             = */ 20
};


/*!
 * @brief application properties as used by the GBL creation tool
 */
static __root const ApplicationProperties_t appProperties = {
    .magic = APPLICATION_PROPERTIES_MAGIC,
    .structVersion = APPLICATION_PROPERTIES_VERSION,
    .signatureType = APPLICATION_SIGNATURE_ECDSA_P256,
    .signatureLocation = 0xFFFFFFFF,
    .app = {
        .type = APPLICATION_TYPE_ZIGBEE,
        .version = (uint32_t)APPLICATION_VERSION_MAJOR<<16 + (uint32_t)APPLICATION_VERSION_MINOR << 8 + APPLICATION_VERSION_REV,
        .capabilities = 0,
        .productId = { /* TODO: Determine productId */
            0x01,0x00,0x02,0x03,0x04,0x05,0x07,0x06,
            0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
        }
    }
};

/*!
 * @brief Buffer used for the memory bank read operations
 */
#ifdef HASEEPROM
cwpnode_MbBuffer_t l_cwpnode_mbBuffer;
#endif

#if defined(DALI_MG12_ENABLED)
static bool is_Allow_Upgrade_JG = true;
#endif

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/
static void cwpnode_InitTaskLevel(void)
{
#ifdef DALI_MG12_ENABLED
    uint8_t aocMaxDutyCycle[2] = {0x38, 0x23};
    uint8_t aocMaxCurrent[2] =   {0x52, 0x03};
    uint8_t minDutyCycle[2] =    {0x38, 0x02};
    uint8_t minCurrent[2] =      {0x32, 0x00};
    uint8_t aocMinCurrent[2] =   {0xF4, 0x01};
    uint16_t aoc = 250;

    zigbee_lightcontrolserver_CalAOC(aocMaxDutyCycle,
                                     aocMaxCurrent,
                                     minDutyCycle,
                                     minCurrent,
                                     aocMinCurrent,
                                     aoc);

    ledpwmdrv_SetParams(1 * 1000, aocMaxDutyCycle, minDutyCycle);
#else

#if 0 //has to be redesigned interface to support aoc parameters from external
#ifdef HASEEPROM
    memorybank_114_t memBank114;
    memBank114.aocCurrent_msb = memBank114.aocCurrent_lsb = 0xff;
    void *pHandler = fileio_Open(MEMORYBANK_RAM_FILENAME(114), fileio_Mode_Read);
    if(pHandler != NULL)
    {
        (void)fileio_Read(pHandler, (uint8_t *)&memBank114, 0, sizeof(memorybank_114_t));
        fileio_Close(pHandler);
    }
    uint16_t aoc = (uint16_t)memBank114.aocCurrent_msb * 256 + memBank114.aocCurrent_lsb;

    const filedefs_DeviceConf_Data_t *psDevConfData = cwpnode_persistency_GetDeviceConfData();
    assert(psDevConfData);
    if (psDevConfData)
    {
        zigbee_lightcontrolserver_CalAOC(psDevConfData->aocMaxDutyCycle,
                                            psDevConfData->aocMaxCurrent,
                                            psDevConfData->minDutyCycle,
                                            psDevConfData->minCurrent,
                                            psDevConfData->aocMinCurrent,
                                            aoc);

        ledpwmdrv_SetParams(psDevConfData->pwmFreq*1000,
                psDevConfData->aocMaxDutyCycle, psDevConfData->minDutyCycle);
    }
#else
    /* use the same values as those reading from eeprom,
       otherwise Maximum dim level will be different. */
    uint8_t aocMaxDutyCycle[2] = {0x10, 0x27};
    uint8_t aocMaxCurrent[2] =   {0x52, 0x03};
    uint8_t minDutyCycle[2] =    {0x0a, 0x00};
    uint8_t minCurrent[2] =      {0x32, 0x00};
    uint8_t aocMinCurrent[2] =   {0xF4, 0x01};
    uint16_t aoc = 0xffff;

    zigbee_lightcontrolserver_CalAOC(aocMaxDutyCycle,
                                     aocMaxCurrent,
                                     minDutyCycle,
                                     minCurrent,
                                     aocMinCurrent,
                                     aoc);

    ledpwmdrv_SetParams(1 * 1000, aocMaxDutyCycle, minDutyCycle);
#endif
#else
    uint8_t MaxDutyCycle[2] = {BOARDDIV_LED_PWM_PHYS_MAX_PERC, 0x00};
    uint8_t minDutyCycle[2] = {BOARDDIV_LED_PWM_PHYS_MIN_PERC, 0x00};

    /* todo, code refactor */
    /* To update attribute physicalMinLevel in ballast configuration cluster from PHYS_MIN_PERC */
    // convert from zigbeeActual to dimlevel
    // aDimLevel = zigbee_actual*10000/254
    uint16_t aDimLevel = ROUNDDIV(ZIGBEE_BALLASTCONFIGURATIONSERVER_PHYSICAL_MINLEVEL * 10000, 254 );
    uint16_t dimPerc = unitconv_ZigbeeLog_DimLevel2Percentage(aDimLevel)/100;
    minDutyCycle[0] = dimPerc;

    // convert from zigbeeActual to dimlevel
    // aDimLevel = zigbee_actual*10000/254
    aDimLevel = ROUNDDIV(ZIGBEE_BALLASTCONFIGURATIONSERVER_PHYSICAL_MAXLEVEL * 10000, 254 );
    dimPerc = unitconv_ZigbeeLog_DimLevel2Percentage(aDimLevel)/100;;
    MaxDutyCycle[0] = dimPerc;

    ledpwmdrv_SetParams(s_ledpwmdrvConfig.pwmCarrier,
                        MaxDutyCycle,
                        minDutyCycle);
    zigbee_ballastconfigurationserver_atttribute_config.physicalMinLevel = ZIGBEE_BALLASTCONFIGURATIONSERVER_PHYSICAL_MINLEVEL;
    zigbee_ballastconfigurationserver_atttribute_config.physicalMaxLevel = ZIGBEE_BALLASTCONFIGURATIONSERVER_PHYSICAL_MAXLEVEL;

#endif

#endif
}

static void FailSafeModeDoWork(SLONG i32dummy, ULONG u32Power)
{
    (void)i32dummy;
    static uint32_t u32SumEnergy = 0;
    static uint32_t u32LocCount = 0;
    if (s_bIsFailSafeMode)
    {
        if (u32LocCount++ < FAIL_SAFE_COUNT_THRESHOLD)
        {
            u32SumEnergy += u32Power;
        }
        else
        {
            u32LocCount = 0;
            if (u32SumEnergy < (FAIL_SAFE_POWER_THRESHOLD<<FAIL_SAFE_COUNT_THRESHOLD_SHIFT))
            {
                if (zigbee_ballastconfigurationserver_atttribute_config.ballastStatus == E_STATUS_OK)
                {
                    log_Printfbasic("Driver fail, avg power:%d", (u32SumEnergy>>FAIL_SAFE_COUNT_THRESHOLD_SHIFT));
                    zigbee_ballastconfigurationserver_atttribute_config.ballastStatus = E_STATUS_NOK;
                }
            }
            else
            {
                if (zigbee_ballastconfigurationserver_atttribute_config.ballastStatus == E_STATUS_NOK)
                {
                    log_Printfbasic("Driver ok, avg power:%d", (u32SumEnergy>>FAIL_SAFE_COUNT_THRESHOLD_SHIFT));
                    zigbee_ballastconfigurationserver_atttribute_config.ballastStatus = E_STATUS_OK;
                }
            }
            u32SumEnergy = 0;
        }
    }

    if (s_pfnLogPower)
    {
        s_pfnLogPower(u32Power);
    }
}

/*!
 * @brief Function to output the build information
 */
static void cwpnode_LogBuildInfo( void )
{
    const char* url = strstr( buildinfo_RepoUrl, "/LCP/" );

#ifdef DALI_MG12_ENABLED
    const char* prj = "EasySense";
#else
    const char* prj = "APP";
#endif

    if (url == NULL)
    {
        url = buildinfo_RepoUrl;
    }

    // using scip/flexunit syntax "comp,func,args*"
    log_Printfbasic( ",Source URL      , %s", url );
    log_Printfbasic( ",Revision        , %d%s", buildinfo_Revision, (buildinfo_IsModified ? "*" : "") ); /*lint !e506*/
    log_Printfbasic( ",Rev. date/time  , %s", buildinfo_RevDateTime );
    log_Printfbasic( ",Build date/time , %s", buildinfo_BuildDateTime );
    log_Printfbasic( ",Build by        , %s", buildinfo_UserName );
    log_Printfbasic( ",Build on        , %s", buildinfo_ComputerName );
    log_Printfbasic( ",Build variant   , %s (%s)", buildinfo_Variant, buildinfo_BuildType );
    log_Printfbasic( ",Project name    , %s, %s", PKGID, prj );

#if defined(DALI_MG12_ENABLED)
    if ((halGetResetInfo() == 0x02) && (halGetExtendedResetInfo() == 0x03))// 0x02 is BTL, 0x03 is BADIMAGE, see reset-def.h
    {
        is_Allow_Upgrade_JG = false;
    }
#endif
}


static bool_t GetMyZigBeeAddresses( uint16_t* shortAddress, uint8_t** ieeeAddress )
{
    bool_t valid = (s_myZigBeeAddress.IeeeAddressValid == true) && (s_myZigBeeAddress.ShortAddressValid == true);

    /*If Address is initialized store address (s) in provided parameter*/
    if( (valid) &&
        (shortAddress!=NULL && ieeeAddress != NULL))
    {
        *shortAddress = s_myZigBeeAddress.ShortAddress;
        *ieeeAddress = s_myZigBeeAddress.IeeeAddress;
    }

    return valid;
}

extern uint8_t emLocalEui64[8];
static void SwapMsbToLsb(uint8_t * array, uint8_t num)
{
    uint8_t i,temp;
    for ( i = 0; i < num / 2; i++)
    {
        temp = array[i];
        array[i] = array[num - 1 - i];
        array[num - 1 - i] = temp;
    }
}

static void GetMyZigbeeAddr(void)
{
    s_myZigBeeAddress.ShortAddress = emberGetNodeId();
    s_myZigBeeAddress.ShortAddressValid = true;

    memcpy(&s_myZigBeeAddress.IeeeAddress[0], emLocalEui64, sizeof(s_myZigBeeAddress.IeeeAddress));
    SwapMsbToLsb(&s_myZigBeeAddress.IeeeAddress[0],sizeof(s_myZigBeeAddress.IeeeAddress));
    s_myZigBeeAddress.IeeeAddressValid = true;

    uint16_t panID = zigbeeStack_GetPanId();

    log_Printfbasic( "PAN,0x%04X", panID );
    log_Printfbasic( "ADR,0x%04X", s_myZigBeeAddress.ShortAddress );
    log_Printfbasic( "MAC,%02X%02X%02X%02X%02X%02X%02X%02X",
                         s_myZigBeeAddress.IeeeAddress[0],
                         s_myZigBeeAddress.IeeeAddress[1],
                         s_myZigBeeAddress.IeeeAddress[2],
                         s_myZigBeeAddress.IeeeAddress[3],
                         s_myZigBeeAddress.IeeeAddress[4],
                         s_myZigBeeAddress.IeeeAddress[5],
                         s_myZigBeeAddress.IeeeAddress[6],
                         s_myZigBeeAddress.IeeeAddress[7] );
}

static void Start_lightcontrol_fsm(void)
{
    if ( (s_myZigBeeAddress.IeeeAddressValid  == true) &&
         (s_myZigBeeAddress.ShortAddressValid == true) )
    {
        (void)lc_Fsm_Start(s_myZigBeeAddress.ShortAddress);
    }
}

#ifdef DALI_MG12_ENABLED
static bool_t cwpnode_FactoryCtuneConf ( uint16_t custom_ctune )
{
    void* pHandle = NULL;
    pHandle = fileio_Open("userdata.ctune", fileio_Mode_ReadWrite);

    if (pHandle == NULL)
    {
        log_Printfbasic( "config Ctune error" );
        return false;
    }

    if( fileio_Write(pHandle, (uint8_t *)&custom_ctune, 0, sizeof( custom_ctune )) == sizeof( custom_ctune ) )
    {
        fileio_Close(pHandle);
        return true;
    }
    else
    {
        fileio_Close(pHandle);
        log_Printfbasic( "config Ctune error" );
        return false;
    }
}
#endif

static void cwpnode_Init( void )
{
    // if debug lock is disabled, then hold on the mcu
    iflashlock_Check();

    const map_Entry_t* pMapEntry = l_cwpnode_mapEntry;
    // Initialize hw modules
    random_Init();
    stim_Init( NULL );

#ifndef DALI_MG12_ENABLED
    adc_Init();     // Dali mg12 has no adc.
#endif

    spiflash_Init();
    internalflash_Init();
    fileio_Init( l_cwpnode_fileioFileTable, l_cwpnode_fileioMountTable, 384, l_cwpnode_nrOfFileioWorkers, l_cwpnode_fileioWorkers );

    cwpnode_ZGBTaskInit();
#if defined (CWP_COMBOzb) && defined (CWP_NODE)
    // dunfa: tbc
    bleStack_Init();
#endif

    // Initialize svc modules

    // Initialize other util modules
    lsb_Init();
    map_Init( &pMapEntry, l_cwpnode_nrOfItems );
    identify_feedbackbridge_Init();
    fta_Init( s_pConsoleScip );
#ifdef DALI_MG12_ENABLED
    dali_Init( s_pConsoleScip_AppUart);
#endif
    testframework_Init();
    appuartframework_Init();
#ifdef HASEEPROM
    memorybank_Init();
#endif

#ifndef DALI_MG12_ENABLED
    ledpwmdrv_Init( &s_ledpwmdrvConfig );
    ledlightactuateserver_Init();

    ENERGYMETERING_vInit();
#endif

    evdisp_Init( cwpnode_TurnOn, &s_evdispConfig );
    datetime_Init();
    lsb_powermeter_Init();

    // Initialize vertical modules
    // Initialize product modules
    cwpnode_EventsInit();
    cwpnode_FtaModeInit();

    zigbee_commissioningattrserver_Init();
    zigbee_otaserver_Init();
    zigbee_lightcontrolserver_Init();
    zigbee_groupsserver_Init(&l_cwpnode_zigbee_groupsserver_config);
    zigbee_scenesserver_Init();
    zigbee_basicserver_Init();
    zigbee_ballastconfigurationserver_Init();
    zigbee_zgpcommissioningplserver_Init();
    zigbee_zgp2zcl_Init(GetMyZigBeeAddresses,(zigbee_zgp2zcl_Config_t *)&l_cwpnode_zigbee_zgp2zcl_config);
    zigbee_zgpsensorserver_Init();

    lc_ddr_Init(&l_lc_ddr_CbConfig, &zigbee_lcddr_cluster_attribute_config);
    lc_dc_Init(&zigbee_lcddr_cluster_attribute_config);
    zigbee_lcddr_cluster_Init();
    zigbee_lcfsm_cluster_Init(GetMyZigBeeAddresses);
    zigbee_illummeasurementclient_Init();
    zigbee_illummeasurementserver_Init(GetMyZigBeeAddresses);

    //lsb_lightactuatearbitratebridge_Init();
    //lsb_lightactuateflowcontrolbridge_Init();
    lsb_switchserver_Init();

    zigbee_feedbackserver_Init();

    cwpnode_persistency_Init();

    zigbee_reportingserver_Init();
    zigbee_occupancysensingserver_Init();
    burningtime_Init(1);

    CTIMER_Init();

    //initialize the software updater
    softwareupdate_SetRecipe(&l_cwpnode_Uu_Recipe);
    softwareupdate_Init();
	dsvcq_cmdQueue_init();
	dsvcq_evtQueue_init();
}

static void printNetworkSetting(void)
{
    uint16_t updateId;
    uint8_t  channel;
    uint16_t nwkAddr;
    uint16_t panId;
    uint8_t ieeeAddr[8] = {0};
    uint8_t nwkExtPanId[8] = {0};

    zigbeeStack_DumpInfo(&updateId,&channel,&nwkAddr,&panId,ieeeAddr);
    zigbeeStack_GetExtendedPanId(nwkExtPanId);
    log_Printfbasic("Bridge,NetworkSettings,False,0x%04X,%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,%d,%d,S=0x%04X",
                    panId,
                    nwkExtPanId[7], nwkExtPanId[6], nwkExtPanId[5], nwkExtPanId[4],
                    nwkExtPanId[3], nwkExtPanId[2], nwkExtPanId[1], nwkExtPanId[0],
                    channel, updateId, nwkAddr);
}

static void NetworkJoinCompleteCallback(void)
{
    #define LUM_25_PERCENTAGE       64 // 254 * 0.25 = 63.5
    #define TRANSITION_TIME_100MS   10 // 1s

    GetMyZigbeeAddr();

    printNetworkSetting();

    ENERGYMETERING_TurnOn(1 << E_LINESIDE, FailSafeModeDoWork);
    zigbee_lightcontrolserver_SetLevel(LUM_25_PERCENTAGE, TRANSITION_TIME_100MS); //
}

/*!
 * @brief Start all the cwpnode packages
 */
void cwpnode_Start( void )
{
    CTIMER_TurnOn();
    softwareupdate_TurnOn();

    fileio_Start();
    //zigbee_zgp2zcl_Start();

    zigbee_basicserver_Start();
    zigbee_ballastconfigurationserver_Start();
    zigbee_basicserver_Start();
    zigbee_lightcontrolserver_Start();
    zigbee_commissioningattrserver_Start();
    zigbee_groupsserver_Start();
    zigbee_illummeasurementserver_Start();
    zigbee_occupancysensingserver_Start();
    zigbee_lcddr_cluster_Start();
    zigbee_lcfsm_cluster_Start();
    zigbee_timerserver_cluster_Start();
    zigbee_otaserver_Start();
    zigbee_reportingserver_Start();

    Start_lightcontrol_fsm();
    cwpnode_NormalModeStart();
    log_Printfbasic("cfg,started");
}


#ifdef DALI_MG12_ENABLED
#define HFXCO_CTUNE (0x00E1)

/* copy from sns441 for compiling */
static bool is_lum_test_enabled(int testId)
{
    int byteInd = testId / 8;
    int bitOffset = testId % 8;
    struct memorybank_191_2_t mb191_2;

    if (testId < 0 || testId > 31)
        return false;

    if (memorybank_Read(MEMORYBANK_191_2, 0, sizeof(mb191_2), &mb191_2) != sizeof(mb191_2))
    {
        log_Printfbasic("mb,read,failed,191_2");
        return false;
    }

    if (mb191_2.testMask[3 - byteInd] & (0x1 << bitOffset))
        return true;
    else
        return false;
}

#define LUMINAIRE_TEST_BLINK_TIME  1000
static void luminaire_test_memorybank_Read()
{
    struct memorybank_191_t mb191;
    const zigbee_Identify_Server_Callback_t** pIdentifySvrCb = (const zigbee_Identify_Server_Callback_t**)zigbee_Identify_Server_pCallback;

    if (memorybank_Read(MEMORYBANK_191, 0, sizeof(mb191), &mb191) != sizeof(mb191))
    {
        log_Printfbasic("mb,read,failed,191");
        return;
    }

    if (mb191.testId != 0)
    {
        log_Printfbasic("mb,testid,unsupported,%d", mb191.testId);
        return;
    }

    if (!is_lum_test_enabled(mb191.testId))
    {
        log_Printfbasic("mb,testmask,disable,%d", mb191.testId);
        return;
    }

    if (mb191.state == MB_191_TEST_STATE_START)
    {
        (void)emberAfPluginNetworkSteeringStop();
        log_Printfbasic("start luminaire test");
        // blink once will cost 300ms, so blink times = (blink duration second * 10 / 3)
        if ((*pIdentifySvrCb)->identify != NULL)
            (*pIdentifySvrCb)->identify(LUMINAIRE_TEST_BLINK_TIME * 10 / 3);

        // write test result, and reset test state to idle
        mb191.testResult = MB_191_TEST_RESULT_PASSED;
        mb191.state = MB_191_TEST_STATE_IDLE;

        void *file = fileio_Open(MEMORYBANK_ROM_FILENAME(MEMORYBANK_191), fileio_Mode_Write);
        if (file == NULL)
        {
            log_Printfbasic("mb,open,failed,191");
            return;
        }

        if (memorybank_DirectWrite(file, (uint8_t*)&mb191, 0, sizeof(mb191)) != sizeof(mb191))
        {
            log_Printfbasic("mb,write,failed,191");
        }
        fileio_Close(file);
        fta_StartFtaMode();
    }
}

int is_JG_version_ok(void)
{
    return jg_ver_info == JG_VERSION_ALIGNED;
}
#endif

static void cwpnode_TurnOn( void )
{
    uint32_t burnMinutes;

    fileio_TurnOn();
#ifdef HASEEPROM
    memorybank_TurnOn();
#endif
    fta_TurnOn();
#ifdef DALI_MG12_ENABLED
    dali_TurnOn();
#endif
    lsb_TurnOn();
    testframework_TurnOn( s_pConsoleScip );
    appuartframework_TurnOn(s_pConsoleScip_AppUart);
    datetime_TurnOn();
    scip_TurnOn( s_pConsoleScip );
    scip_TurnOn( s_pConsoleScip_AppUart );
    cwpnode_persistency_TurnOn();
    map_TurnOn();

    #if defined(_DEBUG)
    // Include debug-console only for debug builds
    cwpnode_DebugConsoleTurnOn();
    #endif

    ledlightactuateserver_TurnOn(ledpwmdrv_DimCurve_ZigbeeLog);

    // Start vertical modules
    // TODO::optimize for join process, is there any better solution?
    // Currently, there is side effects enable energy metering before join network.
    if (emberNetworkState() == 2 /*EMBER_JOINED_NETWORK*/) {
        ENERGYMETERING_TurnOn(1 << E_LINESIDE, FailSafeModeDoWork);
        GetMyZigbeeAddr();
        printNetworkSetting();
    } else {
        emberAfRegisterJoinCompleteCallback(NetworkJoinCompleteCallback);
    }

    identify_feedbackbridge_TurnOn();
    lsb_powermeter_TurnOn();
    //lsb_lightactuatearbitratebridge_TurnOn();
    //lsb_lightactuateflowcontrolbridge_TurnOn();
    lsb_switchserver_TurnOn();

    // Start product modules
    cwpnode_EventsTurnOn();
    cwpnode_FtaModeTurnOn();

#ifdef DALI_MG12_ENABLED
    cwpnode_FactoryCtuneConf(HFXCO_CTUNE);
#endif
    cwpnode_InitTaskLevel();
    // So initialized it after cwpnode_persistency_TurnOn().

    zigbee_commissioningattrserver_TurnOn();
    zigbee_otaserver_TurnOn();
    zigbee_lightcontrolserver_TurnOn();
    zigbee_groupsserver_TurnOn();
    zigbee_scenesserver_TurnOn();
    zigbee_basicserver_TurnOn();
    zigbee_ballastconfigurationserver_TurnOn();
    zigbee_zgpcommissioningplserver_TurnOn();
    zigbee_zgp2zcl_TurnOn();
    zigbee_timerserver_TurnOn();
    zigbee_lcddr_cluster_TurnOn();

    zigbee_feedbackserver_TurnOn();
    zigbee_zgpsensorserver_TurnOn(); // Must after peri_occserver_TurnOn!
    zigbee_lcfsm_cluster_TurnOn();
    zigbee_reportingserver_TurnOn();
    zigbee_occupancysensingserver_TurnOn();
    zigbee_illummeasurementclient_TurnOn();
    zigbee_illummeasurementserver_TurnOn();
    lc_dc_TurnOn();
    lc_ddr_TurnOn();

#ifdef HASEEPROM
    fccmemorybank_Read();
#endif

    // TODO:: start pwm out only when valid aoc is read from device.conf
    ledpwmdrv_TurnOn();
    burnMinutes = cwpnode_BurningTime_RestoreBurnTime();
    s_burningminutes[0] = burnMinutes;
    burningtime_TurnOn(s_burningminutes);

#if defined(DALI_MG12_ENABLED)
    luminaire_test_memorybank_Read();
    if ( is_Allow_Upgrade_JG )
    {
        CmdBootloaderInit();
    }
    else
    {
        cwpnode_EraseSoftwareUpdateDrive();
    }
#endif
}

usart_t consoleUsart;
usart_t consoleUsart_AppUart;
stdio_Device_t *AppUartin = NULL;
stdio_Device_t *AppUartout = NULL;

static void cwpnode_PreInit( void )
{
    // Necessary board work
    gpio_Init();
    board_Init();

    // Uart package
    usart_Init();

    // Start the console and the logging service
    // In the Qos test, if the baudrate is set to 115200, packets will be lost when the serial DMA interrupts the receiving data
    Stdout = &s_out;
    Stdin  = &s_in;
    s_out.dev = s_in.dev = console_Init(
            &consoleUsart,
            19200, //UART_MAX_SPEED¡¢9600¡¢19200
            true, // Use DMA
            CONSOLE_WBUF_SIZE,
            CONSOLE_RBUF_SIZE );

    // Initialize command interpreter on the console, with given command buffer
    // and given maximum number of positional parameters to support.
    s_pConsoleScip = scip_Init( Stdin, SCIP_BUF_SIZE, MAX_CONSOLE_SCIP_ARGC, scip_ChecksumType_None );

    console_TurnOn( s_out.dev, BOARDDIV_CONSOLE );
    log_Init( Stdout );

    AppUartout = &s_out_AppUart;
    AppUartin  = &s_in_AppUart;
    s_out_AppUart.dev = s_in_AppUart.dev = console_Init(
            &consoleUsart_AppUart,
            UART_MAX_SPEED,
            true, // Use DMA
            CONSOLE_WBUF_SIZE,
            CONSOLE_RBUF_SIZE );
    s_pConsoleScip_AppUart = scip_Init( AppUartin, SCIP_BUF_SIZE, MAX_CONSOLE_SCIP_ARGC, scip_ChecksumType_None );
    console_TurnOn( s_out_AppUart.dev, BOARDDIV_RFLINK );
    logcom_Init( AppUartout );
}

#ifdef HASEEPROM
static void fccmemorybank_Read()
{
    memorybank_143_t *pBuffer = (memorybank_143_t*)osa_Malloc( sizeof( memorybank_143_t ) );
    if (pBuffer == NULL)
    {
        return;
    }

    memorybank_Read( MEMORYBANK_143, 0, sizeof( memorybank_143_t ), pBuffer );
    if(pBuffer->fccMode == 1)
    {
        zigbeeStack_NetworkSteeringStop();
        hal_uSleep(500000);
        switch(pBuffer->testNumber)
        {
        case 1:
            ModuRadioTest(pBuffer->zigbeeChannel, pBuffer->txPower);
            break;
        case 2:
            RadioTest(pBuffer->zigbeeChannel, pBuffer->txPower);
            break;
        case 3:
           CmdRxOnly();
           break;
        default:
           break;
        }
	    zigbee_lightcontrolserver_SetLevel(pBuffer->lightOutputLevel, 10);
    }
    osa_Free( pBuffer );
}
#endif

static bool_t DeviceHasDevelopmentKey(void)
{
    uint8_t encrypted[OKB_VERIFICATION_STRING_LENGTH];
    uint8_t nonce[OKB_VERIFICATION_STRING_LENGTH];
    uint8_t derivedKey[OKB_VERIFICATION_STRING_LENGTH];
    bool_t hasDevelopementKey = false;

    memset(nonce, 0x00, sizeof(nonce));
    memset(encrypted, 0x00, sizeof(encrypted));
    memset(derivedKey, 0x00, sizeof(derivedKey));

    SECURITY_vSetKey(KEYTYPE_ID_SW_UPDATE , NULL);
    SECURITY_vGetDerived_Key(&nonce[0], DERIVATION_ENCRYPTION, &derivedKey[0]);
    SECURITY_vSetKey(KEYTYPE_ID_PLAINTEXT_KEY , &derivedKey[0]);
    SECURITY_vEncryptAES_ECB(&encrypted[0], (uint8_t const *)&testOkbVerificationString[0], OKB_VERIFICATION_STRING_LENGTH);

    if (memcmp(&encrypted[0], &encryptedString[0], OKB_VERIFICATION_STRING_LENGTH) == 0)
    {
        // the encryption result is the same as the encryption result using the test OKB
        hasDevelopementKey = true;
    }
    return hasDevelopementKey;
}

/******************************************************************************
* EXPORTED VARIABLES
******************************************************************************/
bool_t cwpnode_IsSizeOk(filedefinition_PuType_t puType, uint32_t size)
{
	uint32_t maxSize;
	bool_t ret = false;

	/* set maximum size according to puType */
	switch (puType)
	{
	case filedefinition_PuType_Sensor_Main:
		maxSize = 0x40000 - 0x8000; // 256Kbyte - 32Kbyte for bootloader
		break;
	case filedefinition_PuType_Sensor_Sub1:
		maxSize = 0x3E000;
		break;
	default:
		maxSize = 0;
	}

	/* check size */
	if (size < maxSize)
	{
		ret = true;
	}
	return ret;
}

void cwpnode_SaveLightStatus(void)
{
    // store metering data and burning hour
    zigbee_meteringserver_SaveWork();
    cwpnode_BurningTime_SaveWork();
}

#ifdef __ICCARM__ // If IAR
#pragma diag_suppress=Pe186 // Suppress IAR warning: [Pe186] applicable as long as APPLICATION_VERSION_MAJOR equals 0
#endif
void cwpnode_PrepareForUpdate(void)
{
#if 0
    static uint32_t s_VersionToUpgradeTo = 0;

    // if down-grading, erase e: and f: drive
    //lint -e{568,587,685} : applicable as long as APPLICATION_VERSION_MAJOR equals 0
    if ((s_VersionToUpgradeTo >> 16) < APPLICATION_VERSION_MAJOR)
    {
        envisionapp_Stop();
        fileio_Suspend();
        // erase e: and f:
        if (g_DriveXnvE.pErase)
        {
            g_DriveXnvE.pErase();
        }
        if (g_DriveXnvF.pErase)
        {
            g_DriveXnvF.pErase();
        }
    }
#endif
}

#ifdef __ICCARM__ // If IAR
#pragma diag_default=Pe186 // Suppress IAR warning: [Pe186] applicable as long as APPLICATION_VERSION_MAJOR equals 0
#endif
void cwpnode_EraseSoftwareUpdateDrive(void)
{
    ENERGYMETERING_TurnOff();
    (void)g_Spiflashdisk.pErase(DRIVE_D_START, DRIVE_D_SIZE);
    log_Printfbasic("drive D: erased");
}

softwareupdate_Parse_Result_t cwpnode_IsUpdateAllowed(filedefinition_PuType_t puType, uint32_t uuVersion, uint32_t toVersion)
{
	softwareupdate_Parse_Result_t result = softwareupdate_Parse_Result_Ok;

	(void)uuVersion;

	(void)toVersion;
	switch (puType)
	{
	case filedefinition_PuType_Sensor_Main:
		if (DeviceHasDevelopmentKey())
		{
			/* for the 'normal' update the same major or major + 1 is allowed */
			if ( !(( (toVersion >> 16) <=  APPLICATION_VERSION_MAJOR	  ) ||	 /* lower or same major version */
				   ( (toVersion >> 16) == (APPLICATION_VERSION_MAJOR + 1) )) )	 /* major version is one higher */
			{
				result = softwareupdate_Parse_Result_Illegal_Pu_Sw_Version;
			}
		}
		else
		{
			/* for the 'normal' update the same major or major + 1 is allowed */
			if ( !(( (toVersion >> 16) ==  APPLICATION_VERSION_MAJOR	  ) ||	 /* same major version */
				   ( (toVersion >> 16) == (APPLICATION_VERSION_MAJOR + 1) )) )	 /* major version is one higher */
			{
				result = softwareupdate_Parse_Result_Illegal_Pu_Sw_Version;
			}
		}
		break;
	case filedefinition_PuType_Sensor_Sub1: 		 /* allow all versions of the RF chip */
		break;
	case filedefinition_PuType_Erase1:				 /* do not allow erase images */
		result = softwareupdate_Parse_Result_Fail;
		break;
	default:
		result = softwareupdate_Parse_Result_Illegal_Pu_Sw_Version;
		break;
	}
	return result;
}

/******************************************************************************
* OVERLOADED FUNCTIONS
******************************************************************************/

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/
void appPreInitCallback(void)
{
    ;
}

int main( void )
{
    // Initialize the OSA including the OSA timers
    osa_Init( &s_heapConfig );

    ///< Initialize app callbacks
    zigbee_otaserver_RegisterCompleteCallback(cwpnode_SaveLightStatus);
    zigbeeStack_SetCMUClkHfxoCtune(PRODUCT_C4M_CLK_HFXO_CTUNE);
    zigbeeStack_SetMAXPowerLevel(PRODUCT_C4M_TX_POWER);
    zigbeeStack_Init();
    // Set preconditions for other init's
    cwpnode_PreInit();

    // Print reset source
    zigbeeStack_ResetInfo();

    // Initialize the osa timer service; if needed...
    osa_InitTimers( osa_TaskPrio_Medium, 128, 3 );

    // Initialize the packages
    cwpnode_Init();

    // make the ZigBee task runnable
    cwpnode_ZGBTaskTurnOn();

    // Make the event dispatcher task runnable
    evdisp_TurnOn();

    // Print the build information
    cwpnode_LogBuildInfo();

#ifdef HASEEPROM
    // a timer test to show how to use ctimer thread.
    nfc_Test();
#endif

    // Start OS scheduler and the event dispatcher task
    // This call should not return
    osa_SchedulerStart();

    return 0;
}

void registerLogPowerCb(void (*cb)(uint32_t u32Power))
{
    s_pfnLogPower = cb;
}
