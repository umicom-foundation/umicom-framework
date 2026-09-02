/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/build_readiness/readiness_report.c
 * PURPOSE: Reduce build observations into one pre-CTest release gate.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/build_readiness/readiness_report.h"

#include <string.h>

/*
 * Initialise test platform build readiness report from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_build_readiness_report_create(
    const UmiTestPlatformBuildInventory *inventory,
    UmiTestPlatformBuildReadinessReport *report)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(report, 0, sizeof(*report));
    report->total_count = inventory->count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->count; ++index) {
        const UmiTestPlatformBuildObservation *observation =
            &inventory->items[index];
        /* Create this optional product surface only when its build option is enabled. */
        if (observation->state == UMI_TEST_PLATFORM_BUILD_STATE_READY) {
            report->ready_count++;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            report->blocked_count++;
            /* Apply this branch only when its contract condition is satisfied. */
            if (observation->state ==
                UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING)
                report->missing_count++;
            /* Apply this branch only when its contract condition is satisfied. */
            if (observation->artifact->required)
                report->required_failure_count++;
        }
    }
    report->ready_to_run = report->total_count > 0U &&
        report->required_failure_count == 0U;
    return UMI_STATUS_OK;
}
