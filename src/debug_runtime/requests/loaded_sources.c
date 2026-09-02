/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/loaded_sources.c
 *
 * PURPOSE:
 *   Implement the DAP loadedSources request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/loaded_sources.h"

/*
 * Provide the debug runtime request loaded sources operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_loaded_sources(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_no_arguments(
        adapter, "loadedSources", out_sequence);
}
