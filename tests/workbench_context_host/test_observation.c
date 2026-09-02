/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_observation.c
 *
 * PURPOSE:
 *   Verify workbench observation change flags.
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

    UmiWorkbenchContextHostObservation a;
    UmiWorkbenchContextHostObservation b;
    uint32_t diff;
    umi_workbench_context_host_observation_init(&a);
    umi_workbench_context_host_observation_init(&b);
    assert(umi_workbench_context_host_copy_text(
        a.document_id, sizeof(a.document_id), "doc-a") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_copy_text(
        b.document_id, sizeof(b.document_id), "doc-b") == UMI_STATUS_OK);
    diff = umi_workbench_context_host_observation_diff(&a, &b);
    assert((diff & UMI_WORKBENCH_CONTEXT_HOST_OBSERVATION_DOCUMENT) != 0U);
    assert(!umi_workbench_context_host_observation_equal(&a, &b));
    assert(umi_workbench_context_host_copy_text(
        b.document_id, sizeof(b.document_id), "doc-a") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_observation_equal(&a, &b));
    return 0;
}
