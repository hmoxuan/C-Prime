/******************************************************************************/
/*                                                                            */
/*    $Workfile::   sourcefile.c                                   $          */
/*                                                                            */
/*    $Revision:: 1277                                             $          */
/*                                                                            */
/*     $Archive::                                                  $          */
/*                                                                            */
/*     $Modtime::                                                  $          */
/*                                                                            */
/*        $Date:: 2011-10-26 22:34:37 +0200 (Wed, 26 Oct 2011)     $          */
/*                                                                            */
/*      $Author:: maase                                            $          */
/*                                                                            */
/*        Owner::   <Enter the name of the original owner>                    */
/*                                                                            */
/*  Description::   <Description>                                             */
/*                                                                            */
/*   Department::   B.G. Lighting Electronics                                 */
/*                                                                            */
/*                  Philips Company Confidential.                             */
/*                  Copyright (C) 2010 Koninklijke Philips Electronics N.V.   */
/*                  All rights reserved.                                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                               INCLUDE FILES                                */
/******************************************************************************/


#include "log.h"

#include "ctimer.h"
#include "zigbeestack-api.h"


/* Own headerfile last */
#include "cwp_dongle_startup.h"

/******************************************************************************/
/*                            CONSTANT DEFINITIONS                            */
/******************************************************************************/



/******************************************************************************/
/*                              MACRO DEFINITIONS                             */
/******************************************************************************/




/******************************************************************************/
/*                              TYPE DEFINITIONS                              */
/******************************************************************************/







/******************************************************************************/
/*                          PUBLIC DATA DEFINITIONS                           */
/******************************************************************************/


extern uint16_t openNetworkTimeRemainingS;





/******************************************************************************/
/*                        PRIVATE FUNCTION PROTOTYPES                         */
/******************************************************************************/


static void dongle_StartupReady(void);
static void dongle_OpenNetwork(void);




/******************************************************************************/
/*                          PRIVATE DATA DEFINITIONS                          */
/******************************************************************************/

static uint8_t nowTime = 0;
static bool printStartupMessage = true;




/******************************************************************************/
/*                          EXTERNAL FUNCTION DEFINITIONS                          */
/******************************************************************************/

extern uint8_t emberAfNetworkState(void);






/******************************************************************************/
/*                      PUBLIC FUNCTION IMPLEMENTATIONS                       */
/******************************************************************************/


void dongle_Startup(void)
{
    /* Start a dummy task, just print a debug message repeatly */
    //CTIMER_Set(1000, dongle_StartupReady);
    CTIMER_Set(1000, dongle_OpenNetwork);
}



/******************************************************************************/
/*                      PRIVATE FUNCTION IMPLEMENTATIONS                       */
/******************************************************************************/

static void dongle_StartupReady(void)
{
    // dunfa comments: the implementation MUST be reviewed!!!
    // degrade system performance very much!!!

	if(printStartupMessage)
	{
	    nowTime++;
	    
		if(nowTime == 1)
		{
			logcom_Printfbasic("TH,Ready,0");
		}
		// delay 2 seconds to print dongle device announce
		else if(nowTime == 3)
		{
			printStartupMessage = false;
			uint16_t updateId;
			uint8_t  channel;
			uint16_t nwkAddr;
			uint16_t panId;
			uint8_t ieeeAddr[8] = {0};
					 
			zigbeeStack_DumpInfo(&updateId,&channel,&nwkAddr,&panId,ieeeAddr);
					 			
//			if(nwkAddr == 0x0000u)
            if(2 /*EMBER_JOINED_NETWORK*/ == emberAfNetworkState())
			{
				logcom_Printfbasic("Zdp,ReceivedDeviceAnnounce,S=0x%04X.0,L=%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X,%d",
									/*nwkAddr*/0, ieeeAddr[7],ieeeAddr[6],ieeeAddr[5],ieeeAddr[4],ieeeAddr[3],
									ieeeAddr[2],ieeeAddr[1],ieeeAddr[0],143);
			}
		}
	}

}

static void dongle_OpenNetwork(void)
{
    // dunfa comments: the implementation MUST be reviewed!!!
    // degrade system performance very much!!!

	if(openNetworkTimeRemainingS)
	{
	    openNetworkTimeRemainingS--;
	    if(0 == openNetworkTimeRemainingS)
	    {
	        logcom_Printfbasic("Zdp,JoinPermitted,False");
	    }
	}
}



/******************************************************************************/
/*                                 END OF FILE                                */
/******************************************************************************/
