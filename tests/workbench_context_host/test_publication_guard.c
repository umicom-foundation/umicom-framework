/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_publication_guard.c
 *
 * PURPOSE:
 *   Verify immediate duplicate contexts from the same source are suppressed.
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
#include "umicom/workbench_context_host/publication_guard.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostPublicationGuard guard;
    UmiContextPayload payload;
    umi_workbench_context_host_publication_guard_init(&guard);
    assert(umi_wch_test_selection_payload(
        &payload, "selection", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publication_guard_accept(
        &guard, "test.source", "blue", &payload, 100U));
    assert(!umi_workbench_context_host_publication_guard_accept(
        &guard, "test.source", "blue", &payload, 120U));
    assert(guard.duplicate_count == 1U);
    assert(umi_workbench_context_host_publication_guard_accept(
        &guard, "test.source", "blue", &payload, 200U));

    return 0;
}
