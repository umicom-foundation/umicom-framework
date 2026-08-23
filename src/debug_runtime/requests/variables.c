/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/variables.c
 *
 * PURPOSE:
 *   Implement the DAP variables request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/variables.h"

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
