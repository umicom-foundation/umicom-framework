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

UmiStatus umi_test_platform_build_readiness_report_create(
    const UmiTestPlatformBuildInventory *inventory,
    UmiTestPlatformBuildReadinessReport *report)
{
    size_t index;
    if (inventory == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(report, 0, sizeof(*report));
    report->total_count = inventory->count;
    for (index = 0U; index < inventory->count; ++index) {
        const UmiTestPlatformBuildObservation *observation =
            &inventory->items[index];
        if (observation->state == UMI_TEST_PLATFORM_BUILD_STATE_READY) {
            report->ready_count++;
        } else {
            report->blocked_count++;
            if (observation->state ==
                UMI_TEST_PLATFORM_BUILD_STATE_ARTIFACT_MISSING)
                report->missing_count++;
            if (observation->artifact->required)
                report->required_failure_count++;
        }
    }
    report->ready_to_run = report->total_count > 0U &&
        report->required_failure_count == 0U;
    return UMI_STATUS_OK;
}
