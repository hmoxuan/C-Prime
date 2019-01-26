//

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"

#include EMBER_AF_API_NETWORK_CREATOR
#include EMBER_AF_API_NETWORK_CREATOR_SECURITY
#include EMBER_AF_API_NETWORK_STEERING
#include EMBER_AF_API_FIND_AND_BIND_TARGET
#include EMBER_AF_API_ZLL_PROFILE

#include "zigbee_feedbackserver.h"
#include "zigbee_groupsserver.h"
#include "zigbee_commissioningattrserver.h"
#include "zigbee_lcddr_cluster.h"
#include "zigbee_lcfsm_cluster.h"
#include "zigbee_lightcontrolserver.h"
#include "zigbee_occupancysensingserver.h"
#include "zigbee_ballastconfigurationserver.h"
#include "zigbeestack-api.h"
#include "zcl_cmd_reponse.h"
#include "zigbeestack-callback-api.h"
#include "zigbeestack-ota-api.h"
#include "gatt_db.h"
#include "rtcdriver.h"
#include "cwp_node_BleCommand.h"


#define LIGHT_ENDPOINT (64)

#define REPORT_DELAY_TIME_SEC (600)
#define REPORT_PERIOD_SEC (600)
#define CAL_NEXT_REPORT_TIME_MS(u32sec, u32delayTime) ((u32sec + (u32delayTime % REPORT_DELAY_TIME_SEC))*1000)

/* Write response codes*/
#define ES_WRITE_OK                         0
#define ES_ERR_CCCD_CONF                    0x81
#define ES_ERR_PROC_IN_PROGRESS             0x80
#define ES_NO_CONNECTION                    0xFF
// Advertisement data
#define UINT16_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8))
#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
// Ble TX test macros and functions
#define BLE_TX_TEST_DATA_SIZE   2
// We need to put the device name into a scan response packet,
// since it isn't included in the 'standard' beacons -
// I've included the flags, since certain apps seem to expect them
#define DEVNAME "DMP%02X%02X"
#define DEVNAME_LEN 8  // incl term null
#define UUID_LEN 16 // 128-bit UUID
#define SOURCE_ADDRESS_LEN 8

#define LE_GAP_MAX_DISCOVERABLE_MODE   0x04
#define LE_GAP_MAX_CONNECTABLE_MODE    0x03
#define LE_GAP_MAX_DISCOVERY_MODE      0x02
#define BLE_INDICATION_TIMEOUT         30000
#define BUTTON_LONG_PRESS_TIME_MSEC    3000

#define DMP_DEBUG

EmberEventControl commissioningLedEventControl;
EmberEventControl findingAndBindingEventControl;
EmberEventControl touchlinkWindowEventControl;
EmberEventControl reportOnOffStatusEventControl;
EmberEventControl delayOnEventControl;
EmberEventControl networkDownEventControl;
EmberEventControl networkUpEventControl;
EmberEventControl delayOffWithEffectEventControl;

enum {DMP_LIGHT_OFF, DMP_LIGHT_ON}; // Light state
enum {HANDLE_DEMO, HANDLE_IBEACON, HANDLE_EDDYSTONE}; //advertisers handle

typedef enum {
  GAT_SERVER_CLIENT_CONFIG = 1,
  GAT_SERVER_CONFIRMATION,
}BLE_GAT_STATUS_FLAG;

typedef enum {
  GAT_DISABLED,
  GAT_RECEIVE_NOTIFICATION,
  GAT_RECEIVE_INDICATION,
}BLE_GAT_CLIENT_CONFIG_FLAG;

// {{{ copy from dmp-ui.h
typedef enum {
  DMP_UI_DIRECTION_BLUETOOTH,
  DMP_UI_DIRECTION_ZIGBEE,
  DMP_UI_DIRECTION_SWITCH,
  DMP_UI_DIRECTION_INVALID
}DmpUiLightDirection_t;
//}}}

static BLE_GAT_CLIENT_CONFIG_FLAG ble_lightState_config = GAT_DISABLED;
static BLE_GAT_CLIENT_CONFIG_FLAG ble_triggerSrc_config = GAT_DISABLED;
static BLE_GAT_CLIENT_CONFIG_FLAG ble_bleSrc_config = GAT_DISABLED;
static uint8_t ble_lightState = DMP_LIGHT_OFF;
static uint8_t ble_lastEvent = DMP_UI_DIRECTION_INVALID;
static uint8_t SourceAddress[SOURCE_ADDRESS_LEN];
static uint8_t SwitchEUI[SOURCE_ADDRESS_LEN];
static EmberEUI64 LightEUI;
static uint8_t ActiveBleConnections = 0;
static uint8_t lastButton;
static bool longPress = false;
static uint16_t bleNotificationsPeriodMs;
static DmpUiLightDirection_t lightDirection = DMP_UI_DIRECTION_INVALID;

struct {
  bool inUse;
  bool isMaster;
  uint8_t connectionHandle;
  uint8_t bondingHandle;
  uint8_t remoteAddress[6];
} bleConnectionTable[EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS];


/** GATT Server Attribute User Read Configuration.
 *  Structure to register handler functions to user read events. */
typedef struct {
  uint16 charId;                        /**< ID of the Characteristic. */
  void (*fctn)(uint8_t connection); /**< Handler function. */
} AppCfgGattServerUserReadRequest_t;

/** GATT Server Attribute Value Write Configuration.
 *  Structure to register handler functions to characteristic write events. */
typedef struct {
  uint16 charId;                        /**< ID of the Characteristic. */
  /**< Handler function. */
  void (*fctn)(uint8_t connection, uint8array * writeValue);
} AppCfgGattServerUserWriteRequest_t;
// iBeacon structure and data
static struct {
  uint8_t flagsLen;     /* Length of the Flags field. */
  uint8_t flagsType;    /* Type of the Flags field. */
  uint8_t flags;        /* Flags field. */
  uint8_t mandataLen;   /* Length of the Manufacturer Data field. */
  uint8_t mandataType;  /* Type of the Manufacturer Data field. */
  uint8_t compId[2];    /* Company ID field. */
  uint8_t beacType[2];  /* Beacon Type field. */
  uint8_t uuid[16];     /* 128-bit Universally Unique Identifier (UUID). The UUID is an identifier for the company using the beacon*/
  uint8_t majNum[2];    /* Beacon major number. Used to group related beacons. */
  uint8_t minNum[2];    /* Beacon minor number. Used to specify individual beacons within a group.*/
  uint8_t txPower;      /* The Beacon's measured RSSI at 1 meter distance in dBm. See the iBeacon specification for measurement guidelines. */
}
iBeaconData
  = {
  /* Flag bits - See Bluetooth 4.0 Core Specification , Volume 3, Appendix C, 18.1 for more details on flags. */
  2,  /* length  */
  0x01, /* type */
  0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */

  /* Manufacturer specific data */
  26,  /* length of field*/
  0xFF, /* type of field */

  /* The first two data octets shall contain a company identifier code from
   * the Assigned Numbers - Company Identifiers document */
  { UINT16_TO_BYTES(0x004C) },

  /* Beacon type */
  /* 0x0215 is iBeacon */
  { UINT16_TO_BYTE1(0x0215), UINT16_TO_BYTE0(0x0215) },

  /* 128 bit / 16 byte UUID - generated specially for the DMP Demo */
  { 0x00, 0x47, 0xe7, 0x0a, 0x5d, 0xc1, 0x47, 0x25, 0x87, 0x99, 0x83, 0x05, 0x44, 0xae, 0x04, 0xf6 },

  /* Beacon major number - not used for this application */
  { UINT16_TO_BYTE1(256), UINT16_TO_BYTE0(256) },

  /* Beacon minor number  - not used for this application*/
  { UINT16_TO_BYTE1(0), UINT16_TO_BYTE0(0) },

  /* The Beacon's measured RSSI at 1 meter distance in dBm */
  /* 0xC3 is -61dBm */
  // TBD: check?
  0xC3
  };

static struct {
  uint8_t flagsLen;          /**< Length of the Flags field. */
  uint8_t flagsType;         /**< Type of the Flags field. */
  uint8_t flags;             /**< Flags field. */
  uint8_t serLen;            /**< Length of Complete list of 16-bit Service UUIDs. */
  uint8_t serType;           /**< Complete list of 16-bit Service UUIDs. */
  uint8_t serviceList[2];    /**< Complete list of 16-bit Service UUIDs. */
  uint8_t serDataLength;     /**< Length of Service Data. */
  uint8_t serDataType;       /**< Type of Service Data. */
  uint8_t uuid[2];           /**< 16-bit Eddystone UUID. */
  uint8_t frameType;         /**< Frame type. */
  uint8_t txPower;           /**< The Beacon's measured RSSI at 0 meter distance in dBm. */
  uint8_t urlPrefix;         /**< URL prefix type. */
  uint8_t url[11];           /**< URL. */
} eddystone_data = {
  /* Flag bits - See Bluetooth 4.0 Core Specification , Volume 3, Appendix C, 18.1 for more details on flags. */
  2,  /* length  */
  0x01, /* type */
  0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */
  /* Service field length */
  0x03,
  /* Service field type */
  0x03,
  /* 16-bit Eddystone UUID */
  { UINT16_TO_BYTES(0xFEAA) },
  /* Eddystone-TLM Frame length */
  0x11,
  /* Service Data data type value */
  0x16,
  /* 16-bit Eddystone UUID */
  { UINT16_TO_BYTES(0xFEAA) },
  /* Eddystone-URL Frame type */
  0x10,
  /* Tx power */
  0x00,
  /* URL prefix - standard */
  0x00,
  /* URL */
  { 's', 'i', 'g', 'n', 'i', 'f', 'y', '.', 'c', 'o', 'm' }
};

static struct {
  uint16_t txDelayMs;
  uint8_t connHandle;
  uint16_t characteristicHandle;
  uint8_t size;
} bleTxTestParams;

struct responseData_t{
  uint8_t flagsLen;          /**< Length of the Flags field. */
  uint8_t flagsType;         /**< Type of the Flags field. */
  uint8_t flags;             /**< Flags field. */
  uint8_t shortNameLen;      /**< Length of Shortened Local Name. */
  uint8_t shortNameType;     /**< Shortened Local Name. */
  uint8_t shortName[DEVNAME_LEN]; /**< Shortened Local Name. */
  uint8_t uuidLength;        /**< Length of UUID. */
  uint8_t uuidType;          /**< Type of UUID. */
  uint8_t uuid[UUID_LEN];    /**< 128-bit UUID. */
};

static struct responseData_t responseData = {
  2,  /* length (incl type) */
  0x01, /* type */
  0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */
  DEVNAME_LEN + 1,        // length of local name (incl type)
  0x08,               // shortened local name
  { 'D', 'M', '0', '0', ':', '0', '0' },
  UUID_LEN + 1,           // length of UUID data (incl type)
  0x06,               // incomplete list of service UUID's
  // custom service UUID for silabs lamp in little-endian format
  { 0xc9, 0x1b, 0x80, 0x3d, 0x61, 0x50, 0x0c, 0x97, 0x8d, 0x45, 0x19, 0x7d, 0x96, 0x5b, 0xe5, 0xba }
};

typedef void (*NetworkJoinCompleteCb)(void);
NetworkJoinCompleteCb fnNetworkJoinCompleteCb = NULL;

typedef void (*EmberNetworkUpCb_t)(void);
static EmberNetworkUpCb_t s_pfnEmberNetworkUpCb = NULL;

extern bool emRadioGetRandomNumbers(uint16_t *rn, uint8_t count);
extern void ENERGYMETERING_TurnOff(void);
extern void emGpClearProxyTable(void);
extern int8_t zigbeeStack_SendCluster_command(zigbee_Address_t* destAddr, uint16_t clusterId, uint8_t* buf, uint8_t len);
extern void zigbee_lightcontrolserver_Set2AOCLevel(uint32_t transitionTime);
extern uint8_t zigbee_lightcontrolserver_GetOnOff( void );
extern void network_steering_cleanupAndStop(EmberStatus status);


static uint8_t bleConnectionInfoTableLookup(uint8_t connHandle)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS; i++) {
    if (bleConnectionTable[i].inUse
        && bleConnectionTable[i].connectionHandle == connHandle) {
      return i;
    }
  }
  return 0xFF;
}

/**
 * Custom CLI.  This command tree is executed by typing "custom <command>"
 * See app/util/serial/command-interpreter2.h for more detail on writing commands.
 **/
/*  Example sub-menu */
//  extern void doSomethingFunction(void);
//  static EmberCommandEntry customSubMenu[] = {
//    emberCommandEntryAction("do-something", doSomethingFunction, "", "Do something description"),
//    emberCommandEntryTerminator()
//  };
//  extern void actionFunction(void);

static const AppCfgGattServerUserReadRequest_t appCfgGattServerUserReadRequest[] =
{
  { gattdb_form_network, readZigbeeNetworkFormStatus },
  { gattdb_permit_join, readPermitJoinStatus },
  { gattdb_join_network, readJoinNetworkStatus },
  { gattdb_lighting_test, readLightCommand },
  { 0, NULL }
};

static const AppCfgGattServerUserWriteRequest_t appCfgGattServerUserWriteRequest[] =
{
  { gattdb_form_network, zigbeeNetworkFormCmd },
  { gattdb_permit_join, permitJoinCmd },
  { gattdb_join_network, JoinNetworkCmd },
  { gattdb_lighting_test, lightingControlCmd },
  { 0, NULL }
};

size_t appCfgGattServerUserReadRequestSize = COUNTOF(appCfgGattServerUserReadRequest) - 1;
size_t appCfgGattServerUserWriteRequestSize = COUNTOF(appCfgGattServerUserWriteRequest) - 1;

void static networkUpCallback(void)
{
  emberEventControlSetDelayMS(networkUpEventControl, 100);
}

void static registerNetworkUpCallback(void (*cb)(void))
{
    s_pfnEmberNetworkUpCb = cb;
}

static void eraseUserData(void)
{
    zigbee_Groups_Server_Callback.removeAllGroups(); // this function will remove all scene, too
    emberAfGroupsClusterClearGroupTableCallback(EMBER_BROADCAST_ENDPOINT);
    emGpClearProxyTable(); // when leaves network, clear GP proxy table as well
    emberAfCorePrintln("reset user data before join network");

	if(fn_zigbeeStack_EraseUserDataCb)
	{
        fn_zigbeeStack_EraseUserDataCb();
	}
}

void commissioningLedEventHandler(void)
{
  extern void afMainCommon_RegisterDeviceAnnounceCallback(void (*cb)(void));
  uint16_t u16delayTime = 0;
  emberEventControlSetInactive(commissioningLedEventControl);

  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    (void)emRadioGetRandomNumbers(&u16delayTime, 1);
    //emberEventControlSetDelayMS(reportOnOffStatusEventControl, CAL_NEXT_REPORT_TIME_MS(REPORT_PERIOD_SEC, u16delayTime));
    //emberAfCorePrintln("delay %d+%d sec to reportOnOffStatus", REPORT_PERIOD_SEC, u16delayTime%REPORT_DELAY_TIME_SEC);
  } else {
    uint8_t autojoin_enabled;
    eraseUserData(); // crease group/scene info
    registerNetworkUpCallback(networkUpCallback);
    halCommonGetToken(&autojoin_enabled, TOKEN_AUTOJOIN_ENABLED);
    if (autojoin_enabled)
    {
        EmberStatus status = emberAfPluginNetworkSteeringStart();
        emberAfCorePrintln("%p network %p: 0x%X", "Join", "start", status);
    }
    else
    {
        EmberStatus status = EMBER_SUCCESS;
        emberAfCorePrintln("%p network %p: 0x%X", "Join", "manually", status);
    }
  }
}

void findingAndBindingEventHandler()
{
  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    emberEventControlSetInactive(findingAndBindingEventControl);
  }
}

void reportOnOffStatusEventHandler(void)
{
  zigbee_Address_t destAddress;
  uint8_t buf[8];
  uint16_t u16delayTime = 0;
  uint8_t u8len = 0;

  if (emberNetworkState() == EMBER_JOINED_NETWORK)
  {
    buf[u8len++] = 0x18;//0x19; // frame control: disable default response(0x10) | server to client (0x8) | specific command (0x01)
    buf[u8len++] = emberAfNextSequence();
    buf[u8len++] = ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID;//ZCL_REPORT_NODE_ONOFF_STATUS_ID;
    buf[u8len++] = 0x00; // attribute id
    buf[u8len++] = 0x00; // attribute id
    buf[u8len++] = 0x00; // status
    buf[u8len++] = 0x10; // type: boolean
    buf[u8len++] = zigbee_lightcontrolserver_GetOnOff();

    destAddress.address.zAddress = 0x0000;
    destAddress.endpoint = 0x40;
    destAddress.mode = zigbee_AddrMode_Short;
    (void)zigbeeStack_SendCluster_command(&destAddress, ZCL_ON_OFF_CLUSTER_ID, &buf[0], u8len);
  }

  (void)emRadioGetRandomNumbers(&u16delayTime, 1);
  //emberEventControlSetDelayMS(reportOnOffStatusEventControl, CAL_NEXT_REPORT_TIME_MS(REPORT_PERIOD_SEC, u16delayTime));
  //emberAfCorePrintln("reportOnOffStatus event, delay %d+%d sec", REPORT_PERIOD_SEC, u16delayTime%REPORT_DELAY_TIME_SEC);
}

void delayOnEventHandler(void)
{
  extern void zigbee_lightcontrolserver_SetOnLevel(void);

  emberEventControlSetInactive(delayOnEventControl);
  zigbee_lightcontrolserver_SetOnLevel();
}

void delayOnCallback(uint32_t u32milsec)
{
  emberEventControlSetDelayMS(delayOnEventControl, u32milsec);
}

uint32_t OffWithEffectTimout100ms = 0;

void delayOffWithEffectOffEventHandler(void)
{
  extern void zigbee_lightcontrolserver_OffWithEffectControl(uint8_t level, uint16_t transitionTime);

  emberEventControlSetInactive(delayOffWithEffectEventControl);
  zigbee_lightcontrolserver_OffWithEffectControl(0,OffWithEffectTimout100ms);
}

void delayOffWithEffectOffCallback( uint8_t id, uint8_t var)
{

    uint16_t timeout;
    uint8_t level;
    if (id == 0)
    {
        if (var == 0)
        {
            //Fade to off in 0.8 seconds
            timeout = 8;
            level = 0;
            zigbee_lightcontrolserver_OffWithEffectControl(level, timeout);
        }
        else if (var == 0x01)
        {//No fade
            timeout = 0;
            level = 0;
            zigbee_lightcontrolserver_OffWithEffectControl(level, timeout);
        }
        else if (var == 0x02)
        {
            //50% dim down in 0.8 seconds then fade to off in 12 seconds
            timeout = 8;
            level = zigbee_lightcontrolserver_GetCurrentLevel() / 2;
            if (level == 0)
            {
                level = 1;
            }
            zigbee_lightcontrolserver_OffWithEffectControl(level, timeout);
            OffWithEffectTimout100ms = 120;
            emberEventControlSetDelayMS(delayOffWithEffectEventControl,800);
        }
    }
    else if (id == 1)
    {
        if (var == 0)
        {
            //20% dim up in 0.5s then fade to off in 1 second
            timeout = 5;
            level = zigbee_lightcontrolserver_GetCurrentLevel();
            level += level * 0.2;
            zigbee_lightcontrolserver_OffWithEffectControl(level, timeout);
            OffWithEffectTimout100ms = 10;
            emberEventControlSetDelayMS(delayOffWithEffectEventControl,500);
        }
    }

}

void networkDownEventHandler(void)
{
  #define EMBER_BROADCAST_ENDPOINT 0xFF
  #define TIME_UNIT_100MS  30

  emberEventControlSetInactive(networkDownEventControl);

  if (emberNetworkState() == EMBER_NO_NETWORK)
  {
    // only "leave network" command will run into here.
    // After EMBER_NETWORK_DOWN, stack task has a chance to run too long time before switching to metering task.
    // In that case, it will cause queue buf run out.
    ENERGYMETERING_TurnOff();

    zigbee_lightcontrolserver_Set2AOCLevel(TIME_UNIT_100MS);
    log_Printfbasic("Set light to aoc level");

    network_steering_cleanupAndStop(0x01/*EMBER_ERR_FATAL*/);
  }
}

void networkUpEventHandler(void)
{
  uint16_t u16delayTime = 0;

  emberEventControlSetInactive(networkUpEventControl);

  if (fnNetworkJoinCompleteCb)
  {
    fnNetworkJoinCompleteCb();
  }

  (void)emRadioGetRandomNumbers(&u16delayTime, 1);
  //emberEventControlSetDelayMS(reportOnOffStatusEventControl, CAL_NEXT_REPORT_TIME_MS(1800, u16delayTime));
  //emberAfCorePrintln("delay %d+%d sec to reportOnOffStatus", 1800, u16delayTime%REPORT_DELAY_TIME_SEC);
}

static void setZllState(uint16_t clear, uint16_t set)
{
  EmberTokTypeStackZllData token;
  emberZllGetTokenStackZllData(&token);
  token.bitmask &= ~clear;
  token.bitmask |= set;
  emberZllSetTokenStackZllData(&token);
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
bool emberAfStackStatusCallback(EmberStatus status)
{
  // Make sure to change the ZLL factory new state based on whether or not
  // we are on a network.
  if (status == EMBER_NETWORK_DOWN) {
    emberEventControlSetInactive(networkUpEventControl);
    setZllState(0, (EMBER_ZLL_STATE_FACTORY_NEW | EMBER_ZLL_STATE_PROFILE_INTEROP));
    emberEventControlSetDelayMS(networkDownEventControl, 3000); // delay 3 seconds, trigger network down event
  } else if (status == EMBER_NETWORK_UP) {
    emberEventControlSetInactive(networkDownEventControl);

    setZllState(EMBER_ZLL_STATE_FACTORY_NEW, EMBER_ZLL_STATE_PROFILE_INTEROP);

    //status = emberAfPluginNetworkCreatorSecurityOpenNetwork();
    //emberAfCorePrintln("%p network %p: 0x%X", "Open", "for joining", status);
    emberEventControlSetActive(findingAndBindingEventControl);

    if (s_pfnEmberNetworkUpCb)
    {
      s_pfnEmberNetworkUpCb();
    }
  }

  // This value is ignored by the framework.
  return false;
}

static void bleConnectionInfoTableInit(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS; i++) {
    bleConnectionTable[i].inUse = false;
  }
}

static void emAfPluginNetworkSteeringGetChannelMask(void)
{
	uint32_t primary_channel_mask = 0xFFFFFFFFUL, second_channel_mask = 0xFFFFFFFFUL;
	halCommonGetToken(&primary_channel_mask, TOKEN_NWK_STEERING_PRIMARY_CHANNEL_MASK);
	halCommonGetToken(&second_channel_mask, TOKEN_NWK_STEERING_SECONDARY_CHANNEL_MASK);
	if(primary_channel_mask == 0xFFFFFFFFUL)
	{
	  //emberAfDebugNetworkPrint("Use default emAfPluginNetworkSteeringPrimaryChannelMask");
	}
	else
	{
	  emAfPluginNetworkSteeringPrimaryChannelMask = primary_channel_mask;
	}

	if(second_channel_mask == 0xFFFFFFFFUL)
	{
	  //emberAfDebugNetworkPrint("Use default emAfPluginNetworkSteeringSecondaryChannelMask");
	}
	else
	{
	  emAfPluginNetworkSteeringSecondaryChannelMask = second_channel_mask;
	}
}


/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup.
 * Any code that you would normally put into the top of the application's
 * main() routine should be put into this function.
        Note: No callback
 * in the Application Framework is associated with resource cleanup. If you
 * are implementing your application on a Unix host where resource cleanup is
 * a consideration, we expect that you will use the standard Posix system
 * calls, including the use of atexit() and handlers for signals such as
 * SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal()
 * function to register your signal handler, please mind the returned value
 * which may be an Application Framework function. If the return value is
 * non-null, please make sure that you call the returned function from your
 * handler to avoid negating the resource cleanup of the Application Framework
 * itself.
 *
 */
void emberAfMainInitCallback(void)
{
  emAfPluginNetworkSteeringGetChannelMask();
#ifdef AUTO_START_COMMISSIONING
  emberEventControlSetActive(commissioningLedEventControl);
#endif
  bleConnectionInfoTableInit();
}

#ifndef AUTO_START_COMMISSIONING
void emberActivateCommissioningLedEventControl(void)
{
  emberEventControlSetActive(commissioningLedEventControl);
}
#endif

void emberAfRegisterJoinCompleteCallback(void (*cb)(void))
{
    fnNetworkJoinCompleteCb = cb;
}

/** @brief Complete
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  emberAfCorePrintln("%p network %p: 0x%X", "Join", "complete", status);

  if (status != EMBER_SUCCESS) {
    // Initialize our ZLL security now so that we are ready to be a touchlink
    // target at any point.
    status = emberAfZllSetInitialSecurityState();
    if (status != EMBER_SUCCESS) {
      emberAfCorePrintln("Error: cannot initialize ZLL security: 0x%X", status);
    }

    //status = emberAfPluginNetworkCreatorStart(false); // distributed
    //emberAfCorePrintln("%p network %p: 0x%X", "Form", "start", status);
  }
  else
  {
      //comment out below line according to new light behavior requirements.[2017-8-15]
      //zigbee_feedbackserver_Identify_Server_Callback.identify(3);
      #if 0
      if (fnNetworkJoinCompleteCb)
      {
        fnNetworkJoinCompleteCb();
      }

      uint32_t u32delayTime = 1000;
      u32delayTime = (uint8_t)(((uint32_t)(halCommonGetRandom() * u32delayTime)) >> 16);
      emberEventControlSetDelayMS(reportOnOffStatusEventControl, CAL_NEXT_REPORT_TIME_MS(1800, u32delayTime));
      emberAfCorePrintln("delay %d+%d sec to reportOnOffStatus", 1800, u32delayTime%REPORT_DELAY_TIME_SEC);
      #endif
  }
}

/** @brief Complete
 *
 * This callback notifies the user that the network creation process has
 * completed successfully.
 *
 * @param network The network that the network creator plugin successfully
 * formed. Ver.: always
 * @param usedSecondaryChannels Whether or not the network creator wants to
 * form a network on the secondary channels Ver.: always
 */
void emberAfPluginNetworkCreatorCompleteCallback(const EmberNetworkParameters *network,
                                                 bool usedSecondaryChannels)
{
  emberAfCorePrintln("%p network %p: 0x%X",
                     "Form distributed",
                     "complete",
                     EMBER_SUCCESS);
}

/** @brief Server Init
 *
 * On/off cluster, Server Init
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 */
void emberAfOnOffClusterServerInitCallback(uint8_t endpoint)
{
  // At startup, trigger a read of the attribute and possibly a toggle of the
  // LED to make sure they are always in sync.
  emberAfOnOffClusterServerAttributeChangedCallback(endpoint,
                                                    ZCL_ON_OFF_ATTRIBUTE_ID);
}

/** @brief Server Attribute Changed
 *
 * On/off cluster, Server Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfOnOffClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                       EmberAfAttributeId attributeId)
{
    void *   dataPtr = NULL;
    uint16_t shortAttribute = 0;
    uint8_t  byteAttribute = 0;

    switch (attributeId)
    {
    case ZIGBEE_ZCL_ONOFF_ATTRIBUTEID_ONOFF:
    case ZIGBEE_ZCL_ONOFF_ATTRIBUTEID_GLOBALSCENECONTROL:
        //READ ONLY
        break;
    case ZIGBEE_ZCL_ONOFF_ATTRIBUTEID_ONTIME:
    case ZIGBEE_ZCL_ONOFF_ATTRIBUTEID_OFFWAITTIME:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                                    ZCL_ON_OFF_CLUSTER_ID,
                                                                    attributeId,
                                                                    (uint8_t *)&shortAttribute,
                                                                    sizeof(shortAttribute)) )
        {
            dataPtr = (void*)&shortAttribute;
        }
        break;
    case ZIGBEE_ZCL_ONOFF_ATTRIBUTEID_STARTUPONOFF:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                                    ZCL_ON_OFF_CLUSTER_ID,
                                                                    attributeId,
                                                                    (uint8_t *)&byteAttribute,
                                                                    sizeof(byteAttribute)) )
        {
            dataPtr = (void*)&byteAttribute;
        }
        break;
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_onoffserver_WriteAttribute(endpoint, attributeId, dataPtr);
    }
}

void emberAfCommissioningClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                               EmberAfAttributeId attributeId)
{
    void *   dataPtr = NULL;
    uint8_t  extPanId[8] = {0};
    uint32_t channelMask = 0;
    uint8_t  startupControl = 0;

    switch (attributeId)
    {
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_EXTENDEDPANID:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                                    ZCL_COMMISSIONING_CLUSTER_ID,
                                                                    attributeId,
                                                                    (uint8_t *)&extPanId[0],
                                                                    sizeof(extPanId)) )
        {
            dataPtr = (void*)&extPanId[0];
        }
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_CHANNELMASK:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                                    ZCL_COMMISSIONING_CLUSTER_ID,
                                                                    attributeId,
                                                                    (uint8_t *)&channelMask,
                                                                    sizeof(channelMask)) )
        {
            dataPtr = (void*)&channelMask;
        }
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_STARTUPCONTROL:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                                    ZCL_COMMISSIONING_CLUSTER_ID,
                                                                    attributeId,
                                                                    (uint8_t *)&startupControl,
                                                                    sizeof(startupControl)) )
        {
            dataPtr = (void*)&startupControl;
        }
        break;
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_commissioningattrserver_WriteAttribute(endpoint, attributeId, dataPtr);
    }
}

void emberAfCommissioningClusterServerManufacturerSpecificAttributeChangedCallback(uint8_t endpoint,
                                                                                   EmberAfAttributeId attributeId,
                                                                                   uint16_t manufacturerCode)
{
    void *  dataPtr = NULL;
    bool_t  booleanAttribute = 0;
    uint8_t byteAttribute = 0;

    switch (attributeId)
    {
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_ROUTEREQUESTRETRANSMISSION:
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_BROADCASTRETRANSMISSION:
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_MULTICASTRETRANSMISSION:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadManufacturerSpecificServerAttribute(endpoint,
                                                                 ZCL_COMMISSIONING_CLUSTER_ID,
                                                                 attributeId,
                                                                 manufacturerCode,
                                                                 (uint8_t *)&booleanAttribute,
                                                                 sizeof(booleanAttribute)) )
        {
            dataPtr = (void*)&booleanAttribute;
        }
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_PASSIVEACKTHRESHOLD:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadManufacturerSpecificServerAttribute(endpoint,
                                                                 ZCL_COMMISSIONING_CLUSTER_ID,
                                                                 attributeId,
                                                                 manufacturerCode,
                                                                 &byteAttribute,
                                                                 sizeof(byteAttribute)) )
        {
            dataPtr = (void*)&byteAttribute;
        }
        break;
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_commissioningattrserver_WriteAttribute(endpoint, attributeId, dataPtr);
    }
}

void emberAfDaylightRegulationClusterServerManufacturerSpecificAttributeChangedCallback(uint8_t endpoint,
                                                                                        EmberAfAttributeId attributeId,
                                                                                        uint16_t manufacturerCode)
{
    void *   dataPtr = NULL;
    uint8_t  byteAttribute = 0;
    uint16_t shortAttribute = 0;

    switch (attributeId)
    {
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_DAYLIGHTREGULATIONENABLED:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_FADEINTERRUPTIONALLOWED:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_DAYLIGHTREGULATIONALGORITHM:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_CURRENTSETPOINT:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_MINIMUMDIMLEVEL:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_OFFSETESTIMATE:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadManufacturerSpecificServerAttribute(endpoint,
                                                                ZIGBEE_ZCL_CLUSTERID_DAYLIGHTREGULATION,
                                                                attributeId,
                                                                manufacturerCode,
                                                                (uint8_t *)&byteAttribute,
                                                                sizeof(byteAttribute)) )
        {
            dataPtr = (void*)&byteAttribute;
        }
        else
        {
            log_Printfbasic("FAILED TO READ ATTRIBUTE");
        }
        break;
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_CALIBRATEDSETPOINT:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_DEADBANDLOWLIMIT:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_DEADBANDHIGHLIMIT:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_FADEUPTIME:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_FADEDOWNTIME:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_ATTENUATIONDIMUP:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_ATTENUATIONDIMDOWN:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_MAXIMUMSTEPDIMUP:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_MAXIMUMSTEPDIMDOWN:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_MINIMUMSTEP:
    case ZIGBEE_ZCL_DAYLIGHTREGULATION_ATTRIBUTEID_DDRGAIN:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadManufacturerSpecificServerAttribute(endpoint,
                                                                ZIGBEE_ZCL_CLUSTERID_DAYLIGHTREGULATION,
                                                                attributeId,
                                                                manufacturerCode,
                                                                (uint8_t *)&shortAttribute,
                                                                sizeof(shortAttribute)) )
        {
            dataPtr = (void*)&shortAttribute;
        }
        break;
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_lcddr_WriteAttribute(attributeId, dataPtr);
    }
}

void emberAfLCFSMClusterServerManufacturerSpecificAttributeChangedCallback(uint8_t endpoint,
                                                                           EmberAfAttributeId attributeId,
                                                                           uint16_t manufacturerCode)
{
    void *  dataPtr = NULL;
    bool_t  booleanAttribute = 0;
    uint8_t byteAttribute = 0;

    switch (attributeId)
    {
    case ZIGBEE_ZCL_LCFSM_CLUSTER_ATTRIBUTEID_INITIALLIGHTSTATE:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadManufacturerSpecificServerAttribute(endpoint,
                                                                    ZIGBEE_ZCL_CLUSTERID_LIGHTCONTROLFSM,
                                                                    attributeId,
                                                                    manufacturerCode,
                                                                    (uint8_t *)&byteAttribute,
                                                                    sizeof(byteAttribute)) )
        {
            dataPtr = (void*)&byteAttribute;
        }
        break;
    case ZIGBEE_ZCL_LCFSM_CLUSTER_ATTRIBUTEID_LOCALOCCUPANCYREPORTING:
    case ZIGBEE_ZCL_LCFSM_CLUSTER_ATTRIBUTEID_AREALINKING:
    case ZIGBEE_ZCL_LCFSM_CLUSTER_ATTRIBUTEID_LOCALOCCUPANCYPROCESSING:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadManufacturerSpecificServerAttribute(endpoint,
                                                                    ZIGBEE_ZCL_CLUSTERID_LIGHTCONTROLFSM,
                                                                    attributeId,
                                                                    manufacturerCode,
                                                                    (uint8_t *)&booleanAttribute,
                                                                    sizeof(booleanAttribute)) )
        {
            dataPtr = (void*)&booleanAttribute;
        }
        break;
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_lcfsmserver_WriteAttribute(attributeId, dataPtr);
    }
}

void emberAfLevelControlClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                              EmberAfAttributeId attributeId)
{
    void *   dataPtr = NULL;
    uint8_t  byteAttribute = 0;
    uint16_t shortAttribute = 0;

    switch (attributeId)
    {
    case ZIGBEE_ZCL_LEVELCONTROL_ATTRIBUTEID_OPTIONS:
    case ZIGBEE_ZCL_LEVELCONTROL_ATTRIBUTEID_ONLEVEL:
    case ZIGBEE_ZCL_LEVELCONTROL_ATTRIBUTEID_STARTUPCURRENTLEVEL:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                        ZCL_LEVEL_CONTROL_CLUSTER_ID,
                                                        attributeId,
                                                        (uint8_t *)&byteAttribute,
                                                        sizeof(byteAttribute)) )
        {
            dataPtr = (void*)&byteAttribute;
        }
        break;
    case ZIGBEE_ZCL_LEVELCONTROL_ATTRIBUTEID_ONOFF_TRANSITION_TIME:
    case ZIGBEE_ZCL_LEVELCONTROL_ATTRIBUTEID_ON_TRANSITION_TIME:
    case ZIGBEE_ZCL_LEVELCONTROL_ATTRIBUTEID_OFF_TRANSITION_TIME:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                        ZCL_LEVEL_CONTROL_CLUSTER_ID,
                                                        attributeId,
                                                        (uint8_t *)&shortAttribute,
                                                        sizeof(shortAttribute)) )
        {
            dataPtr = (void*)&shortAttribute;
        }
        break;
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_levelcontrolserver_WriteAttribute(endpoint, attributeId, dataPtr);
    }
}

void emberAfBallastConfigurationClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                                      EmberAfAttributeId attributeId)
{
    void *   dataPtr = NULL;
    uint8_t  byteAttribute = 0;

    switch (attributeId)
    {
    case ZIGBEE_ZCL_BALLASTCONFIGURATION_ATTRIBUTEID_BALLASTSETTING_MIN_LEVEL:
    case ZIGBEE_ZCL_BALLASTCONFIGURATION_ATTRIBUTEID_BALLASTSETTING_MAX_LEVEL:
    case ZIGBEE_ZCL_BALLASTCONFIGURATION_ATTRIBUTEID_BALLASTSETTING_POWERON_LEVEL:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                        ZCL_BALLAST_CONFIGURATION_CLUSTER_ID,
                                                        attributeId,
                                                        (uint8_t *)&byteAttribute,
                                                        sizeof(byteAttribute)) )
        {
            dataPtr = (void*)&byteAttribute;
        }
        break;
    case ZIGBEE_ZCL_BALLASTCONFIGURATION_ATTRIBUTEID_LAMP_BURN_HOURS:
        //TODO
        break;
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_ballastconfigurationserver_WriteAttribute(endpoint, attributeId, dataPtr);
    }
}

/** @brief Hal Button Isr
 *
 * This callback is called by the framework whenever a button is pressed on the
 * device. This callback is called within ISR context.
 *
 * @param button The button which has changed state, either BUTTON0 or BUTTON1
 * as defined in the appropriate BOARD_HEADER.  Ver.: always
 * @param state The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED
 * if the button has been released.  Ver.: always
 */
void emberAfHalButtonIsrCallback(uint8_t button, uint8_t state)
{
    // we don't use button to trig the find and binding.
}

void emberIncomingCommandHandler(EmberZigbeeCommandType commandType,
                                 EmberMessageBuffer commandBuffer,
                                 uint8_t indexOfCommand,
                                 void *data)
{
  switch(commandType)
  {
    // Provides workaround to overly sensitive PAN ID Conflicts issue
    case EMBER_ZIGBEE_COMMAND_TYPE_RAW_MAC:
    {
#define MAC_FRAME_TYPE_BEACON (0)
#define MAC_FRAME_TYPE_MASK (0x0007)
      uint8_t packetLength = emberMessageBufferLength(commandBuffer);

      if(emberNetworkState() != EMBER_JOINED_NETWORK)
      {
        // Only want to filter out beacons when we're joined [not re/joining]
        break;
      }

      if( packetLength >= (indexOfCommand + 2))
      {
	    uint16_t frameControl;
	    frameControl = HIGH_LOW_TO_INT(emberGetLinkedBuffersByte(commandBuffer, indexOfCommand+1),
        emberGetLinkedBuffersByte(commandBuffer, indexOfCommand));
        if((frameControl & MAC_FRAME_TYPE_MASK) == MAC_FRAME_TYPE_BEACON)
        {
	      // Reject the packet
	      *((bool*)(data)) = false;
	      //emberAfDebugPrintln("Discarding frame as beacon");
        }
      }
      break;
    }
    default:
      // Ignore other cmd frame types
      break;
  }
}

void appGroupCmdCallback(uint8_t u8endpoint, uint8_t u8cmd)
{
    extern bool emberAfIsGroupTableEmpty(uint8_t endpoint);
    extern void zigbee_lightcontrolserver_SetLevel(uint8_t level, uint32_t transitionTime );

    switch(u8cmd)
    {
    case ZCL_REMOVE_GROUP_COMMAND_ID:
    case ZCL_REMOVE_ALL_GROUPS_COMMAND_ID:
    {
        if (emberAfIsGroupTableEmpty(u8endpoint))
        {
            // no groups exist in light, reset task level and dim to 100% (AOC level)
            zigbee_lightcontrolserver_Set2AOCLevel(10);
        }
        else
        {
            // dim to 100%
            zigbee_lightcontrolserver_SetLevel(254, 10);
        }
    }break;
    default:
    break;
    }
}

void ota_lightSwitchOffCallback(void)
{
    extern void zigbee_lightcontrolserver_SaveOnLevel(void);

    // store onlevel
    zigbee_lightcontrolserver_SaveOnLevel();
}

void appSendMessageCallback(void)
{
    zigbeeStack_OtaRspSendMessage();
    zigbeeStack_ZclCommand_Send();
}


void touchlinkWindowEventHandler(void)
{
    extern int8_t emZllRssiThreshold;

    emberEventControlSetInactive(touchlinkWindowEventControl);
    emZllRssiThreshold = 10; // set an impossible high threshold to disable touchlink
    emberAfCorePrintln("Disable touchlink");
}

void appTouchlinkWindowCallback(void)
{
    emberEventControlSetDelayMS(touchlinkWindowEventControl, 300000);   // 300 seconds
}

#if 0
void emberAfLevelControlClusterServerTickCallback(uint8_t endpoint)
{
    // dummy function
}
#endif

static void BeaconAdvertisements(uint16_t devId)
{
  static uint8_t *advData;
  static uint8_t advDataLen;

  iBeaconData.minNum[0] = UINT16_TO_BYTE1(devId);
  iBeaconData.minNum[1] = UINT16_TO_BYTE0(devId);

  advData = (uint8_t*)&iBeaconData;
  advDataLen = sizeof(iBeaconData);
  /* Set custom advertising data */
  gecko_cmd_le_gap_bt5_set_adv_data(HANDLE_IBEACON, 0, advDataLen, advData);
  gecko_cmd_le_gap_bt5_set_adv_parameters(HANDLE_IBEACON, 160, 160, 7, 0);
  gecko_cmd_le_gap_bt5_set_mode(HANDLE_IBEACON, le_gap_user_data, le_gap_non_connectable, 0, le_gap_non_resolvable);

  advData = (uint8_t*)&eddystone_data;
  advDataLen = sizeof(eddystone_data);
  /* Set custom advertising data */
  gecko_cmd_le_gap_bt5_set_adv_data(HANDLE_EDDYSTONE, 0, advDataLen, advData);
  gecko_cmd_le_gap_bt5_set_adv_parameters(HANDLE_EDDYSTONE, 160, 160, 7, 0);
  gecko_cmd_le_gap_bt5_set_mode(HANDLE_EDDYSTONE, le_gap_user_data, le_gap_non_connectable, 0, le_gap_non_resolvable);
}

static void enableBleAdvertisements(void)
{
  /* set transmit power to 0 dBm */
  gecko_cmd_system_set_tx_power(0);

  /* Create the device name based on the 16-bit device ID */
  uint16_t devId;
  struct gecko_msg_system_get_bt_address_rsp_t* btAddr;
  btAddr = gecko_cmd_system_get_bt_address();
  devId = *((uint16*)(btAddr->address.addr));

  // Copy to the local GATT database - this will be used by the BLE stack
  // to put the local device name into the advertisements, but only if we are
  // using default advertisements
  static char devName[DEVNAME_LEN];
  snprintf(devName, DEVNAME_LEN, DEVNAME, devId >> 8, devId & 0xff);
  emberAfCorePrintln("devName = %s", devName);
  gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name,
                                              0,
                                              strlen(devName),
                                              (uint8_t *)devName);
  //dmpUiSetBleDeviceName(devName);

  // Copy the shortened device name to the response data, overwriting
  // the default device name which is set at compile time
  MEMCOPY(((uint8_t*)&responseData) + 5, devName, 8);

  // Set the response data
  struct gecko_msg_le_gap_bt5_set_adv_data_rsp_t *rsp;
  rsp = gecko_cmd_le_gap_bt5_set_adv_data(HANDLE_DEMO, 0, sizeof(responseData), (uint8_t*)&responseData);
  (void)rsp;

  // Set nominal 100ms advertising interval, so we just get
  // a single beacon of each type
  gecko_cmd_le_gap_bt5_set_adv_parameters(HANDLE_DEMO, 160, 160, 7, 1);
  /* Start advertising in user mode and enable connections*/
  gecko_cmd_le_gap_bt5_set_mode(HANDLE_DEMO, le_gap_user_data, le_gap_undirected_connectable, 0, le_gap_identity_address);

  emberAfCorePrintln("BLE custom advertisements enabled");
  BeaconAdvertisements(devId);
}

static void printBleAddress(uint8_t *address)
{
  emberAfCorePrint("[%x %x %x %x %x %x]",
                   address[5], address[4], address[3],
                   address[2], address[1], address[0]);
}

//------------------------------------------------------------------------------
// BLE connection info table functions
static uint8_t bleConnectionInfoTableFindUnused(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS; i++) {
    if (!bleConnectionTable[i].inUse) {
      return i;
    }
  }
  return 0xFF;
}

static bool bleConnectionInfoTableIsEmpty(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS; i++) {
    if (bleConnectionTable[i].inUse) {
      return false;
    }
  }
  return true;
}

static void bleConnectionInfoTablePrintEntry(uint8_t index)
{
  assert(index < EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS
         && bleConnectionTable[index].inUse);
  emberAfCorePrintln("**** Connection Info index[%d]****", index);
  emberAfCorePrintln("connection handle 0x%x",
                     bleConnectionTable[index].connectionHandle);
  emberAfCorePrintln("bonding handle = 0x%x",
                     bleConnectionTable[index].bondingHandle);
  emberAfCorePrintln("local node is %s",
                     (bleConnectionTable[index].isMaster) ? "master" : "slave");
  emberAfCorePrint("remote address: ");
  printBleAddress(bleConnectionTable[index].remoteAddress);
  emberAfCorePrintln("");
  emberAfCorePrintln("*************************");
}

/** @brief
 *
 * This function is called at init time. The following fields will be
 * overwritten by the framework:
 *  - max_connections
 *  - heap
 *  - heap_size
 */
void emberAfPluginBleGetConfigCallback(gecko_configuration_t* config)
{
  // Change the BLE configuration here if needed
  config->bluetooth.max_advertisers = 3;
}

/** @brief
 *
 * This function is called from the BLE stack to notify the application of a
 * stack event.
 */
void emberAfPluginBleEventCallback(struct gecko_cmd_packet* evt)
{
  switch (BGLIB_MSG_ID(evt->header)) {
    /* This event indicates that a remote GATT client is attempting to read a value of an
     *  attribute from the local GATT database, where the attribute was defined in the GATT
     *  XML firmware configuration file to have type="user". */

    case gecko_evt_gatt_server_user_read_request_id:
      for (int i = 0; i < appCfgGattServerUserReadRequestSize; i++) {
        if ( (appCfgGattServerUserReadRequest[i].charId
              == evt->data.evt_gatt_server_user_read_request.characteristic)
             && (appCfgGattServerUserReadRequest[i].fctn) ) {
          appCfgGattServerUserReadRequest[i].fctn(evt->data.evt_gatt_server_user_read_request.connection);
        }
      }
      break;

    /* This event indicates that a remote GATT client is attempting to write a value of an
     * attribute in to the local GATT database, where the attribute was defined in the GATT
     * XML firmware configuration file to have type="user".  */

    case gecko_evt_gatt_server_user_write_request_id:
      for (int i = 0; i < appCfgGattServerUserWriteRequestSize; i++) {
        if ( (appCfgGattServerUserWriteRequest[i].charId
              == evt->data.evt_gatt_server_characteristic_status.characteristic)
             && (appCfgGattServerUserWriteRequest[i].fctn) ) {
          appCfgGattServerUserWriteRequest[i].fctn(evt->data.evt_gatt_server_user_read_request.connection,
                                                   &(evt->data.evt_gatt_server_attribute_value.value));
        }
      }
      break;

    case gecko_evt_system_boot_id:
      {
        struct gecko_msg_system_hello_rsp_t *hello_rsp;
        struct gecko_msg_system_get_bt_address_rsp_t *get_address_rsp;

        // Call these two APIs upon boot for testing purposes.
        hello_rsp = gecko_cmd_system_hello();
        get_address_rsp = gecko_cmd_system_get_bt_address();
        emberAfCorePrintln("BLE hello: %s",
                           (hello_rsp->result == bg_err_success) ? "success" : "error");
        emberAfCorePrint("BLE address: ");
        printBleAddress(get_address_rsp->address.addr);
        emberAfCorePrintln("");
        // start advertising
        enableBleAdvertisements();
        (void)hello_rsp;
      }
      break;
    case gecko_evt_gatt_server_characteristic_status_id:
    {
      bool isTimerRunning = 0;
      struct gecko_msg_gatt_server_characteristic_status_evt_t *StatusEvt =
        (struct gecko_msg_gatt_server_characteristic_status_evt_t*)&(evt->data);
      if (StatusEvt->status_flags == GAT_SERVER_CONFIRMATION) {
        emberAfCorePrintln("characteristic= %d , GAT_SERVER_CLIENT_CONFIG_FLAG = %d\r\n", StatusEvt->characteristic, StatusEvt->client_config_flags);

      } else if (StatusEvt->status_flags == GAT_SERVER_CLIENT_CONFIG) {

        emberAfCorePrintln("SERVER : ble_lightState_config= %d , ble_triggerSrc_config = %d , ble_bleSrc_config = %d\r\n", ble_lightState_config, ble_triggerSrc_config, ble_bleSrc_config);
      }
    }
    break;
    case gecko_evt_le_connection_opened_id:
      {
        emberAfCorePrintln("gecko_evt_le_connection_opened_id \n");
        struct gecko_msg_le_connection_opened_evt_t *conn_evt =
          (struct gecko_msg_le_connection_opened_evt_t*)&(evt->data);
        uint8_t index = bleConnectionInfoTableFindUnused();
        if (index == 0xFF) {
          emberAfCorePrintln("MAX active BLE connections");
          assert(index < 0xFF);
        } else {
          bleConnectionTable[index].inUse = true;
          bleConnectionTable[index].isMaster = (conn_evt->master > 0);
          bleConnectionTable[index].connectionHandle = conn_evt->connection;
          bleConnectionTable[index].bondingHandle = conn_evt->bonding;
          memcpy(bleConnectionTable[index].remoteAddress, conn_evt->address.addr, 6);

          ActiveBleConnections++;
          gecko_cmd_le_connection_set_phy(conn_evt->connection, 2);
          enableBleAdvertisements();
          emberAfCorePrintln("BLE connection opened");
          bleConnectionInfoTablePrintEntry(index);
          emberAfCorePrintln("%d active BLE connection", ActiveBleConnections);
        }
      }
      break;
    case gecko_evt_le_connection_phy_status_id:
    {
      // indicate the PHY that has been selected
      emberAfCorePrintln("now using the %dMPHY\r\n", evt->data.evt_le_connection_phy_status.phy);
    }
    break;
    case gecko_evt_le_connection_closed_id:
      {
        struct gecko_msg_le_connection_closed_evt_t *conn_evt =
          (struct gecko_msg_le_connection_closed_evt_t*)&(evt->data);
        uint8_t index = bleConnectionInfoTableLookup(conn_evt->connection);
        assert(index < 0xFF);

        bleConnectionTable[index].inUse = false;

        --ActiveBleConnections;
        // restart advertising, set connectable
        enableBleAdvertisements();
        if (bleConnectionInfoTableIsEmpty()) {
          //dmpUiBluetoothConnected(false);
        }
        emberAfCorePrintln("BLE connection closed, handle=0x%x, reason=0x%2x : [%d] active BLE connection",
                           conn_evt->connection, conn_evt->reason, ActiveBleConnections);
      }
      break;
    case gecko_evt_le_gap_scan_response_id:
      {
        struct gecko_msg_le_gap_scan_response_evt_t *scan_evt =
          (struct gecko_msg_le_gap_scan_response_evt_t*)&(evt->data);
        emberAfCorePrint("Scan response, address type=0x%x, address: ",
                         scan_evt->address_type);
        printBleAddress(scan_evt->address.addr);
        emberAfCorePrintln("");
      }
      break;
    case gecko_evt_sm_list_bonding_entry_id:
      {
        struct gecko_msg_sm_list_bonding_entry_evt_t * bonding_entry_evt =
          (struct gecko_msg_sm_list_bonding_entry_evt_t*)&(evt->data);
        emberAfCorePrint("Bonding handle=0x%x, address type=0x%x, address: ",
                         bonding_entry_evt->bonding, bonding_entry_evt->address_type);
        printBleAddress(bonding_entry_evt->address.addr);
        emberAfCorePrintln("");
      }
      break;
    case gecko_evt_gatt_service_id:
      {
        struct gecko_msg_gatt_service_evt_t* service_evt =
          (struct gecko_msg_gatt_service_evt_t*)&(evt->data);
        uint8_t i;
        emberAfCorePrintln("GATT service, conn_handle=0x%x, service_handle=0x%4x",
                           service_evt->connection, service_evt->service);
        emberAfCorePrint("UUID=[");
        for (i = 0; i < service_evt->uuid.len; i++) {
          emberAfCorePrint("0x%x ", service_evt->uuid.data[i]);
        }
        emberAfCorePrintln("]");
      }
      break;
    case gecko_evt_gatt_characteristic_id:
      {
        struct gecko_msg_gatt_characteristic_evt_t* char_evt =
          (struct gecko_msg_gatt_characteristic_evt_t*)&(evt->data);
        uint8_t i;
        emberAfCorePrintln("GATT characteristic, conn_handle=0x%x, char_handle=0x%2x, properties=0x%x",
                           char_evt->connection, char_evt->characteristic, char_evt->properties);
        emberAfCorePrint("UUID=[");
        for (i = 0; i < char_evt->uuid.len; i++) {
          emberAfCorePrint("0x%x ", char_evt->uuid.data[i]);
        }
        emberAfCorePrintln("]");
      }
      break;
    case gecko_evt_gatt_characteristic_value_id:
      {
        struct gecko_msg_gatt_characteristic_value_evt_t* char_evt =
          (struct gecko_msg_gatt_characteristic_value_evt_t*)&(evt->data);
        uint8_t i;

        if (char_evt->att_opcode == gatt_handle_value_indication) {
          gecko_cmd_gatt_send_characteristic_confirmation(char_evt->connection);
        }
        emberAfCorePrintln("GATT (client) characteristic value, handle=0x%x, characteristic=0x%2x, att_op_code=0x%x",
                           char_evt->connection,
                           char_evt->characteristic,
                           char_evt->att_opcode);
        emberAfCorePrint("value=[");
        for (i = 0; i < char_evt->value.len; i++) {
          emberAfCorePrint("0x%x ", char_evt->value.data[i]);
        }
        emberAfCorePrintln("]");
      }
      break;
    case gecko_evt_gatt_server_attribute_value_id:
    {
      struct gecko_msg_gatt_server_attribute_value_evt_t* attr_evt =
        (struct gecko_msg_gatt_server_attribute_value_evt_t*)&(evt->data);
      emberAfCorePrintln("GATT (server) attribute value, handle=0x%x, attribute=0x%2x, att_op_code=0x%x",
                         attr_evt->connection,
                         attr_evt->attribute,
                         attr_evt->att_opcode);

        for (int i = 0; i < appCfgGattServerUserWriteRequestSize; i++)
        {
            if ( (appCfgGattServerUserWriteRequest[i].charId == attr_evt->attribute)
                 && (appCfgGattServerUserWriteRequest[i].fctn) ) {
              appCfgGattServerUserWriteRequest[i].fctn(attr_evt->connection,
                                                       &(attr_evt->value));
            }
          }
    }
    break;
    case gecko_evt_le_connection_parameters_id:
      {
        struct gecko_msg_le_connection_parameters_evt_t* param_evt =
          (struct gecko_msg_le_connection_parameters_evt_t*)&(evt->data);
        emberAfCorePrintln("BLE connection parameters are updated, handle=0x%x, interval=0x%2x, latency=0x%2x, timeout=0x%2x, security=0x%x, txsize=0x%2x",
                           param_evt->connection,
                           param_evt->interval,
                           param_evt->latency,
                           param_evt->timeout,
                           param_evt->security_mode,
                           param_evt->txsize);
        //dmpUiBluetoothConnected(true);
      }
      break;
    case gecko_evt_gatt_procedure_completed_id:
      {
        struct gecko_msg_gatt_procedure_completed_evt_t* proc_comp_evt =
          (struct gecko_msg_gatt_procedure_completed_evt_t*)&(evt->data);
        emberAfCorePrintln("BLE procedure completed, handle=0x%x, result=0x%2x",
                           proc_comp_evt->connection,
                           proc_comp_evt->result);
      }
      break;
    default:
      break;
  }
}

bool emberAfPluginIdleSleepRtosCallback(uint32_t *durationMs, bool sleepOk)
{
  uint32_t actualDurationMs = *durationMs;
  uint32_t deltaMs, startTicks = RTCDRV_GetWallClockTicks32();
  OS_TICK yieldTimeTicks = (OSCfg_TickRate_Hz * actualDurationMs) / 1000;
  OS_ERR err;
  CPU_TS ts;

  if (!sleepOk) {
    // TODO: disable deep sleep at the sleep manager.
    //rtosBlockDeepSleep();
  }

  INTERRUPTS_ON();

  // Yield the stack task.
  OSTaskSemPend(yieldTimeTicks, OS_OPT_PEND_BLOCKING, &ts, &err);

  if (!sleepOk) {
    // TODO: re-enable deep sleep at the sleep manager.
    //if (rtosGetDeepSleepBlockCount() > 0) {
    //  rtosUnblockDeepSleep();
    //}
  }

  deltaMs = RTCDRV_TicksToMsec(RTCDRV_GetWallClockTicks32() - startTicks);
  if ( deltaMs <= actualDurationMs ) {
    *durationMs = actualDurationMs - deltaMs;
  } else {
    *durationMs = 0;
  }

  return true;
}

bool emberRtosIdleHandler(uint32_t *idleTimeMs)
{
  return emberAfPluginIdleSleepRtosCallback(idleTimeMs, false);
}

void emberAfOccupancySensingClusterClientAttributeChangedCallback(uint8_t endopint,
                                                                             EmberAfAttributeId attributeId)
{
//TODO
}

void emberAfOccupancySensingClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                                  EmberAfAttributeId attributeId)
{
    void *   dataPtr = NULL;
    uint16_t shortAttribute = 0;
    uint8_t  byteAttribute = 0;

    switch (attributeId)
    {
    case ZIGBEE_ZCL_OCCUPANCYSENSING_ATTRIBUTEID_PIROCCTOUNOCCDELAY:
    case ZIGBEE_ZCL_OCCUPANCYSENSING_ATTRIBUTEID_PIRUNOCCTOOCCDELAY:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                                    ZCL_OCCUPANCY_SENSING_CLUSTER_ID,
                                                                    attributeId,
                                                                    (uint8_t *)&shortAttribute,
                                                                    sizeof(shortAttribute)) )
        {
            dataPtr = (void*)&shortAttribute;
        }
        break;
    case ZIGBEE_ZCL_OCCUPANCYSENSING_ATTRIBUTEID_PIRUNOCCTOOCCTHRESHOLD:
        if ( EMBER_ZCL_STATUS_SUCCESS == emberAfReadServerAttribute(endpoint,
                                                                    ZCL_OCCUPANCY_SENSING_CLUSTER_ID,
                                                                    attributeId,
                                                                    (uint8_t *)&byteAttribute,
                                                                    sizeof(byteAttribute)) )
        {
            dataPtr = (void*)&byteAttribute;
        }
        break;
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_occupancysensingserver_WriteAttribute(endpoint, attributeId, dataPtr);
    }
}

void emberAfOccupancySensingClusterServerManufacturerSpecificAttributeChangedCallback(uint8_t endpoint,
                                                                                      EmberAfAttributeId attributeId,
                                                                                      uint16_t manufacturerCode)
{
#if 1
    (void)endpoint;
    (void)attributeId;
    (void)manufacturerCode;
#else
    void * dataPtr = NULL;

    switch (attributeId)
    {
    default:
        break;
    }
    if (dataPtr != NULL)
    {
        zigbee_occupancysensingserver_WriteAttribute(endpoint, attributeId, dataPtr);
    }
#endif
}

void emberStackTaskYield(void)
{
  OS_ERR err;
  CPU_TS ts;

  OSTaskSemPend(1, OS_OPT_PEND_BLOCKING, &ts, &err);
}
