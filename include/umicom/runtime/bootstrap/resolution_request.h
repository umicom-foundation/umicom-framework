/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/resolution_request.h
 *
 * PURPOSE:
 *   Describe one dependency-resolution request including scope and optionality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_REQUEST_H
#define UMICOM_RUNTIME_BOOTSTRAP_RESOLUTION_REQUEST_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_resolution_request_init(
    UmiBootstrapResolutionRequest *request,
    const UmiBootstrapServiceKey *key,
    UmiBootstrapScopeKind scope,
    bool required);

#ifdef __cplusplus
}
#endif

#endif
