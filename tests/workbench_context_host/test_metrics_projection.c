/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_metrics_projection.c
 *
 * PURPOSE:
 *   Verify delivery and duplicate ratios are derived from host metrics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/metrics_projection.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostMetricsProjection projection;
    UmiContextPayload payload;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(&payload, "selection", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_metrics_projection_build(
        fixture.host, &projection) == UMI_STATUS_OK);
    assert(projection.host.publication_count == 1U);
    assert(projection.deliveries_per_publication == 1.0);
    assert(projection.drop_ratio == 0.0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
