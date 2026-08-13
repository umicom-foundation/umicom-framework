/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/filter.h
 *
 * PURPOSE:
 *   Define deterministic Test Explorer filters and bounded selections across
 *   framework-neutral discovered test records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_FILTER_H
#define UMICOM_TEST_PLATFORM_FILTER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/item.h"
#include "umicom/test_platform/result.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_SELECTION_CAPACITY 1024U

typedef struct UmiTestPlatformFilter {
    /* Free text is matched case-insensitively against id, name and labels. */
    char text[256];
    char suite_id[128];
    char label[128];
    int outcome;
    int include_disabled;
    int failed_only;
} UmiTestPlatformFilter;

typedef struct UmiTestPlatformSelection {
    /* Selections carry stable ids rather than borrowed registry pointers. */
    char item_ids[UMI_TEST_PLATFORM_SELECTION_CAPACITY][128];
    size_t count;
    uint64_t source_revision;
} UmiTestPlatformSelection;

void umi_test_platform_filter_init(UmiTestPlatformFilter *filter);
/* latest_result may be NULL when the test has never run. */
int umi_test_platform_filter_matches(
    const UmiTestPlatformItemSnapshot *item,
    const UmiTestPlatformResultSnapshot *latest_result,
    const UmiTestPlatformFilter *filter
);
UmiStatus umi_test_platform_filter_select(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection
);

#ifdef __cplusplus
}
#endif
#endif
