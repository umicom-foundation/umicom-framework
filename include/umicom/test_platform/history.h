/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/history.h
 *
 * PURPOSE:
 *   Query retained result history, latest outcomes, failure sets and duration
 *   trends without coupling callers to registry storage details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_HISTORY_H
#define UMICOM_TEST_PLATFORM_HISTORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/filter.h"
#include "umicom/test_platform/result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestPlatformDurationHistory {
    size_t sample_count;
    double latest_ms;
    double minimum_ms;
    double maximum_ms;
    double average_ms;
} UmiTestPlatformDurationHistory;

/* Latest is defined by sequence, then registry revision as a stable tie-break. */
UmiStatus umi_test_platform_history_latest(
    const UmiTestPlatformResultRegistry *results,
    const char *item_id,
    UmiTestPlatformResultSnapshot *out_result
);
UmiStatus umi_test_platform_history_duration(
    const UmiTestPlatformResultRegistry *results,
    const char *item_id,
    UmiTestPlatformDurationHistory *out_history
);
UmiStatus umi_test_platform_history_failed_selection(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    UmiTestPlatformSelection *out_selection
);

#ifdef __cplusplus
}
#endif
#endif
