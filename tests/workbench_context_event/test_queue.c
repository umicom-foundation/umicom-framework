/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_queue.c
 *
 * PURPOSE:
 *   Verify FIFO event queue behavior.
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

#include "umicom/workbench_context_event/queue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextEventQueue queue;
    UmiWorkbenchContextEvent event;
    UmiWorkbenchContextEvent popped;
    umi_workbench_context_event_queue_init(&queue);
    umi_workbench_context_event_init(
        &event,
        UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION,
        "event");
    assert(umi_workbench_context_event_copy_text(
        event.application_id, sizeof(event.application_id),
        "org.umicom.test") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        event.panel_id, sizeof(event.panel_id),
        "panel") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_queue_push(
        &queue, &event) == UMI_STATUS_OK);
    assert(umi_workbench_context_event_queue_peek(&queue) != NULL);
    assert(umi_workbench_context_event_queue_pop(
        &queue, &popped) == UMI_STATUS_OK);
    assert(strcmp(popped.event_id, "event") == 0);
    umi_workbench_context_event_queue_destroy(&queue);

    return 0;
}
