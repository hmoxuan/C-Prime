/** \file

$Id: zigbee_commissioningattrserver_Controller.c $

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

#include "zigbee_commissioningattrserver_FsmController.h"
#include "zigbee_commissioningattrserver.h"
#include "zigbee_commissioningattrserver_Map.h"

/******************************************************************************
* EXTERNAL INCLUDE FILES
******************************************************************************/

#include "lcp_variant.h"
#include "lcp_types.h"
#include "lcp_macros.h"
#include "Peripheral/zigbee_Peripheral.h"
#include "Connection\zigbee_Connection.h"
#include "Zcl/Foundation/zigbee_Foundation_Server.h"
#include "Zcl/zigbee_Zcl.h"
#include "string.h"
#include "random.h"
#include "lsb.h"
#include "lsb_Device_Config.h"
#include "Zdp\zigbee_Zdp.h"
#include "log.h"

#ifndef __EMBERSTATUS_TYPE__
#define __EMBERSTATUS_TYPE__
typedef uint8_t EmberStatus;
#endif // __EMBERSTATUS_TYPE__

extern EmberStatus emberLeaveNetwork(void);
extern void emberActivateCommissioningLedEventControl(void);
extern void emberBroadcastSetMinAcksNeeded(uint8_t minAcksNeeded);

extern void zigbeeStack_NetworkSteeringSetChannelMask(uint32_t mask, bool secondaryMask);
extern void zigbeeStack_NetworkSteeringSetExtendedPanIdFilter(uint8_t* extendedPanId, bool turnFilterOn);

extern void InternalWriteServerAttribute(uint8_t  endpoint,
                                         uint16_t cluster,
                                         uint16_t attributeId,
                                         void *   data);

extern void InternalWriteManufacturerSpecificServerAttribute(uint8_t  endpoint,
                                                             uint16_t cluster,
                                                             uint16_t attributeId,
                                                             void *   data,
                                                             uint16_t manufacturerCode);

extern uint8_t emAllowRelay;

/******************************************************************************
* LOCAL MACROS AND CONSTANTS
******************************************************************************/

//#define _DEBUG

#define COMMISSIONING_CLUSTERREVISION      (0x0001u)

/*!
 * @brief Enables FSM logging (0=disabled, 1=enabled)
 */
#define LOGGING_ENABLED  (0)

/*!
 * @brief The timeout on ZigBee response messages.
 */
#define RESPONSE_TIMEOUT (10 * 1000)

/*!
 * @brief The timeout on ZigBee response messages.
 */
#define RF_RESPONSE_TIMEOUT (5 * 1000)

/*!
 * @brief The macro for min to millsec
 */
#define MINUTES_TO_MILLI_SEC (60 * 1000)

/*!
* @brief Network rejoin start up control value
 */
#define STARTUP_CONTROL_2     (2) //Network rejoin

/*!
* @brief Network unsec join start up control value
 */
#define STARTUP_CONTROL_3     (3) //Network unsec join

#define WAIT_TIME_TO_SEND_COMMAND_RESONSE (2) //wait time to send restart cmd response (sec)

// Copied from ZCL definitions.

#define ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESTART_DEVICE                0x00u
#define ZCL_COMMAND_COMMISSIONINGATTR_SERVER_SAVE_STARTUP_PARAMS           0x01u
#define ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESTORE_STARTUP_PARAMS        0x02u
#define ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESET_STARTUP_PARAMS          0x03u
//#define ZCL_COMMAND_COMMISSIONINGATTR_SERVER_MANSPECIFIC_DEVICE_PUBLISH    0x01u

#define PERSIST_DELAY                (2000)   // 2 sec
#define EMBER_RESET_NETWORK_DELAY    (100)    // 100 milli sec

#define ZIGBEE_CHANNEL_MASK          (0x07FFF800)   // channel 11 till 26

/******************************************************************************
* EXPORTED VARIABLES
******************************************************************************/

/*!
 * @brief State machine instance for FSM Controller
 */
fsmh_StateMachine_t  l_zigbee_commissioningattrserver_fsmController;

/******************************************************************************
* LOCAL TYPES
******************************************************************************/

/******************************************************************************
* LOCAL VARIABLES
******************************************************************************/

// Event timers
static evdisp_TimerId_t s_delayTimer;

// Commissioning cluster attributes
static commParams_t  s_CurrentCommParams;

// Restart device command data
static uint8_t  s_startupMode;
//static bool_t   s_immediate;
static uint8_t  s_delay;
static uint8_t  s_jitter;

// Reset startup parameters command data
static bool_t   s_resetcurrent;
static bool_t   s_resetall;
static bool_t   s_eraseindex;

// parameters available for each command
static uint8_t  s_index;
static zigbee_Commissioning_AddressInfo_t s_addressInfo;

/******************************************************************************
* LOCAL FUNCTION DECLARATIONS
******************************************************************************/

static void UpdateAttributesInZigbeeStack( bool_t allAttributes, uint16_t attributeId );
static void SetStartupParamsToFactoryDefaults( void );
static void StartDelayTimer(uint32_t delay);
static void DoSendReadAttributeResponse(const zigbee_commissioningattrserver_PvtEventData_ReadAttribute_t* pEventData,
                                        zigbee_Zcl_Status_t status,zigbee_Zcl_AttributeData_t attributeData );
//static void DoSendWriteAttributeResponse(const zigbee_commissioningattrserver_PvtEventData_WriteAttribute_t* pEventData,
//                                         zigbee_Zcl_Status_t status);
static void ConfigureRFRebroadcastParameters(void);

/******************************************************************************
* GENERIC GUARD/ACTION HANDLERS
******************************************************************************/

bool_t zigbee_commissioningattrserver_Controller_GuardHandler( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    return zigbee_commissioningattrserver_Controller_InnerGuardHandler( pStateMachine, pEventData );
}

void zigbee_commissioningattrserver_Controller_ActionHandler( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    zigbee_commissioningattrserver_Controller_InnerActionHandler( pStateMachine, pEventData );
}

/******************************************************************************
* GUARD/ACTION HANDLERS
******************************************************************************/

FSMH_HANDLER void DoSendUnsupportReadAttribute( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEventData_ReadAttribute_t* pEventData )
{
    (void)pStateMachine;

    zigbee_Zcl_AttributeData_t attributeData;

    attributeData.attributeId    = pEventData->info.attributeId;
    attributeData.dataType       = zigbee_Zcl_AttributeDataType_UNSIGNEDINTEGER_8bit;
    attributeData.data.dataUint8 = 0;
    DoSendReadAttributeResponse(pEventData, ZCL_STATUS_UNSUPPORTED_ATTRIBUTE, attributeData );
}

FSMH_HANDLER void ResetNetwork( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    (void)emberLeaveNetwork();
}

FSMH_HANDLER void ReadAttributeRequest( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEventData_ReadAttribute_t* pEventData )
{
// From this moment the Silabs Zigbee stack will send the read attribute response message
#if 1
    (void)pStateMachine;
    (void)pEventData;
#else
    (void)pStateMachine;

    zigbee_Zcl_Status_t zclStatus = ZCL_STATUS_SUCCESS;
    zigbee_Zcl_AttributeData_t attributeData;

    attributeData.attributeId = pEventData->info.attributeId;

    switch (pEventData->info.attributeId)
    {
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_EXTENDEDPANID:
        {
            attributeData.dataType = zigbee_Zcl_AttributeDataType_MISCELLANEOUS_IEEEaddress;
            memcpy(&attributeData.data.dataUint64[0], &s_CurrentCommParams.extPanId, sizeof(zigbee_ExtPanId_t));
        }
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_CHANNELMASK:
        attributeData.dataType          = zigbee_Zcl_AttributeDataType_BITMAP_32bit;
        attributeData.data.dataBitmap32 = s_CurrentCommParams.channelmask;
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_STARTUPCONTROL:
        attributeData.dataType       = zigbee_Zcl_AttributeDataType_ENUMERATION_8bit;
        attributeData.data.dataUint8 = s_CurrentCommParams.startupControl;
        break;
//    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_AUTOJOININGENABLED:
//        attributeData.dataType = zigbee_Zcl_AttributeDataType_LOGICAL_Boolean;
//        if (zigbee_commissioningattrserver_Config.pfGetAutoJoin != NULL)
//        {
//            bool_t autojoinEnabled = false;
//            if (!zigbee_commissioningattrserver_Config.pfGetAutoJoin(&autojoinEnabled))
//            {
//                zclStatus = ZCL_STATUS_FAILURE;
//            }
//            attributeData.data.dataBoolean = autojoinEnabled;
//        }
//        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_ROUTEREQUESTRETRANSMISSION:
        attributeData.dataType         = zigbee_Zcl_AttributeDataType_LOGICAL_Boolean;
        attributeData.data.dataBoolean = zigbee_commissioningattrserver_Rebroadcasting.routeRequestRetransmission;
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_BROADCASTRETRANSMISSION:
        attributeData.dataType         = zigbee_Zcl_AttributeDataType_LOGICAL_Boolean;
        attributeData.data.dataBoolean = zigbee_commissioningattrserver_Rebroadcasting.broadcastRetransmission;
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_MULTICASTRETRANSMISSION:
        attributeData.dataType         = zigbee_Zcl_AttributeDataType_LOGICAL_Boolean;
        attributeData.data.dataBoolean = zigbee_commissioningattrserver_Rebroadcasting.multicastRetransmission;
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_PASSIVEACKTHRESHOLD:
        attributeData.dataType       = zigbee_Zcl_AttributeDataType_UNSIGNEDINTEGER_8bit;
        attributeData.data.dataUint8 = zigbee_commissioningattrserver_Rebroadcasting.passiveAckThreshold;
        break;
    default:
        attributeData.dataType       = zigbee_Zcl_AttributeDataType_UNSIGNEDINTEGER_8bit;
        attributeData.data.dataUint8 = 0;
        zclStatus = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
        break;
    }
    DoSendReadAttributeResponse(pEventData, zclStatus, attributeData );
#endif
}

FSMH_HANDLER void WriteAttributeRequest( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEventData_WriteAttribute_t* pEventData )
{
    (void)pStateMachine;

    zigbee_Zcl_Status_t zclStatus = ZCL_STATUS_SUCCESS;
//    bool_t needResponse = pEventData->needResponse;
    bool_t rebroadcastParamChanged = false;
    serialization_ClassType_t persistentAttributeType = (serialization_ClassType_t)0;

    switch (pEventData->info.attributeId)
    {
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_EXTENDEDPANID:
        memcpy(&s_CurrentCommParams.extPanId, pEventData->attributeData.data.dataUint64, sizeof(zigbee_ExtPanId_t));
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_CHANNELMASK:
        {
            uint32_t channelmask = pEventData->attributeData.data.dataBitmap32;
            if ((channelmask & ZIGBEE_CHANNEL_MASK) == 0)
            {
                // Enable all channels when non was enabled
                channelmask = ZIGBEE_CHANNEL_MASK; // More safe then default value!
            }
            s_CurrentCommParams.channelmask = channelmask;
        }
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_STARTUPCONTROL:
        {
            int res;
            if (pEventData->attributeData.data.dataUint8 == STARTUP_CONTROL_2) // Startup control value 2
            {
                res =  memcmp(&s_CurrentCommParams.extPanId, &zigbee_commissioningattrserver_Config.defaultExtPanId, sizeof(zigbee_ExtPanId_t));
                if (res == 0)
                {
                    zclStatus = ZCL_STATUS_INCONSISTENT_STARTUP_STATE;
                }
            }
            if (zclStatus == ZCL_STATUS_SUCCESS)
            {
                s_CurrentCommParams.startupControl = pEventData->attributeData.data.dataUint8;
            }
        }
        break;
//    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_AUTOJOININGENABLED:
//        zclStatus = ZCL_STATUS_FAILURE;
//        if (zigbee_commissioningattrserver_Config.pfSetAutoJoin != NULL)
//        {
//            if (zigbee_commissioningattrserver_Config.pfSetAutoJoin(pEventData->attributeData.data.dataBoolean))
//            {
//                zclStatus = ZCL_STATUS_SUCCESS;
//            }
//        }
//        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_ROUTEREQUESTRETRANSMISSION:
        zigbee_commissioningattrserver_Rebroadcasting.routeRequestRetransmission =
                    pEventData->attributeData.data.dataBoolean;
        persistentAttributeType = serialization_ClassType_CommParams_Rebroadcasting;
        rebroadcastParamChanged = true;
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_BROADCASTRETRANSMISSION:
        zigbee_commissioningattrserver_Rebroadcasting.broadcastRetransmission =
                    pEventData->attributeData.data.dataBoolean;
        persistentAttributeType = serialization_ClassType_CommParams_Rebroadcasting;
        rebroadcastParamChanged = true;
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_MULTICASTRETRANSMISSION:
        zigbee_commissioningattrserver_Rebroadcasting.multicastRetransmission =
                    pEventData->attributeData.data.dataBoolean;
        persistentAttributeType = serialization_ClassType_CommParams_Rebroadcasting;
        rebroadcastParamChanged = true;
        break;
    case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_PASSIVEACKTHRESHOLD:
        zigbee_commissioningattrserver_Rebroadcasting.passiveAckThreshold =
                    pEventData->attributeData.data.dataUint8;
        persistentAttributeType = serialization_ClassType_CommParams_Rebroadcasting;
        rebroadcastParamChanged = true;
        break;
    default:
//        needResponse = true;
        zclStatus = ZCL_STATUS_UNSUPPORTED_ATTRIBUTE;
        break;
    }

    if ((zigbee_commissioningattrserver_Config.persistencyWriteCallback != NULL) &&
        (zclStatus == ZCL_STATUS_SUCCESS) && (persistentAttributeType != (serialization_ClassType_t) 0))
    {
        zigbee_commissioningattrserver_Config.persistencyWriteCallback(persistentAttributeType);
    }
//    if (needResponse == true)
//    {
//        DoSendWriteAttributeResponse(pEventData, zclStatus);
//    }
    if (rebroadcastParamChanged)
    {
        ConfigureRFRebroadcastParameters();
    }
    (void)zclStatus;
}

FSMH_HANDLER void SendRestartDeviceResponse( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    zigbee_Zcl_Status_t Status = ZCL_STATUS_SUCCESS;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESTART_DEVICE, (uint8_t)Status);
}

FSMH_HANDLER void SendResetStartupParametersResponse( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    zigbee_Zcl_Status_t Status = ZCL_STATUS_SUCCESS;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESET_STARTUP_PARAMS, (uint8_t)Status);
}

FSMH_HANDLER void SendSaveStartupParametersResponse( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    zigbee_Zcl_Status_t status = ZCL_STATUS_SUCCESS;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_SAVE_STARTUP_PARAMS, (uint8_t)status);
}

FSMH_HANDLER void RestoreCurrentStartupParameters( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    if (!zigbee_commissioningattrserver_Map_Params_GetCurrent(&s_CurrentCommParams))
    {
        SetStartupParamsToFactoryDefaults();
        (void)zigbee_commissioningattrserver_Map_Params_TableClear();
        (void)zigbee_commissioningattrserver_Map_Params_AddOrUpdateCurrent(&s_CurrentCommParams);
    }
    UpdateAttributesInZigbeeStack(true, 0);
}

FSMH_HANDLER void SetStartupParametersToDefaults( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    SetStartupParamsToFactoryDefaults();
    (void)zigbee_commissioningattrserver_Map_Params_AddOrUpdateCurrent(&s_CurrentCommParams);
    (void)zigbee_commissioningattrserver_Map_Params_AddOrUpdate(s_index, &s_CurrentCommParams);
    UpdateAttributesInZigbeeStack(true, 0);
}

FSMH_HANDLER void SetupRFRebroadcastParameters( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    ConfigureRFRebroadcastParameters();
}

FSMH_HANDLER void StartDelayWithJitter( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    uint32_t delay;

    if (s_delay > WAIT_TIME_TO_SEND_COMMAND_RESONSE)
    {
        uint32_t rand_num = 0;
        if (s_jitter > 0)
        {
            rand_num = random_Get(0, (s_jitter * 80)); //ms
        }
        delay = (s_delay * 1000) + rand_num; //delay also in ms (delay * 1000)
    }
    else
    {
        /* Delay for some minimum time for the restart device response command to be sent out */
        delay = WAIT_TIME_TO_SEND_COMMAND_RESONSE * 1000;
    }
    StartDelayTimer(delay);
}

FSMH_HANDLER void StartPersistDelay( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    StartDelayTimer(PERSIST_DELAY);
}

FSMH_HANDLER void StartResetNetworkDelay( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    StartDelayTimer(EMBER_RESET_NETWORK_DELAY);
}

FSMH_HANDLER bool_t IfEraseIndex( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    return s_eraseindex;
}

FSMH_HANDLER bool_t IfResetAll( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    return s_resetall;
}

FSMH_HANDLER bool_t IfRestartCurrentParemeters( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    // If s_startupMode is zero then restart with CurrentParameters / If 1 then reset with network parameters.
    return (s_startupMode == 0);
}

FSMH_HANDLER bool_t IfResetCurrent( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    return s_resetcurrent;
}

FSMH_HANDLER void RestoreStartupParameters( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    if (zigbee_commissioningattrserver_Map_Params_Get(s_index, &s_CurrentCommParams))
    {
//        (void)zigbee_commissioningattrserver_Map_Params_AddOrUpdateCurrent(&s_CurrentCommParams);
        UpdateAttributesInZigbeeStack(true, 0);
    }
}

FSMH_HANDLER void SendRestoreFailure( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    zigbee_Zcl_Status_t status = ZCL_STATUS_FAILURE;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESTORE_STARTUP_PARAMS, (uint8_t)status);
}

FSMH_HANDLER void SendRestoreSuccess( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    zigbee_Zcl_Status_t status = ZCL_STATUS_SUCCESS;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESTORE_STARTUP_PARAMS, (uint8_t)status);
}

FSMH_HANDLER void SendSaveStartupParamsFailure( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    zigbee_Zcl_Status_t status = ZCL_STATUS_INSUFFICIENT_SPACE;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_SAVE_STARTUP_PARAMS, (uint8_t)status);
}

FSMH_HANDLER bool_t IfIndexExists( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    commParams_t params;
    return zigbee_commissioningattrserver_Map_Params_Get(s_index, &params);
}

FSMH_HANDLER bool_t IfFreeIndex( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    uint8_t count = zigbee_commissioningattrserver_Map_Params_GetCount();
    return (count < COMM_PARAMS_MAP_RECORDCOUNT);
}

FSMH_HANDLER void StoreRestartParameters( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEvent_RestartDeviceCommand_t* pEventData )
{
    (void)pStateMachine;

    s_startupMode = pEventData->startupMode;
//    s_immediate   = pEventData->immediate;
    s_delay       = pEventData->delay;
    s_jitter      = pEventData->jitter;
    s_addressInfo = pEventData->AddressInfo;
}

FSMH_HANDLER void StoreSaveStartupParameters( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEvent_SaveStartupParameters_t* pEventData )
{
    (void)pStateMachine;

    s_index       = pEventData->index;
    s_addressInfo = pEventData->AddressInfo;
}

FSMH_HANDLER void StoreRestoreParameters( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEvent_RestoreStartupParameters_t* pEventData )
{
    (void)pStateMachine;

    s_index       = pEventData->index;
    s_addressInfo = pEventData->AddressInfo;
}

FSMH_HANDLER void StoreResetStartupParameters( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEvent_ResetStartupParameters_t* pEventData )
{
    (void)pStateMachine;

    s_resetcurrent = pEventData->resetcurrent;
    s_resetall     = pEventData->resetall;
    s_eraseindex   = pEventData->eraseindex;
    s_index        = pEventData->index;
    s_addressInfo  = pEventData->AddressInfo;
}

FSMH_HANDLER void EraseIndexedAttributeFromPersistency( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    (void)zigbee_commissioningattrserver_Map_Params_Delete(s_index);
}

FSMH_HANDLER void ResetCurrentParameters( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    SetStartupParamsToFactoryDefaults();
    UpdateAttributesInZigbeeStack(true, 0);
}

FSMH_HANDLER void ResetAllParameterSets( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    SetStartupParamsToFactoryDefaults();
    (void)zigbee_commissioningattrserver_Map_Params_UpdateAll(&s_CurrentCommParams);
    UpdateAttributesInZigbeeStack(true, 0);
}

FSMH_HANDLER void DeleteAllParameterSets( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    (void)zigbee_commissioningattrserver_Map_Params_TableClear();
}

FSMH_HANDLER void PersistCurrentParameters( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    (void)zigbee_commissioningattrserver_Map_Params_AddOrUpdateCurrent(&s_CurrentCommParams);
}

FSMH_HANDLER void SetupCommParams( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    uint32_t channelmask = s_CurrentCommParams.channelmask;

    if ((channelmask & ZIGBEE_CHANNEL_MASK) == 0)
    {
        // Enable all channels when non was enabled
        channelmask = ZIGBEE_CHANNEL_MASK; // More safe then default value!
    }
    log_Printfbasic("ChannelMask,0x%08X", channelmask);

    // set the channel mask in the Ember stack
//    zigbeeStack_NetworkSteeringSetChannelMask(channelmask, false); // primary channel
//    channelmask = (~channelmask & ZIGBEE_CHANNEL_MASK);
//    zigbeeStack_NetworkSteeringSetChannelMask(channelmask, true);  // secondary channel

    // set the extended PAN id in the Ember stack
    log_Printfbasic("extPANID,%02X%02X%02X%02X%02X%02X%02X%02X",
                    s_CurrentCommParams.extPanId.b[7],
                    s_CurrentCommParams.extPanId.b[6],
                    s_CurrentCommParams.extPanId.b[5],
                    s_CurrentCommParams.extPanId.b[4],
                    s_CurrentCommParams.extPanId.b[3],
                    s_CurrentCommParams.extPanId.b[2],
                    s_CurrentCommParams.extPanId.b[1],
                    s_CurrentCommParams.extPanId.b[0]);
    int i;
    uint8_t value = 0xFF;
    for (i=0;i<8;i++)
    {
        value &= s_CurrentCommParams.extPanId.b[i];
    }
    if (value == 0xFF)
    {
        log_Printfbasic("Don't use the default extPanId");
    }
    else
    {
        zigbeeStack_NetworkSteeringSetExtendedPanIdFilter((uint8_t*)&s_CurrentCommParams.extPanId, true);
    }

    // Now enable the channel scanning in the Ember stack
    emberActivateCommissioningLedEventControl();
}

FSMH_HANDLER void PersistStartupParameters( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    (void)zigbee_commissioningattrserver_Map_Params_AddOrUpdate(s_index, &s_CurrentCommParams);
}

FSMH_HANDLER void SendInconsistentReadAttributeResponse( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEventData_ReadAttribute_t* pEventData )
{
    (void)pStateMachine;

    zigbee_Zcl_AttributeData_t attributeData;

    attributeData.attributeId    = pEventData->info.attributeId;
    attributeData.dataType       = zigbee_Zcl_AttributeDataType_UNSIGNEDINTEGER_8bit;
    attributeData.data.dataUint8 = 0;
    DoSendReadAttributeResponse(pEventData, ZCL_STATUS_INCONSISTENT_STARTUP_STATE, attributeData );
}

FSMH_HANDLER void SendInconsistentResetResponse( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEvent_ResetStartupParameters_t* pEventData )
{
    (void)pStateMachine;

    zigbee_Zcl_Status_t Status = ZCL_STATUS_INCONSISTENT_STARTUP_STATE;
    s_addressInfo = pEventData->AddressInfo;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESET_STARTUP_PARAMS, (uint8_t)Status);
}

FSMH_HANDLER void SendInconsistentRestartResponse( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEvent_RestartDeviceCommand_t* pEventData )
{
    (void)pStateMachine;

    zigbee_Zcl_Status_t Status = ZCL_STATUS_INCONSISTENT_STARTUP_STATE;
    s_addressInfo = pEventData->AddressInfo;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESTART_DEVICE, (uint8_t)Status);
}

FSMH_HANDLER void SendInconsistentRestoreResponse( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEvent_RestoreStartupParameters_t* pEventData )
{
    (void)pStateMachine;

    zigbee_Zcl_Status_t Status = ZCL_STATUS_INCONSISTENT_STARTUP_STATE;
    s_addressInfo = pEventData->AddressInfo;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_RESTORE_STARTUP_PARAMS, (uint8_t)Status);
}

FSMH_HANDLER void SendInconsistentSaveStartupResponse( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEvent_SaveStartupParameters_t* pEventData )
{
    (void)pStateMachine;

    zigbee_Zcl_Status_t Status = ZCL_STATUS_INCONSISTENT_STARTUP_STATE;
    s_addressInfo = pEventData->AddressInfo;
    zigbee_commissioningServer_Response(&s_addressInfo, ZCL_COMMAND_COMMISSIONINGATTR_SERVER_SAVE_STARTUP_PARAMS, (uint8_t)Status);
}

FSMH_HANDLER void SendInconsistentWriteAttributeResponse( fsmh_StateMachine_t* pStateMachine, const zigbee_commissioningattrserver_PvtEventData_WriteAttribute_t* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    log_Printfbasic("InconsistentWriteAttribute");
//    DoSendWriteAttributeResponse(pEventData, ZCL_STATUS_INCONSISTENT_STARTUP_STATE);
}

FSMH_HANDLER void DoDeviceReboot( fsmh_StateMachine_t* pStateMachine, const void* pEventData )
{
    (void)pStateMachine;
    (void)pEventData;

    lsb_Device_Commands.Reboot(lsb_UniverseId_Product, 0);
}

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/

static void UpdateAttributesInZigbeeStack( bool_t allAttributes, uint16_t attributeId )
{
    if (allAttributes)
    {
        UpdateAttributesInZigbeeStack(false, ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_EXTENDEDPANID);
        UpdateAttributesInZigbeeStack(false, ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_CHANNELMASK);
        UpdateAttributesInZigbeeStack(false, ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_STARTUPCONTROL);
        UpdateAttributesInZigbeeStack(false, ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_BROADCASTRETRANSMISSION);
        UpdateAttributesInZigbeeStack(false, ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_MULTICASTRETRANSMISSION);
        UpdateAttributesInZigbeeStack(false, ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_ROUTEREQUESTRETRANSMISSION);
        UpdateAttributesInZigbeeStack(false, ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_PASSIVEACKTHRESHOLD);
        UpdateAttributesInZigbeeStack(false, ZIGBEE_ZCL_GLOBAL_ATTRIBUTE_CLUSTER_REVISION);
    }
    else
    {
        void*    pData = NULL;
        zigbee_ExtPanId_t extPanId;
        uint32_t channelmask;
        uint8_t  startupControl;
        bool_t   manufacturerSpecific = false;
        uint16_t revision = 0;

        switch (attributeId)
        {
        case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_EXTENDEDPANID:
            extPanId = s_CurrentCommParams.extPanId;
            pData = (void*)&extPanId;
            break;
        case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_CHANNELMASK:
            channelmask = s_CurrentCommParams.channelmask;
            pData = (void*)&channelmask;
            break;
        case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_STARTUPCONTROL:
            startupControl = s_CurrentCommParams.startupControl;
            pData = (void*)&startupControl;
            break;
        case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_BROADCASTRETRANSMISSION:
            pData = (void*)&zigbee_commissioningattrserver_Rebroadcasting.broadcastRetransmission;
            manufacturerSpecific = true;
            break;
        case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_MULTICASTRETRANSMISSION:
            pData = (void*)&zigbee_commissioningattrserver_Rebroadcasting.multicastRetransmission;
            manufacturerSpecific = true;
            break;
        case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_ROUTEREQUESTRETRANSMISSION:
            pData = (void*)&zigbee_commissioningattrserver_Rebroadcasting.routeRequestRetransmission;
            manufacturerSpecific = true;
            break;
        case ZIGBEE_ZCL_COMMISSIONINGATTRSERVER_ATTRIBUTEID_MANSPECIFIC_PASSIVEACKTHRESHOLD:
            pData = (void*)&zigbee_commissioningattrserver_Rebroadcasting.passiveAckThreshold;
            manufacturerSpecific = true;
            break;
        case ZIGBEE_ZCL_GLOBAL_ATTRIBUTE_CLUSTER_REVISION:
            revision = COMMISSIONING_CLUSTERREVISION;
            pData = (void*)&revision;
            break;
        default:
            break;
        }
        if (pData != NULL)
        {
            if (manufacturerSpecific)
            {
                uint16_t manufacturerCode = ZIGBEE_ZCL_MANUFACTURERCODE_PHILIPS;
                (void)InternalWriteManufacturerSpecificServerAttribute(64,
                             ZIGBEE_ZCL_CLUSTERID_COMMISSIONING, attributeId, pData, manufacturerCode);
            }
            else
            {
                (void)InternalWriteServerAttribute(64,
                             ZIGBEE_ZCL_CLUSTERID_COMMISSIONING, attributeId, pData);
            }
        }
    }
}

static void SetStartupParamsToFactoryDefaults( void )
{
    memcpy(&s_CurrentCommParams.extPanId,
           &zigbee_commissioningattrserver_Config.defaultExtPanId,
           sizeof(zigbee_ExtPanId_t));
    s_CurrentCommParams.channelmask    = zigbee_commissioningattrserver_Config.defaultChannelmask;
    s_CurrentCommParams.startupControl = zigbee_commissioningattrserver_Config.defaultStartupControl;
}

static void StartDelayTimer(uint32_t delay)
{
    if (zigbee_commissioningattrserver_TimerIsActive(s_delayTimer))
    {
        zigbee_commissioningattrserver_TimerStop(s_delayTimer);
    }
    zigbee_commissioningattrserver_TimerSet(s_delayTimer, delay);
    zigbee_commissioningattrserver_TimerStart(s_delayTimer);
}

//static void DoSendWriteAttributeResponse(const zigbee_commissioningattrserver_PvtEventData_WriteAttribute_t* pEventData,
//                                         zigbee_Zcl_Status_t status)
//{
//    zigbee_Zcl_AttributeData_t   attribute;
//
//    attribute.attributeId = pEventData->attributeData.attributeId;
//
//    /* Send Attribute Response only for Write Attribute*/
//    if(pEventData->needResponse)
//    {
//        (void) zigbee_Foundation_Server.sendWriteAttributeResponse( pEventData->info.transSeqNum,
//                                                                    &pEventData->info.srcAddress,
//                                                                    pEventData->info.endpoint,
//                                                                    pEventData->info.clusterId,
//                                                                    pEventData->info.manufacturerCode,
//                                                                    status,
//                                                                    &attribute);
//    }
//}

static void DoSendReadAttributeResponse(const zigbee_commissioningattrserver_PvtEventData_ReadAttribute_t* pEventData,
                                        zigbee_Zcl_Status_t status,zigbee_Zcl_AttributeData_t attributeData )
{
    (void)zigbee_Foundation_Server.sendReadAttributeResponse( pEventData->info.transSeqNum,
                                                              &pEventData->info.srcAddress,
                                                              pEventData->info.endpoint,
                                                              pEventData->info.clusterId,
                                                              pEventData->info.manufacturerCode,
                                                              status,
                                                              &attributeData );
}

static void ConfigureRFRebroadcastParameters(void)
{
    emberBroadcastSetMinAcksNeeded(zigbee_commissioningattrserver_Rebroadcasting.passiveAckThreshold);
    if (zigbee_commissioningattrserver_Rebroadcasting.broadcastRetransmission &&
        zigbee_commissioningattrserver_Rebroadcasting.multicastRetransmission &&
        zigbee_commissioningattrserver_Rebroadcasting.routeRequestRetransmission)
    {
        emAllowRelay = true;
    }
    else
    {
        emAllowRelay = false;
    }
}

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/

void zigbee_commissioningattrserver_ControllerInit( void )
{
    (void)fsmh_StateMachineCreate( &l_zigbee_commissioningattrserver_fsmController, &l_zigbee_commissioningattrserver_ControllerDef );
}

void zigbee_commissioningattrserver_ControllerTurnOn( void )
{
    s_delayTimer       = zigbee_commissioningattrserver_TimerCreate( zigbee_commissioningattrserver_PvtEvent_Timeout, RESPONSE_TIMEOUT );

#if defined(_DEBUG) && (LOGGING_ENABLED == 1)
    fsmh_EnableLogging(&l_zigbee_commissioningattrserver_fsmController);
#endif

    // Remove below line if the state machine should not be started on the turn-on
    zigbee_commissioningattrserver_ControllerStartStateMachine();
}

inline void zigbee_commissioningattrserver_ControllerStartStateMachine( void )
{
    fsmh_StateMachineStart( &l_zigbee_commissioningattrserver_fsmController, NULL );
}

inline void zigbee_commissioningattrserver_ControllerStopStateMachine( void )
{
    fsmh_StateMachineStop( &l_zigbee_commissioningattrserver_fsmController );
}

inline void zigbee_commissioningattrserver_ControllerEvaluateStateMachine( evdisp_EventId_t eventId, const void* pEventData )
{
    fsmh_StateMachineEvaluate( &l_zigbee_commissioningattrserver_fsmController, (fsmh_Stimulus_t)eventId, pEventData );
}

inline int zigbee_commissioningattrserver_ControllerNrOfInstances( void )
{
    return 1;
}
