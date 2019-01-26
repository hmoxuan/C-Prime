// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef __APP_CWP_DONGLE_H__
#define __APP_CWP_DONGLE_H__


/**** Included Header Section ****/

/**** ZCL Section ****/
#define ZA_PROMPT "c4m_dongle"
#define ZCL_USING_BASIC_CLUSTER_CLIENT
#define ZCL_USING_BASIC_CLUSTER_SERVER
#define ZCL_USING_IDENTIFY_CLUSTER_CLIENT
#define ZCL_USING_IDENTIFY_CLUSTER_SERVER
#define ZCL_USING_GROUPS_CLUSTER_CLIENT
#define ZCL_USING_GROUPS_CLUSTER_SERVER
#define ZCL_USING_SCENES_CLUSTER_CLIENT
#define ZCL_USING_SCENES_CLUSTER_SERVER
#define ZCL_USING_ON_OFF_CLUSTER_CLIENT
#define ZCL_USING_ON_OFF_CLUSTER_SERVER
#define ZCL_USING_LEVEL_CONTROL_CLUSTER_CLIENT
#define ZCL_USING_LEVEL_CONTROL_CLUSTER_SERVER
#define ZCL_USING_OTA_BOOTLOAD_CLUSTER_SERVER
#define ZCL_USING_COMMISSIONING_CLUSTER_CLIENT
#define ZCL_USING_GREEN_POWER_CLUSTER_CLIENT
#define ZCL_USING_GREEN_POWER_CLUSTER_SERVER
#define ZCL_USING_COLOR_CONTROL_CLUSTER_SERVER
#define ZCL_USING_ZLL_COMMISSIONING_CLUSTER_CLIENT
#define ZCL_USING_ZLL_COMMISSIONING_CLUSTER_SERVER
#define ZCL_USING_ZCL_C4M_SPECIFIC_CLUSTER_CLIENT
#define ZCL_USING_ZCL_TIMER_CLUSTER_SERVER
#define ZCL_USING_ZCL_TIMER_CLUSTER_CLIENT
#define ZCL_USING_ZCL_LIGHT_CONTROL_CLUSTER_CLIENT
#define ZCL_USING_ZCL_DAYLIGHTREGULATION_CLUSTER_CLIENT
/**** Optional Attributes ****/
#define ZCL_USING_BASIC_CLUSTER_APPLICATION_VERSION_ATTRIBUTE 
#define ZCL_USING_BASIC_CLUSTER_STACK_VERSION_ATTRIBUTE 
#define ZCL_USING_BASIC_CLUSTER_HW_VERSION_ATTRIBUTE 
#define ZCL_USING_BASIC_CLUSTER_MANUFACTURER_NAME_ATTRIBUTE 
#define ZCL_USING_BASIC_CLUSTER_MODEL_IDENTIFIER_ATTRIBUTE 
#define ZCL_USING_BASIC_CLUSTER_DATE_CODE_ATTRIBUTE 
#define ZCL_USING_BASIC_CLUSTER_SW_BUILD_ID_ATTRIBUTE 
#define ZCL_USING_ON_OFF_CLUSTER_GLOBAL_SCENE_CONTROL_ATTRIBUTE 
#define ZCL_USING_ON_OFF_CLUSTER_ON_TIME_ATTRIBUTE 
#define ZCL_USING_ON_OFF_CLUSTER_OFF_WAIT_TIME_ATTRIBUTE 
#define ZCL_USING_LEVEL_CONTROL_CLUSTER_LEVEL_CONTROL_REMAINING_TIME_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_CURRENT_HUE_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_CURRENT_SATURATION_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_REMAINING_TIME_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_TEMPERATURE_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_MODE_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_NUMBER_OF_PRIMARIES_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_1_X_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_1_Y_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_1_INTENSITY_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_2_X_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_2_Y_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_2_INTENSITY_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_3_X_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_3_Y_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_3_INTENSITY_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_4_X_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_4_Y_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_4_INTENSITY_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_5_X_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_5_Y_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_5_INTENSITY_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_6_X_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_6_Y_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_PRIMARY_6_INTENSITY_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_ENHANCED_CURRENT_HUE_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_ENHANCED_COLOR_MODE_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_LOOP_ACTIVE_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_LOOP_DIRECTION_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_LOOP_TIME_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_LOOP_START_ENHANCED_HUE_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_LOOP_STORED_ENHANCED_HUE_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_CAPABILITIES_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_ATTRIBUTE 
#define ZCL_USING_COLOR_CONTROL_CLUSTER_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_ATTRIBUTE 
#define EMBER_AF_MANUFACTURER_CODE 0x1002
#define EMBER_AF_DEFAULT_RESPONSE_POLICY_ALWAYS

/**** Cluster endpoint counts ****/
#define EMBER_AF_BASIC_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_BASIC_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_IDENTIFY_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_GROUPS_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_GROUPS_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_SCENES_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_SCENES_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_ON_OFF_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_ON_OFF_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_LEVEL_CONTROL_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_LEVEL_CONTROL_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_OTA_BOOTLOAD_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_COMMISSIONING_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_GREEN_POWER_CLUSTER_CLIENT_ENDPOINT_COUNT (1)
#define EMBER_AF_GREEN_POWER_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_COLOR_CONTROL_CLUSTER_SERVER_ENDPOINT_COUNT (1)
#define EMBER_AF_ZLL_COMMISSIONING_CLUSTER_CLIENT_ENDPOINT_COUNT (2)
#define EMBER_AF_ZLL_COMMISSIONING_CLUSTER_SERVER_ENDPOINT_COUNT (1)

/**** CLI Section ****/
#define EMBER_AF_GENERATE_CLI

/**** Security Section ****/
#define EMBER_AF_HAS_SECURITY_PROFILE_Z3

/**** Network Section ****/
#define EMBER_SUPPORTED_NETWORKS (1)
#define EMBER_AF_NETWORK_INDEX_PRIMARY (0)
#define EMBER_AF_DEFAULT_NETWORK_INDEX EMBER_AF_NETWORK_INDEX_PRIMARY
#define EMBER_AF_HAS_COORDINATOR_NETWORK
#define EMBER_AF_HAS_ROUTER_NETWORK
#define EMBER_AF_HAS_RX_ON_WHEN_IDLE_NETWORK
#define EMBER_AF_TX_POWER_MODE EMBER_TX_POWER_MODE_USE_TOKEN

/**** HAL Section ****/
//#define ZA_CLI_FULL

/**** Callback Section ****/
#define EMBER_CALLBACK_MAIN_INIT
#define EMBER_CALLBACK_STACK_STATUS
#define EMBER_CALLBACK_ON_OFF_CLUSTER_ON_OFF_CLUSTER_SERVER_ATTRIBUTE_CHANGED
#define EMBER_CALLBACK_ON_OFF_CLUSTER_ON_OFF_CLUSTER_SERVER_INIT
#define EMBER_CALLBACK_HAL_BUTTON_ISR
#define EMBER_CALLBACK_BASIC_CLUSTER_RESET_TO_FACTORY_DEFAULTS
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_NOTIFICATION_RESPONSE
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_PAIRING
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_PROXY_COMMISSIONING_MODE
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_RESPONSE
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_SINK_TABLE_RESPONSE
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_PROXY_TABLE_REQUEST
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_NOTIFICATION
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_COMMISSIONING_NOTIFICATION
#define EMBER_CALLBACK_GREEN_POWER_CLUSTER_GP_SINK_COMMISSIONING_MODE
#define EMBER_CALLBACK_GROUPS_CLUSTER_GROUPS_CLUSTER_SERVER_INIT
#define EMBER_CALLBACK_GROUPS_CLUSTER_ADD_GROUP
#define EMBER_CALLBACK_GROUPS_CLUSTER_VIEW_GROUP
#define EMBER_CALLBACK_GROUPS_CLUSTER_GET_GROUP_MEMBERSHIP
#define EMBER_CALLBACK_GROUPS_CLUSTER_REMOVE_GROUP
#define EMBER_CALLBACK_GROUPS_CLUSTER_REMOVE_ALL_GROUPS
#define EMBER_CALLBACK_GROUPS_CLUSTER_ADD_GROUP_IF_IDENTIFYING
#define EMBER_CALLBACK_GROUPS_CLUSTER_ENDPOINT_IN_GROUP
#define EMBER_CALLBACK_GROUPS_CLUSTER_CLEAR_GROUP_TABLE
#define EMBER_CALLBACK_IDENTIFY_CLUSTER_IDENTIFY_CLUSTER_SERVER_INIT
#define EMBER_CALLBACK_IDENTIFY_CLUSTER_IDENTIFY_CLUSTER_SERVER_TICK
#define EMBER_CALLBACK_IDENTIFY_CLUSTER_IDENTIFY_CLUSTER_SERVER_ATTRIBUTE_CHANGED
#define EMBER_CALLBACK_IDENTIFY_CLUSTER_IDENTIFY
#define EMBER_CALLBACK_IDENTIFY_CLUSTER_IDENTIFY_QUERY
#define EMBER_CALLBACK_MAC_FILTER_MATCH_MESSAGE
#define EMBER_APPLICATION_HAS_MAC_FILTER_MATCH_MESSAGE_HANDLER
#define EMBER_CALLBACK_EZSP_MAC_FILTER_MATCH_MESSAGE
#define EZSP_APPLICATION_HAS_MAC_FILTER_MATCH_HANDLER
#define EMBER_CALLBACK_INTERPAN_SEND_MESSAGE
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_LEVEL_CONTROL_CLUSTER_SERVER_TICK
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_MOVE_TO_LEVEL_WITH_ON_OFF
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_MOVE
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_MOVE_WITH_ON_OFF
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_STEP
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_STEP_WITH_ON_OFF
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_STOP
#define EMBER_CALLBACK_LEVEL_CONTROL_CLUSTER_STOP_WITH_ON_OFF
#define EMBER_CALLBACK_ON_OFF_CLUSTER_ON_OFF_CLUSTER_LEVEL_CONTROL_EFFECT
#define EMBER_CALLBACK_ON_OFF_CLUSTER_OFF
#define EMBER_CALLBACK_ON_OFF_CLUSTER_ON
#define EMBER_CALLBACK_ON_OFF_CLUSTER_TOGGLE
#define EMBER_CALLBACK_ON_OFF_CLUSTER_ON_OFF_CLUSTER_SET_VALUE
#define EMBER_CALLBACK_OTA_BOOTLOAD_CLUSTER_OTA_BOOTLOAD_CLUSTER_SERVER_INIT
#define EMBER_CALLBACK_OTA_BOOTLOAD_CLUSTER_OTA_BOOTLOAD_CLUSTER_SERVER_TICK
#define EMBER_CALLBACK_OTA_SERVER_INCOMING_MESSAGE_RAW
#define EMBER_CALLBACK_OTA_SERVER_SEND_IMAGE_NOTIFY
#define EMBER_CALLBACK_OTA_SERVER_QUERY
#define EMBER_CALLBACK_OTA_SERVER_BLOCK_SIZE
#define EMBER_CALLBACK_OTA_SERVER_UPGRADE_END_REQUEST
#define EMBER_CALLBACK_OTA_PAGE_REQUEST_SERVER_POLICY
#define EMBER_CALLBACK_ENERGY_SCAN_RESULT
#define EMBER_CALLBACK_SCAN_COMPLETE
#define EMBER_CALLBACK_NETWORK_FOUND
#define EMBER_CALLBACK_SCENES_CLUSTER_SCENES_CLUSTER_SERVER_INIT
#define EMBER_CALLBACK_SCENES_CLUSTER_ADD_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_VIEW_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_REMOVE_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_REMOVE_ALL_SCENES
#define EMBER_CALLBACK_SCENES_CLUSTER_STORE_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_RECALL_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_GET_SCENE_MEMBERSHIP
#define EMBER_CALLBACK_SCENES_CLUSTER_STORE_CURRENT_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_RECALL_SAVED_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_CLEAR_SCENE_TABLE
#define EMBER_CALLBACK_SCENES_CLUSTER_SCENES_CLUSTER_MAKE_INVALID
#define EMBER_CALLBACK_SCENES_CLUSTER_REMOVE_SCENES_IN_GROUP
#define EMBER_CALLBACK_ZIGBEE_KEY_ESTABLISHMENT
#define EMBER_APPLICATION_HAS_ZIGBEE_KEY_ESTABLISHMENT_HANDLER
#define EMBER_CALLBACK_EZSP_ZIGBEE_KEY_ESTABLISHMENT
#define EZSP_APPLICATION_HAS_ZIGBEE_KEY_ESTABLISHMENT_HANDLER
#define EMBER_CALLBACK_START_SEARCH_FOR_JOINABLE_NETWORK
#define EMBER_CALLBACK_FIND_UNUSED_PAN_ID_AND_FORM
#define EMBER_CALLBACK_UNUSED_PAN_ID_FOUND
#define EMBER_CALLBACK_SCAN_ERROR
#define EMBER_CALLBACK_GET_FORM_AND_JOIN_EXTENDED_PAN_ID
#define EMBER_CALLBACK_SET_FORM_AND_JOIN_EXTENDED_PAN_ID
#define EMBER_CALLBACK_ZLL_ADDRESS_ASSIGNMENT_HANDLER
#define EMBER_APPLICATION_HAS_ZLL_ADDRESS_ASSIGNMENT_HANDLER
#define EMBER_CALLBACK_ZLL_NETWORK_FOUND_HANDLER
#define EMBER_APPLICATION_HAS_ZLL_NETWORK_FOUND_HANDLER
#define EMBER_CALLBACK_ZLL_SCAN_COMPLETE_HANDLER
#define EMBER_APPLICATION_HAS_ZLL_SCAN_COMPLETE_HANDLER
#define EMBER_CALLBACK_ZLL_TOUCH_LINK_TARGET_HANDLER
#define EMBER_APPLICATION_HAS_ZLL_TOUCH_LINK_TARGET_HANDLER
#define EMBER_CALLBACK_EZSP_ZLL_ADDRESS_ASSIGNMENT_HANDLER
#define EZSP_APPLICATION_HAS_ZLL_ADDRESS_ASSIGNMENT_HANDLER
#define EMBER_CALLBACK_EZSP_ZLL_NETWORK_FOUND_HANDLER
#define EZSP_APPLICATION_HAS_ZLL_NETWORK_FOUND_HANDLER
#define EMBER_CALLBACK_EZSP_ZLL_SCAN_COMPLETE_HANDLER
#define EZSP_APPLICATION_HAS_ZLL_SCAN_COMPLETE_HANDLER
#define EMBER_CALLBACK_EZSP_ZLL_TOUCH_LINK_TARGET_HANDLER
#define EZSP_APPLICATION_HAS_ZLL_TOUCH_LINK_TARGET_HANDLER
#define EMBER_CALLBACK_IDENTIFY_CLUSTER_TRIGGER_EFFECT
#define EMBER_CALLBACK_ON_OFF_CLUSTER_ON_OFF_CLUSTER_SERVER_TICK
#define EMBER_CALLBACK_ON_OFF_CLUSTER_OFF_WITH_EFFECT
#define EMBER_CALLBACK_ON_OFF_CLUSTER_ON_WITH_RECALL_GLOBAL_SCENE
#define EMBER_CALLBACK_ON_OFF_CLUSTER_ON_WITH_TIMED_OFF
#define EMBER_CALLBACK_SCENES_CLUSTER_ENHANCED_ADD_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_ENHANCED_VIEW_SCENE
#define EMBER_CALLBACK_SCENES_CLUSTER_COPY_SCENE
#define EMBER_CALLBACK_ZLL_COMMISSIONING_CLUSTER_GET_GROUP_IDENTIFIERS_REQUEST
#define EMBER_CALLBACK_ZLL_COMMISSIONING_CLUSTER_GET_ENDPOINT_LIST_REQUEST
#define EMBER_CALLBACK_INCOMING_ROUTE_ERROR_HANDLER
#define EMBER_APPLICATION_HAS_INCOMING_ROUTE_ERROR_HANDLER
#define EMBER_CALLBACK_EZSP_INCOMING_ROUTE_ERROR_HANDLER
#define EZSP_APPLICATION_HAS_INCOMING_ROUTE_ERROR_HANDLER
#define EMBER_CALLBACK_GET_SOURCE_ROUTE_OVERHEAD
#define EMBER_CALLBACK_SET_SOURCE_ROUTE_OVERHEAD
#define EMBER_PARAMETERIZED_BROADCAST_TABLE
/**** Debug printing section ****/

// Global switch
#define EMBER_AF_PRINT_ENABLE
// Individual areas
#define EMBER_AF_PRINT_CORE 0x0001
#define EMBER_AF_PRINT_APP 0x0002
#define EMBER_AF_PRINT_ZDO 0x0004
#define EMBER_AF_PRINT_BITS { 0x07 }
#define EMBER_AF_PRINT_NAMES { \
  "Core",\
  "Application",\
  "ZDO (ZigBee Device Object)",\
  NULL\
}
#define EMBER_AF_PRINT_NAME_NUMBER 3


#define EMBER_AF_SUPPORT_COMMAND_DISCOVERY


// Generated plugin macros

// Use this macro to check if AES-CMAC plugin is included
#define EMBER_AF_PLUGIN_AES_CMAC

// Use this macro to check if Basic Server Cluster plugin is included
#define EMBER_AF_PLUGIN_BASIC

// Use this macro to check if Concentrator Support plugin is included
#define EMBER_AF_PLUGIN_CONCENTRATOR
#define EMBER_APPLICATION_HAS_SOURCE_ROUTING
#define EZSP_APPLICATION_HAS_ROUTE_RECORD_HANDLER
// User options for plugin Concentrator Support
#define EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE HIGH_RAM_CONCENTRATOR
#define EMBER_SOURCE_ROUTE_TABLE_SIZE 255
#define EMBER_ROUTE_TABLE_SIZE        32
#define EZSP_HOST_SOURCE_ROUTE_TABLE_SIZE 32
#define EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS 10
#define EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS 60
#define EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD 3
#define EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD 1
#define EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS 0
#define EMBER_AF_PLUGIN_CONCENTRATOR_NCP_SUPPORT
#define EMBER_AF_PLUGIN_CONCENTRATOR_DEFAULT_ROUTER_BEHAVIOR FULL

// Use this macro to check if Ember Minimal Printf plugin is included
#define EMBER_AF_PLUGIN_EMBER_MINIMAL_PRINTF

// Use this macro to check if Find and Bind Target plugin is included
#define EMBER_AF_PLUGIN_FIND_AND_BIND_TARGET
// User options for plugin Find and Bind Target
#define EMBER_AF_PLUGIN_FIND_AND_BIND_TARGET_COMMISSIONING_TIME 180

// Use this macro to check if Form and Join Library plugin is included
#define EMBER_AF_PLUGIN_FORM_AND_JOIN

// Use this macro to check if Green Power Client plugin is included
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT
#define EZSP_APPLICATION_HAS_GPEP_INCOMING_MESSAGE_HANDLER
#define EZSP_APPLICATION_HAS_DGP_SENT_HANDLER
// User options for plugin Green Power Client
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_GPP_COMMISSIONING_WINDOW 160
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_GPP_DUPLICATE_TIMEOUT_SEC 160
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_ADDR_ENTRIES 3
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_SEQ_NUM_ENTRIES_PER_ADDR 4

// Use this macro to check if Green Power Common plugin is included
#define EMBER_AF_PLUGIN_GREEN_POWER_COMMON

// Use this macro to check if Green Power Server plugin is included
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER
// User options for plugin Green Power Server
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_HIDDEN_PROXY_ZCL_MESSAGE_SRC_ENDPOINT 2
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT 1
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE 1

// Use this macro to check if Groups Server Cluster plugin is included
#define EMBER_AF_PLUGIN_GROUPS_SERVER

// Use this macro to check if Heartbeat plugin is included
#define EMBER_AF_PLUGIN_HEARTBEAT
// User options for plugin Heartbeat
#define EMBER_AF_PLUGIN_HEARTBEAT_PERIOD_QS 1

// Use this macro to check if Identify Cluster plugin is included
#define EMBER_AF_PLUGIN_IDENTIFY

// Use this macro to check if Interpan plugin is included
#define EMBER_AF_PLUGIN_INTERPAN
// User options for plugin Interpan
#define EMBER_AF_PLUGIN_INTERPAN_ALLOW_REQUIRED_SMART_ENERGY_MESSAGES
#define EMBER_AF_PLUGIN_INTERPAN_DELIVER_TO PRIMARY_ENDPOINT
#define EMBER_AF_PLUGIN_INTERPAN_DELIVER_TO_SPECIFIED_ENDPOINT_VALUE 1

// Use this macro to check if Level Control Server Cluster plugin is included
#define EMBER_AF_PLUGIN_LEVEL_CONTROL
// User options for plugin Level Control Server Cluster
#define EMBER_AF_PLUGIN_LEVEL_CONTROL_MAXIMUM_LEVEL 255
#define EMBER_AF_PLUGIN_LEVEL_CONTROL_MINIMUM_LEVEL 0
#define EMBER_AF_PLUGIN_LEVEL_CONTROL_RATE 0

// Use this macro to check if Network Creator plugin is included
#define EMBER_AF_PLUGIN_NETWORK_CREATOR
// User options for plugin Network Creator
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SCAN_DURATION 4
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_CHANNEL_MASK 34637824
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_CHANNEL_BEACONS_THRESHOLD 5
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_RADIO_POWER 8

// Use this macro to check if Network Creator Security plugin is included
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_TRUST_CENTER_SUPPORT
// User options for plugin Network Creator Security
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_NETWORK_OPEN_TIME_S 300

// Use this macro to check if Network Steering plugin is included
#define EMBER_AF_PLUGIN_NETWORK_STEERING
// User options for plugin Network Steering
#define EMBER_AF_PLUGIN_NETWORK_STEERING_CHANNEL_MASK 0x0318C800UL
#define EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_POWER 3 // Add this macro for fixing compile error, it's not in use for dongle
#define EMBER_AF_PLUGIN_NETWORK_STEERING_SCAN_DURATION 5
#define EMBER_AF_PLUGIN_NETWORK_STEERING_COMMISSIONING_TIME_S 180

// Use this macro to check if On/Off Server Cluster plugin is included
#define EMBER_AF_PLUGIN_ON_OFF

// Use this macro to check if OTA Bootload Cluster Common Code plugin is included
#define EMBER_AF_PLUGIN_OTA_COMMON

// Use this macro to check if OTA Bootload Cluster Server plugin is included
#define EMBER_AF_PLUGIN_OTA_SERVER
// User options for plugin OTA Bootload Cluster Server

// Use this macro to check if OTA Bootload Cluster Server Policy plugin is included
#define EMBER_AF_PLUGIN_OTA_SERVER_POLICY

// User options for plugin Reporting
#define EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE MAX_CONFIGURE_REPORTING

// Use this macro to check if Scan Dispatch plugin is included
#define EMBER_AF_PLUGIN_SCAN_DISPATCH
// User options for plugin Scan Dispatch
#define EMBER_AF_PLUGIN_SCAN_DISPATCH_SCAN_QUEUE_SIZE 10

// Use this macro to check if Scenes Server Cluster plugin is included
#define EMBER_AF_PLUGIN_SCENES
// User options for plugin Scenes Server Cluster
#define EMBER_AF_PLUGIN_SCENES_TABLE_SIZE 3
#define EMBER_AF_PLUGIN_SCENES_USE_TOKENS

// Use this macro to check if Simple Main plugin is included
#define EMBER_AF_PLUGIN_SIMPLE_MAIN

// Use this macro to check if Update TC Link Key plugin is included
#define EMBER_AF_PLUGIN_UPDATE_TC_LINK_KEY

// Use this macro to check if ZLL Commissioning plugin is included
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING
#define EMBER_AF_DISABLE_FORM_AND_JOIN_TICK
// User options for plugin ZLL Commissioning
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RX_ON_AT_STARTUP_PERIOD 300
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_DEFAULT_RADIO_CHANNEL 11
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RADIO_TX_POWER 3
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_EXTENDED_PAN_ID { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_PRIMARY_CHANNEL_MASK 0x02108800UL
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_SECONDARY_CHANNELS
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SECONDARY_CHANNEL_MASK 0x05EF7000UL
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_JOINABLE_SCAN_TIMEOUT_MINUTES 1

#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR            true
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_TOUCH_LINK_SECONDS_DELAY  3
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SUB_DEVICE_TABLE_SIZE     1
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_POWER_LEVEL          0
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_IDENTIFY_DURATION         65535
// Use this macro to check if ZLL Identify Server plugin is included
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER
// User options for plugin ZLL Identify Server
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_EVENT_DELAY 1024
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_BLINK_EVENTS 2
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_BREATHE_EVENTS 4
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_OKAY_EVENTS 6
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_CHANNEL_CHANGE_EVENTS 8

// Use this macro to check if ZLL Level Control Server Cluster Enhancements plugin is included
#define EMBER_AF_PLUGIN_ZLL_LEVEL_CONTROL_SERVER

// Use this macro to check if ZLL On/Off Server Cluster Enhancements plugin is included
#define EMBER_AF_PLUGIN_ZLL_ON_OFF_SERVER

// Use this macro to check if ZLL Scenes Server Cluster Enhancements plugin is included
#define EMBER_AF_PLUGIN_ZLL_SCENES_SERVER

// Use this macro to check if ZLL Utility Server Cluster plugin is included
#define EMBER_AF_PLUGIN_ZLL_UTILITY_SERVER

// Use this macro to check if Antenna Stub plugin is included
#define EMBER_AF_PLUGIN_ANTENNA_STUB
// Use this macro to check if BLE plugin is included
#define EMBER_AF_PLUGIN_BLE
// User options for plugin BLE
#define EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS 5

// Use this macro to check if RAIL Library Multiprotocol plugin is included
#define EMBER_AF_PLUGIN_RAIL_LIBRARY_MP

// Use this macro to check if mbed TLS plugin is included
#define EMBER_AF_PLUGIN_MBEDTLS

// Use this macro to check if BLE PSStore Library plugin is included
#define EMBER_AF_PLUGIN_BLE_PSSTORE_LIBRARY
// Use this macro to check if Binding Table Library plugin is included
#define EMBER_AF_PLUGIN_BINDING_TABLE_LIBRARY
// User options for plugin Binding Table Library
#define EMBER_BINDING_TABLE_SIZE 2

// Use this macro to check if Debug Basic Library plugin is included
#define EMBER_AF_PLUGIN_DEBUG_BASIC_LIBRARY

// Use this macro to check if Green Power Library plugin is included
#define EMBER_AF_PLUGIN_GP_LIBRARY
// User options for plugin Green Power Library
#define EMBER_GP_PROXY_TABLE_SIZE 5

// Use this macro to check if Install Code Library plugin is included
#define EMBER_AF_PLUGIN_INSTALL_CODE_LIBRARY

// Use this macro to check if Packet Validate Library plugin is included
#define EMBER_AF_PLUGIN_PACKET_VALIDATE_LIBRARY

// Use this macro to check if Security Core Library plugin is included
#define EMBER_AF_PLUGIN_SECURITY_LIBRARY_CORE
// User options for plugin Security Core Library
#define EMBER_TRANSIENT_KEY_TIMEOUT_S 300

// Use this macro to check if ZigBee PRO Stack Library plugin is included
#define EMBER_AF_PLUGIN_ZIGBEE_PRO_LIBRARY
// User options for plugin ZigBee PRO Stack Library
#define EMBER_MAX_END_DEVICE_CHILDREN 6
#define EMBER_PACKET_BUFFER_COUNT 200
#define EMBER_END_DEVICE_POLL_TIMEOUT 5
#define EMBER_END_DEVICE_POLL_TIMEOUT_SHIFT 6
#define EMBER_APS_UNICAST_MESSAGE_COUNT 100

// Use this macro to check if ZigBee Light Link Library plugin is included
#define EMBER_AF_PLUGIN_ZLL_LIBRARY
// User options for plugin ZigBee Light Link Library
#define EMBER_ZLL_GROUP_ADDRESSES 0
#define EMBER_ZLL_RSSI_THRESHOLD -128

// Use this macro to check if HAL Library plugin is included
#define EMBER_AF_PLUGIN_HAL_LIBRARY

// Use this macro to check if Simulated EEPROM version 1 Library plugin is included
#define EMBER_AF_PLUGIN_SIM_EEPROM1
// User options for plugin Simulated EEPROM version 1 Library

// Use this macro to check if ADC plugin is included
#define EMBER_AF_PLUGIN_ADC

// Use this macro to check if Antenna Stub plugin is included
#define EMBER_AF_PLUGIN_ANTENNA_STUB

// Use this macro to check if RAIL Library plugin is included
#define EMBER_AF_PLUGIN_RAIL_LIBRARY


// Generated API headers

// API aes-cmac from AES-CMAC plugin
#define EMBER_AF_API_AES_CMAC "protocol/zigbee/app/framework/plugin/aes-cmac/aes-cmac.h"

// API find-and-bind-target from Find and Bind Target plugin
#define EMBER_AF_API_FIND_AND_BIND_TARGET "protocol/zigbee/app/framework/plugin/find-and-bind-target/find-and-bind-target.h"

// API network-creator from Network Creator plugin
#define EMBER_AF_API_NETWORK_CREATOR "protocol/zigbee/app/framework/plugin/network-creator/network-creator.h"

// API network-creator-security from Network Creator Security plugin
#define EMBER_AF_API_NETWORK_CREATOR_SECURITY "protocol/zigbee/app/framework/plugin/network-creator-security/network-creator-security.h"

// API network-steering from Network Steering plugin
#define EMBER_AF_API_NETWORK_STEERING "protocol/zigbee/app/framework/plugin/network-steering/network-steering.h"

// API scan-dispatch from Scan Dispatch plugin
#define EMBER_AF_API_SCAN_DISPATCH "protocol/zigbee/app/framework/plugin/scan-dispatch/scan-dispatch.h"

// API update-tc-link-key from Update TC Link Key plugin
#define EMBER_AF_API_UPDATE_TC_LINK_KEY "protocol/zigbee/app/framework/plugin/update-tc-link-key/update-tc-link-key.h"

// API zll-profile from ZLL Commissioning plugin
#define EMBER_AF_API_ZLL_PROFILE "protocol/zigbee/app/framework/plugin/zll-commissioning/zll-commissioning.h"

// API serial from Serial plugin
#define EMBER_AF_API_SERIAL "platform/base/hal/plugin/serial/serial.h"

// API adc-cortexm3 from ADC plugin
#define EMBER_AF_API_ADC_CORTEXM3 "platform/base/hal/plugin/adc/adc-cortexm3.h"

// API adc from ADC plugin
#define EMBER_AF_API_ADC "platform/base/hal/plugin/adc/adc.h"

// API antenna from Antenna Stub plugin
#define EMBER_AF_API_ANTENNA "platform/base/hal/plugin/antenna/antenna.h"

// API rail-library from RAIL Library plugin
#define EMBER_AF_API_RAIL_LIBRARY "platform/radio/rail_lib/common/rail.h"


// API ble from BLE plugin
#define EMBER_AF_API_BLE "protocol/zigbee/app/framework/plugin-soc/ble/ble-interface.h"

// Custom macros
#ifdef COMMISSIONING_STATUS_LED
#undef COMMISSIONING_STATUS_LED
#endif
#define COMMISSIONING_STATUS_LED BOARDLED2

#ifdef ON_OFF_LIGHT_LED
#undef ON_OFF_LIGHT_LED
#endif
#define ON_OFF_LIGHT_LED BOARDLED2

#ifdef LED_BLINK_PERIOD_MS
#undef LED_BLINK_PERIOD_MS
#endif
#define LED_BLINK_PERIOD_MS 2000

#ifdef APP_SERIAL
#undef APP_SERIAL
#endif
#define APP_SERIAL 3

#ifdef EMBER_ASSERT_SERIAL_PORT
#undef EMBER_ASSERT_SERIAL_PORT
#endif
#define EMBER_ASSERT_SERIAL_PORT 1

#ifdef EMBER_AF_BAUD_RATE
#undef EMBER_AF_BAUD_RATE
#endif
#define EMBER_AF_BAUD_RATE 115200

#ifdef EMBER_SERIAL0_MODE
#undef EMBER_SERIAL0_MODE
#endif
#define EMBER_SERIAL0_MODE EMBER_SERIAL_FIFO

#ifdef EMBER_SERIAL0_RX_QUEUE_SIZE
#undef EMBER_SERIAL0_RX_QUEUE_SIZE
#endif
#define EMBER_SERIAL0_RX_QUEUE_SIZE 128

#ifdef EMBER_SERIAL0_TX_QUEUE_SIZE
#undef EMBER_SERIAL0_TX_QUEUE_SIZE
#endif
#define EMBER_SERIAL0_TX_QUEUE_SIZE 128

#ifdef EMBER_SERIAL0_BLOCKING
#undef EMBER_SERIAL0_BLOCKING
#endif
#define EMBER_SERIAL0_BLOCKING

#ifdef EMBER_SERIAL1_MODE
#undef EMBER_SERIAL1_MODE
#endif
#define EMBER_SERIAL1_MODE EMBER_SERIAL_FIFO

#ifdef EMBER_SERIAL1_RX_QUEUE_SIZE
#undef EMBER_SERIAL1_RX_QUEUE_SIZE
#endif
#define EMBER_SERIAL1_RX_QUEUE_SIZE 128

#ifdef EMBER_SERIAL1_TX_QUEUE_SIZE
#undef EMBER_SERIAL1_TX_QUEUE_SIZE
#endif
#define EMBER_SERIAL1_TX_QUEUE_SIZE 128

#ifdef EMBER_SERIAL1_BLOCKING
#undef EMBER_SERIAL1_BLOCKING
#endif
#define EMBER_SERIAL1_BLOCKING

#ifdef EMBER_AF_SERIAL_PORT_INIT
#undef EMBER_AF_SERIAL_PORT_INIT
#endif
#define EMBER_AF_SERIAL_PORT_INIT() \
  do { \
    emberSerialInit(0, BAUD_115200, PARITY_NONE, 1); \
    emberSerialInit(1, BAUD_115200, PARITY_NONE, 1); \
  } while (0)



#endif // __APP_CWP_DONGLE_H__
