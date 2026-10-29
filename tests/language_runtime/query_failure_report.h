/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/query_failure_report.h
 *
 * PURPOSE:
 *   Preserve native query stage evidence when a regression expectation fails.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_QUERY_FAILURE_REPORT_H
#define UMICOM_TEST_QUERY_FAILURE_REPORT_H
#include "umicom/base/status.h"
#include <stdio.h>

/* Only fixture scenario names and lifecycle statuses are reported. Do not print
 * source bytes, workspace paths, environment overrides or protocol payloads.
 * The caller still checks the original expectation without retries or skips. */
static void LanguageQueryFailureReport(const char *scenario, UmiStatus actual,
    UmiStatus expected, int started, int initialized, int opened,
    UmiStatus query, UmiStatus close_status, UmiStatus shutdown)
{
    if (actual == expected) return;
    fprintf(stderr, "Native query scenario: %s\n"
        "Result: %s (%d); expected: %s (%d)\n"
        "Started: %d; initialized: %d; document opened: %d\n"
        "Query: %s (%d); close: %s (%d); shutdown: %s (%d)\n",
        scenario, umi_status_text(actual), (int)actual, umi_status_text(expected), (int)expected,
        started, initialized, opened, umi_status_text(query), (int)query,
        umi_status_text(close_status), (int)close_status, umi_status_text(shutdown), (int)shutdown);
    fflush(stderr);
}
#endif
