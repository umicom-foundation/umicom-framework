/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolution_context.h
 *
 * PURPOSE:
 *   Detect recursive resolution cycles using an explicit dependency stack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_CONTEXT_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_CONTEXT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap resolution context from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_resolution_context_init(UmiBootstrapResolutionContext *context);
/**
 * Provide the bootstrap resolution context contains operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_resolution_context_contains(
    const UmiBootstrapResolutionContext *context,
    const UmiBootstrapServiceKey *key);
/**
 * Provide the bootstrap resolution context push operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_resolution_context_push(
    UmiBootstrapResolutionContext *context,
    const UmiBootstrapServiceKey *key);
/**
 * Provide the bootstrap resolution context pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_bootstrap_resolution_context_pop(UmiBootstrapResolutionContext *context);

#ifdef __cplusplus
}
#endif

#endif
