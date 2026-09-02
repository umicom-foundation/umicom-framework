/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/threads.c
 *
 * PURPOSE:
 *   Implement the DAP threads request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/threads.h"

/*
 * Provide the debug runtime request threads operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_threads(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_no_arguments(
        adapter, "threads", out_sequence);
}
