/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/configuration_done.c
 *
 * PURPOSE:
 *   Implement the DAP configurationDone request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/configuration_done.h"

UmiStatus umi_debug_runtime_request_configuration_done(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_no_arguments(
        adapter, "configurationDone", out_sequence);
}
