/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/exception_info.c
 *
 * PURPOSE:
 *   Implement the DAP exceptionInfo request without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/exception_info.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_exception_info(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "exceptionInfo", thread_id, out_sequence);
}
