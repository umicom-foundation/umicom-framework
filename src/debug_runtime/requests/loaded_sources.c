/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/loaded_sources.c
 *
 * PURPOSE:
 *   Implement the DAP loadedSources request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/loaded_sources.h"

UmiStatus umi_debug_runtime_request_loaded_sources(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_no_arguments(
        adapter, "loadedSources", out_sequence);
}
