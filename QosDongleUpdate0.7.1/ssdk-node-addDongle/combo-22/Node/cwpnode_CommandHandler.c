/** \file

$Id: sensic_CommandHandler.c 14449 2014-10-17 13:35:25Z jan.lansdaal@philips.com $

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

#include "cwpnode_Local.h"
#include "cwpnode.h"
#include "cwpnode_fta.h"
#include "cwpnode_CommandHandler.h"


/******************************************************************************
* EXTERNAL INCLUDE FILES
******************************************************************************/

#include "lcp_variant.h"
#include "lcp_types.h"
#include "lcp_assert.h"

#include "testframework.h"
#include "scip.h"
#include "scip_Encoder.h"
#include "scip_Decoder.h"
#include "scipzigbeeext_Decoder.h"
#include "fileio.h"
#include "base64.h"
#include "buildinfo.h"


#include <string.h>
#include <stdio.h>

#include "zigbeestack-api.h"

#include "log.h"

#include "lsb_Factory_Commands.h"
#if defined(DALI_MG12_ENABLED)
#include "xmodem_server.h"
#endif

#include "spiflash.h"

#include "Zcl/Identify/zigbee_Identify_Server.h"
/******************************************************************************
* LOCAL MACROS AND DEFINITIONS
******************************************************************************/

#define UARTTRANSFER_BLOCK_LENGTH       (80u)
#define keyBitMask   "0x8001"

/******************************************************************************
* EXTERNAL VARIABLES
******************************************************************************/
extern bool clear_flash_ota_info(void);
extern int dump_spi_ota_info(void);

/******************************************************************************
* LOCAL TYPES
******************************************************************************/

/******************************************************************************
* LOCAL FUNCTION DECLARATIONS
******************************************************************************/

/******************************************************************************
* LOCAL CONSTANTS
******************************************************************************/

/******************************************************************************
* LOCAL VARIABLES
******************************************************************************/
static	Scip_t* s_pScip;

static uint8_t transactionSeqNr = 0;

/******************************************************************************
* COMMAND HANDLER DECLARATIONS
******************************************************************************/

/*!
 * @brief Start the config mode. The device will reboot and start up the config operation mode.
 */
static void CmdStart( void );

/*!
 * @brief Reboot the device, after a reboot the device will boot into the normal mode
 */
static void CmdReboot( void );

/*!
 * @brief Command handler for the CmdJoinZBNwk command
 */
static void CmdJoinZBNwk( void );

static void CmdSetAutoJoinEnabled(void);
/*!
 * @brief Command handler for the reset to FN
 */
static void CmdResetToFN(void);

/*!
 * @brief Command handler to Trigger Occupancy (for test purposes)
 */
static void CmdTriggerOccupancy(void);

/*!
 * @brief Command handler to print heap info (for test purposes)
 */
static void CmdHeapInfo( void );

/*!
 * @brief Command handler to print stack info (for test purposes)
 */
static void CmdStackInfo( void );

static void ConCmdAssociate(void);

static void ZclCmdSend(void);
static void ZclCmdRegisterFoundationCommand(void);
static void ZclCmdRegisterEndpoint(void);
static void ZclCmdAddClusterToSimpleDescriptor(void);
static void ZclCmdRegisterCluster(void);

static void ThCmdGetResetType(void);
static void ThCmdShortSleep(void);
static void ThCmdLongSleep(void);
static void ThCmdDumpInfo(void);
static void ThCmdGetSwVersion(void);
static void ThCmdSetChannelMask(void);
static void ThCmdResetToFactoryDefaults(void);
static void ThCmdGetMacAddress(void);
static void ThCmdSetMacAddress(void);
static void ThCmdSetTxPower(void);
static void ThCmdGetTxPower(void);
static void ThCmdReset(void);
static void ThCmdGetRandom(void);
static void ThCmdGetSpiFlashStorageData(void);
static void ThCmdResetToBootloader(void);
static void ThCmdEraseSpiFlash(void);
static void ThCmdWriteSpiFlash(void);
static void ThCmdGetFirstMissOffset(void);
static void ThCmdResetToBootloader(void);
static void ThCmdCheckOtaImage(void);
static void ThCmdCheckOtaImageCrc(void);
static void ThCmdClearOtaState(void);
static void ThCmdDumpOtaState(void);
static void ThCmdDumpOtaRecord(void);
static void ThCmdWriteOtaRecord(void);
static void ThCmdDeleteGroup(void);
static void ThCmdIdentify( void );

#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY)
static void BleCmdGetMacAddress(void);
static void BleCmdStartBeaconing(void);
static void BleCmdStopBeaconing(void);
static void BleCmdStartScanning(void);
static void BleCmdStopScanning(void);
#endif

/******************************************************************************
* COMMAND MAP
******************************************************************************/

/*!
 * @brief Command map
 */
static const testframework_Command_t s_cfgCommandTable[] =
{
    // Command                  Command handler
    { "Start",                  CmdStart},
    { "Reboot",                 CmdReboot},
    { "JoinZBNwk",              CmdJoinZBNwk },
    { "ResetToFN",              CmdResetToFN},
    { "TrgMotion",              CmdTriggerOccupancy},  // Todo: change "TrgMotion" into "TrgOcc"
    { "Heap",                   CmdHeapInfo},
    { "Stack",                  CmdStackInfo},
    { "SetAutoJoinEnabled",     CmdSetAutoJoinEnabled},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// Connection
static const testframework_Command_t s_connectCommandTable[] =
{
    // Command                  Command handler
    { "Associate",              ConCmdAssociate},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// Zcl
static const testframework_Command_t s_zclCommandTable[] =
{
    // Command                  Command handler
    { "Req",                          ZclCmdSend},
    { "RegisterFoundationCommand",    ZclCmdRegisterFoundationCommand},
    { "RegisterEndpoint",             ZclCmdRegisterEndpoint},
    { "AddClusterToSimpleDescriptor", ZclCmdAddClusterToSimpleDescriptor},
    { "RegisterCluster",              ZclCmdRegisterCluster},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// TH
static const testframework_Command_t s_thCommandTable[] =
{
    // Command                  Command handler
    { "GetResetType",         ThCmdGetResetType},
    { "ShortSleep",           ThCmdShortSleep},
	{ "LongSleep", 		      ThCmdLongSleep},
	{ "DumpInfo", 		      ThCmdDumpInfo},
	{ "GetSwVersion", 		  ThCmdGetSwVersion},
	{ "SetChannelMask", 	  ThCmdSetChannelMask},
	{ "ResetToFactoryDefaults", ThCmdResetToFactoryDefaults},
	{ "GetMacAddress", 		  ThCmdGetMacAddress},
	{ "SetMacAddress", 		  ThCmdSetMacAddress},
	{ "SetTxPower", 		  ThCmdSetTxPower},
	{ "GetTxPower", 		  ThCmdGetTxPower},
	{ "Reset", 		          ThCmdReset},
	{ "GetRandom",		      ThCmdGetRandom},
	{ "GetSpiFlashStorageData", ThCmdGetSpiFlashStorageData},
#if defined(DALI_MG12_ENABLED)
	{ "BootloaderInit", CmdBootloaderInit},
	{ "BootloaderCleanup", CmdBootloaderCleanup},
#endif
	{ "ResetToBootloader",    ThCmdResetToBootloader},
	{ "EraseSpiFlash",        ThCmdEraseSpiFlash},
	{ "WriteSpiFlash",        ThCmdWriteSpiFlash},
    { "GetFirstMissOffset",   ThCmdGetFirstMissOffset},
    { "RestBootloader",       ThCmdResetToBootloader},
    { "CheckOtaImage",        ThCmdCheckOtaImage},
    { "CheckOtaImageCrc",     ThCmdCheckOtaImageCrc},
    { "ClearOtaState",        ThCmdClearOtaState},
    { "DumpOtaState",         ThCmdDumpOtaState},
    { "WriteOtaRecord",       ThCmdWriteOtaRecord},
    { "DumpOtaRecord",        ThCmdDumpOtaRecord},
    { "DeleteGroup",          ThCmdDeleteGroup},
    { "Identify",             ThCmdIdentify},
    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// BLE
#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY)
static const testframework_Command_t s_bleCommandTable[] =
{
    // Command                  Command handler
    { "GetMacAddress",          BleCmdGetMacAddress},
    { "StartBeaconing",         BleCmdStartBeaconing},
    { "StopBeaconing",          BleCmdStopBeaconing},
    { "StartScanning",          BleCmdStartScanning},
    { "StopScanning",           BleCmdStopScanning},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};
#endif

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/
static void C4MCmd_Init(void)
{
  
}

static void C4MCmd_TurnOn(Scip_t *pScip)
{
  s_pScip = pScip;

  (void)s_pScip; // Compiler/lint satisfaction
}

static void C4MCmd_Th_Init(void)
{

}

static void C4MCmd_Th_TurnOn(Scip_t *pScip)
{
    scip_PutCommand(pScip, l_c4m_thCommandHandler.wrapperId, "Ready", NULL, false, 1, 0);
}

/******************************************************************************
* COMMAND HANDLERS
******************************************************************************/

static void CmdStart( void )
{

}

static void CmdReboot( void )
{

}

static void CmdJoinZBNwk( void )
{
    // cmd: [TH,JoinZBNwk]
    // response:
    // description: Manually join ZB network
	if(scip_GetInputArgumentCount(s_pScip) == 0)
	{
        zigbeeStack_JoinNwk();
    }

}

static void CmdResetToFN(void)
{

}

static void CmdTriggerOccupancy(void)
{

}

static void CmdHeapInfo( void )
{

}

static void CmdStackInfo( void )
{

}


static void ThCmdGetResetType(void)
{
    // cmd: [TH,GetResetType]
    // response: [TH,GetResetType,0,<n>]
    // description: Returns the reason for the last reboot. 0=power on, 1=external reset, 2=watchdog, 3=unknown

    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
        uint8_t resetType;
        zigbeeStack_GetResetType(&resetType);
		scip_PutReplyFmt( s_pScip, "sdd", 3, "GetResetType",0,resetType);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }    
}

static void ThCmdShortSleep(void)
{
    // cmd: [TH,ShortSleep]
    // response: [TH,ShortSleep,0]
    // description: Busyloops for 400 ms. To test that the watchdog is not set too tight

    // comments: not required in dongle, and could be implemented in future
}

static void ThCmdLongSleep(void)
{
    // cmd: [TH,LongSleep]
    // response: 
    // description: Busyloops for 1.5s (TI) or 9 s (Atmel). Should trigger a watchdog reset, so the given response
    //                  should normally not be received

    // comments: not required in dongle, and could be implemented in future
}

static void ThCmdDumpInfo(void)
{
    // cmd: [TH,DumpInfo]
    // response: [TH,DumpInfo,nwkUpdateId: <updateId>, channel:<channel>, addr:<nwkAddr>,pan:<panId>]
    //                [TH,DumpInfo,extAddr:<ieeeAddr>]
    //                [TH,DumpInfo,0]
    // description: Shows network settings

    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
        uint16_t updateId;
        uint8_t  channel;
        uint16_t nwkAddr;
        uint16_t panId;
		uint8_t ieeeAddr[8] = {0};

        int8_t result = zigbeeStack_DumpInfo(&updateId,&channel,&nwkAddr,&panId,ieeeAddr);

		uint8_t bufFmt[128] = {0};
		sprintf((char*)bufFmt,"nwkUpdateId:%d,channel:%d,addr:0x%04X,pan:0x%04X",updateId,channel,nwkAddr,panId);
		scip_PutReplyFmt( s_pScip, "ss", 2, "DumpInfo", bufFmt);

		memset(bufFmt,0,sizeof(bufFmt));
		sprintf((char*)bufFmt,"extAddr:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
								 ieeeAddr[7],ieeeAddr[6],ieeeAddr[5],ieeeAddr[4],ieeeAddr[3],
								 ieeeAddr[2],ieeeAddr[1],ieeeAddr[0]);
		scip_PutReplyFmt( s_pScip, "ss", 2, "DumpInfo", bufFmt);
		
		scip_PutReplyFmt( s_pScip, "sd", 2, "DumpInfo", result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }     
}

static void ThCmdGetSwVersion(void)
{
    // cmd: [TH,GetSwVersion]
    // response: [TH,GetSwVersion,0,<projectName>,<keyBitMask>,<buildVersionString>]
    // description: Gets the build name, key bit mask and software version string
    
    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
        uint8_t bufFmt[30] = {0};
        sprintf((char*)bufFmt,"%d.%d.%d", APPLICATION_VERSION_MAJOR, APPLICATION_VERSION_MINOR, APPLICATION_VERSION_REV);
        scip_PutReplyFmt( s_pScip, "sdsss", 5, "GetSwVersion", 0, PKGID, keyBitMask, bufFmt);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }    
}

static void ThCmdSetChannelMask(void)
{
    // cmd: [TH,SetChannelMask,<channelMask>,<OPT:secondaryChannelMask>]
    // response: [TH,SetChannelMask,0]
    // description: Sets the channel mask. If the secondary channel mask is not given, the
    //                  inverse of the primary channel mask is used

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
        uint32_t channelMask = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
        uint8_t result = 0;
#if defined(CWP_NODE)
        result = zigbeeStack_NetworkSteeringSetChannelMask(channelMask, 0);
        result |= zigbeeStack_NetworkSteeringSetChannelMask(channelMask, 1);
#else
        result = zigbeeStack_SetChannelMask(channelMask, 0);
#endif
        scip_PutReplyFmt( s_pScip, "sd", 2, "SetChannelMask",result);
    }
    else if (scip_GetInputArgumentCount(s_pScip) == 2)
    {
        uint32_t channelMask = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
        uint32_t secondaryChannelMask = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
        uint8_t result;
#if defined(CWP_NODE)
        result = zigbeeStack_NetworkSteeringSetChannelMask(channelMask, 0);
        result |= zigbeeStack_NetworkSteeringSetChannelMask(secondaryChannelMask, 1);
#else
        result = zigbeeStack_SetChannelMask(channelMask, secondaryChannelMask);
#endif
        scip_PutReplyFmt( s_pScip, "sd", 2, "SetChannelMask",result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ThCmdResetToFactoryDefaults(void)
{
    // cmd: [TH,ResetToFactoryDefaults,0]
    // description: Resets the device to factory new settings and reboots. This version of the command should
    //                  normally be used, to keep superpersistent items like IEEE address and network frame counter intact.

    // cmd: [TH,ResetToFactoryDefaults,2]
    // description: Resets the device to factory new settings and reboots. ALL items are erased, including IEEE
    //                  address and network frame counter

    // cmd: [TH,ResetToFactoryDefaults,3]
    // description: Resets the device to factory new settings. ALL items are erased, including IEEE address (on TI only) and network
    //                  frame counter. Then the device busyloops, allowing to reflash different software and preventing the current
    //                  software to reinitialize the flash memory again on reboot.

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
        uint8_t resetIndex = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );
		zigbeeStack_ResetToFactoryDefaults(resetIndex);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ThCmdGetMacAddress(void)
{
    // cmd: [TH,GetMacAddress]
    // response: [TH,GetMacAddress,0,<macAddress>]
    // description: Returns the MAC address in the form 00:11:22:33:44:55:66:77
    
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
		uint8_t ieeeAddr[8] = {0};
		
		uint8_t result = zigbeeStack_GetMacAddress(ieeeAddr);
		
		uint8_t bufFmt[128] = {0};
		sprintf((char*)bufFmt,"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
								 ieeeAddr[7],ieeeAddr[6],ieeeAddr[5],ieeeAddr[4],ieeeAddr[3],
								 ieeeAddr[2],ieeeAddr[1],ieeeAddr[0]);
		
		scip_PutReplyFmt( s_pScip, "sds", 3, "GetMacAddress",result,bufFmt);
	}
    else if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
        uint8_t ieeeAddr[8] = {0};
        uint8_t macFind = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );
        uint8_t bufFmt[128] = {0};
        uint8_t result = zigbeeStack_GetMacAddressOptimize(ieeeAddr, macFind);
        sprintf((char*)bufFmt,"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                                 ieeeAddr[7],ieeeAddr[6],ieeeAddr[5],ieeeAddr[4],ieeeAddr[3],
                                 ieeeAddr[2],ieeeAddr[1],ieeeAddr[0]);
        scip_PutReplyFmt( s_pScip, "sds", 3, "GetMacAddress",result,bufFmt);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ThCmdSetMacAddress(void)
{
    // cmd: [TH,SetMacAddress,L=<macAddress>] 
    // response: [TH,SetMacAddress,0]
    // description: Sets the MAC address.
    
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
        zigbee_IeeeAddress_t ieeeAddr = scipzigbeeext_Decoder_GetIeeeAddress(s_pScip,&scipIndex,&error);
        
        /*log_Printfbasic("Mac Addr %x:%x:%x:%x:%x:%x:%x:%x",ieeeAddr.b[7],ieeeAddr.b[6],ieeeAddr.b[5],ieeeAddr.b[4],
                        ieeeAddr.b[3],ieeeAddr.b[2],ieeeAddr.b[1],ieeeAddr.b[0]);*/

        // comments: it seemed that SiLabs doesn't provide this API to set mac address (ieee address)
		uint8_t result = zigbeeStack_SetMacAddress(ieeeAddr.b);

		scip_PutReplyFmt( s_pScip, "sd", 2, "SetMacAddress", (result == 1) ? 0 : 1);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }    
}

static void CmdSetAutoJoinEnabled(void)
{
	// cmd: [TH,SetAutoJoinEnabled,<enabled>]
	// response: [TH,SetAutoJoinEnabled,0]
	// description: Sets JoinMode.
    //                  The enabled value:
    // 1:enable auto join; 0:disable auto join

    // comments: the mode setting is not stored in flash at present, which it is derived form serial command stack.
    // this means, the setting will be lost if reset.
    // we'd check the requirement whethor or not to store settings in flash.

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
		uint8_t modeIndex = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );

		uint8_t result = zigbeeStack_SetAutoJoinEnabled(modeIndex);

		scip_PutReplyFmt( s_pScip, "sd", 2, "SetAutoJoinEnabled", result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }

}

#define    TRUE     1
#define    FALSE    0
bool isAssociateStarted = FALSE;
static void ConCmdAssociate(void)
{
    // cmd: [Connection,Associate]
    // response: [Connection,Associate,0,<True|False status>] then [Connection,AssociateDone,<status>]
    // description: Join a network via association (not supported on Coordinator builds. <status> is -1)

    // cmd: [Connection,Associate,<nwkExtPanId>]
    // response: [Connection,Associate,0,<True|False status>] then [Connection,AssociateDone,<status>]
    // description: Join the network with <nwkExtPanId> via association. (not supported on coordinator build. <status> is -1).

    if(scip_GetInputArgumentCount(s_pScip) == 0)
    {
        if (zigbeeStack_GetNetwrokState() == 2) // EMBER_JOINED_NETWORK
        {
            scip_PutReplyFmt( s_pScip, "sds", 3, "Associate", 0, "False");
        }
        else
        {
            isAssociateStarted = TRUE;
            zigbeeStack_JoinNwk();
            scip_PutReplyFmt( s_pScip, "sds", 3, "Associate", 0, "True");
        }
    }
}

void conAssociateDone(uint8_t result)
{
    if (TRUE == isAssociateStarted)
    {
        scip_PutReplyFmt( s_pScip, "sd", 2, "AssociateDone", result);
        isAssociateStarted = FALSE;
    }
}

#define BIT(x) (1U << (x))
static void ZclCmdSend(void)
{
    // cmd: [Zcl,S,<dstAddrS>,<clusterId>,<zclFrame>,<sourceEndPoint>,<destinationEndPoint>,<txOptions (optional)>]
    // response: [Zcl,A] / [Zcl,N,<uint8_t status>]
    // description: Send a ZCL command.
    int index = 2;
    uint8_t buf[90]={0};
    uint8_t frametype;

    zclCmdTransmit_t zclCmdTx;
    memset(&zclCmdTx, 0, sizeof(zclCmdTransmit_t));
    zclCmdTx.payload = &buf[0];

    //int8_t status;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    //scip_PutReplyFmt( s_pScip, "s", 1, "A" );
    zclCmdTx.length = (int)scip_Decoder_GetPayloadSize( s_pScip, index+2, &error );
    if (scip_GetInputArgumentCount(s_pScip) == 4)
    {
        zclCmdTx.dstAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &index, &error );
        zclCmdTx.clusterId = scip_Decoder_GetUint16( s_pScip, &index, &error );
        scip_Decoder_GetData(&zclCmdTx.length, zclCmdTx.payload, s_pScip, &index, &error);
        frametype = zclCmdTx.payload[0];
        if (frametype & BIT(2))
        {
            zclCmdTx.payload[3] = transactionSeqNr;
        }
        else
        {
            zclCmdTx.payload[1] = transactionSeqNr;
        }
        transactionSeqNr++;
        zclCmdTx.srcEndPoint = scip_Decoder_GetUint8( s_pScip, &index, &error );
        //zclCmdTx.txOptions = 0x040;  //EMBER_APS_OPTION_RETRY
        //Route repair is performed automatically when EMBER_APS_OPTION_RETRY(0x040) and EMBER_APS_OPTION_ENABLE_ROUTE_DISCOVERY(0x0100) are both set in the message options.
        zclCmdTx.txOptions  = (0x040 | 0x0100);
        zigbeeStack_ZclCommand_PreSend(&zclCmdTx);
    }
    else if(scip_GetInputArgumentCount(s_pScip) == 5)
    {
        zclCmdTx.dstAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &index, &error );
        zclCmdTx.clusterId = scip_Decoder_GetUint16( s_pScip, &index, &error );
        scip_Decoder_GetData(&zclCmdTx.length, zclCmdTx.payload, s_pScip, &index, &error);
        frametype = zclCmdTx.payload[0];
        if (frametype & BIT(2))
        {
            zclCmdTx.payload[3] = transactionSeqNr;
        }
        else
        {
            zclCmdTx.payload[1] = transactionSeqNr;
        }
        transactionSeqNr++;
        zclCmdTx.srcEndPoint = scip_Decoder_GetUint8( s_pScip, &index, &error );
        zclCmdTx.txOptions = scip_Decoder_GetUint16( s_pScip, &index, &error );
        //uint8_t frametype = buf[0];
        zigbeeStack_ZclCommand_PreSend(&zclCmdTx);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ZclCmdRegisterFoundationCommand(void)
{
    // cmd: [Zcl,RegisterFoundationCommand,<foundationCommandId>]
    // response: [Zcl,RegisterFoundationCommand,<status>] on success, [Zcl,N,-1] on fail
    // description: Subscribes to received Foundation commands.
    //int scipIndex = 2;
    //scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
        scip_PutReplyFmt( s_pScip, "su", 2, "RegisterFoundationCommand", 0 );
    }
}

static void ZclCmdRegisterEndpoint(void)
{
    // cmd: [Zcl,RegisterEndpoint,<sourceEndPoint>,<profileId>,<deviceId>,<appDevVer>,
    //        <maxInClusters>,<maxOutClusters>,<clientGroupCount>]
    // response: [Zcl,RegisterEndpoint,<status>] on success, [Zcl,N,<status>] on errors.
    // description: Register a specific endpoint. The ZigBee chip will check that endpoints are
    //                  within valid range and unique, the profileID is supported and that the input/output clusters are
    //                  within range and there is enough space to allocate them. Note: Registering the first ZLL endpoint will
    //                  trigger the initialization of the RemoteReset functionality.
    //int scipIndex = 2;
    //scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    //if (scip_GetInputArgumentCount(s_pScip) == 7)
    {
        scip_PutReplyFmt( s_pScip, "su", 2, "RegisterEndpoint", 0 );
    }
}

static void ZclCmdAddClusterToSimpleDescriptor(void)
{
    // cmd: [Zcl,AddClusterToSimpleDescriptor,<sourceEndPoint>,<direction>,<clusterId>]
    // response: [Zcl,AddClusterToSimpleDescriptor,<status>] on success. [Zcl,N,-1] on fail
    // description: Adds the cluster information to the simple descriptor.
    //                  Note: Registered clusters should also have the cluster added to the appropriate simple descriptor.
    //int scipIndex = 2;
    //scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 3)
    {
        scip_PutReplyFmt( s_pScip, "su", 2, "AddClusterToSimpleDescriptor", 0 );
    }

}

static void ZclCmdRegisterCluster(void)
{
    // cmd: [Zcl,RegisterCluster,<clusterId>,<manufacturerCode>,<direction>]
    // response: [Zcl,RegisterCluster,<status>] on success. [Zcl,N,-1] on fail
    // description: Subscribes to incoming ZCL data frames for the specified cluster and source endpoint
    //int scipIndex = 2;
    //scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 3)
    {
        scip_PutReplyFmt( s_pScip, "su", 2, "RegisterCluster", 0 );
    }
}

static void ThCmdSetTxPower(void)
{
	// cmd: [TH,SetTxPower,<powerIndex>]
	// response: [TH,SetTxPower,0]
	// description: Sets the transmit power. Note: the transmit power is not stored persistently!
    //                  The power index range is chip dependent (see ZigBee Platform's N_Radio_SetTxPower interface)

    // comments: the tx power setting is not stored in flash at present, which it is derived form SiLabs stack. 
    //                 this means, the setting will be lost if reset.
    //                 we'd check the requirement whethor or not to store settings in flash.

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
		uint8_t powerIndex = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );

		uint8_t result = zigbeeStack_SetTxPower(powerIndex);

		scip_PutReplyFmt( s_pScip, "sd", 2, "SetTxPower", result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }

}

static void ThCmdGetTxPower(void)
{
	// cmd: [TH,GetTxPower]
	// response: [TH,GetTxPower,<powerIndex>]
	// description: Gets the transmit power. Note: the transmit power is not stored persistently!
    //                  The power index range is chip dependent (see ZigBee Platform's N_Radio_SetTxPower interface)

    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
		uint8_t powerIndex = 0;

		zigbeeStack_GetTxPower(&powerIndex);

		scip_PutReplyFmt( s_pScip, "sd", 2, "GetTxPower", powerIndex);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }

}

static void ThCmdReset(void)
{
    // cmd: [TH,Reset]
    // response: 
    // description: Reboots the device
	if(scip_GetInputArgumentCount(s_pScip) == 0) 
	{    
        zigbeeStack_SysReset();
    }
}

static void ThCmdGetRandom(void)
{
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    
    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
		uint8_t range = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );
		uint16_t random = 0;

		zigbeeStack_GetRandom(&random,range);

		scip_PutReplyFmt( s_pScip, "sd", 2, "GetRandom", random);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ThCmdGetSpiFlashStorageData(void)
{
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if(scip_GetInputArgumentCount(s_pScip) == 1)
    {
       uint8_t buf[60];
	   uint32_t addr;
	   addr = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
       spiflash_Read(buf, addr, 60);
       for(uint8_t i=0; i<60; i++)
	   {
	       log_Printfbasic("buf[%d] = %x", i,buf[i]);
	   }
    }
}

static void ThCmdEraseSpiFlash(void)
{
    if(scip_GetInputArgumentCount(s_pScip) == 1)
    {
       log_Printfbasic("Erase flash");
       spiflash_Erase(0, 524288);
    }
}

static void ThCmdWriteSpiFlash(void)
{
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if(scip_GetInputArgumentCount(s_pScip) == 1)
    {
       uint32_t address = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
       uint8_t buf[40]={0,1,2,3,4,5,6,7,8,9,0xA,0xB};
       spiflash_Write(buf, address, 40);
    }
}


static void ThCmdGetFirstMissOffset(void)
{
    if(scip_GetInputArgumentCount(s_pScip) == 0)
    {
        extern uint32_t ota_GetFirstMissingPackageOffset(void);
		uint32_t offset = ota_GetFirstMissingPackageOffset();
		log_Printfbasic("MaxOffset=%x", offset);
    }
}

static void ThCmdResetToBootloader(void)
{
    // dunfa: tbc
	extern uint8_t halAppBootloaderInstallNewImage(void);

    if(scip_GetInputArgumentCount(s_pScip) == 0)
    {
        halAppBootloaderInstallNewImage();
    }
}

static void ThCmdCheckOtaImage(void)
{
    if(scip_GetInputArgumentCount(s_pScip) == 0)
    {
        uint32_t addr = 0;
		uint8_t count = 0;
		uint8_t buf[64]= {0};
		while(addr<0x5e7c8)
		{
		    spiflash_Read(buf, addr, 64);
			for(uint8_t i=0; i<64; i++)
			{
			    if(buf[i] == 0xFF)
			    {
			        count++;
			    }
			}
			if(count == 64)
			{
			    log_Printfbasic("No write start addr in image= %x", addr);
			}
			memset(buf, 0, 64);
			count = 0;
			addr += 64;
		}
    }
}


static void ThCmdClearOtaState(void)
{
    // dunfa: tbc
	extern bool clear_flash_ota_info(void);
    clear_flash_ota_info();
}

static void ThCmdDumpOtaState(void)
{
    // dunfa: tbc
	extern int dump_spi_ota_info();
    dump_spi_ota_info();
}

static void ThCmdDumpOtaRecord(void)
{
    // dunfa: tbc
	extern int dump_spi_ota_record();
    dump_spi_ota_record();
}

static void ThCmdWriteOtaRecord(void)
{
    // dunfa: tbc
	extern bool write_spi_ota_record(uint8_t *buf, uint32_t offset, uint32_t size);
	extern void close_spi_ota_record(void);

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    uint8_t buf[8] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef};

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
       uint32_t addr = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
       if (error == scip_Decoder_Error_NoError)
       {
            bool_t ret = false;
            log_Printfbasic("param: %d", addr);

            ret = write_spi_ota_record(&buf[3], 3, 1);
            write_spi_ota_record(&buf[0], 0, 1);
            write_spi_ota_record(&buf[2], 2, 1);
            write_spi_ota_record(&buf[4], 4, 1);
            write_spi_ota_record(&buf[6], 6, 1);
            write_spi_ota_record(&buf[7], 7, 1);
            write_spi_ota_record(&buf[5], 5, 1);
            write_spi_ota_record(&buf[1], 1, 1);

            if (ret)
                log_Printfbasic("write record ok");
            else
                log_Printfbasic("failed to write record");

            close_spi_ota_record();
       }
    }
}

#define CRC32_POLYNOMIAL              (0xEDB88320UL)
static uint32_t btl_crc32(const uint8_t newByte, uint32_t prevResult)
{
  uint8_t jj;
  uint32_t previous;
  uint32_t oper;

  previous = (prevResult >> 8u) & 0x00FFFFFFUL;
  oper = (prevResult ^ newByte) & 0xFFu;
  for (jj = 0; jj < 8u; jj++) {
    oper = ((oper & 0x01u) != 0u
            ? ((oper >> 1u) ^ CRC32_POLYNOMIAL)
            : (oper >> 1u));
  }

  return (previous ^ oper);
}


static void ThCmdCheckOtaImageCrc(void)
{
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
	uint32_t crc_value = 0;

    if(scip_GetInputArgumentCount(s_pScip) == 2)
    {
        uint32_t Addr_Begin = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
	   	uint32_t Addr_End = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
		assert(Addr_Begin<Addr_End);
		uint32_t i,len; 
		uint8_t buf[64];
		uint8_t j;
		len = Addr_End - Addr_Begin;
		log_Printfbasic("Addr_Begin=%x,Addr_End=%x,len=%d", Addr_Begin,Addr_End, len);
        if(len > 64)
        {
            i = len;
            while(i > 64)
            {
                spiflash_Read(buf, Addr_Begin, 64);
				for(j=0; j< 64; j++)
				{
				    crc_value = btl_crc32(buf[j], crc_value);
				}
				Addr_Begin += 64;
				i -= 64;
            }
			spiflash_Read(buf, Addr_Begin, i);
			for(j=0; j<i; j++)
			{
			    crc_value = btl_crc32(buf[j], crc_value);
			}
        }
        else
        {
            spiflash_Read(buf, Addr_Begin, len);
			for(j=0; j<len; j++)
			{
			    crc_value = btl_crc32(buf[j], crc_value);
			}
        }
        log_Printfbasic("ImageCrc=%x", crc_value);
    }
}

static void ThCmdDeleteGroup(void)
{
    // dunfa: tbc
}

static void ThCmdIdentify( void )
{
    uint16_t u16time = 0;
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    zigbee_Zcl_Frame_t zclFrame;
    zclFrame.header.commandId = 0x00; //zigbee_Identify_Opcode_Identify

    zigbee_Address_t zigbeeAddr;
    zigbeeAddr.endpoint = 64;

    log_Printbasicfta("Identify");

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
        u16time = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
        log_Printbasicfta("Identify:time:%d", u16time);
    }
    else
    {
        log_Printbasicfta("INPUT ERROR!");
        return;
    }

    zclFrame.payload.length = 2;
    zclFrame.payload.data[0] = (uint8_t)(u16time&0x00FF);
    zclFrame.payload.data[1] = (uint8_t)((u16time>>8)&0x00FF);

    zigbeeidentifyserver_clusterConfiguration.receiver(NULL, &zigbeeAddr, &zclFrame);
}

#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY)
static void BleCmdGetMacAddress(void)
{
    // cmd: [BLE,GetMacAddress]
    // response: [BLE,GetMacAddress,0,<macAddress>]
    // description: Returns the MAC address in the form 00:11:22:33:44:55

    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
		uint8_t ieeeAddr[6] = {0};

		uint8_t result = zigbeeStack_BleGetMacAddress(ieeeAddr);

		uint8_t bufFmt[128] = {0};
		sprintf((char*)bufFmt,"%02X:%02X:%02X:%02X:%02X:%02X",
								 ieeeAddr[5],ieeeAddr[4],ieeeAddr[3],
								 ieeeAddr[2],ieeeAddr[1],ieeeAddr[0]);

		scip_PutReplyFmt( s_pScip, "sds", 3, "GetMacAddress",result,bufFmt);
	}
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void BleCmdStartBeaconing(void)
{
    // cmd: [BLE,StartBeaconing,<IntervalInMilliseconds>,<ChannelMap>,<tx_power_dBm>]
    // response: [BLE,StartBeaconingDone]
    // description: Returns the

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 3)
    {
		uint32_t intervalInMillseconds = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
		uint8_t channel_map = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );
		int tx_power_dBm = scip_Decoder_GetInt16( s_pScip, &scipIndex, &error ); //dBm

		int8_t result = zigbeeStack_BleStartBeaconings(intervalInMillseconds, channel_map, tx_power_dBm);

		scip_PutReplyFmt( s_pScip, "sd", 2, "StartBeaconingDone", result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void BleCmdStopBeaconing(void)
{
    // cmd: [BLE,StopBeaconing]
    // response: [BLE,StopBeaconing,iBeacon [OFF]]
    // description: Returns the

    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
		int8_t result = zigbeeStack_BleStopBeaconings();
		scip_PutReplyFmt( s_pScip, "ss", 2, "StopBeaconingDone",result);
	}
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void BleCmdStartScanning(void)
{
    // dunfa: tbc
}

static void BleCmdStopScanning(void)
{
    // dunfa: tbc
}
#endif

#if defined(BUILD_TEST_AID)
extern uint8_t start_distribute_network(uint8_t u8Channel);

static void CmdStartNetwork(void)
{
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
        uint8_t status;
        uint8_t u8Channel = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );
        status = start_distribute_network(u8Channel);

        log_Printfbasic("testAid,StartNetwork,%d", status);
    }
    else
    {
        log_Printfbasic("testAid,StartNetwork,1"); // arguments error
    }
}

static void CmdReset(void)
{
    extern void ENERGYMETERING_TurnOff(void);
    extern uint8_t emberAfNetworkState(void);
    extern uint8_t emberLeaveNetwork(void);
    extern uint8_t emberAfPluginNetworkSteeringStop(void);

    ENERGYMETERING_TurnOff();
    if (emberAfNetworkState() == 2 /*EMBER_JOINED_NETWORK*/)
    {
        (void)emberLeaveNetwork();
    }
    else
    {
        emberAfPluginNetworkSteeringStop();
    }
    log_Printfbasic("testAid,Reset,0");
}

static void CmdCfgEui64(void)
{
    extern bool_t set_eui_addr_token(uint8_t *data);

    lsb_Factory_EUI64_t ieeeAddress;
    int scipIndex = 2;

    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    int length = 8;

    scip_Decoder_GetData(&length, (uint8_t *)&ieeeAddress, s_pScip, &scipIndex, &error);

    if ((length == 8) && (error == scip_Decoder_Error_NoError))
    {
        if (set_eui_addr_token((uint8_t *)&ieeeAddress))
        {
            log_Printfbasic("testAid,CfgEui64,0");
        }
    }
    else
    {
        log_Printfbasic("testAid,CfgEui64,%d", error);
    }
}

/**
* [testAid,Interpan,<eui64(bigendian)>,<commandID>,<args>]
* Reset to factory new: [testAid,Interpan,<eui64(bigendian)>,reset,<channel>], channel:11~26
* exp: [testAid,Interpan,000B57FFFE648C32,reset,15] :Reset device 000B57FFFE648C32 to factory new on channel 15
*/
extern uint8_t emberSetRadioChannel(uint8_t channel);
extern uint8_t emberAfZllResetToFactoryNewRequest(void);
extern uint8_t emberAfZllIdentifyRequest(void);
extern void zll_commissioning_RegisterIsResetTargetFoundCallback(bool_t (*cb)(const uint8_t (*pau8Target)[8]));

static void CmdInterpan( void )
{
}

///< [testAid,Touchlink,reset]: Reset remote target to factory new status
static void CmdTouchlink( void )
{
}

extern void registerLogPowerCb(void (*cb)(uint32_t u32Power));
static void logPowerValue(uint32_t u32Power)
{
    log_Printfbasic("power:%d", u32Power);
}

static void CmdEnableLogPower(void)
{
    log_Printfbasic("EnableLogPower");
    registerLogPowerCb(logPowerValue);
}

static void CmdDisableLogPower(void)
{
    log_Printfbasic("DisableLogPower");
    registerLogPowerCb(NULL);
}

static const testframework_Command_t s_testAidCommandTable[] =
{
    // Command                  Command handler
    { "Reset",                  CmdReset},
    { "CfgEui64",               CmdCfgEui64},
    { "StartNetwork",           CmdStartNetwork},
    /**
    * [testAid,Interpan,<eui64(bigendian)>,<commandID>,<args>]
    * Reset to factory new: [testAid,Interpan,<eui64(bigendian)>,reset,<channel>], channel:11~26
    * exp: [testAid,Interpan,000B57FFFE648C32,reset,15] :Reset device 000B57FFFE648C32 to factory new on channel 15
    */
    { "Interpan",                CmdInterpan},
    ///< [testAid,Touchlink,reset]: Reset remote target to factory new status
    { "Touchlink",               CmdTouchlink},
    ///< [testAid,EnableLogPower]: enable printing sample power value to console
    ///< [testAid,DisableLogPower]: disable printing sample power value to console
    { "EnableLogPower",          CmdEnableLogPower},
    { "DisableLogPower",         CmdDisableLogPower},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};
#endif // #if defined(BUILD_TEST_AID)
/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/

/******************************************************************************
* LOCAL EXPORTED FUNCTIONS
******************************************************************************/


/******************************************************************************
* INTERFACE FUNCTIONS
******************************************************************************/

/******************************************************************************
* EXPORTED INTERFACE
******************************************************************************/

const testframework_Wrapper_t  l_c4m_cfgCommandHandler =
{
    "cfg",
    sizeof( "cfg" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_cfgCommandTable,
    NULL,
};


const testframework_Wrapper_t  l_c4m_conCommandHandler =
{
    "Connection",
    sizeof( "Connection" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_connectCommandTable,
    NULL,
};

const testframework_Wrapper_t  l_c4m_zclCommandHandler =
{
    "Zcl",
    sizeof( "Zcl" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_zclCommandTable,
    NULL,
};
const testframework_Wrapper_t  l_c4m_thCommandHandler =
{
    "TH",
    sizeof( "TH" ),
    C4MCmd_Th_Init,
    C4MCmd_Th_TurnOn,
    s_thCommandTable,
    NULL,
};

#if defined(BUILD_TEST_COMBO_NODES_SCALABILITY)
const testframework_Wrapper_t  l_c4m_bleCommandHandler =
{
    "BLE",
    sizeof( "BLE" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_bleCommandTable,
    NULL,
};
#endif

#if defined(BUILD_TEST_AID)
const testframework_Wrapper_t  l_c4m_testAidCommandHandler =
{
    "testAid",
    sizeof( "testAid" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_testAidCommandTable,
    NULL,
};
#endif
