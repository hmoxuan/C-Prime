/******************************************************************************/
/*                                                                            */
/*    $Workfile::   <file>.h                                             $    */
/*                                                                            */
/*    $Revision:: 971                                                    $    */
/*                                                                            */
/*     $Archive::                                                        $    */
/*                                                                            */
/*     $Modtime::                                                        $    */
/*                                                                            */
/*        $Date:: 2011-03-21 13:11:49 +0100 (Mon, 21 Mar 2011)           $    */
/*                                                                            */
/*      $Author:: maase                                                  $    */
/*                                                                            */
/*        Owner::   <Enter the name of the original owner>                    */
/*                                                                            */
/*  Description::   <description of component>                                */
/*                                                                            */
/*   Department::   B.G. Lighting Electronics                                 */
/*                                                                            */
/*                  Philips Company Confidential.                             */
/*                  Copyright (C) 2010 Koninklijke Philips Electronics N.V.   */
/*                  All rights reserved.                                      */
/*                                                                            */
/******************************************************************************/

#ifndef DEVICE_INFO_PRINT_H
#define DEVICE_INFO_PRINT_H

/******************************************************************************/
/*                            CONSTANT DEFINITIONS                            */
/******************************************************************************/

/******************************************************************************/
/*                              MACRO DEFINITIONS                             */
/******************************************************************************/
#define DEVICE_INFO_PRINT_WIDTH   (60)
#define DEVICE_INFO_PRINT_DEPTH   (200)

#define DEVICE_SIGNON_QUEUE_ITEM_SIZE (60)
#define DEVICE_SIGNON_QUEUE_SIZE      (200)
#define DEVICE_SIGNON_QUEUE_TICKS       (10)


#define DEVICE_ANNOUNCE_IN_FIFO   1
#define DEVICE_SERVICE_TAG_IN_INFO 2
#define REPORT_NODE_DEVICE_ADDRESS_INFO 3

/******************************************************************************/
/*                              TYPE DEFINITIONS                              */
/******************************************************************************/


/*! Queue entry structure */
typedef struct deviceSignon_QueueEntry_t
{
    uint8_t  buf[DEVICE_SIGNON_QUEUE_ITEM_SIZE]; 
} deviceSignon_QueueEntry_t;


/******************************************************************************/
/*                          PUBLIC DATA DEFINITIONS                           */
/******************************************************************************/


/******************************************************************************/
/*                         PUBLIC FUNCTION PROTOTYPES                         */
/******************************************************************************/

void Device_SignOn_Init(void);
void Device_SignOn_PreSendMessage(uint8_t* payload);

#endif

/******************************************************************************/
/*                                 END OF FILE                                */
/******************************************************************************/




