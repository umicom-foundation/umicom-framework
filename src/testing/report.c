/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/report.c
 *
 * PURPOSE:
 *   Render test summary and per-case result text without depending on a frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/report.h"

#include <stdio.h>

/*
 * Provide the test report summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_report_summary(const UmiTestRunSummary *summary,
                                  char *out_text,
                                  size_t capacity)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(
        out_text,
        capacity,
        "Tests: total=%zu passed=%zu failed=%zu skipped=%zu "
        "cancelled=%zu timed-out=%zu duration=%llums",
        summary->total,
        summary->passed,
        summary->failed,
        summary->skipped,
        summary->cancelled,
        summary->timed_out,
        (unsigned long long)summary->duration_ms
    );
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/*
 * Provide the test report results operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_report_results(const UmiTestResult *results,
                                  size_t result_count,
                                  char *out_text,
                                  size_t capacity)
{
    size_t index;
    size_t used = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_text[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < result_count; ++index) {
        int written = snprintf(out_text + used,
                               capacity - used,
                               "%s%s: %s (%llums)",
                               index == 0U ? "" : "\n",
                               results[index].test_id,
                               umi_test_state_text(results[index].state),
                               (unsigned long long)results[index].duration_ms);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= capacity - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return UMI_STATUS_OK;
}
