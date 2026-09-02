/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/variables.c
 *
 * PURPOSE:
 *   Implement the DAP variables request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/variables.h"

/*
 * Provide the debug runtime request variables operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_variables(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t reference,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_reference(
        adapter,
        "variables",
        "variablesReference",
        reference,
        out_sequence);
}
