/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolution_context.h
 *
 * PURPOSE:
 *   Detect recursive resolution cycles using an explicit dependency stack.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_CONTEXT_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_CONTEXT_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_resolution_context_init(UmiBootstrapResolutionContext *context);
bool umi_bootstrap_resolution_context_contains(
    const UmiBootstrapResolutionContext *context,
    const UmiBootstrapServiceKey *key);
UmiStatus umi_bootstrap_resolution_context_push(
    UmiBootstrapResolutionContext *context,
    const UmiBootstrapServiceKey *key);
UmiStatus umi_bootstrap_resolution_context_pop(UmiBootstrapResolutionContext *context);

#ifdef __cplusplus
}
#endif

#endif
