/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_health_after_pressure.c
 *
 * PURPOSE:
 *   Verify inbox pressure is reported as degraded health after delivery eviction.
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
#include "umicom/workbench_context_host/health.h"
#include <stdio.h>

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostHealth health;
    UmiContextPayload payload;
    size_t index;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS + 1U;
         ++index) {
        char context_id[64U];
        int written = snprintf(context_id, sizeof(context_id), "ctx-%zu", index);
        assert(written > 0 && (size_t)written < sizeof(context_id));
        assert(umi_wch_test_selection_payload(
            &payload, context_id, "test.source") == UMI_STATUS_OK);
        assert(umi_workbench_context_host_publish(
            fixture.host, "blue", "test.source", &payload,
            (uint64_t)(100U + index)) == UMI_STATUS_OK);
    }
    assert(umi_workbench_context_host_health_evaluate(
        fixture.host, &health) == UMI_STATUS_OK);
    assert(health.state == UMI_WORKBENCH_CONTEXT_HOST_HEALTH_DEGRADED);
    assert(health.dropped_delivery_count == 1U);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
