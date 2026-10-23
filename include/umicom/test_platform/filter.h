/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/filter.h
 *
 * PURPOSE:
 *   Define deterministic Test Explorer filters and bounded selections across
 *   framework-neutral discovered test records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the test platform filter data shared with callers of this public contract.
 */
typedef struct UmiTestPlatformFilter {
    /* Free text is matched case-insensitively against id, name and labels. */
    char text[256];
    char suite_id[128];
    char label[128];
    int outcome;
    int include_disabled;
    int failed_only;
} UmiTestPlatformFilter;

/**
 * Represent the test platform selection data shared with callers of this public contract.
 */
typedef struct UmiTestPlatformSelection {
    /* Selections carry stable ids rather than borrowed registry pointers. */
    char item_ids[UMI_TEST_PLATFORM_SELECTION_CAPACITY][128];
    size_t count;
    uint64_t source_revision;
} UmiTestPlatformSelection;

/**
 * Initialise test platform filter from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_platform_filter_init(UmiTestPlatformFilter *filter);
/* Validate the fixed-size query fields without reading beyond them. */
UmiStatus UmiTestPlatformFilterValidate(const UmiTestPlatformFilter *filter);
/* latest_result may be NULL when the test has never run. Invalid filter or
 * queried item text does not match. failed_only includes failed and timed-out
 * results; skipped, cancelled and unrecorded outcomes are not failures. */
int umi_test_platform_filter_matches(
    const UmiTestPlatformItemSnapshot *item,
    const UmiTestPlatformResultSnapshot *latest_result,
    const UmiTestPlatformFilter *filter
);
/**
 * Provide the test platform filter select operation used by this module and its client
 * applications.
 */
/* Results may be NULL for a catalogue without history. Selection preserves
 * registry order. On any failure the output remains unchanged. Access to both
 * registries must be serialized by their owner for this capture. */
UmiStatus umi_test_platform_filter_select(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter,
    UmiTestPlatformSelection *out_selection
);

/* Catalogue inspection is independent of the bounded execution selection.
 * Counts describe direct filter matches, not hierarchy-only presentation rows.
 * Selected identity is optional. Empty or unmatched identity sets selected_matches
 * to zero; first_item_id retains the first matching registry record. */
typedef struct UmiTestPlatformFilterSummary {
    size_t item_count, matched_count;
    size_t passed_count, failed_count, skipped_count;
    size_t cancelled_count, timed_out_count, not_run_count;
    char first_item_id[128];
    int selected_matches;
    uint64_t item_revision, result_revision;
} UmiTestPlatformFilterSummary;
/* Capture counts and selected membership for the complete catalogue. Results
 * may be NULL. The caller serializes registry access; no callbacks or execution
 * occur. Allocation or validation failure leaves out_summary unchanged. */
UmiStatus UmiTestPlatformFilterInspect(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter,
    const char *selected_id,
    UmiTestPlatformFilterSummary *out_summary);

/* Read one direct match in registry order, without the execution selection cap.
 * An absent latest result is returned as a zero snapshot. Invalid indices and
 * other failures leave both outputs unchanged. out_latest may be NULL. */
UmiStatus UmiTestPlatformFilterItemAt(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter, size_t visible_index,
    UmiTestPlatformItemSnapshot *out_item,
    UmiTestPlatformResultSnapshot *out_latest);

#ifdef __cplusplus
}
#endif
#endif
