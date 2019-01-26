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
* INCLUDE FILES
******************************************************************************/

#include PLATFORM_HEADER


// This source file only exists to link-in the aat table
#include "zigbeestack-api.h"

#include "emlib.h"

#include "app/framework/include/af.h"
#include "app/framework/util/attribute-storage.h"
#include "app/util/serial/command-interpreter2.h"

// ZDO
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "app/util/zigbee-framework/zigbee-device-library.h"

#include "app/util/counters/counters.h"
#include "app/util/security/security.h"
#include "app/util/common/form-and-join.h"

#include "app/framework/util/service-discovery.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/util.h"

#include "app/framework/security/af-security.h"
#include "app/framework/security/crypto-state.h"

#include "app/framework/plugin/partner-link-key-exchange/partner-link-key-exchange.h"
#include "app/framework/plugin/fragmentation/fragmentation.h"
#include "network-creator.h"
#include "app/framework/plugin/network-creator-security/network-creator-security.h"
#include "app/framework/plugin/zll-commissioning/zll-commissioning.h"

#include "afv2-bookkeeping.h"

#include "cwp_dongle_callback.h"

#include "hal/micro/micro.h"
#include "app/framework/plugin/green-power-common/green-power-common.h"
#include "app/framework/plugin/green-power-server/green-power-server.h"

#include "stack/gp/gp-proxy-table.h"



//#include "log.h"
#include "osa.h"

#include "hal-config.h"

#include "gatt_db.h"

/******************************************************************************
* LOCAL MACROS AND DEFINITIONS
******************************************************************************/

#define SPIFLASHDISK_MUTEX_TIMEOUT (5000U)

extern void internalflash_EraseAll(void);
extern void emberAfZllResetToFactoryNew(void);
extern void zclBufferAddByte(uint8_t byte);
extern void zclBufferSetup(uint8_t frameType, uint16_t clusterId, uint8_t commandId);


extern uint8_t emberAfSequenceNumber;
extern EmberApsFrame globalApsFrame;
extern uint8_t appZclBuffer[];
extern uint16_t appZclBufferLen;
extern bool zclCmdIsBuilt;
extern bool zclCmdIsBuilt;
extern uint16_t mfgSpecificId;
extern uint8_t disableDefaultResponse;
extern uint16_t openNetworkTimeRemainingS;


extern void zigbee_zgpdinfo_Save(uint32_t gpdSrcId, uint8_t gpdDeviceId, uint8_t *key, uint16_t alias, uint16_t groupId);
extern bool_t zigbee_zgpdinfo_Load(uint32_t gpdSrcId, uint8_t *gpdDeviceId, uint8_t *key, uint16_t *alias, uint16_t * groupId);
extern void zigbee_zgpdinfo_Clear(void);
extern void zigbee_zgpdinfo_Delete(uint32_t gpdSrcId);
extern int8_t emberAfSetMaxPowerLevel(int8_t maxPowerLevel);

/******************************************************************************
* LOCAL TYPES
******************************************************************************/

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/

/******************************************************************************
* LSB FUNCTION IMPLEMENTATIONS
******************************************************************************/

/******************************************************************************
* LOCAL CONSTANTS
******************************************************************************/

/******************************************************************************
* LOCAL VARIABLES
******************************************************************************/
static uint16_t s_nextGroupId = 0xffff;
//static uint16_t s_nextGroupAlias = 0xffff;
static uint16_t s_nextAssignedAlias = 0xffff;

#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY) && defined(CWP_NODE)
static uint16_t s_advertizer_intervalInMillseconds = 0;
static uint8_t s_advertizer_channel_map = 0;
static bool s_advertizer_isEnabled = false;
#endif

/******************************************************************************
* MAIN EVENT HANDLER
******************************************************************************/



/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/
static void zigbeeStack_GPCommissioningNotification(uint32_t gpdSrcId, uint8_t gpdDeviceId, uint8_t *key);
static void zigbeeStack_GPDeviceInfo(uint32_t gpdSrcId, uint8_t *gpdCommandPayload, uint8_t gpdCommandPayloadLength);

#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY) && defined(CWP_NODE)
static void enableBeaconAdvertisements(uint16_t intervalInMillseconds, uint8_t channel_map, int16 tx_power);
static void disableBeaconAdvertisements(void);
static char* bcnAdvertisingChannels(void);
#endif


/******************************************************************************
* EXPORTED VARIABLES
******************************************************************************/
const stackApiCallbackConfig_t stackApiCallbackConfig =
{
    .gpCommissioningNotificationCallback = zigbeeStack_GPCommissioningNotification,
    .gpDeviceInfoCallback = zigbeeStack_GPDeviceInfo
};


/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/
#define GPD_DUP_TBL_SIZE   (3) // 3 entries for 1 GP commissioning should be enough
static uint32_t gpdDupTable[GPD_DUP_TBL_SIZE];
static void resetDuplicateGpdTable()
{
    memset(gpdDupTable, 0, sizeof(gpdDupTable));
}

static bool_t isDuplicateGPD(uint32_t gpdSrcId)
{
    uint8_t firstEmpty = 0xFF;
    for ( uint8_t i = 0; i < GPD_DUP_TBL_SIZE; ++ i)
    {
        if ((firstEmpty == 0xFF) && (gpdDupTable[i] == 0))
        {
            firstEmpty = i;
        }
        if (gpdDupTable[i] == gpdSrcId )
        {
            return true;
        }
    }
    if (firstEmpty != 0xFF)
    {
        gpdDupTable[firstEmpty] = gpdSrcId;
    }
    return false;
}

int8_t zigbeeStack_TouchLinkAllow(void)
{
    return 0;
}

int8_t zigbeeStack_TouchLinkDisallow(void)
{
    return 0;
}

int8_t zigbeeStack_TouchLink(uint16_t* pNwkaddr, uint16_t* pPanid)
{
    return 0;
}

int8_t zigbeeStack_SendApsTransportNetworkKey(uint16_t nwkAddress,uint8_t* pIeeeAddr,uint16_t parentNwkAddres)
{
    return 0;
}

int8_t zigbeeStack_FindFreePan(uint16_t* pNwkPanId, uint8_t * pNwkExtPanId)
{
    return 0;
}

int8_t zigbeeStack_JoinNetwork(uint16_t nwkPanId, uint8_t* pNwkExtPanId,uint8_t nwkChannel,uint8_t* pNwkUpdateId)
{
    return 0;
}

int8_t zigbeeStack_Associate(uint8_t* pNwkExtPanId)
{
    return 0;
}

int8_t zigbeeStack_GetAddress(uint8_t* pIeeeAddr,uint16_t* pShortAddr,uint8_t* pEndpoint)
{
    memcpy(pIeeeAddr, emberGetEui64(),EUI64_SIZE);
    *pShortAddr = emberGetNodeId();
    *pEndpoint = emberGetEndpoint(0);
    return 0;
}

uint8_t zigbeeStack_GetNetwrokState(void)
{
	return emberNetworkState();
}

uint8_t zigbeeStack_NetworkSteeringStop(void)
{
	return emberAfPluginNetworkSteeringStop();
}

uint8_t zigbeeStack_SetShortAddress(uint16_t addr)
{
    return emberSetNodeId(addr);
}

int8_t zigbeeStack_GetExtendedPanId(uint8_t* pNwkExtPanId)
{
	emberGetExtendedPanId(pNwkExtPanId);

    return 0;
}

int8_t zigbeeStack_GetChannel(uint8_t* pNwkChannel)
{
	EmberNetworkParameters parameters;
	emberGetNetworkParameters(&parameters);
	*pNwkChannel = parameters.radioChannel;
    return 0;
}

int8_t zigbeeStack_ChannelChange(uint8_t nwkChannel)
{
	return emberChannelChangeRequest(nwkChannel);
}

bool zigbeeStack_IsCentralizedNetwork(void)
{
    return getFormCentralizedNetwork();
}

int8_t zigbeeStack_StartCoordinator(uint16_t nwkPanId,uint8_t* pNwkExtPanId,uint8_t nwkChannel)
{
/*
       EmberStatus status;
       EmberNetworkParameters networkParams;
       emberGetNetworkParameters(&networkParams);

      // TODO: factory new the dongle in network level
     //emberLeaveNetwork();

       memcpy(networkParams.extendedPanId, pNwkExtPanId, 8);
       emberAfGetFormAndJoinExtendedPanIdCallback(networkParams.extendedPanId);
       networkParams.radioChannel = nwkChannel;
       networkParams.radioTxPower = emberGetRadioPower();
       networkParams.panId = nwkPanId;

       status = emberAfFormNetwork(&networkParams);
       emberAfAppFlush();
*/
	emberAfZllResetToFactoryNew();

    EmberStatus status = emberAfPluginNetworkCreatorStart(true);
    return (int8_t)status;
}

int8_t zigbeeStack_StartCoordinator2(uint16_t nwkPanId,uint8_t* pNwkExtPanId,uint8_t nwkChannel)
{
	emberAfZllResetToFactoryNew();

    EmberStatus status = emberAfPluginNetworkCreatorStart(false);
    return (int8_t)status;
}

int8_t zigbeeStack_GetUpdatedId(uint8_t* pUpdateId)
{
	EmberNetworkParameters networkParams;
	emberGetNetworkParameters(&networkParams);
    *pUpdateId = networkParams.nwkUpdateId;

    return 0;
}

int8_t zigbeeStack_GPSetAssignedAlias(uint16_t alias)
{
    s_nextAssignedAlias = alias;
    return 0;
}

int8_t zigbeeStack_GPSetGroupId(uint16_t group)
{
    s_nextGroupId = group;
    //s_nextGroupAlias = group;
    return 0;
}

int8_t zigbeeStack_GPCommissioningEnter(bool exitOnPairing, uint16_t timeoutS)
{
    resetDuplicateGpdTable();
    uint8_t options =
      EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_ACTION
      | EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_PROXIES;
    if ( emberAfGreenPowerClusterGpSinkCommissioningModeCallback(options, 0xffff, 0xffff,0xff) )
        return 0;
    else
        return 1;
}

int8_t zigbeeStack_GPCommissioningExit(void)
{
    uint8_t options =
      EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_PROXIES;
    if ( emberAfGreenPowerClusterGpSinkCommissioningModeCallback(options, 0xffff, 0xffff,0xff) )
        return 0;
    else
        return 1;
}

int8_t zigbeeStack_GPAddPairing(zigbee_Address_t* pDestinationAddr,uint32_t gpdSrcId)
{
    uint8_t gpdKey[EMBER_ENCRYPTION_KEY_SIZE];
    uint8_t gpdDeviceId = 0;
    uint16_t alias = 0;
    uint16_t groupId = 0;

    if (false == zigbee_zgpdinfo_Load(gpdSrcId, &gpdDeviceId, gpdKey, &alias, &groupId))
    {
        return -1;
    }
    if ((s_nextGroupId != 0xFFFF) && (groupId != s_nextGroupId))
    // this happens when user changes groupId before deploying
    // because pairing is always sent before pairing configuration
    // so only need to update groupid once here
    {
        zigbee_zgpdinfo_Save(gpdSrcId, gpdDeviceId, gpdKey, alias, s_nextGroupId);
        groupId = s_nextGroupId;
    }

    uint32_t pairingOptions =
    EMBER_AF_GP_PAIRING_OPTION_ADD_SINK
    | (EMBER_GP_SINK_TYPE_GROUPCAST << 5)
    | (EMBER_GP_SECURITY_LEVEL_FC_MIC << 9)
    | (EMBER_GP_SECURITY_KEY_GPD_OOB << 11)
    | EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT
    | EMBER_AF_GP_PAIRING_OPTION_ASSIGNED_ALIAS_PRESENT
    | EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_KEY_PRESENT;

    emberAfFillCommandGreenPowerClusterGpPairingSmart(pairingOptions, // uint32_t options
                                                      gpdSrcId, // uint32_t gpdSrcId
                                                      NULL, // uint8_t *gpdIeee
                                                      242, // uint8_t endpoint
                                                      0, // uint8_t *sinkIeeeAddress
                                                      emberGetNodeId(), // uint16_t sinkNwkAddress
                                                      groupId, // uint16_t sinkGroupId
                                                      gpdDeviceId, // uint8_t deviceId
                                                      0, // uint32_t gpdSecurityFrameCounter
                                                      gpdKey, // uint8_t *gpdKey
                                                      alias, // uint16_t assignedAlias
                                                      0xff // uint8_t forwardingRadius
                                                      );

    EmberApsFrame *apsFrame = emberAfGetCommandApsFrame();
    apsFrame->sourceEndpoint = GP_ENDPOINT;
    apsFrame->destinationEndpoint = GP_ENDPOINT;
    emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, pDestinationAddr->address.zAddress);
    return 0;
}

static void zigbeeStack_GPCommissioningNotification(uint32_t gpdSrcId, uint8_t gpdDeviceId, uint8_t *key)
{
    if (isDuplicateGPD(gpdSrcId))
    {
        return;
    }
    zigbee_zgpdinfo_Save(gpdSrcId, gpdDeviceId, key, s_nextAssignedAlias, s_nextGroupId);
    if (scipConfig.gpCommissioningNotification)
    {
        scipConfig.gpCommissioningNotification(gpdSrcId, gpdDeviceId);
    }
}

static void zigbeeStack_GPDeviceInfo(uint32_t gpdSrcId, uint8_t *payload, uint8_t payloadLength)
{
    uint16_t attrId = 0;
    uint8_t attrType = 0;
    uint16_t fwVersion = 0;
    uint16_t hwVersion = 0;
    uint16_t stackVersion = 0;

    // for GP occupancy sensor
    for (uint8_t i = 0; i < payloadLength;)
    {
        attrId = payload[i++];
        attrId |=  payload[i++] << 8;
        if (attrId == 0x0001) // Application Version
        {
            attrType = payload[i++];
            if (attrType == 0x20 && (payloadLength-i) >= 1)
            {
                fwVersion = payload[i++];
            }
        }
        if (attrId == 0x0002) // Stack Version
        {
            attrType = payload[i++];
            if (attrType == 0x20 && (payloadLength-i) >= 1)
            {
                stackVersion = payload[i++];
            }
        }
        if (attrId == 0x0003) // HW Version
        {
            attrType = payload[i++];
            if (attrType == 0x20 && (payloadLength-i) >= 1)
            {
                hwVersion = payload[i++];
            }
        }
    }
    if (scipConfig.gpDeviceInfo)
    {
        scipConfig.gpDeviceInfo(gpdSrcId, fwVersion,hwVersion,stackVersion);
    }
}

int8_t zigbeeStack_GPRemovePairing(zigbee_Address_t* pDestinationAddr, uint32_t gpdSrcId)
{
    return 0;
}

int8_t zigbeeStack_GPGetEntryForIndex(uint8_t index)
{
    return 0;
}

int8_t zigbeeStack_GPEraseAll(void)
{
    emGpClearProxyTable();
    zigbee_zgpdinfo_Clear();
    return 0;
}

int8_t zigbeeStack_GPErase(uint32_t gpdSrcId)
{
    zigbee_zgpdinfo_Delete(gpdSrcId);
    return 0;
}

int8_t zigbeeStack_GPSendGPPairingConfiguration(uint16_t action, uint32_t gpdSrcId, zigbee_Address_t* pDstAddr)
{
    uint8_t gpdKey[EMBER_ENCRYPTION_KEY_SIZE];
    uint8_t gpdDeviceId = 0;
    uint16_t alias = 0;
    uint16_t groupId = 0;

    if (false == zigbee_zgpdinfo_Load(gpdSrcId, &gpdDeviceId, gpdKey, &alias, &groupId))
    {
        return -1;
    }

    uint16_t pairingConfigurationOptions =
    (EMBER_GP_SINK_TYPE_GROUPCAST << 3)
    | EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SECURITY_USE;

    uint8_t securityOptions =
    EMBER_GP_SECURITY_LEVEL_FC_MIC
    | (EMBER_GP_SECURITY_KEY_GPD_OOB << 2);


    uint8_t groupList[4] = {groupId & 0xff, groupId >> 8, alias & 0xff, alias >> 8};
    emberAfFillCommandGreenPowerClusterGpPairingConfigurationSmart(
                                                      action, // uint8_t action
                                                      pairingConfigurationOptions, // uint16_t options
                                                      gpdSrcId, // uint32_t gpdSrcId
                                                      NULL, // uint8_t gpdIeee:NC
                                                      242, // uint8_t endpoint:NC
                                                      gpdDeviceId, // uint8_t deviceId
                                                      1, // uint8_t groupListCount
                                                      groupList, // uint8_t *groupList
                                                      alias, // uint16_t gpdAssignedAlias
                                                      0x00, // uint8_t forwardingRadius
                                                      securityOptions, // uint8_t securityOptions
                                                      0, // uint32_t gpdSecurityFrameCounter
                                                      gpdKey, // uint8_t *gpdSecurityKey
                                                      0xfe, // uint8_t numberOfPairedEndpoints
                                                      NULL, // uint8_t *pairedEndpoints
                                                      0, // uint8_t applicationInformation
                                                      0, // uint16_t manufacturerId
                                                      0, // uint16_t modeId: NC
                                                      0, // uint8_t numberOfGpdCommands
                                                      NULL, // uint8_t *gpdCommandIdList
                                                      0, // uint8_t clusterIdListCount
                                                      NULL, // uint16_t *clusterListServer
                                                      NULL  // uint16_t *clusterListClient
                                                      );

    EmberApsFrame *apsFrame = emberAfGetCommandApsFrame();
    apsFrame->sourceEndpoint = GP_ENDPOINT;
    apsFrame->destinationEndpoint = GP_ENDPOINT;
    emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, pDstAddr->address.zAddress);
    return 0;
}

int8_t zigbeeStack_GPSendCommissioningMode(uint8_t action, bool involveProxies,zigbee_Address_t* pDstAddr, uint16_t sinkEndpoint)
{
    return 0;
}

int8_t zigbeeStack_ZclCommandCreate(uint16_t clusterId, uint8_t* buf, int len)
{
    uint8_t commandId;
    uint8_t frameType = 0;
    uint8_t commandPayloadOffset = 3;
    if(buf[0] & BIT(4))
    {
        disableDefaultResponse = 1;
    }
    else
    {
        disableDefaultResponse = 0;
    }
    if(buf[0] & BIT(3))        //Direction Whether is Client to server
    {
        frameType = ZCL_FRAME_CONTROL_SERVER_TO_CLIENT;
    }
    else
    {
        frameType = ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    }
    if((buf[0] & 0x03) == 0x01)
    {

        frameType |= ZCL_CLUSTER_SPECIFIC_COMMAND;
    }
    else
    {
        frameType |= ZCL_GLOBAL_COMMAND;
    }
    if(buf[0] & BIT(2))
    {
        if (len < 5) // zclframe at least 5 bytes
            return 1;

        mfgSpecificId = buf[1] | buf[2] << 8;
        emberAfSequenceNumber = buf[3];
        commandId = buf[4];
        commandPayloadOffset = 5;
    }
    else
    {
        mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;
        emberAfSequenceNumber = buf[1];
        commandId = buf[2];
        commandPayloadOffset = 3;
    }
    zclBufferSetup(frameType, clusterId, commandId);
    for(uint16_t i = commandPayloadOffset; i < len; ++i)
    {
        zclBufferAddByte(buf[i]);
    }
    return 0;
}

int8_t zigbeeStack_ZclSend(zigbee_Address_t* pDstAddrS,uint16_t clusterId,uint8_t* pZclFrame,
                          uint8_t sourceEndPoint,uint8_t destinationEndPoint,uint16_t txOptions)
{
    EmberStatus status = 1;
	//uint8_t i;
    globalApsFrame.sourceEndpoint = sourceEndPoint;
    globalApsFrame.destinationEndpoint = destinationEndPoint;
    globalApsFrame.options = txOptions;
    switch(pDstAddrS->mode)
    {
    case zigbee_AddrMode_Short:
        status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT, pDstAddrS->address.zAddress, &globalApsFrame, appZclBufferLen, appZclBuffer);
        break;
    case zigbee_AddrMode_Group:
        status = emberAfSendMulticast(pDstAddrS->address.zAddress, &globalApsFrame, appZclBufferLen, appZclBuffer);
        break;
    case zigbee_AddrMode_Broadcast:
        status = emberAfSendBroadcast(pDstAddrS->address.zAddress, &globalApsFrame, appZclBufferLen, appZclBuffer);
        break;
    case zigbee_AddrMode_Via_Binding:
        status = emberAfSendUnicastToBindings(&globalApsFrame, appZclBufferLen, appZclBuffer);
        break;
    default:
        break;
    }
    zclCmdIsBuilt = false;
    mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;
    disableDefaultResponse = 0;
    return status;
}

int8_t zigbeeStack_ZclRegisterFoundationCommand(uint8_t foundationCommandId)
{
    return 0;
}

int8_t zigbeeStack_ZclRegisterEndpoint(uint8_t sourceEndPoint,uint16_t profileId,uint16_t deviceId, uint8_t appDevVer,
                                        uint8_t maxInClusters,uint8_t maxOutClusters,uint8_t clientGroupCount)
{
    return 0;
}

int8_t zigbeeStack_ZclAddClusterToSimpleDescriptor(uint8_t sourceEndPoint,uint8_t direction,uint16_t clusterId)
{
    return 0;
}

int8_t zigbeeStack_ZclRegisterCluster(uint16_t clusterId,uint16_t manufacturerCode,uint8_t direction)
{
    return 0;
}

int8_t zigbeeStack_ZdpSendMgmtPermitJoiningReq(zigbee_Address_t* pDestAddr,uint8_t permitDuration,uint8_t trustCenterSignificance)
{
    EmberStatus result;
    openNetworkTimeRemainingS = permitDuration;
    if(permitDuration)
    {
        // TODO: check it later. It's a walk-around solution by calling this function twice. What is the right API to permit join?
        result = emberAfPluginNetworkCreatorSecurityOpenNetwork();
    }
    else
    {
        result = emberAfPluginNetworkCreatorSecurityCloseNetwork();
    }
    return (int8_t)result;
}

/*
This API extends zigbeeStack_ZdpSendMgmtPermitJoiningReq to support a longer network open time.
In ZigBee, permitDuration is a 1-byte value. Silabs creates openNetworkRemaingS to extend 
support for longer (uint16_t) network open time.
*/
uint8_t zigbeeStack_ZdpSendMgmtPermitJoiningReq2(uint16_t permitDuration)
{
    EmberStatus result;
    openNetworkTimeRemainingS = permitDuration;
    if(permitDuration)
    {
        result = emberAfPluginNetworkCreatorSecurityOpenNetwork();
    }
    else
    {
        result = emberAfPluginNetworkCreatorSecurityCloseNetwork();
    }
    return result;
}

uint16_t zigbeeStack_GetPermitJoiningRemining()
{
    return openNetworkTimeRemainingS ;
}

uint8_t zigbeeStack_GetNetworkParameters(uint8_t* pNwkChannel, uint8_t* pNwkExtPanId)
{
    uint8_t status;
    EmberNetworkParameters parameters;
    status = emberGetNetworkParameters(&parameters);
    if(status == 0x00) 
    {
        *pNwkChannel = parameters.radioChannel;
        memcpy(pNwkExtPanId,parameters.extendedPanId, 8);
    }
    return status;
}

int8_t zigbeeStack_ZdpSendMgmtLqiReq(zigbee_Address_t* pDestAddr,uint8_t startIndex)
{
    return 0;
}

int8_t zigbeeStack_ZdpSendSimpleDescReq(zigbee_Address_t* pDestAddr,uint16_t nwkAddrOfInterest,uint8_t endPoint)
{
    return 0;
}

int8_t zigbeeStack_ZdpSendNodeDescReq(zigbee_Address_t* pDestAddr,uint16_t nwkAddrOfInterest)
{
    return 0;
}

int8_t zigbeeStack_ZdpSendActiveEndPointReq(zigbee_Address_t* pDestAddr,uint8_t nwkAddrOfInterest)
{
    return 0;
}

int8_t zigbeeStack_ZdpNwkAddrReq(zigbee_Address_t* pDestAddr,uint16_t ieeeAddrOfInterest)
{
    uint8_t ieee[8];
	for(uint8_t i=0; i<8; i++)
	{
	    ieee[i] = pDestAddr->address.ieeeAddress.b[7-i];
	}
    return emberAfFindNodeId(ieee, emberAfServiceDiscoveryCallback);
}

int8_t zigbeeStack_ZdpIeeeAddrReq(zigbee_Address_t* pDestAddr,uint16_t nwkAddrOfInterest)
{
    // dest network address should be equal to nwkAddrOfInterest
    if (pDestAddr->address.zAddress!=nwkAddrOfInterest) return 1;
    
    return emberAfFindIeeeAddress(nwkAddrOfInterest,emberAfServiceDiscoveryCallback);
}

int8_t zigbeeStack_ZdpSendBindReq(zigbee_Address_t* pDestAddr,zigbee_Address_t* pBindSrc,uint16_t clusterId,zigbee_Address_t* pBindDest)
{
    return 0;
}

int8_t zigbeeStack_ZdpSendUnbindReq(zigbee_Address_t* pDestAddr,zigbee_Address_t* pBindSrc,uint16_t clusterId,zigbee_Address_t* pBindDest)
{
    return 0;
}

int8_t zigbeeStack_ZdpSendMgmtBindReq(zigbee_Address_t* pDestAddr,uint8_t startIndex)
{
    return 0;
}

int8_t zigbeeStack_ZdpSendMatchDescReq(zigbee_Address_t* pDestAddr,uint16_t networkAddressOfInterest)
{
    return 0;
}

int8_t zigbeeStack_ZdpSendMgmtLeaveReq(zigbee_Address_t* pDestAddr,uint8_t* pDeviceIeeeAddr,bool removeChildren,bool rejoin)
{
/*
	EmberLeaveRequestFlags flags = 0;
    if(removeChildren) flags |= EMBER_ZIGBEE_LEAVE_AND_REMOVE_CHILDREN;
    if(rejoin) flags |= EMBER_ZIGBEE_LEAVE_AND_REJOIN;
    
    return emberSendZigbeeLeave(pDestAddr->address.zAddress, flags);
*/
  EmberNodeId target = pDestAddr->address.zAddress;
  EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
  EmberStatus status;

  uint8_t options = 0;
  if (rejoin) {
    options |= EMBER_ZIGBEE_LEAVE_AND_REJOIN;
  }
  if (removeChildren) {
    options |= EMBER_ZIGBEE_LEAVE_AND_REMOVE_CHILDREN;
  }

  status = emberLeaveRequest(target,
                             nullEui64,
                             options,
                             EMBER_APS_OPTION_RETRY);

  return (int8_t)status;    
}

int8_t zigbeeStack_ZdpGetSequenceNr(uint8_t* seqNr)
{
    *seqNr = emberGetLastStackZigDevRequestSequence();
    return 0;
}

int8_t zigbeeStack_RoutingSendMtoRR(bool noRouteCache)
{
    return 0;
}

int8_t zigbeeStack_RoutingClearEntry(uint16_t nwkAddress)
{
    return 0;
}

int8_t zigbeeStack_BrStoreGroupRange(uint16_t groupRangeBegin,uint16_t groupRangeEnd)
{
    return 0;
}

int8_t zigbeeStack_BrIdentify(uint16_t identifyTime)
{
    return 0;
}

int8_t zigbeeStack_GetResetType(uint8_t * pResetType)
{
    uint8_t resetCause = halGetResetInfo();
    switch(resetCause)
    {
    case 3:
        // external reset
        *pResetType = 1;
        break;
    case 4:
        // power on reset
        *pResetType = 0;
        break;
    case 5:
        // watchdog reset
        *pResetType = 2;
    case 6:
        // software reset
        *pResetType = 3;
        break;
    default:
        *pResetType = 3;
        break;
    }

    return 0;
}

int8_t zigbeeStack_ShortSleep(void)
{
    return 0;
}

int8_t zigbeeStack_LongSleep(void)
{
    return 0;
}

int8_t zigbeeStack_DumpInfo(uint16_t* pUpdateId,uint8_t* pChannel,
                            uint16_t* pNwkAddr,uint16_t* pPanId,uint8_t* pIeeeAddr)
{
    EmberNetworkParameters parameters;
#if defined(CWP_NODE)
	uint32_t currentChannelMask = emAfPluginNetworkSteeringPrimaryChannelMask;
#else
	uint32_t currentChannelMask = emAfPluginNetworkCreatorPrimaryChannelMask;
#endif
    uint8_t channel;
    bool found;

    emberGetNetworkParameters(&parameters);

    *pUpdateId = parameters.nwkUpdateId;
    if (emberAfNetworkState() == EMBER_JOINED_NETWORK)
    {
        *pChannel = parameters.radioChannel;
    }
    else
    {
#if defined(CWP_NODE)
        currentChannelMask = emAfPluginNetworkSteeringPrimaryChannelMask;
#else
        currentChannelMask = emAfPluginNetworkCreatorPrimaryChannelMask;
#endif

        channel = EMBER_MIN_802_15_4_CHANNEL_NUMBER;
        found = false;
        while ((currentChannelMask) && !(channel > EMBER_MAX_802_15_4_CHANNEL_NUMBER))
        {
            if (READBIT(currentChannelMask, channel))
            {
                found = true;
                break;
            }
            channel++;
        }
        *pChannel = (found == true) ? channel : parameters.radioChannel;
    }
    *pPanId = parameters.panId;
    
    *pNwkAddr = emberGetNodeId();
    memcpy(pIeeeAddr, emberGetEui64(),EUI64_SIZE);
    return 0;
}

int8_t zigbeeStack_GetSwVersion(uint8_t* pProjectName,uint8_t keyBitMask,uint8_t* pBuildVersionString)
{
    return 0;
}

int8_t zigbeeStack_SetChannelMask(uint32_t channelMask,uint32_t secondaryChannelMask)
{
#if defined(CWP_NODE)
	emAfPluginNetworkCreatorPrimaryChannelMask = channelMask;

	if(secondaryChannelMask)
	{
	    emAfPluginNetworkCreatorSecondaryChannelMask = secondaryChannelMask;
	}
#else
	emberAfPluginNetworkCreatorSetChannelMask(channelMask, FALSE);

	if(secondaryChannelMask)
	{
	    emberAfPluginNetworkCreatorSetChannelMask(channelMask, TRUE);
	}
#endif
    return 0;
}

int8_t zigbeeStack_ResetToFactoryDefaults(uint8_t reset)
{
    // TODO: to be implemented later
    
    switch(reset)
    {
    case 0:
        #if defined(CWP_DONGLE)
        zigbeeStack_GPEraseAll();
        #else
        emGpClearProxyTable();
        #endif
        emberAfZllResetToFactoryNew();
        halReboot();
        break;
    case 2: 
        #if defined(CWP_DONGLE)
        zigbeeStack_GPEraseAll();
        #else
        emGpClearProxyTable();
        #endif
        internalflash_EraseAll();
        emberAfZllResetToFactoryNew();
        halReboot();
	    break;
    case 3:
        #if defined(CWP_DONGLE)
        zigbeeStack_GPEraseAll();
        #else
        emGpClearProxyTable();
        #endif
        internalflash_EraseAll();
        emberAfZllResetToFactoryNew();
        halReboot();
        break;
    default:
        break;
    }

    return 0;
}

int8_t zigbeeStack_GetMacAddress(uint8_t* pMacAddr)
{
    memcpy(pMacAddr, emberGetEui64(),EUI64_SIZE);
    return 0;
}

int8_t zigbeeStack_GetMacAddressOptimize(uint8_t* pMacAddr, uint8_t macFind)
{
    // Chip's Mac address
    if(macFind == ZCL_USE_EMBER_EUI_64)
    {
      halCommonGetMfgToken(pMacAddr, TOKEN_MFG_EMBER_EUI_64);
    }
    else if(macFind == ZCL_USE_CUSTOM_EUI_64) // User's Mac address
    {
      halCommonGetMfgToken(pMacAddr, TOKEN_MFG_CUSTOM_EUI_64);
    }
    else // Default Mac address
    {
      memcpy(pMacAddr, emberGetEui64(),EUI64_SIZE);  
    }
    return 0;
}

extern bool_t set_eui_addr_token(uint8_t *data);
int8_t zigbeeStack_SetMacAddress(uint8_t* pMacAddr)
{
   return set_eui_addr_token(pMacAddr);
}

int8_t zigbeeStack_SetTxPower(uint8_t powerIndex)
{
	return emberSetRadioPower(powerIndex);
}

int8_t zigbeeStack_SetAutoJoinEnabled(uint8_t enabled)
{
    halCommonSetToken(TOKEN_AUTOJOIN_ENABLED, &enabled);
    return 0;
}

void   zigbeeStack_JoinNwk()
{
    emberAfPluginNetworkSteeringStart();
}

int8_t zigbeeStack_NetworkSteeringSetChannelMask(uint32_t mask, bool secondaryMask)
{
    emAfPluginNetworkSteeringSetChannelMask(mask, secondaryMask);
#if defined(CWP_NODE)
    if (secondaryMask) {
        halCommonSetToken(TOKEN_NWK_STEERING_SECONDARY_CHANNEL_MASK, &mask);
    } else {
        halCommonSetToken(TOKEN_NWK_STEERING_PRIMARY_CHANNEL_MASK, &mask);
    }
#endif
    return 0;
}

void zigbeeStack_NetworkSteeringSetExtendedPanIdFilter(uint8_t* extendedPanId, bool turnFilterOn)
{
    emAfPluginNetworkSteeringSetExtendedPanIdFilter(extendedPanId, turnFilterOn);
}

int8_t zigbeeStack_SetCMUClkHfxoCtune(uint16_t cmuClkHFXOCtune)
{
    BSP_CLK_HFXO_CTUNE = cmuClkHFXOCtune;
    return 0;
}

int8_t zigbeeStack_SetMAXPowerLevel(int8_t maxPowerLevel)
{
    return emberAfSetMaxPowerLevel(maxPowerLevel);
}

int8_t zigbeeStack_GetTxPower(uint8_t* pPowerIndex)
{
	*pPowerIndex = emberGetRadioPower();
	return 0;
}

uint8_t zigbeeStack_GetRSSI(void)
{
	return emberAfZllGetRssi();
}

int8_t zigbeeStack_SysReset(void)
{
	halReboot();
    return 0;
}

int8_t zigbeeStack_GetRandom(uint16_t* pRandom, uint8_t range)
{
	*pRandom = ((uint32_t)(halCommonGetRandom() * range)) >> 16;
    return 0;
}

uint16_t zigbeeStack_GetRandom2(void)
{
    return halCommonGetRandom();
}

int8_t zigbeeStack_GetNetWorkStatus(void)
{
   if(emberAfNetworkState() == EMBER_JOINED_NETWORK)
   {
      return 0;
   }
   else
   {
      return 1;
   }
}

int8_t zigbeeStack_SendC4MSpecificCluster_command(zigbee_Address_t* destAddr, uint8_t* buf, uint8_t len)
{
    int8_t status;
    status = zigbeeStack_ZclCommandCreate(ZCL_C4M_SPECIFIC_CLUSTER_ID, buf, len);
	if(!status)
	{
	    uint16_t txOption = (uint16_t)0x040; ///EMBER_APS_OPTION_RETRY
        status = zigbeeStack_ZclSend(destAddr, ZCL_C4M_SPECIFIC_CLUSTER_ID, buf, 0x40, 0x40, txOption);
	}
	return status;
}

int8_t zigbeeStack_SendCluster_command(zigbee_Address_t* destAddr, uint16_t clusterId, uint8_t* buf, uint8_t len)
{
    int8_t status;
    status = zigbeeStack_ZclCommandCreate(clusterId, buf, len);
	if(!status)
	{
	    uint16_t txOption = (uint16_t)0x040; ///EMBER_APS_OPTION_RETRY
        status = zigbeeStack_ZclSend(destAddr, clusterId, buf, 0x40, 0x40, txOption);
	}
	return status;
}

int8_t zigbeeStack_PrintSourceRouteTable(void)
{
    uint8_t i;
	uint8_t tableSize = emberAfGetSourceRouteTableFilledSize();

	if(!tableSize) return 0;

	logcom_Printc("[");


    for (i = 0; i < emberAfGetSourceRouteTableFilledSize(); i++)
	{
        uint8_t index = i;
        uint8_t closerIndex;
        EmberNodeId destination;
        EmberStatus status = emberAfGetSourceRouteTableEntry(index,
                                                             &destination,
                                                             &closerIndex);
        if (status != EMBER_SUCCESS)
        {
            logcom_Printfbasic("0 of 0 total entries.");
            return 0;
        }

		logcom_Printc("%3d:0x%2X-> ", i, destination);
        if (i && (i%10 == 0)) logcom_Printc("\r");
    }

	logcom_Printc("]\r");

    return 0;
}

int8_t zigbeeStack_ManyToOneRouteRequestSend(void)
{
    #if defined(CWP_DONGLE)
    emberSendManyToOneRouteRequest(EMBER_HIGH_RAM_CONCENTRATOR, EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS);
    #endif
	return 0;
}

uint16_t zigbeeStack_GetPanId(void)
{
  EmberPanId panID = emberGetPanId();
  return (uint16_t)panID;
}

/******************************************************************************
* LOCAL EXPORTED FUNCTIONS
******************************************************************************/


/*!
 * @brief The command queue handle
 */
static osa_QueueHandle_t  s_zclCommandTxQueue;

static uint8_t s_zclCommandCount = 0;

/******************************************************************************
* EXPORTED INTERFACE
******************************************************************************/

void zigbeeStack_ZclCommand_Init(void)
{
    s_zclCommandTxQueue = osa_QueueCreate( "ZclCmdQ", ZCL_COMMAND_QUEUE_SIZE, sizeof( zclCmdTransmit_QueueEntry_t ) );
}

void zigbeeStack_ZclCommand_PreSend(zclCmdTransmit_t * pZclCmd)
{
	zclCmdTransmit_QueueEntry_t zclCmdQueueEntry;
	memset(&zclCmdQueueEntry,0,sizeof(zclCmdTransmit_QueueEntry_t));

	zclCmdQueueEntry.buf[0] = (uint8_t)pZclCmd->clusterId;
	zclCmdQueueEntry.buf[1] = (uint8_t)(pZclCmd->clusterId>>8);
	zclCmdQueueEntry.buf[2] = pZclCmd->srcEndPoint;
	zclCmdQueueEntry.buf[3] = pZclCmd->dstAddr.endpoint;
	zclCmdQueueEntry.buf[4] = pZclCmd->dstAddr.mode;
	zclCmdQueueEntry.buf[5] = (uint8_t)pZclCmd->dstAddr.address.zAddress;
	zclCmdQueueEntry.buf[6] = (uint8_t)(pZclCmd->dstAddr.address.zAddress>>8);
	zclCmdQueueEntry.buf[7] = (uint8_t)pZclCmd->txOptions;
	zclCmdQueueEntry.buf[8] = (uint8_t)(pZclCmd->txOptions>>8);
	zclCmdQueueEntry.buf[9] = pZclCmd->length;
	memcpy(&zclCmdQueueEntry.buf[10], pZclCmd->payload, pZclCmd->length);

    // maxTicks para need to be checked
    osa_QueueSendToFrontCritical( s_zclCommandTxQueue, &zclCmdQueueEntry.buf[0], 0 );
	s_zclCommandCount++;
}

void zigbeeStack_ZclCommand_Send(void)
{
    if(s_zclCommandCount == 0)
    {
        // no message to be sent. directly return so as to avoid of unnecessary queue operations, to save CPU resource
        return;
    }
	
	uint8_t frametype;
	int8_t status;
	uint8_t seq;

	zclCmdTransmit_t zclCmd;
	memset(&zclCmd,0,sizeof(zclCmdTransmit_t));

	zclCmdTransmit_QueueEntry_t zclCmdQueueEntry;
	memset(&zclCmdQueueEntry,0,sizeof(zclCmdTransmit_QueueEntry_t));

	// handle message queue
    bool_t ret = osa_QueueReceive(s_zclCommandTxQueue, (void *)&zclCmdQueueEntry.buf[0], ZCL_COMMAND_QUEUE_TICKS);
    if(ret)
	{
	    // message is valid, already been consumed now.
	    s_zclCommandCount--;

		zclCmd.clusterId = (uint16_t)zclCmdQueueEntry.buf[0]+((uint16_t)zclCmdQueueEntry.buf[1]<<8);
		zclCmd.srcEndPoint = zclCmdQueueEntry.buf[2];
		zclCmd.dstAddr.endpoint = zclCmdQueueEntry.buf[3];
		// warning: enumerated type 4 bytes in application, but 1 byte in stack
		zclCmd.dstAddr.mode = (zigbee_AddrMode_t)zclCmdQueueEntry.buf[4];
		zclCmd.dstAddr.address.zAddress = (uint16_t)zclCmdQueueEntry.buf[5]+((uint16_t)zclCmdQueueEntry.buf[6]<<8);
		zclCmd.txOptions = (uint16_t)zclCmdQueueEntry.buf[7]+((uint16_t)zclCmdQueueEntry.buf[8]<<8);
		zclCmd.length = (int)zclCmdQueueEntry.buf[9];
		zclCmd.payload = &zclCmdQueueEntry.buf[10];
		frametype = zclCmdQueueEntry.buf[10];
		if (frametype & BIT(2))
		{
			seq = zclCmdQueueEntry.buf[13];
		}
		else
		{
			seq = zclCmdQueueEntry.buf[11];
		}

		// structure the zcl frame to be sent later
		status = zigbeeStack_ZclCommandCreate(zclCmd.clusterId, &zclCmdQueueEntry.buf[10], zclCmd.length);
		if (status)
		{
    		logcom_Printfbasic("zigbeeStack_ZclCommandCreate failed");
    		return;
		}

		// send the zcl frame to network
        status = zigbeeStack_ZclSend(&zclCmd.dstAddr, zclCmd.clusterId, zclCmd.payload, zclCmd.srcEndPoint, zclCmd.dstAddr.endpoint, zclCmd.txOptions);
        if(status)
        {
			logcom_Printfbasic("zigbeeStack_ZclSend err");
			logcom_PrintBuf2(&zclCmdQueueEntry.buf[10],zclCmd.length);
			// if message is broadcast, send back to message queue for next round execution
			//if ((zclCmd.dstAddr.mode==zigbee_AddrMode_Group)||(zclCmd.dstAddr.mode==zigbee_AddrMode_Broadcast))
			//	zigbeeStack_ZclCommand_PreSend(&zclCmd);

			return;
        }

        // respond to gateway
        if(frametype & 0x08)    //server to client
        {
#if defined (CWP_NODE)
            log_Printfbasic("Zcl,Req,%d,%d", 0, seq);
            log_Printfbasic("Zcl,Conf,%d,%d,%d", zclCmd.srcEndPoint, seq, 0);
#elif defined (CWP_DONGLE)
            logcom_Printfbasic("Zcl,Req,%d,%d", 0, seq);
            logcom_Printfbasic("Zcl,Conf,%d,%d,%d", zclCmd.srcEndPoint, seq, 0);
#endif
        }
        else
        {
#if defined (CWP_NODE)
            log_Printfbasic("Zcl,Req,%d,%d", 0, seq);
            log_Printfbasic("Zcl,Conf,%d,%d,%d", zclCmd.srcEndPoint, seq, 0);
#elif defined (CWP_DONGLE)
            logcom_Printfbasic("Zcl,Req,%d,%d", 0, seq);
            logcom_Printfbasic("Zcl,Conf,%d,%d,%d", zclCmd.srcEndPoint, seq, 0);
#endif
        }

    }

	//logcom_Printfbasic("zigbeeStack_ZclCommand_Send--");
}

#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY) && defined(CWP_NODE)
int8_t zigbeeStack_BleGetMacAddress(uint8_t* pMacAddr)
{
  struct gecko_msg_system_get_bt_address_rsp_t *rsp;
  rsp = gecko_cmd_system_get_bt_address();
  memcpy(pMacAddr, rsp->address.addr,6);
  return 0;
}

int8_t zigbeeStack_BleStartBeaconings(uint32_t intervalInMillseconds,uint8_t channel_map,uint16_t tx_power_dBm)
{
	int8_t result=-1;

	if ((intervalInMillseconds<100) || (intervalInMillseconds>10240))
	{
		log_Printfbasic("Interval must range between 100 and 10240 milliseconds");
		return result;
	}

	if ((channel_map<1) || (channel_map>7))
	{
		log_Printfbasic("Channel map must range between 1 and 7");
		return result;
	}

	if (s_advertizer_isEnabled)
	{
		log_Printfbasic("Stop beaconing first before start");
		return result;
	}
	s_advertizer_intervalInMillseconds = intervalInMillseconds;
	s_advertizer_channel_map = channel_map;
	s_advertizer_isEnabled = true;

	enableBeaconAdvertisements(intervalInMillseconds, channel_map, tx_power_dBm*10);
	log_Printfbasic("Advertising on channels %s", bcnAdvertisingChannels());
	log_Printfbasic("iBeacon [ON]");
	log_Printfbasic("Chl %s", bcnAdvertisingChannels());
	log_Printfbasic("Interval %dms", s_advertizer_intervalInMillseconds);
	log_Printfbasic("TxPower %ddBm", tx_power_dBm);
	result=0;

	return result;
}

int8_t zigbeeStack_BleStopBeaconings(void)
{
	disableBeaconAdvertisements();
	s_advertizer_channel_map = 0;
	s_advertizer_intervalInMillseconds = 0;
	s_advertizer_isEnabled = false;
	log_Printfbasic("iBeacon [OFF]");

	return 0;
}

// Advertisement data
#define UINT16_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8))
#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))

#define DEVNAME "DMP%02X%02X"
#define DEVNAME_LEN 8  // incl term null
#define UUID_LEN 16 // 128-bit UUID

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
  uint8_t url[10];           /**< URL. */
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
  0x10,
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
  { 's', 'i', 'l', 'a', 'b', 's', '.', 'c', 'o', 'm' }
};

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
  
enum {HANDLE_DEMO, HANDLE_IBEACON, HANDLE_EDDYSTONE}; //advertisers handle
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

/**
 * @brief Function for creating a custom advertisement package
 *
 * The function builds the advertisement package according to Apple iBeacon specifications,
 * configures this as the device advertisement data and starts broadcasting.
 */
static void enableBeaconAdvertisements(uint16_t intervalInMillseconds, uint8_t channel_map, int16 tx_power)
{
  /* This function sets up a custom advertisement package according to iBeacon specifications.
   * The advertisement package is 30 bytes long. See the iBeacon specification for further details.
   */

  uint8_t len = sizeof(iBeaconData);
  uint8_t *pData = (uint8_t*)(&iBeaconData);

  /* Set tx_power/10 dBm Transmit Power */
  gecko_cmd_system_set_tx_power(tx_power);


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
  log_Printfbasic("devName = %s", devName);
  gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name,
                                              0,
                                              strlen(devName),
                                              (uint8_t *)devName);


  // Copy the shortened device name to the response data, overwriting
  // the default device name which is set at compile time
  MEMCOPY(((uint8_t*)&responseData) + 5, devName, 8);
  
  /* Set custom advertising data */
  struct gecko_msg_le_gap_bt5_set_adv_data_rsp_t *rsp;
  rsp = gecko_cmd_le_gap_bt5_set_adv_data(HANDLE_DEMO, 0, sizeof(responseData), (uint8_t*)&responseData);

  /* Set advertising parameters. 100ms advertisement interval. All channels used.
   * The first two parameters are minimum and maximum advertising interval, both in
   * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */
  gecko_cmd_le_gap_bt5_set_adv_parameters(HANDLE_DEMO, (uint16)(intervalInMillseconds*1.6), (uint16)(intervalInMillseconds*1.6), channel_map, 1);

  /* Start advertising in user mode and enable connections */
  gecko_cmd_le_gap_bt5_set_mode(HANDLE_DEMO, le_gap_user_data, le_gap_non_connectable, 0, le_gap_non_resolvable);
  log_Printfbasic("BLE custom advertisements enabled");
  
  BeaconAdvertisements(devId);
}

static void disableBeaconAdvertisements(void)
{
  gecko_cmd_le_gap_bt5_set_mode(0, le_gap_non_discoverable, le_gap_non_connectable, 0, le_gap_non_resolvable);
}

static char* bcnAdvertisingChannels(void)
{
   static char chl_list[20];

    switch (s_advertizer_channel_map)
    {
      case 1:
        strcpy(chl_list, "37");
        break;

      case 2:
        strcpy(chl_list, "38");
        break;

      case 3:
        strcpy(chl_list, "37 and 38");
        break;

      case 4:
        strcpy(chl_list, "39");
        break;

      case 5:
        strcpy(chl_list, "37 and 39");
        break;

      case 6:
        strcpy(chl_list, "38 and 39");
        break;

      case 7:
      default:
        strcpy(chl_list, "37 38 and 39");
        break;
    }

    return chl_list;
}
#endif

