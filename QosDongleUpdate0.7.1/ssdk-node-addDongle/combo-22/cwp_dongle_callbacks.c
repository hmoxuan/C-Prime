//

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
//#include "cwpnode_CommandHandler.h"
#include "log.h"

#include "zigbee_types.h"
#include "zigbeestack-api.h"
#include "command-id.h"
#include "cluster-id.h"

#include EMBER_AF_API_NETWORK_CREATOR
#include EMBER_AF_API_NETWORK_CREATOR_SECURITY
#include EMBER_AF_API_NETWORK_STEERING
#include EMBER_AF_API_FIND_AND_BIND_TARGET
#include EMBER_AF_API_ZLL_PROFILE

#include "zigbeestack-api.h"

#define LIGHT_ENDPOINT (64)

#define NODE_SEND_DEVICEADDRESS_DEALY_RANGE  200U
#define NODE_RECEIVE_DONGLE_REPLY_TIMEOUT  60U


EmberEventControl commissioningLedEventControl;
EmberEventControl findingAndBindingEventControl;
extern void PowerOnNetworkStatusUartSend(void);

extern void ZdpCmdIeeeAddrResponse(uint8_t* pIeeeAddr, uint16_t nodeId);
extern void ZdpCmdNwkAddrResponse(uint8_t* pIeeeAddr, uint16_t nodeId);


void commissioningLedEventHandler(void)
{
  emberEventControlSetInactive(commissioningLedEventControl);
  PowerOnNetworkStatusUartSend();
  
  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    uint16_t identifyTime;
    emberAfReadServerAttribute(LIGHT_ENDPOINT,
                               ZCL_IDENTIFY_CLUSTER_ID,
                               ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                               (uint8_t *)&identifyTime,
                               sizeof(identifyTime));
    if (identifyTime > 0) {
      halToggleLed(COMMISSIONING_STATUS_LED);
      emberEventControlSetDelayMS(commissioningLedEventControl,
                                  LED_BLINK_PERIOD_MS << 1);
    } else {
      halSetLed(COMMISSIONING_STATUS_LED);
    }
  } else {  
  #if 0
    EmberStatus status = emberAfPluginNetworkSteeringStart();
    emberAfCorePrintln("%p network %p: 0x%X", "Join", "start", status);
  //#else
    EmberStatus status = emberAfPluginNetworkCreatorStart(true); // centralized
    emberAfCorePrintln("%p network %p: 0x%X", "Form", "start", status);
  #endif
  
  } 
}

void findingAndBindingEventHandler()
{
  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    emberEventControlSetInactive(findingAndBindingEventControl);
    emberAfCorePrintln("Find and bind target start: 0x%X",
                       emberAfPluginFindAndBindTargetStart(LIGHT_ENDPOINT));
  }
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
    halClearLed(COMMISSIONING_STATUS_LED);
    setZllState(0, (EMBER_ZLL_STATE_FACTORY_NEW | EMBER_ZLL_STATE_PROFILE_INTEROP));
  } else if (status == EMBER_NETWORK_UP) {
    halSetLed(COMMISSIONING_STATUS_LED);
    setZllState(EMBER_ZLL_STATE_FACTORY_NEW, EMBER_ZLL_STATE_PROFILE_INTEROP);

    //status = emberAfPluginNetworkCreatorSecurityOpenNetwork();
    //emberAfCorePrintln("%p network %p: 0x%X", "Open", "for joining", status);
    emberEventControlSetActive(findingAndBindingEventControl);
  }

  // This value is ignored by the framework.
  return false;
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
  emberAfPluginNetworkCreatorGetChannelMask();
  emberEventControlSetActive(commissioningLedEventControl);
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

    status = emberAfPluginNetworkCreatorStart(true); // centralized
    emberAfCorePrintln("%p network %p: 0x%X", "Form", "start", status);
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
extern void NetworkCreatorCompleteUartSend(void);
void emberAfPluginNetworkCreatorCompleteCallback(const EmberNetworkParameters *network,
                                                 bool usedSecondaryChannels)
{
  emberAfCorePrintln("%p network %p: 0x%X",
                     "Form centralized",
                     "complete",
                     EMBER_SUCCESS);

  NetworkCreatorCompleteUartSend();
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
  // When the on/off attribute changes, set the LED appropriately.  If an error
  // occurs, ignore it because there's really nothing we can do.
  if (attributeId == ZCL_ON_OFF_ATTRIBUTE_ID) {
    bool onOff;
    if (emberAfReadServerAttribute(endpoint,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   (uint8_t *)&onOff,
                                   sizeof(onOff))
        == EMBER_ZCL_STATUS_SUCCESS) {
      if (onOff) {
        halSetLed(ON_OFF_LIGHT_LED);
      } else {
        halClearLed(ON_OFF_LIGHT_LED);
      }
    }
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
  if (state == BUTTON_RELEASED) {
    emberEventControlSetActive(findingAndBindingEventControl);
  }
}

void emberAfServiceDiscoveryCallback(const EmberAfServiceDiscoveryResult* result)
{
#if defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_APP)
  if (!emberAfHaveDiscoveryResponseStatus(result->status)) {
    // Do nothing
  } else if (result->zdoRequestClusterId == MATCH_DESCRIPTORS_REQUEST) {
    const EmberAfEndpointList* epList = (const EmberAfEndpointList*)result->responseData;
    emberAfCorePrintln("Match %py from 0x%2X, ep %d",
                      "discover",
                      result->matchAddress,
                      epList->list[0]);
  } else if (result->zdoRequestClusterId == NETWORK_ADDRESS_REQUEST) {
    emberAfCorePrintln("NWK Address response: 0x%2X", result->matchAddress);
	ZdpCmdNwkAddrResponse((uint8_t*)(result->responseData), result->matchAddress);
  } else if (result->zdoRequestClusterId == IEEE_ADDRESS_REQUEST) {
    uint8_t* eui64ptr = (uint8_t*)(result->responseData);
    uint16_t nodeId = result->matchAddress;
    ZdpCmdIeeeAddrResponse(eui64ptr, nodeId);
  }

  if (result->status != EMBER_AF_BROADCAST_SERVICE_DISCOVERY_RESPONSE_RECEIVED) {
    emberAfCorePrintln("Service %py done.", 
                      "discover");
  }
#endif //defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_APP)
}

extern void emDecrementPanConflictMeter(void);
void emberAfMainTickCallback(void)
{
    emDecrementPanConflictMeter();
}

void appSendMessageCallback(void)
{
    zigbeeStack_ZclCommand_Send();
}

void appGetNodeAddrList(void)
{
    int8_t status;
    zigbee_Address_t destAddress;
    uint16_t clusterId;
    uint8_t buf[10];
    uint8_t len = 0;
    buf[len++] = 0x15; //frametype: Disable Default Response(0x10) | client to server(0x00) | manufacture specific command(0x04) | specific command(0x01)
    buf[len++] = (uint8_t)EMBER_AF_PHILIPS_MANUFACTURER_CODE;
    buf[len++] = (uint8_t)(EMBER_AF_PHILIPS_MANUFACTURER_CODE>>8);
    buf[len++] = emberAfNextSequence();
    buf[len++] = ZCL_GET_NODE_DEVICE_ADDRESS_COMMAN_ID;
    buf[len++] = (uint8_t)NODE_SEND_DEVICEADDRESS_DEALY_RANGE;
    buf[len++] = (uint8_t)false;   //true:node reply with brocast, false:node reply with unicast
    buf[len++] = NODE_RECEIVE_DONGLE_REPLY_TIMEOUT;
    uint16_t srcEndpoint = 0x40;
    uint16_t dstEndpoint = 0x40;
    uint16_t txOptions = 0x1040; //(EMBER_APS_OPTION_RETRY|EMBER_APS_OPTION_ENABLE_ADDRESS_DISCOVERY)
    clusterId = ZCL_C4M_SPECIFIC_CLUSTER_ID;
    destAddress.mode = zigbee_AddrMode_Broadcast;
    destAddress.address.zAddress = 0xFFFF;
    status = zigbeeStack_ZclCommandCreate(clusterId, &buf[0], len);
    if(!status)
    {
        (void)zigbeeStack_ZclSend(&destAddress, clusterId, &buf[0], srcEndpoint, dstEndpoint, txOptions);
    }
}

