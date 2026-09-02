/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/fallback_policy.h
 *
 * PURPOSE:
 *   canonical fallback selection for unavailable frontend capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_FALLBACK_POLICY_H
#define UMICOM_FRONTEND_CONFORMANCE_FALLBACK_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named fc fallback kind values accepted by this public contract.
 */
typedef enum UmiFcFallbackKind { UMI_FC_FALLBACK_NONE=0,UMI_FC_FALLBACK_TAB=1,UMI_FC_FALLBACK_STACK=2,UMI_FC_FALLBACK_DIALOG=3,UMI_FC_FALLBACK_STATIC=4,UMI_FC_FALLBACK_SERVER_DRIVEN=5 } UmiFcFallbackKind;
/**
 * Provide the fc fallback policy for operation used by this module and its client
 * applications.
 */
UmiFcFallbackKind umi_fc_fallback_policy_for(UmiFcCapabilityKind capability,UmiFcFrontendKind frontend);
/**
 * Provide the fc fallback policy preserves semantics operation used by this module and its
 * client applications.
 */
bool umi_fc_fallback_policy_preserves_semantics(UmiFcFallbackKind fallback);

#ifdef __cplusplus
}
#endif
#endif
