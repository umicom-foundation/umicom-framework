/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_event_queue.c
 *
 * PURPOSE:
 *   Verify asynchronous event FIFO ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/event_queue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeEventQueue *queue = NULL;
    UmiDebugRuntimeEnvelope input = {0};
    UmiDebugRuntimeEnvelope output;

    assert(umi_debug_runtime_event_queue_create(&queue) == UMI_STATUS_OK);
    input.kind = UMI_DEBUG_RUNTIME_MESSAGE_EVENT;
    (void)strcpy(input.event, "stopped");

    assert(umi_debug_runtime_event_queue_push(
        queue, &input) == UMI_STATUS_OK);
    assert(umi_debug_runtime_event_queue_count(queue) == 1U);
    assert(umi_debug_runtime_event_queue_pop(
        queue, &output) == UMI_STATUS_OK);
    assert(strcmp(output.event, "stopped") == 0);
    umi_debug_runtime_event_queue_destroy(queue);
    return 0;
}
