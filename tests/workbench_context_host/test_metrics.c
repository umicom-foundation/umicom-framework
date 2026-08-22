/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_metrics.c
 *
 * PURPOSE:
 *   Verify metric initialisation has a stable revision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostMetrics metrics;
    umi_workbench_context_host_metrics_init(&metrics);
    assert(metrics.revision == 1U);
    assert(metrics.publication_count == 0U);
    assert(metrics.delivery_queued_count == 0U);
    return 0;
}
