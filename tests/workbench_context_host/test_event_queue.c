/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_event_queue.c
 *
 * PURPOSE:
 *   Verify bounded event queue FIFO behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/event_queue.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostEventQueue queue;
    UmiWorkbenchContextHostEvent event;
    UmiWorkbenchContextHostEvent popped;
    umi_workbench_context_host_event_queue_init(&queue);
    umi_workbench_context_host_event_init(
        &event, UMI_WORKBENCH_CONTEXT_HOST_EVENT_ACTIVE_GROUP_CHANGED);
    assert(umi_workbench_context_host_copy_text(
        event.event_id, sizeof(event.event_id), "event") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_event_queue_push(
        &queue, &event) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_event_queue_pop(
        &queue, &popped) == UMI_STATUS_OK);
    assert(strcmp(popped.event_id, "event") == 0);
    umi_workbench_context_host_event_queue_destroy(&queue);

    return 0;
}
