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

#ifndef __ZIGBEE_STACK_API_H
#define __ZIGBEE_STACK_API_H

/******************************************************************************
* INCLUDE FILES
******************************************************************************/
#include "zigbee_types.h"






/******************************************************************************
* C++ DECLARATION WRAPPER
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* EXPORTED MACROS AND CONSTANTS
******************************************************************************/
#define BUILD_TEST_COMBO_NODES_SCALABILITY

#define ZCL_COMMAND_SEND_WIDTH   (100)
#define ZCL_COMMAND_SEND_DEPTH   (50)

#define ZCL_COMMAND_QUEUE_ITEM_SIZE   (100)
#define ZCL_COMMAND_QUEUE_SIZE        (50)
#define ZCL_COMMAND_QUEUE_TICKS       (1)

/*! ZCL command structure */
typedef struct zclCmdTransmit_t
{
    uint8_t  srcEndPoint;
    zigbee_Address_t dstAddr;
    uint16_t clusterId;
    uint8_t* payload;
    int      length;
    uint16_t txOptions;
} zclCmdTransmit_t;

/*! Queue entry structure */
typedef struct zclCmdTransmit_QueueEntry_t
{
    uint8_t  buf[ZCL_COMMAND_QUEUE_ITEM_SIZE];
} zclCmdTransmit_QueueEntry_t;

/** Use to get mac address. */
typedef enum {
  ZCL_USE_CUSTOM_EUI_64 = 0,     /**< MFG_CUSTOM_EUI_64 */
  ZCL_USE_EMBER_EUI_64,  /**< MFG_EMBER_EUI_64 */
  ZCL_USE_DEFAULT_EUI_64,  /**< MFG_EMBER_EUI_64 */
} zclGetMacAddress_TypeDef;








/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/


int8_t zigbeeStack_TouchLinkAllow(void);
int8_t zigbeeStack_TouchLinkDisallow(void);
int8_t zigbeeStack_TouchLink(uint16_t* pNwkaddr, uint16_t* pPanid);
int8_t zigbeeStack_SendApsTransportNetworkKey(uint16_t nwkAddress,uint8_t* pIeeeAddr,uint16_t parentNwkAddres);
int8_t zigbeeStack_FindFreePan(uint16_t* pNwkPanId, uint8_t * pNwkExtPanId);
int8_t zigbeeStack_JoinNetwork(uint16_t nwkPanId, uint8_t* pNwkExtPanId,uint8_t nwkChannel,uint8_t* pNwkUpdateId);
int8_t zigbeeStack_Associate(uint8_t* pNwkExtPanId);
int8_t zigbeeStack_GetAddress(uint8_t* pIeeeAddr,uint16_t* pShortAddr,uint8_t* pEndpoint);
uint8_t zigbeeStack_GetNetwrokState(void);
uint8_t zigbeeStack_NetworkSteeringStop(void);
uint8_t zigbeeStack_SetShortAddress(uint16_t addr);
int8_t zigbeeStack_GetExtendedPanId(uint8_t* pNwkExtPanId);
int8_t zigbeeStack_GetChannel(uint8_t* pNwkChannel);
int8_t zigbeeStack_ChannelChange(uint8_t nwkChannel);
bool zigbeeStack_IsCentralizedNetwork(void);
int8_t zigbeeStack_StartCoordinator(uint16_t nwkPanId,uint8_t* pNwkExtPanId,uint8_t nwkChannel);
int8_t zigbeeStack_StartCoordinator2(uint16_t nwkPanId,uint8_t* pNwkExtPanId,uint8_t nwkChannel);
int8_t zigbeeStack_GetUpdatedId(uint8_t* pUpdateId);
int8_t zigbeeStack_GPSetGroupId(uint16_t group);
int8_t zigbeeStack_GPCommissioningEnter(bool exitOnPairing, uint16_t timeoutS);
int8_t zigbeeStack_GPCommissioningExit(void);
int8_t zigbeeStack_GPAddPairing(zigbee_Address_t* pDestinationAddr,uint32_t gpdSourceId);
int8_t zigbeeStack_GPRemovePairing(zigbee_Address_t* pDestinationAddr, uint32_t gpdSourceId);
int8_t zigbeeStack_GPGetEntryForIndex(uint8_t index);
int8_t zigbeeStack_GPEraseAll(void);
int8_t zigbeeStack_GPErase(uint32_t gpdSrcId);
int8_t zigbeeStack_GPSendGPPairingConfiguration(uint16_t action, uint32_t gpdSrcId, zigbee_Address_t* pDstAddr);
int8_t zigbeeStack_GPSendCommissioningMode(uint8_t action, bool involveProxies,zigbee_Address_t* pDstAddr, uint16_t sinkEndpoint);
int8_t zigbeeStack_GPSetAssignedAlias(uint16_t alias);
int8_t zigbeeStack_ZclCommandCreate(uint16_t clusterId, uint8_t* buf, int len);
int8_t zigbeeStack_ZclSend(zigbee_Address_t* pDstAddrS,uint16_t clusterId,uint8_t* pZclFrame,
                          uint8_t sourceEndPoint,uint8_t destinationEndPoint,uint16_t txOptions);
int8_t zigbeeStack_ZclRegisterFoundationCommand(uint8_t foundationCommandId);
int8_t zigbeeStack_ZclRegisterEndpoint(uint8_t sourceEndPoint,uint16_t profileId,uint16_t deviceId, uint8_t appDevVer,
                                        uint8_t maxInClusters,uint8_t maxOutClusters,uint8_t clientGroupCount);
int8_t zigbeeStack_ZclAddClusterToSimpleDescriptor(uint8_t sourceEndPoint,uint8_t direction,uint16_t clusterId);
int8_t zigbeeStack_ZclRegisterCluster(uint16_t clusterId,uint16_t manufacturerCode,uint8_t direction);
int8_t zigbeeStack_ZdpSendMgmtPermitJoiningReq(zigbee_Address_t* pDestAddr,uint8_t permitDuration,uint8_t trustCenterSignificance);
int8_t zigbeeStack_ZdpSendMgmtLqiReq(zigbee_Address_t* pDestAddr,uint8_t startIndex);
int8_t zigbeeStack_ZdpSendSimpleDescReq(zigbee_Address_t* pDestAddr,uint16_t nwkAddrOfInterest,uint8_t endPoint);
int8_t zigbeeStack_ZdpSendNodeDescReq(zigbee_Address_t* pDestAddr,uint16_t nwkAddrOfInterest);
int8_t zigbeeStack_ZdpSendActiveEndPointReq(zigbee_Address_t* pDestAddr,uint8_t nwkAddrOfInterest);
int8_t zigbeeStack_ZdpNwkAddrReq(zigbee_Address_t* pDestAddr,uint16_t ieeeAddrOfInterest);
int8_t zigbeeStack_ZdpIeeeAddrReq(zigbee_Address_t* pDestAddr,uint16_t nwkAddrOfInterest);
int8_t zigbeeStack_ZdpSendBindReq(zigbee_Address_t* pDestAddr,zigbee_Address_t* pBindSrc,uint16_t clusterId,zigbee_Address_t* pBindDest);
int8_t zigbeeStack_ZdpSendUnbindReq(zigbee_Address_t* pDestAddr,zigbee_Address_t* pBindSrc,uint16_t clusterId,zigbee_Address_t* pBindDest);
int8_t zigbeeStack_ZdpSendMgmtBindReq(zigbee_Address_t* pDestAddr,uint8_t startIndex);
int8_t zigbeeStack_ZdpSendMatchDescReq(zigbee_Address_t* pDestAddr,uint16_t networkAddressOfInterest);
int8_t zigbeeStack_ZdpSendMgmtLeaveReq(zigbee_Address_t* pDestAddr,uint8_t* pDeviceIeeeAddr,bool removeChildren,bool rejoin);
int8_t zigbeeStack_ZdpGetSequenceNr(uint8_t* seqNr);
int8_t zigbeeStack_RoutingSendMtoRR(bool noRouteCache);
int8_t zigbeeStack_RoutingClearEntry(uint16_t nwkAddress);
int8_t zigbeeStack_BrStoreGroupRange(uint16_t groupRangeBegin,uint16_t groupRangeEnd);
int8_t zigbeeStack_BrIdentify(uint16_t identifyTime);
int8_t zigbeeStack_GetResetType(uint8_t * pResetType);
int8_t zigbeeStack_ShortSleep(void);
int8_t zigbeeStack_LongSleep(void);
int8_t zigbeeStack_DumpInfo(uint16_t* pUpdateId,uint8_t* pChannel,
                            uint16_t* pNwkAddr,uint16_t* pPanId,uint8_t* pIeeeAddr);
int8_t zigbeeStack_GetSwVersion(uint8_t* pProjectName,uint8_t keyBitMask,uint8_t* pBuildVersionString);
int8_t zigbeeStack_SetChannelMask(uint32_t channelMask,uint32_t secondaryChannelMask);
int8_t zigbeeStack_ResetToFactoryDefaults(uint8_t reset);
int8_t zigbeeStack_GetMacAddress(uint8_t* pMacAddr);
int8_t zigbeeStack_GetMacAddressOptimize(uint8_t* pMacAddr, uint8_t macFind);
int8_t zigbeeStack_SetMacAddress(uint8_t* pMacAddr);
int8_t zigbeeStack_SetTxPower(uint8_t powerIndex);
int8_t zigbeeStack_GetTxPower(uint8_t* pPowerIndex);
uint8_t zigbeeStack_GetRSSI(void);
int8_t zigbeeStack_SysReset(void);
int8_t zigbeeStack_GetRandom(uint16_t* pRandom, uint8_t range);
uint16_t zigbeeStack_GetRandom2(void);
int8_t zigbeeStack_PrintSourceRouteTable(void);
int8_t zigbeeStack_ManyToOneRouteRequestSend(void);
/* Steady-state phase CTUNE (load capacitance) value
 * Common value:
 * SNS441: 225
 * MW SmartTLED: 345
 * C4M node/dongle: 360
*/
int8_t zigbeeStack_SetCMUClkHfxoCtune(uint16_t cmuClkHFXOCtune);
int8_t zigbeeStack_SetMAXPowerLevel(int8_t maxPowerLevel);


typedef void (*scipGpCommissioningNotification_t)(uint32_t gpdSrcId, uint8_t gpdDeviceId);
typedef void (*scipGpDeviceInfo_t)(uint32_t gpdSrcId, uint16_t fwVersion, uint16_t hwVersion, uint16_t stackVersion);
typedef struct {
    scipGpCommissioningNotification_t     gpCommissioningNotification;
    scipGpDeviceInfo_t                    gpDeviceInfo;
} scipConfig_t;
extern const scipConfig_t   scipConfig;
int8_t zigbeeStack_GetNetWorkStatus(void);
int8_t zigbeeStack_SendC4MSpecificCluster_command(zigbee_Address_t* destAddr, uint8_t* buf, uint8_t len);
int8_t zigbeeStack_SendCluster_command(zigbee_Address_t* destAddr, uint16_t clusterId, uint8_t* buf, uint8_t len);


void zigbeeStack_ZclCommand_Init(void);
void zigbeeStack_ZclCommand_PreSend(zclCmdTransmit_t * pZclCmd);
void zigbeeStack_ZclCommand_Send(void);

int8_t zigbeeStack_SetAutoJoinEnabled(uint8_t enabled);
void   zigbeeStack_JoinNwk();
int8_t zigbeeStack_NetworkSteeringSetChannelMask(uint32_t mask, bool secondaryMask);
void zigbeeStack_NetworkSteeringSetExtendedPanIdFilter(uint8_t* extendedPanId, bool turnFilterOn);

#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY)
int8_t zigbeeStack_BleGetMacAddress(uint8_t* pMacAddr);
int8_t zigbeeStack_BleStartBeaconings(uint32_t intervalInMillseconds,uint8_t channel_map,uint16_t tx_power_dBm);
int8_t zigbeeStack_BleStopBeaconings(void);
#endif

uint8_t zigbeeStack_ZdpSendMgmtPermitJoiningReq2(uint16_t permitDuration);
uint16_t zigbeeStack_GetPermitJoiningRemining(void);
uint8_t zigbeeStack_GetNetworkParameters(uint8_t* pNwkChannel, uint8_t* pNwkExtPanId);
uint16_t zigbeeStack_GetPanId(void);

/*!
 * @brief Initializes the BleStack
 */


/******************************************************************************
* END OF C++ DECLARATION WRAPPER
******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __STACK_API_H
