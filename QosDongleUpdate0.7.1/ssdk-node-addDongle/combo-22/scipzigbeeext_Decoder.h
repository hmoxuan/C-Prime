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

#ifndef __SCIPZIGBEEEXT_DECODER_H
#define __SCIPZIGBEEEXT_DECODER_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "scip.h"
#include "scip_Decoder.h"

#include "zigbee_types.h"

#include "lcp_variant.h"
#include "lcp_types.h"

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED TYPES
***************************************************************************************************/

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/*!
 * @brief  Attempt to parse an zigbee_ExtPanId_t from a pScip entity.
 * @param  [in]     pScip          A pScip instance
 * @param  [in,out] pIndex         The pIndex of the pScip entity to parse. This function will increase the pIndex regardless of the pError state.
 * @param  [in,out] pError         The first error encountered by the parser. Parsing is skipped in case *pError != zigbeelink_Decoder_Error_NoError
 * @return The parsed zigbee_ExtPanId_t value in case *pError == zigbeelink_Decoder_Error_NoError, the value is undefined otherwise.
 */
zigbee_ExtPanId_t scipzigbeeext_Decoder_GetExtPanId(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError);

/*!
 * @brief  Attempt to parse an zigbee_IeeeAddress_t from a pScip entity.
 * @param  [in]     pScip          A pScip instance
 * @param  [in,out] pIndex         The pIndex of the pScip entity to parse. This function will increase the pIndex regardless of the pError state.
 * @param  [in,out] pError         The first error encountered by the parser. Parsing is skipped in case *pError != zigbeelink_Decoder_Error_NoError
 * @return The parsed zigbee_IeeeAddress_t value in case *pError == zigbeelink_Decoder_Error_NoError, the value is undefined otherwise.
 */
zigbee_IeeeAddress_t scipzigbeeext_Decoder_GetIeeeAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError);

/*!
 * @brief  Attempt to parse an zigbee_Address_t from a pScip entity.
 * @param  [in]     pScip          A pScip instance
 * @param  [in,out] pIndex         The pIndex of the pScip entity to parse. This function will increase the pIndex regardless of the pError state.
 * @param  [in,out] pError         The first error encountered by the parser. Parsing is skipped in case *pError != zigbeelink_Decoder_Error_NoError
 * @return The parsed zigbee_Address_t value in case *pError == zigbeelink_Decoder_Error_NoError, the value is undefined otherwise.
 */
zigbee_Address_t scipzigbeeext_Decoder_GetZAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError);

#if defined (CWP_DONGLE)
/*!
 * @brief  Attempt to parse an zigbe broad-address from a pScip entity.
 * @param  [in]     pScip          A pScip instance
 * @param  [in,out] pIndex         The pIndex of the pScip entity to parse. This function will increase the pIndex regardless of the pError state.
 * @param  [in,out] pError         The first error encountered by the parser. Parsing is skipped in case *pError != zigbeelink_Decoder_Error_NoError
 * @return The parsed zigbee broad-address value in case *pError == zigbeelink_Decoder_Error_NoError, the value is undefined otherwise.
 */
zigbee_Address_t scipzigbeeext_Decoder_GetBroadcastAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError);
#endif

/*!
 * @brief  Attempt to parse an zigbe short-address from a pScip entity.
 * @param  [in]     pScip          A pScip instance
 * @param  [in,out] pIndex         The pIndex of the pScip entity to parse. This function will increase the pIndex regardless of the pError state.
 * @param  [in,out] pError         The first error encountered by the parser. Parsing is skipped in case *pError != zigbeelink_Decoder_Error_NoError
 * @return The parsed zigbee short-address value in case *pError == zigbeelink_Decoder_Error_NoError, the value is undefined otherwise.
 */
uint16_t scipzigbeeext_Decoder_GetShortAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError);

/*!
 * @brief  Attempt to parse an zigbee_GetInstallCodeAddress_t from a pScip entity.
 * @param  [in]     pScip          A pScip instance
 * @param  [in,out] pIndex         The pIndex of the pScip entity to parse. This function will increase the pIndex regardless of the pError state.
 * @param  [in,out] pError         The first error encountered by the parser. Parsing is skipped in case *pError != zigbeelink_Decoder_Error_NoError
 * @return The parsed zigbee_IeeeAddress_t value in case *pError == zigbeelink_Decoder_Error_NoError, the value is undefined otherwise.
 */
zigbee_InstallCodeAddress_t scipzigbeeext_Decoder_GetInstallCodeAddress(const Scip_t* pScip, int32_t *pIndex, scip_Decoder_Error_t* pError);

/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __SCIP_DECODER_H
