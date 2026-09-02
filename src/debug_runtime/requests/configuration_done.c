/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/configuration_done.c
 *
 * PURPOSE:
 *   Implement the DAP configurationDone request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/configuration_done.h"

/*
 * Provide the debug runtime request configuration done operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_request_configuration_done(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_no_arguments(
        adapter, "configurationDone", out_sequence);
}
