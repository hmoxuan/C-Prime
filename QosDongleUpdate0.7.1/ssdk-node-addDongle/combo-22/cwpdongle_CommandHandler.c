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

#include "cwpdongle_Local.h"
#include "cwpdongle.h"
#include "cwpdongle_Fta.h"
#include "cwpdongle_CommandHandler.h"


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
#include "osa.h"


/******************************************************************************
* LOCAL MACROS AND DEFINITIONS
******************************************************************************/

#define UARTTRANSFER_BLOCK_LENGTH       (80u)
#define keyBitMask   "0x8001"

/******************************************************************************
* EXTERNAL VARIABLES
******************************************************************************/
static	Scip_t* s_pScip;

static uint8_t transactionSeqNr = 0;


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

static void LinkCmdAllow(void);
static void LinkCmdDisallow(void);
static void LinkCmdTouchlink(void);
static void TcCmdSendApsTransportNetworkKey(void);
static void ConCmdFindFreePan(void);
static void ConCmdJoinNetwork(void);
static void ConCmdAssociate(void);
static void ConCmdGetAddress(void);
static void ConCmdGetExtendedPanId(void);
static void ConCmdGetChannel(void);
static void ConCmdChannelChange(void);
static void ConCmdStartCoordinator(void);
static void ConCmdStartCoordinator2(void);
static void ZGPCmdSetGroupId(void);
static void ZGPCmdCommissioningEnter(void);
static void ZGPCmdCommissioningExit(void);
static void ZGPCmdAddPairing(void);
static void ZGPCmdRemovePairing(void);
static void ZGPCmdGetEntryForIndex(void);
static void ZGPCmdEraseAll(void);
static void ZGPCmdErase(void);
static void ZGPCmdSendGPPairingConfiguration(void);
static void ZGPCmdSendCommissioningMode(void);
static void ZGPCmdSetGPDAssignedAlias(void);
static void ZclCmdSend(void);
static void ZclCmdRegisterFoundationCommand(void);
static void ZclCmdRegisterEndpoint(void);
static void ZclCmdAddClusterToSimpleDescriptor(void);
static void ZclCmdRegisterCluster(void);
static void ZdpCmdSendMgmtPermitJoiningReq(void);
static void ZdpCmdSendMgmtLqiReq(void);
static void ZdpCmdSendSimpleDescReq(void);
static void ZdpCmdSendNodeDescReq(void);
static void ZdpCmdSendActiveEndPointReq(void);
static void ZdpCmdNwkAddrReq(void);
static void ZdpCmdIeeeAddrReq(void);
static void ZdpCmdSendBindReq(void);
static void ZdpCmdSendUnbindReq(void);
static void ZdpCmdSendMgmtBindReq(void);
static void ZdpCmdSendMatchDescReq(void);
static void ZdpCmdSendMgmtLeaveReq(void);
static void RoutingCmdSendMtoRR(void);
static void RoutingCmdClearEntry(void);
static void BrCmdStoreGroupRange(void);
static void BrCmdIdentify(void);
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
static void ThCmdSuspendScheduler(void);
static void ThCmdGetRandom(void);
static void ThCmdPrintSourceRouteTable(void);

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

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// touch link
static const testframework_Command_t s_linkCommandTable[] =
{
    // Command                  Command handler
    { "Allow",                  LinkCmdAllow},
    { "Disallow",               LinkCmdDisallow},
    { "Touchlink",              LinkCmdTouchlink },

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// TrustCenter
static const testframework_Command_t s_trustCenterCommandTable[] =
{
    // Command                  Command handler
    { "Send_ApsTransportNetworkKey",    TcCmdSendApsTransportNetworkKey},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// Connection
static const testframework_Command_t s_connectCommandTable[] =
{
    // Command                  Command handler
    { "FindFreePan",            ConCmdFindFreePan},
    { "JoinNetwork",            ConCmdJoinNetwork},
    { "Associate",              ConCmdAssociate},
	{ "GetAddress",				ConCmdGetAddress},
	{ "GetExtendedPanId",		ConCmdGetExtendedPanId},
	{ "GetChannel",		        ConCmdGetChannel},
	{ "ChannelChange",		    ConCmdChannelChange},
	{ "StartCoordinator",		ConCmdStartCoordinator},
	{ "StartCoordinator2",		ConCmdStartCoordinator2},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// ZGP
static const testframework_Command_t s_ZGPCommandTable[] =
{
    // Command                  Command handler
    { "SetGroupId",                ZGPCmdSetGroupId},
    { "CommissioningEnter",        ZGPCmdCommissioningEnter},
    { "CommissioningExit",         ZGPCmdCommissioningExit},
	{ "AddPairing",				   ZGPCmdAddPairing},
	{ "RemovePairing",		       ZGPCmdRemovePairing},
	{ "GetEntryForIndex",		   ZGPCmdGetEntryForIndex},
	{ "EraseAll",		           ZGPCmdEraseAll},
	{ "Erase",		             ZGPCmdErase},
	{ "SendGPPairingConfiguration",ZGPCmdSendGPPairingConfiguration},
	{ "SendCommissioningMode",     ZGPCmdSendCommissioningMode},
	{ "SetGPDAssignedAlias",       ZGPCmdSetGPDAssignedAlias},

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
	{ "RegisterCluster",		      ZclCmdRegisterCluster},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// Zdp
static const testframework_Command_t s_zdpCommandTable[] =
{
    // Command                  Command handler
    { "SendMgmtPermitJoiningReq", ZdpCmdSendMgmtPermitJoiningReq},
    { "SendMgmtLqiReq",           ZdpCmdSendMgmtLqiReq},
    { "SendSimpleDescReq",        ZdpCmdSendSimpleDescReq},
	{ "SendNodeDescReq",          ZdpCmdSendNodeDescReq},
	{ "SendActiveEndPointReq",    ZdpCmdSendActiveEndPointReq},
	{ "NwkAddrReq",			      ZdpCmdNwkAddrReq},
	{ "IeeeAddrReq",			  ZdpCmdIeeeAddrReq},
	{ "SendBindReq",			  ZdpCmdSendBindReq},
	{ "SendUnbindReq",			  ZdpCmdSendUnbindReq},
	{ "SendMgmtBindReq",	      ZdpCmdSendMgmtBindReq},
	{ "SendMatchDescReq",	      ZdpCmdSendMatchDescReq},
	{ "SendMgmtLeaveReq",		  ZdpCmdSendMgmtLeaveReq},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// Routing
static const testframework_Command_t s_routingCommandTable[] =
{
    // Command                  Command handler
    { "SendMtoRR",            RoutingCmdSendMtoRR},
    { "ClearEntry",           RoutingCmdClearEntry},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};

// Bridge
static const testframework_Command_t s_bridgeCommandTable[] =
{
    // Command                  Command handler
    { "StoreGroupRange",    BrCmdStoreGroupRange},
    { "Identify",           BrCmdIdentify},

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
	{ "SuspendScheduler",     ThCmdSuspendScheduler},
	{ "GetRandom",			  ThCmdGetRandom},
    { "PrintSourceRouteTable",ThCmdPrintSourceRouteTable},

    // Mandatory end of the command list marker
    TESTFRAMEWORK_COMMANDLIST_END
};


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
    // comments: not included in smart bridge interface, and could be implemented in future
}

static void CmdReboot( void )
{
    // comments: not included in smart bridge interface, and could be implemented in future
}

static void CmdJoinZBNwk( void )
{
    // comments: not included in smart bridge interface, and could be implemented in future
}

static void CmdResetToFN(void)
{
    // comments: not included in smart bridge interface, and could be implemented in future
}

static void CmdTriggerOccupancy(void)
{
    // comments: not included in smart bridge interface, and could be implemented in future
}

static void CmdHeapInfo( void )
{
    // comments: not included in smart bridge interface, and could be implemented in future
}

static void CmdStackInfo( void )
{
    // comments: not included in smart bridge interface, and could be implemented in future
}

static void LinkCmdAllow(void)
{
    // cmd: [Link,Allow] 
    // response: [Link,Allow,0] 
	// description: Enables touchlinking (target) (not supported on coordinator build. <status> is -1)

}

static void LinkCmdDisallow(void)
{
    // cmd: [Link,Disallow]
    // response: [Link,Disallow,0]
    // description: Disables touchlinking (target) (not supported on coordinator build. <status> is -1)
    
}

static void LinkCmdTouchlink(void)
{
    // cmd: [Link,Touchlink]
    // response: [Link,Touchlink,success,<uint16_t nwkaddr>,<uint16_t panid>]
    //                [Link,Touchlink,failed]
    //                [Link,Touchlink,-1]
    // description: Initiates a touchlink. Returns own network address and PAN ID (not supported on coordinator
    //                  build. <status> is -1)
    
}

static void TcCmdSendApsTransportNetworkKey(void)
{
    // cmd: [TrustCenter,Send_ApsTransportNetworkKey,<nwkAddress>,<ieeeAddr>,<parentNwkAddres>]
    // response: [TrustCenter,Send_ApsTransportNetworkKey,0]
    // description: Send network key to lamp.
    
}

static void ConCmdFindFreePan(void)
{
    // cmd: [Connection,FindFreePan]
    // response: [Connection,A] then [Connection,FindFreePanDone,<uint16_t nwkPanId>,<nwkExtPanId>]
    // description: Find a free PAN
	uint16_t nwkPanId = 0;
	uint8_t extPanId[24] = "11:22:33:44:55:66:77:88";

    scip_PutReplyFmt( s_pScip, "sd", 2, "FindFreePan", 0 );

    zigbeeStack_FindFreePan(&nwkPanId, extPanId);

    // Tell the other side that we handled the message
    scip_PutReplyFmt( s_pScip, "sds", 3, "FindFreePanDone", nwkPanId, extPanId);
}

static void ConCmdJoinNetwork(void)
{
    // cmd: [Connection,JoinNetwork,<uint16_t nwkPanId>,<nwkExtPanId>,<uint8_t nwkChannel>]
    // response: [Connection,A] then [Connection,JoinNetworkDone,<uint8_t status>,*]
    //                * = only if success: <uint16_t nwkPanId>,<nwkExtPanId>,<uint8_t nwkChannel>,
    //                <uint8_t nwkUpdateId>,0x0001u,0xFFF7u
    // description: Start a new network. Optionally specify the network channel to be used. Leaving the channel parameter out or
    //                  setting it to 0 makes the bridge randomly select a channel. If a specified channel is not in the primary channel
    //                  mask, <status> will be -2 (invalid parameter). (not supported on Coordinator builds. <status> is -1)

    // comments: not required in dongle
    
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 3)
    {
		uint16_t nwkPanId = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
		uint8_t* pNwkExtPanId;
		scip_Decoder_GetString((char**)&pNwkExtPanId,s_pScip,&scipIndex,&error);
		uint8_t nwkChannel = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );
		uint8_t nwkUpdateId;

		scip_PutReplyFmt( s_pScip, "s", 1, "A" );

		int8_t result = zigbeeStack_JoinNetwork(nwkPanId, pNwkExtPanId, nwkChannel, &nwkUpdateId);

        uint8_t bufFmt[128] = {0};
        sprintf((char*)bufFmt,"%d,%s,%d,%d,0x%04x,0x%04x",nwkPanId,pNwkExtPanId,nwkChannel,nwkUpdateId,0x0001u,0xFFF7u);

		scip_PutReplyFmt( s_pScip, "sds", 3, "JoinNetworkDone", result, bufFmt);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
    
}

static void ConCmdAssociate(void)
{
    // cmd: [Connection,Associate]
    // response: [Connection,Associate,0,<True|False status>] then [Connection,AssociateDone,<status>]
    // description: Join a network via association (not supported on Coordinator builds. <status> is -1)

    // cmd: [Connection,Associate,<nwkExtPanId>]
    // response: [Connection,Associate,0,<True|False status>] then [Connection,AssociateDone,<status>]
    // description: Join the network with <nwkExtPanId> via association. (not supported on coordinator build. <status> is -1).


    // comments: not required in dongle
}

static void ConCmdGetAddress(void)
{
    // cmd: [Connection,GetAddress]
    // response: [Connection,A] then [Connection,GetAddress,<ieeeAddr>,<shortAddr>]
    // description: Gets the IEEE and NWK short address of the bridge

    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
		uint16_t shortAddr;
		uint8_t ieeeAddr[8] = {0};
		uint8_t endpoint;

		//scip_PutReplyFmt( s_pScip, "s", 1, "A" );

		zigbeeStack_GetAddress(ieeeAddr,&shortAddr,&endpoint);

        uint8_t bufFmt[128] = {0};
        sprintf((char*)bufFmt,"L=%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,S=0x%04X.%d",
                                 ieeeAddr[7],ieeeAddr[6],ieeeAddr[5],ieeeAddr[4],ieeeAddr[3],
                                 ieeeAddr[2],ieeeAddr[1],ieeeAddr[0],shortAddr,endpoint);

		scip_PutReplyFmt( s_pScip, "sds", 3, "GetAddress", 0, bufFmt);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }

}

static void ConCmdGetExtendedPanId(void)
{
    // cmd: [Connection,GetExtendedPanId]
    // response: [Connection,A] then [Connection,GetExtendedPanId,<nwkExtPanId>]
    // description: Gets the ExtendedPanId of the bridge
    
    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
		uint8_t nwkExtPanId[8] = {0};

		//scip_PutReplyFmt( s_pScip, "s", 1, "A" );

		zigbeeStack_GetExtendedPanId(nwkExtPanId);

        uint8_t bufFmt[128] = {0};
        sprintf((char*)bufFmt,"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                                 nwkExtPanId[7],nwkExtPanId[6],nwkExtPanId[5],nwkExtPanId[4],nwkExtPanId[3],
                                 nwkExtPanId[2],nwkExtPanId[1],nwkExtPanId[0]);

		scip_PutReplyFmt( s_pScip, "ss", 2, "GetExtendedPanId", bufFmt);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ConCmdGetChannel(void)
{
    // cmd: [Connection,GetChannel]
    // response: [Connection,A] then [Connection,GetChannel,<nwkChannel>]
    // description: Gets the Channel of the bridge
    
    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
		uint8_t nwkChannel = 0;

		//scip_PutReplyFmt( s_pScip, "s", 1, "A" );

		zigbeeStack_GetChannel(&nwkChannel);

		scip_PutReplyFmt( s_pScip, "sdd", 3, "GetChannel", 0, nwkChannel);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ConCmdChannelChange(void)
{
    // cmd: [Connection,ChannelChange,<uint8_t nwkChannel>]
    // response: [Connection,ChannelChange,<uint8_t result>]
    // description: Changes the network channel. (Sending a Zdp MgmtNwkUpdateReq)

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
		uint8_t nwkChannel = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );

		//scip_PutReplyFmt( s_pScip, "s", 1, "A" );

		uint8_t result = zigbeeStack_ChannelChange(nwkChannel);

		scip_PutReplyFmt( s_pScip, "sd", 2, "ChannelChange", result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ConCmdStartCoordinator(void)
{
    // cmd: [Connection,StartCoordinator,<uint16_t nwkPanId>,<nwkExtPanId>,<uint8_t nwkChannel>]
    // response: [Connection,StartCoordinator,0] then [Connection,StartCoordinatorDone,<uint8_t result>,
    //                <uint16_t nwkPanId>,<nwkExtPanId>,<nwkChannel>,<nwkUpdateId>]
    // description: Start network with bridge as coordinator Optionally specify the network channel to be used. Leaving
    //                  the channel parameter out or setting it to 0 makes the bridge randomly select a channel. If a specified
    //                  channel is not in the primary channel mask, <status> will be -2 (invalid parameter). (only supported
    //                  on Coordinator builds)

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 2)
    {
		uint16_t nwkPanId = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
		zigbee_ExtPanId_t nwkExtPanId = scipzigbeeext_Decoder_GetExtPanId( s_pScip, &scipIndex, &error );

		(void)nwkPanId;
		(void)nwkExtPanId;

		logcom_Printfbasic( "Zdp,JoinPermitted,False");
		zigbeeStack_StartCoordinator2(NULL,NULL,NULL);
    }
    else if (scip_GetInputArgumentCount(s_pScip) == 3)
    {
		uint16_t nwkPanId = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
		zigbee_ExtPanId_t nwkExtPanId = scipzigbeeext_Decoder_GetExtPanId( s_pScip, &scipIndex, &error );
		uint8_t nwkChannel = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );

		//scip_PutReplyFmt( s_pScip, "sd", 2, "StartCoordinator",0);
		logcom_Printfbasic( "Zdp,JoinPermitted,False");

		zigbeeStack_StartCoordinator(nwkPanId,nwkExtPanId.b,nwkChannel);

        //log_Printfbasic("\r\n%x:%x:%x:%x:%x:%x:%x:%x\r\n",nwkExtPanId.b[0],nwkExtPanId.b[1],nwkExtPanId.b[2],
        //    nwkExtPanId.b[3],nwkExtPanId.b[4],nwkExtPanId.b[5],nwkExtPanId.b[6],nwkExtPanId.b[7]);
      /*  
		uint8_t bufFmt[128] = {0};		
		uint8_t nwkExtPanIdRes[8] = {0};
		zigbeeStack_GetExtendedPanId(nwkExtPanIdRes);
		uint8_t updateId = 0;
		zigbeeStack_GetUpdatedId(&updateId);
		sprintf((char*)bufFmt,"0x%04X,%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,%d,%d",
		                nwkPanId,nwkExtPanIdRes[7],nwkExtPanIdRes[6],nwkExtPanIdRes[5],nwkExtPanIdRes[4],
		                nwkExtPanIdRes[3],nwkExtPanIdRes[2],nwkExtPanIdRes[1],nwkExtPanIdRes[0],nwkChannel,updateId);

		scip_PutReplyFmt( s_pScip, "sds", 3, "StartCoordinatorDone", result,bufFmt);
	*/
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }    
}

static void ConCmdStartCoordinator2(void)
{
    // cmd: [Connection,StartCoordinator2]
    // response: [Connection,StartCoordinator,0] then [Connection,StartCoordinatorDone,<uint8_t result>,
    //                <uint16_t nwkPanId>,<nwkExtPanId>,<nwkChannel>,<nwkUpdateId>]
    // description: Start network with bridge as coordinator Optionally specify the network channel to be used. Leaving
    //                  the channel parameter out or setting it to 0 makes the bridge randomly select a channel. If a specified
    //                  channel is not in the primary channel mask, <status> will be -2 (invalid parameter). (only supported
    //                  on Coordinator builds)

//	emberSetNodeId(0);
	logcom_Printfbasic( "Zdp,JoinPermitted,False");
	zigbeeStack_StartCoordinator2(NULL,NULL,NULL);
}

static void ZGPCmdSetGroupId(void)
{
    // cmd: [Zgp,SetGroupId,<uint16 group>]
    // response: [Zgp,SetGroupId,0]
    // description: Sets the group ID to use for ZGP. Should never change.
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
		uint8_t group = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
        zigbeeStack_GPSetGroupId(group);
		scip_PutReplyFmt( s_pScip, "sd", 2, "SetGroupId", 0);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ZGPCmdCommissioningEnter(void)
{
    // cmd: [Zgp,CommissioningEnter,<bool exitOnPairing>,<uint16 timeoutS>]
    // response: [Zgp,CommissioningEnter,0, <retval>]
    // description: Enters commissioning mode. retval==0:OK, retval==1:Failure, retval==2:sink table is full
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    if (scip_GetInputArgumentCount(s_pScip) <= 4)
    {
        bool exitOnPairing = scip_Decoder_GetBool( s_pScip, &scipIndex, &error );
        uint16_t timeoutS = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
        int8_t result = zigbeeStack_GPCommissioningEnter(exitOnPairing, timeoutS);
        scip_PutReplyFmt( s_pScip, "sdd", 3, "CommissioningEnter", 0, result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }

}

static void ZGPCmdCommissioningExit(void)
{
    // cmd: [Zgp,CommissioningExit]
    // response: [Zgp,CommissioningExit,0]
    // description: Exits commissioning mode.
    int8_t result = zigbeeStack_GPCommissioningExit();
    scip_PutReplyFmt( s_pScip, "sd", 2, "CommissioningExit", result);
}

static void ZGPCommissioningNotification(uint32_t gpdSrcId, uint8_t gpdDeviceId)
{
    logcom_Printfbasic("Zgp,CommissioningNotification,%d,%d,0,,,",gpdSrcId,gpdDeviceId);
}

static void ZGPDeviceInfo(uint32_t gpdSrcId, uint16_t fwVersion, uint16_t hwVersion, uint16_t stackVersion)
{
    logcom_Printfbasic("Zgp,DeviceInfo,%d,%d,%d,%d",
                        gpdSrcId,fwVersion, hwVersion, stackVersion);
}

static void ZGPCmdAddPairing(void)
{
    // cmd: [Zgp,AddPairing,<destinationAddr>,<uint32 gpdSourceId>]
    // response: [Zgp,AddPairing,0]
    // description: Adds pairing to the destination address' proxies.
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    if (scip_GetInputArgumentCount(s_pScip) == 2)
    {
		zigbee_Address_t destAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );
        uint32_t gpdSrcId = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
        zigbeeStack_GPAddPairing(&destAddr, gpdSrcId);
        scip_PutReplyFmt( s_pScip, "sd", 2, "AddPairing", 0);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ZGPCmdRemovePairing(void)
{
    // cmd: [Zgp,RemovePairing,<destinationAddr>,<uint32 gpdSourceId>]
    // response: [Zgp,RemovePairing,0]
    // description: Removes pairing from the destination address' proxies.
    
}

static void ZGPCmdGetEntryForIndex(void)
{
    // cmd: [Zgp,GetEntryForIndex,<uint8 index>]
    // response: [Zgp,GetEntryForIndex,0,<Status>,<uint32 gpdSourceId>,<uint8 deviceId>]
    // description: Query the sink table
    
}

static void ZGPCmdEraseAll(void)
{
    // cmd: [Zgp,EraseAll]
    // response: [Zgp,EraseAll,0]
    // description: Erase all entries from the proxy and sink table.
    int8_t result = zigbeeStack_GPEraseAll();
    scip_PutReplyFmt( s_pScip, "sd", 2, "EraseAll", result);
    
}

static void ZGPCmdErase(void)
{
    // cmd: [Zgp,Erase, <SrcId>]
    // response: [Zgp,EraseA,0]
    // description: Erase entry from the dongle for specific GPD.
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
      uint32_t gpdSrcId = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
      zigbeeStack_GPErase(gpdSrcId);
		  scip_PutReplyFmt( s_pScip, "sd", 2, "Erase", 0);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ZGPCmdSendGPPairingConfiguration(void)
{
    // cmd: [Zgp,SendGPPairingConfiguration,<uint16_t action>,<uint32_t gpdSrcId>,<dstAddr>]
    // response: [Zgp,SendGPPairingConfiguration,0,<retval>]
    // description: Sends pairing config command with the SinkTable for entry with gpdSrcId to
    //                  the given dstAddr. ZigBee GP endpoint is 242 and should be taken into account.
    //                  Only supported action "Replace" (0x02). retval==0:OK, retval==1:Failure,retval==2:Entry Not Found,
    //                  retval==0x89:ZclOutOfMemory. See GreenPower multiple sinks feature for more details.
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    if (scip_GetInputArgumentCount(s_pScip) == 3)
    {
        uint16_t action = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
        uint32_t gpdSrcId = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
		zigbee_Address_t destAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );
        int8_t result = zigbeeStack_GPSendGPPairingConfiguration(action, gpdSrcId, &destAddr);
        scip_PutReplyFmt( s_pScip, "sdd", 3, "SendGPPairingConfiguration", 0, result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }


}

static void ZGPCmdSendCommissioningMode(void)
{
    // cmd: [Zgp,SendCommissioningMode,<uint8_t action>,<bool involveProxies>,<dstAddr>,<sinkEndpoint>]
    // response: [Zgp,SendCommissioningMode,0,<retval>]
    // description: Sends Commissioning Mode command. <dstAddr> field to send GP Sink Commissioning Mode message.
    //                  <involveProxies> to send GP Proxy Commissioning message (see feature design for extra details). <action>
    //                  Enter(1) or Exit(0). retval==0:OK,retval==1:Failure, retval==2:sink table is full. 
    //                  Note: It is applications responsibility to provide an appropriate SinkEndpoint(refer to defined in
    //                           ZGP1.1/A.3.3.4.8) while !Unicasting/!Groupcasting/!Broadcasting this command. No explicit verification
    //                           is performed in the platform to ensure the correctness of the SinkEndpoint. Please refer to ZGP1.1/A.3.3.4.8.2 to
    //                           ensure compliant behaviour when receiving SinkCommissioningMode command.

}

static void ZGPCmdSetGPDAssignedAlias(void)
{
    // cmd: [Zgp,SetGPDAssignedAlias,<uint16_t assignedAlias>]
    // response: [Zgp,SetGPDAssignedAlias,0,<retval>]
    // description: Sets the GPD assigned alias which should be used as the short address for the next commissioned GPD.
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
        uint16_t alias = scip_Decoder_GetUint16( s_pScip, &scipIndex, &error );
        int8_t result = zigbeeStack_GPSetAssignedAlias(alias);
        scip_PutReplyFmt( s_pScip, "sdd", 3, "SetGPDAssignedAlias", 0, result);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
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
		zclCmdTx.txOptions = 0x040;  //EMBER_APS_OPTION_RETRY
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

static void ZdpCmdSendMgmtPermitJoiningReq(void)

{
    // cmd: [Zdp,SendMgmtPermitJoiningReq,<destAddr>,<uint8_t permitDuration>,<uint8_t trustCenterSignificance>]
    // response: [Zdp,SendMgmtPermitJoiningReq,0,<uint8_t seqNr>]. [Zdp,SendMgmtPermitJoiningReq,<uint8_t error>]
    // description: Send a ZDP MgmtPermitJoiningReq command

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 3)
    {
		zigbee_Address_t destAddr = scipzigbeeext_Decoder_GetBroadcastAddress( s_pScip, &scipIndex, &error );
        uint8_t permitDuration = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );
        uint8_t trustCenterSignificance = scip_Decoder_GetUint8( s_pScip, &scipIndex, &error );

        //log_Printfbasic("destAddr:0x%04x,duration:%d",destAddr.address.zAddress,permitDuration);
		uint8_t result = zigbeeStack_ZdpSendMgmtPermitJoiningReq(&destAddr,permitDuration,trustCenterSignificance);
        uint8_t seqNr = 0;
        zigbeeStack_ZdpGetSequenceNr(&seqNr);

		if(result == 0)
		{
            // suppose: 0 indicates success, then get seqNr
            if(permitDuration>0)
            {
                scip_PutReplyFmt( s_pScip, "ss", 2, "JoinPermitted", "True");
            }
            else
            {
                scip_PutReplyFmt( s_pScip, "ss", 2, "JoinPermitted", "False");
            }
		    scip_PutReplyFmt( s_pScip, "sdd", 3, "SendMgmtPermitJoiningReq", 0, seqNr);
		}
        else
        {
            scip_PutReplyFmt( s_pScip, "ss", 2, "JoinPermitted", "False");
	    scip_PutReplyFmt( s_pScip, "sdd", 3, "SendMgmtPermitJoiningReq", 0, seqNr);
	}
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }    
}

static void ZdpCmdSendMgmtLqiReq(void)
{
    // cmd: [Zdp,SendMgmtLqiReq,<destAddr>,<uint8_t startIndex>]
    // response: [Zdp,SendMgmtLqiReq,0,<uint8_t seqNr>]. [Zdp,SendMgmtLqiReq,<uint8_t error>]
    // description: Send a ZDP MgmtLqiReq command
    
}

static void ZdpCmdSendSimpleDescReq(void)
{
    // cmd: [Zdp,SendSimpleDescReq,<destAddr>,<uint16_t nwkAddrOfInterest>, <uint8_t endPoint>]
    // response: [Zdp,SendSimpleDescReq,0,<uint8_t seqNr>]. [Zdp,SendSimpleDescReq,<uint8_t error>]
    // description: Send a ZDP SimpleDescriptorReq command

}

static void ZdpCmdSendNodeDescReq(void)
{
    // cmd: [Zdp,SendNodeDescReq,<destAddr>,<uint16_t nwkAddrOfInterest>]
    // response: [Zdp,SendNodeDescReq,0,<uint8_t seqNr>]. [Zdp,SendNodeDescReq,<uint8_t error>]
    // description: Send a ZDP SendNodeDescReq command
    
}

static void ZdpCmdSendActiveEndPointReq(void)
{
    // cmd: [Zdp,SendActiveEndPointReq,<destAddr>,<uint8_t nwkAddrOfInterest>]
    // response: [Zdp,SendActiveEndPointReq,0,<uint8_t seqNr>]. [Zdp,SendActiveEndPointReq,<uint8_t error>]
    // description: Send a ZDP ActiveEndPointReq command

}

static void ZdpCmdNwkAddrReq(void)
{
    // cmd: [Zdp,NwkAddrReq,<destAddr>,<ieeeAddrOfInterest>]
    // response: [Zdp,NwkAddrReq,0,<uint8_t seqNr>]. [Zdp,NwkAddrReq,<uint8_t error>]
    // description: Send a ZDP NwkAddrReq command
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 2)
    {
		zigbee_Address_t destAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );

        uint16_t ieeeAddrOfInterest = scip_Decoder_GetUint16(s_pScip,&scipIndex,&error);
		zigbeeStack_ZdpNwkAddrReq(&destAddr, ieeeAddrOfInterest);

        // suppose: 0 indicates success, then get seqNr
        uint8_t seqNr = 0;
        zigbeeStack_ZdpGetSequenceNr(&seqNr);
        scip_PutReplyFmt( s_pScip, "sdd", 3, "NwkAddrReq", 0, seqNr);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ZdpCmdIeeeAddrReq(void)
{
    // cmd: [Zdp,IeeeAddrReq,<destAddr>,<uint16_t nwkAddrOfInterest>]
    // response: [Zdp,IeeeAddrReq,0,<uint8_t seqNr>]. [Zdp,IeeeAddrReq,<uint8_t error>]
    // description: Send a ZDP IeeeAddrReq command

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 2)
    {
		zigbee_Address_t destAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );
		
        uint16_t nwkAddrOfInterest = scip_Decoder_GetUint16(s_pScip,&scipIndex,&error);
		uint8_t result = zigbeeStack_ZdpIeeeAddrReq(&destAddr,nwkAddrOfInterest);

		if(result == 0)
		{
            // suppose: 0 indicates success, then get seqNr
            uint8_t seqNr = 0;
            zigbeeStack_ZdpGetSequenceNr(&seqNr);
		    scip_PutReplyFmt( s_pScip, "sdd", 3, "IeeeAddrReq", 0, seqNr);
		}
        else
        {
		    scip_PutReplyFmt( s_pScip, "sd", 2, "IeeeAddrReq", result);
		}
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }  
}

void ZdpCmdNwkAddrResponse(uint8_t* pIeeeAddr, uint16_t nodeId)
{
    uint8_t seqNr = 0;
	zigbeeStack_ZdpGetSequenceNr(&seqNr);

	logcom_Printfbasic("Zdp,ReceivedNwkAddrRsp,%d,S=0x%04X.0,%d,L=%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,S=0x%04X.0",
	                seqNr,
	                nodeId,
	                0,
					*(pIeeeAddr+7),*(pIeeeAddr+6),*(pIeeeAddr+5),*(pIeeeAddr+4),*(pIeeeAddr+3),*(pIeeeAddr+2),*(pIeeeAddr+1),*(pIeeeAddr+0),
					nodeId);
}

void ZdpCmdIeeeAddrResponse(uint8_t* pIeeeAddr, uint16_t nodeId)
{
	uint8_t seqNr = 0;
	zigbeeStack_ZdpGetSequenceNr(&seqNr);

	logcom_Printfbasic("Zdp,ReceivedIeeeAddrRsp,%d,S=0x%04X.0,%d,L=%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,S=0x%04X.0",
	                seqNr,
	                nodeId,
	                0,
					*(pIeeeAddr+7),*(pIeeeAddr+6),*(pIeeeAddr+5),*(pIeeeAddr+4),*(pIeeeAddr+3),*(pIeeeAddr+2),*(pIeeeAddr+1),*(pIeeeAddr+0),
					nodeId);	
}

static void ZdpCmdSendBindReq(void)
{
    // cmd: [Zdp,SendBindReq,<destAddr>,<bindSrc>,<uint16_t clusterId>,<bindDest>,<EPbindDest>]
    // response: [Zdp,SendBindReq,0,<uint8_t seqNr>]. [Zdp,SendBindReq,<uint8_t error>]
    // description: Send a ZDP BindReq command
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    uint8_t scip_InputArgumentCout = 0;
    scip_InputArgumentCout = scip_GetInputArgumentCount(s_pScip);
    if ((scip_InputArgumentCout == 4) || (scip_InputArgumentCout == 5))
    {
	    zigbee_Address_t destAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );
        zigbee_Address_t bindSrc = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );		
        uint16_t clusterId = scip_Decoder_GetUint16(s_pScip,&scipIndex,&error);
        zigbee_Address_t bindDest = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );
        if(scip_InputArgumentCout == 4)
        {
            bindDest.endpoint = 0;
        }else
        {
            uint8_t EPbindDest = scip_Decoder_GetUint8(s_pScip,&scipIndex,&error);
            bindDest.endpoint = EPbindDest;
        }
        if((destAddr.mode != zigbee_AddrMode_Short) || 
        (bindSrc.mode != zigbee_AddrMode_Ieee) ||
        (bindDest.mode != zigbee_AddrMode_Ieee))
        {
            log_Printfbasic("PARAMETER ERROR!");
        }else
        {
            uint8_t result = zigbeeStack_ZdpSendBindReq(&destAddr,&bindSrc,clusterId,&bindDest);

            if(result == 0)
            {
                // suppose: 0 indicates success, then get seqNr
                uint8_t seqNr = 0;
                zigbeeStack_ZdpGetSequenceNr(&seqNr);
                scip_PutReplyFmt( s_pScip, "sdd", 3, "SendBindReq", 0, seqNr);
            }
            else
            {
                scip_PutReplyFmt( s_pScip, "sd", 2, "SendBindReq", result);
            }
        }
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }  
}

static void ZdpCmdSendUnbindReq(void)
{
    // cmd: [Zdp,SendUnbindReq,<destAddr>,<bindSrc>,<uint16_t clusterId>,<bindDest>,<EPbindDest>]
    // response: [Zdp,SendUnbindReq,0,<uint8_t seqNr>]. [Zdp,SendUnbindReq,<uint8_t error>]
    // description: Send a ZDP UnbindReq command
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;
    uint8_t scip_InputArgumentCout = 0;
    scip_InputArgumentCout = scip_GetInputArgumentCount(s_pScip);
    if ((scip_InputArgumentCout == 4) || (scip_InputArgumentCout == 5))
    {
	    zigbee_Address_t destAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );
        zigbee_Address_t bindSrc = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );		
        uint16_t clusterId = scip_Decoder_GetUint16(s_pScip,&scipIndex,&error);
        zigbee_Address_t bindDest = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );
        if(scip_InputArgumentCout == 4)
        {
            bindDest.endpoint = 0;
        }else
        {
            uint8_t EPbindDest = scip_Decoder_GetUint8(s_pScip,&scipIndex,&error);
            bindDest.endpoint = EPbindDest;
        }
        if((destAddr.mode != zigbee_AddrMode_Short) || 
        (bindSrc.mode != zigbee_AddrMode_Ieee) ||
        (bindDest.mode != zigbee_AddrMode_Ieee))
        {
            log_Printfbasic("PARAMETER ERROR!");
        }else
        {
            uint8_t result = zigbeeStack_ZdpSendUnbindReq(&destAddr,&bindSrc,clusterId,&bindDest);

            if(result == 0)
            {
                // suppose: 0 indicates success, then get seqNr
                uint8_t seqNr = 0;
                zigbeeStack_ZdpGetSequenceNr(&seqNr);
                scip_PutReplyFmt( s_pScip, "sdd", 3, "SendUnBindReq", 0, seqNr);
            }
            else
            {
                scip_PutReplyFmt( s_pScip, "sd", 2, "SendUnBindReq", result);
            }
        }
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }
}

static void ZdpCmdSendMgmtBindReq(void)
{
    // cmd: [Zdp,SendMgmtBindReq,<destAddr>,<uint8_t startIndex>]
    // response: [Zdp,SendMgmtBindReq,0,<uint8_t seqNr>]. [Zdp,SendMgmtBindReq,<uint8_t error>]
    // description: Send a ZDP MgmtBindReq command
    
}

static void ZdpCmdSendMatchDescReq(void)
{
    // cmd: [Zdp,SendMatchDescReq,<destAddr>,<uint16_t networkAddressOfInterest>,
    //         <uint16_t profileId>,<uint16_t clusterId>]
    // response: [Zdp,SendMatchDescReq,0,<uint8_t seqNr>]. [Zdp,SendMatchDescReq,<uint8_t error>]
    // description: Send a ZDP MatchDescReq command
    
}

static void ZdpCmdSendMgmtLeaveReq(void)
{
    // cmd: [Zdp,SendMgmtLeaveReq,<destAddr>,<deviceIeeeAddr>,<bool removeChildren>,<bool rejoin>]
    // response: [Zdp,SendMgmtLeaveReq,0,<uint8_t seqNr>]. [Zdp,SendMgmtLeaveReq,<uint8_t error>]
    // description: Send a ZDP MgmtLeaveReq command

    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 4)
    {
		zigbee_Address_t destAddr = scipzigbeeext_Decoder_GetZAddress( s_pScip, &scipIndex, &error );
		
		// comments: is ieeeAddr required in this command? actually destAddr is enough.
        zigbee_IeeeAddress_t ieeeAddr = scipzigbeeext_Decoder_GetIeeeAddress(s_pScip,&scipIndex,&error);
        bool removeChildren = scip_Decoder_GetBool( s_pScip, &scipIndex, &error );
        bool rejoin = scip_Decoder_GetBool( s_pScip, &scipIndex, &error );
		uint8_t result = zigbeeStack_ZdpSendMgmtLeaveReq(&destAddr,ieeeAddr.b,removeChildren,rejoin);

        uint8_t bufFmt[30] = {0};
		sprintf((char*)bufFmt,"L=%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
								ieeeAddr.b[7],ieeeAddr.b[6],ieeeAddr.b[5],ieeeAddr.b[4],ieeeAddr.b[3],
								ieeeAddr.b[2],ieeeAddr.b[1],ieeeAddr.b[0]);
		if(result == 0)
		{
            // suppose: 0 indicates success, then get seqNr
            uint8_t seqNr = 0;
            zigbeeStack_ZdpGetSequenceNr(&seqNr);
            scip_PutReplyFmt( s_pScip, "sdd", 3, "SendMgmtLeaveReq", result, seqNr);
            //scip_PutReplyFmt( s_pScip, "sdsd", 4, "ReceivedMgmtLeaveRsp", seqNr, bufFmt, result);
		}
        else
        {
		    scip_PutReplyFmt( s_pScip, "sd", 2, "SendMgmtLeaveReq", result);
		}
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
    }    
}


static void RoutingCmdSendMtoRR(void)
{
    // cmd: [Routing,SendMtoRR,<bool noRouteCache>]
    // response: [Routing,SendMtoRR,0,<N_SourceRoutering_Status_t>]
    // description: Broadcasts a ManytoOne Route Request commands.
    
}

static void RoutingCmdClearEntry(void)
{
    // cmd: [Routing,ClearEntry,<uint16_t nwkAddress>]
    // resposne: [Routing,ClearEntry,0,<RTG_Status_t>]
    // description: Removes any routing entry for the given <nwkAddress>.
}

static void BrCmdStoreGroupRange(void)
{
	// cmd: [Bridge,StoreGroupRange,<uint16_t groupRangeBegin>,<uint16_t groupRangeEnd>]
    // response: [Bridge,StoreGroupRange,0]
    // description: Stores these 2 integers. They are logged at startup.
    
}

static void BrCmdIdentify(void)
{
    // cmd: [Bridge,Identify,<uint16_t identifyTime100ms>]
    // response: [Bridge,Identify,0]
    // description: Triggers a visual identify on bridges that have a LED. Otherwise noop.
    
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
		uint8_t result = zigbeeStack_SetChannelMask(channelMask,0);
		scip_PutReplyFmt( s_pScip, "sd", 2, "SetChannelMask",result);
    }
    else if (scip_GetInputArgumentCount(s_pScip) == 2)
    {
        uint32_t channelMask = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
        uint32_t secondaryChannelMask = scip_Decoder_GetUint32( s_pScip, &scipIndex, &error );
		uint8_t result = zigbeeStack_SetChannelMask(channelMask,secondaryChannelMask);
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

static void ThCmdSuspendScheduler(void)
{
    int scipIndex = 2;
    scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 1)
    {
		bool_t suspend = scip_Decoder_GetBool( s_pScip, &scipIndex, &error );

		if(suspend) {
		    osa_SchedulerSuspend();
		}else{
            osa_SchedulerResume();
        }
        
		scip_PutReplyFmt( s_pScip, "sd", 2, "SuspendScheduler", 0);
    }
    else
    {
        log_Printfbasic("INPUT ERROR!");
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


static void ThCmdPrintSourceRouteTable(void)
{
    //int scipIndex = 2;
    //scip_Decoder_Error_t error = scip_Decoder_Error_NoError;

    if (scip_GetInputArgumentCount(s_pScip) == 0)
    {
        //int8_t value;
	    zigbeeStack_PrintSourceRouteTable();
    }
}

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/



void NetworkCreatorCompleteUartSend(void)
{
    uint16_t updateId;
    uint8_t  channel;
    uint16_t nwkAddr;
    uint16_t panId;
    uint8_t ieeeAddr[8] = {0};
    uint8_t nwkExtPanId[8] = {0};
    bool formCentralizedNetwork = zigbeeStack_IsCentralizedNetwork();
  
    zigbeeStack_SetShortAddress(0);
    zigbeeStack_DumpInfo(&updateId,&channel,&nwkAddr,&panId,ieeeAddr);
  
    zigbeeStack_GetExtendedPanId(nwkExtPanId);
  
    logcom_Printfbasic("Bridge,NetworkSettings,False,0x%04X,%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,%d,%d,S=0x%04X",
  						   panId,nwkExtPanId[7],nwkExtPanId[6],nwkExtPanId[5],nwkExtPanId[4],nwkExtPanId[3],
  						   nwkExtPanId[2],nwkExtPanId[1],nwkExtPanId[0],channel,updateId,nwkAddr);

    if(formCentralizedNetwork)
    {
      logcom_Printfbasic( "Connection,StartCoordinator,%d",0);
    }
    else
    {
      logcom_Printfbasic( "Connection,StartCoordinator,%d",0);
    }
    logcom_Printfbasic("Connection,StartCoordinatorDone,0,0x%04X,%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,%d,%d",
  						   panId,nwkExtPanId[7],nwkExtPanId[6],nwkExtPanId[5],nwkExtPanId[4],nwkExtPanId[3],
  						   nwkExtPanId[2],nwkExtPanId[1],nwkExtPanId[0],channel,updateId);

}

void PowerOnNetworkStatusUartSend(void)
{
    static bool NetworkStatusLog = true;
	if(NetworkStatusLog)
	{
	    NetworkStatusLog = false;
		if (zigbeeStack_GetNetWorkStatus())
		{
		    logcom_Printfbasic("Bridge,NetworkSettings,True");
		}
		else
		{
		    uint16_t updateId;
			uint8_t  channel;
			uint16_t nwkAddr;
			uint16_t panId;
			uint8_t ieeeAddr[8] = {0};
			uint8_t nwkExtPanId[8] = {0};

     		zigbeeStack_GetExtendedPanId(nwkExtPanId);
     		
			zigbeeStack_DumpInfo(&updateId,&channel,&nwkAddr,&panId,ieeeAddr);

 		    logcom_Printfbasic("Bridge,NetworkSettings,False,0x%04X,%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,%d,%d,S=0x%04X",
								  panId,nwkExtPanId[7],nwkExtPanId[6],nwkExtPanId[5],nwkExtPanId[4],nwkExtPanId[3],
								  nwkExtPanId[2],nwkExtPanId[1],nwkExtPanId[0],channel,updateId,nwkAddr);
		}
	}
}
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

const testframework_Wrapper_t  l_c4m_linkCommandHandler =
{
    "Link",
    sizeof( "Link" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_linkCommandTable,
    NULL,
};

const testframework_Wrapper_t  l_c4m_tcCommandHandler =
{
    "TrustCenter",
    sizeof( "TrustCenter" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_trustCenterCommandTable,
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

const testframework_Wrapper_t  l_c4m_ZGPCommandHandler =
{
    "Zgp",
    sizeof( "Zgp" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_ZGPCommandTable,
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

const testframework_Wrapper_t  l_c4m_zdpCommandHandler =
{
    "Zdp",
    sizeof( "Zdp" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_zdpCommandTable,
    NULL,
};

const testframework_Wrapper_t  l_c4m_routingCommandHandler =
{
    "Routing",
    sizeof( "Routing" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_routingCommandTable,
    NULL,
};

const testframework_Wrapper_t  l_c4m_bridgeCommandHandler =
{
    "Bridge",
    sizeof( "Bridge" ),
    C4MCmd_Init,
    C4MCmd_TurnOn,
    s_bridgeCommandTable,
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


const scipConfig_t scipConfig =
{
    .gpCommissioningNotification = ZGPCommissioningNotification,
    .gpDeviceInfo = ZGPDeviceInfo
};
