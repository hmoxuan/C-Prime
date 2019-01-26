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
* LOCAL INCLUDE FILES
******************************************************************************/

#include "scipzigbeeext_Local.h"
#include "scipzigbeeext_Decoder.h"

/******************************************************************************
* EXTERNAL INCLUDE FILES
******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#include "lcp_assert.h"
#include "lcp_variant.h"
#include "lcp_types.h"

/******************************************************************************
* LOCAL MACROS AND CONSTANTS
******************************************************************************/

/******************************************************************************
* LOCAL TYPES
******************************************************************************/

/******************************************************************************
* LOCAL VARIABLES
******************************************************************************/

/******************************************************************************
* LOCAL FUNCTION DECLARATIONS
******************************************************************************/

static zigbee_Address_t DecodeAddressNotPresent( char *strAddress, scip_Decoder_Error_t* pError );
static zigbee_Address_t DecodeShortAddress( char *strAddress, scip_Decoder_Error_t* pError, bool_t withEndPoint );
static zigbee_Address_t DecodeIeeeAddress( char *strAddress,scip_Decoder_Error_t*  pError );
static zigbee_Address_t DecodeGroupAddress( char *strAddress, scip_Decoder_Error_t* pError );
static zigbee_Address_t DecodeBroadcastAddress( char *strAddress, scip_Decoder_Error_t* pError );

static void DecodeShortAddressAndEndpoint( zigbee_Address_t *    pAddress,
                                           char *                strAddress,
                                           scip_Decoder_Error_t* pError,
                                           bool_t withEndPoint );

/******************************************************************************
* STATE MACHINE
******************************************************************************/

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/

static zigbee_Address_t DecodeAddressNotPresent( char *strAddress, scip_Decoder_Error_t* pError )
{
    zigbee_Address_t result = {{ 0 }};

    (void)strAddress;
    (void)pError;

    result.mode = zigbee_AddrMode_NotPresent;

    return result;
}

static zigbee_Address_t DecodeShortAddress( char *strAddress, scip_Decoder_Error_t* pError, bool_t withEndPoint )
{
    zigbee_Address_t result = {{ 0 }};
    result.mode = zigbee_AddrMode_Short;

    DecodeShortAddressAndEndpoint( &result, strAddress, pError, withEndPoint );

    return result;
}

/*lint -e{613} suppress "Possible use of null pointer" */
static zigbee_Address_t DecodeIeeeAddress( char *strAddress, scip_Decoder_Error_t*  pError )
{
    int length = 8;
    zigbee_Address_t result = {{ 0 }};
    result.mode = zigbee_AddrMode_Ieee;

    if ( *strAddress != (char)'=' )
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
    }

    ++strAddress;
    scip_DecoderHelper_GetHexBytes( &length, result.address.ieeeAddress.b, strAddress, pError );
    if ( length != 8 )
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
    }

    return result;
}

/*lint -e{613} suppress "Possible use of null pointer" */
static zigbee_Address_t DecodeGroupAddress( char *strAddress, scip_Decoder_Error_t* pError )
{
    zigbee_Address_t result = {{ 0 }};
    char *pEnd;

    result.mode = zigbee_AddrMode_Group;
    result.endpoint = 0xFFu;

    if ( *strAddress != (char)'=' )
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
    }

    ++strAddress;
    result.address.zAddress = scip_DecoderHelper_StrToUint32( strAddress, &pEnd, pError );
    if (   (*pError == scip_Decoder_Error_NoError)
        && (*pEnd != '\0') )
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
    }

    return result;
}

static zigbee_Address_t DecodeBroadcastAddress( char *strAddress, scip_Decoder_Error_t* pError )
{
    zigbee_Address_t result;
    result.mode = zigbee_AddrMode_Broadcast;

    DecodeShortAddressAndEndpoint( &result, strAddress, pError, true );

    return result;
}

/*lint -e{613} suppress "Possible use of null pointer" */
static void DecodeShortAddressAndEndpoint( zigbee_Address_t *    result,
                                           char *                strAddress,
                                           scip_Decoder_Error_t* pError,
                                           bool_t withEndPoint )
{
    char *pEnd;

    if ( *strAddress != (char)'=' )
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
        return;
    }

    ++strAddress;
    result->address.zAddress = scip_DecoderHelper_StrToUint32(strAddress, &pEnd, pError);
    if ( *pError != scip_Decoder_Error_NoError )
    {
        return;
    }

    if (withEndPoint)
    {
        strAddress = pEnd;
        if ( *strAddress != (char)'.' )
        {
            *pError = scip_Decoder_Error_IncorrectFormatting;
            return;
        }

        ++strAddress;
        result->endpoint = scip_DecoderHelper_StrToUint32(strAddress, &pEnd, pError);
        if (   (*pError == scip_Decoder_Error_NoError)
            && (*pEnd != '\0') )
        {
            *pError = scip_Decoder_Error_IncorrectFormatting;
        }
    }
}

/******************************************************************************
* EXPORTED FUNCTIONS
******************************************************************************/

zigbee_ExtPanId_t scipzigbeeext_Decoder_GetExtPanId(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError)
{
    zigbee_ExtPanId_t result;
    int length = 8;

    scip_Decoder_GetData(&length, result.b, pScip, pIndex, pError );
    if ( length != 8 )
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
    }

    return result;
}

zigbee_IeeeAddress_t scipzigbeeext_Decoder_GetIeeeAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError)
{
    zigbee_IeeeAddress_t result;
    int length = 8;

    scip_Decoder_GetData(&length, result.b, pScip, pIndex, pError );
    if ( length != 8 )
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
    }

    return result;
}

zigbee_InstallCodeAddress_t scipzigbeeext_Decoder_GetInstallCodeAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError)
{
    zigbee_InstallCodeAddress_t result;
    int length = 16;

    scip_Decoder_GetData(&length, result.b, pScip, pIndex, pError );
    if ( length != 16 )
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
    }

    return result;
}

/*lint -e{613} suppress "Possible use of null pointer" */
zigbee_Address_t scipzigbeeext_Decoder_GetZAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError)
{
    zigbee_Address_t result = {{ 0 }};
    char *strAddress;
    char type;

    assert( pScip );
    assert( pIndex );
    assert( pError );

    strAddress = scip_GetEntity( pScip, *pIndex );
    ++(*pIndex);

    if ( ! strAddress )
    {
        *pError = scip_Decoder_Error_MissingArgument;
        return result;
    }

    // skip leading spaces
    for ( ; isspace(*strAddress); ++strAddress )
    {
        // empty
    }

    type = *strAddress;
    ++strAddress;

    switch ( type )
    {
    case 'N' :
        result = DecodeAddressNotPresent( strAddress, pError );
        break;
    case 'S' :
        result = DecodeShortAddress( strAddress, pError, true );
        break;
    case 'L' :
        result = DecodeIeeeAddress( strAddress, pError );
        break;
    case 'G' :
        result = DecodeGroupAddress( strAddress, pError );
        break;
    case 'B' :
        result = DecodeBroadcastAddress( strAddress, pError );
        break;
    default :
        *pError = scip_Decoder_Error_IncorrectFormatting;
        break;
    }

    return result;
}

#if defined (CWP_DONGLE)
zigbee_Address_t scipzigbeeext_Decoder_GetBroadcastAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError)
{
    zigbee_Address_t result = {{ 0 }};
    char *strAddress;
    char type;

    assert( pScip );
    assert( pIndex );
    assert( pError );

    strAddress = scip_GetEntity( pScip, *pIndex );
    ++(*pIndex);

    if ( ! strAddress )
    {
        *pError = scip_Decoder_Error_MissingArgument;
        return result;
    }

    // skip leading spaces
    for ( ; isspace(*strAddress); ++strAddress )
    {
        // empty
    }

    type = *strAddress;
    ++strAddress;

    switch ( type )
    {
    case 'B' :
        result.mode = zigbee_AddrMode_Broadcast;
        DecodeShortAddressAndEndpoint( &result, strAddress, pError, false );
        break;
    default :
        *pError = scip_Decoder_Error_IncorrectFormatting;
        break;
    }

    return result;
}
#endif

//lint -e613
uint16_t scipzigbeeext_Decoder_GetShortAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError)
{
    zigbee_Address_t result = {{ 0 }};

    char *strAddress;
    char type;

    assert( pScip );
    assert( pIndex );
    assert( pError );

    strAddress = scip_GetEntity( pScip, *pIndex );
    ++(*pIndex);

    if ( ! strAddress )
    {
        *pError = scip_Decoder_Error_MissingArgument;
        return (uint16_t)(-1);
    }

    // skip leading spaces
    for ( ; isspace(*strAddress); ++strAddress )
    {
        // empty
    }

    type = *strAddress;
    ++strAddress;

    if (type == 'S')
    {
        result = DecodeShortAddress( strAddress, pError, false );
    }
    else
    {
        *pError = scip_Decoder_Error_IncorrectFormatting;
    }

    return result.address.zAddress;
}
