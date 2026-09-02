/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/loaded_sources.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol loadedSources request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_LOADED_SOURCES_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_LOADED_SOURCES_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request loaded sources operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_loaded_sources(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
