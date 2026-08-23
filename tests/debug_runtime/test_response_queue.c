/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_response_queue.c
 *
 * PURPOSE:
 *   Verify out-of-order DAP responses are retained by request sequence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/debug_runtime/response_queue.h"

int main(void)
{
    UmiDebugRuntimeResponseQueue *queue = NULL;
    UmiDebugRuntimeEnvelope first = {0};
    UmiDebugRuntimeEnvelope second = {0};
    UmiDebugRuntimeEnvelope output;

    assert(umi_debug_runtime_response_queue_create(&queue) == UMI_STATUS_OK);
    first.kind = UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE;
    first.request_sequence = 1U;
    first.success = 1;
    second = first;
    second.request_sequence = 2U;

    assert(umi_debug_runtime_response_queue_push(
        queue, &first) == UMI_STATUS_OK);
    assert(umi_debug_runtime_response_queue_push(
        queue, &second) == UMI_STATUS_OK);
    assert(umi_debug_runtime_response_queue_take(
        queue, 2U, &output) == UMI_STATUS_OK);
    assert(output.request_sequence == 2U);
    assert(umi_debug_runtime_response_queue_count(queue) == 1U);
    umi_debug_runtime_response_queue_destroy(queue);
    return 0;
}
