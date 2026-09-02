/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/terminate.c
 *
 * PURPOSE:
 *   Implement the DAP terminate request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/terminate.h"

/*
 * Provide the debug runtime request terminate operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_terminate(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_no_arguments(
        adapter, "terminate", out_sequence);
}
