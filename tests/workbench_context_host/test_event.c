/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_event.c
 *
 * PURPOSE:
 *   Verify event record initialization.
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
#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    UmiWorkbenchContextHostEvent event;
    umi_workbench_context_host_event_init(
        &event, UMI_WORKBENCH_CONTEXT_HOST_EVENT_CONTEXT_PUBLISHED);
    assert(event.structure_size == sizeof(event));
    assert(event.kind ==
        UMI_WORKBENCH_CONTEXT_HOST_EVENT_CONTEXT_PUBLISHED);
    return 0;
}
