/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_metrics.c
 *
 * PURPOSE:
 *   Verify metric initialisation has a stable revision.
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

    UmiWorkbenchContextHostMetrics metrics;
    umi_workbench_context_host_metrics_init(&metrics);
    assert(metrics.revision == 1U);
    assert(metrics.publication_count == 0U);
    assert(metrics.delivery_queued_count == 0U);
    return 0;
}
