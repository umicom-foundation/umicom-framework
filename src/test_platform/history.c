/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/history.c
 *
 * PURPOSE:
 *   Provide latest-result, rerun-failed and duration-history queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/history.h"

#include <float.h>
#include <string.h>

UmiStatus umi_test_platform_history_latest(
    const UmiTestPlatformResultRegistry *results,
    const char *item_id,
    UmiTestPlatformResultSnapshot *out_result)
{
    UmiTestPlatformResultSnapshot candidate;
    uint64_t best_sequence = 0U;
    uint64_t best_revision = 0U;
    size_t index;
    int found = 0;
    if (results == NULL || item_id == NULL || item_id[0] == '\0' ||
        out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U;
         index < umi_test_platform_result_registry_count(results);
         ++index) {
        if (umi_test_platform_result_registry_at(results, index, &candidate) !=
            UMI_STATUS_OK || strcmp(candidate.item_id, item_id) != 0) {
            continue;
        }
        if (!found || candidate.sequence > best_sequence ||
            (candidate.sequence == best_sequence &&
             candidate.revision > best_revision)) {
            *out_result = candidate;
            best_sequence = candidate.sequence;
            best_revision = candidate.revision;
            found = 1;
        }
    }
    return found ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_test_platform_history_duration(
    const UmiTestPlatformResultRegistry *results,
    const char *item_id,
    UmiTestPlatformDurationHistory *out_history)
{
    UmiTestPlatformResultSnapshot candidate;
    uint64_t latest_sequence = 0U;
    uint64_t latest_revision = 0U;
    double total = 0.0;
    size_t index;
    if (results == NULL || item_id == NULL || item_id[0] == '\0' ||
        out_history == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_history, 0, sizeof(*out_history));
    out_history->minimum_ms = DBL_MAX;
    for (index = 0U;
         index < umi_test_platform_result_registry_count(results);
         ++index) {
        if (umi_test_platform_result_registry_at(results, index, &candidate) !=
            UMI_STATUS_OK || strcmp(candidate.item_id, item_id) != 0) {
            continue;
        }
        if (candidate.duration_ms < out_history->minimum_ms) {
            out_history->minimum_ms = candidate.duration_ms;
        }
        if (candidate.duration_ms > out_history->maximum_ms) {
            out_history->maximum_ms = candidate.duration_ms;
        }
        if (out_history->sample_count == 0U ||
            candidate.sequence > latest_sequence ||
            (candidate.sequence == latest_sequence &&
             candidate.revision > latest_revision)) {
            out_history->latest_ms = candidate.duration_ms;
            latest_sequence = candidate.sequence;
            latest_revision = candidate.revision;
        }
        total += candidate.duration_ms;
        out_history->sample_count += 1U;
    }
    if (out_history->sample_count == 0U) {
        out_history->minimum_ms = 0.0;
        return UMI_STATUS_NOT_FOUND;
    }
    out_history->average_ms = total / (double)out_history->sample_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_history_failed_selection(
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    UmiTestPlatformSelection *out_selection)
{
    UmiTestPlatformFilter filter;
    umi_test_platform_filter_init(&filter);
    filter.failed_only = 1;
    return umi_test_platform_filter_select(items, results, &filter,
                                           out_selection);
}
