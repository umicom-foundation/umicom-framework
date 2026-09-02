/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/report.h
 *
 * PURPOSE:
 *   Format deterministic human-readable test summaries for consoles, panes and CI logs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_REPORT_H
#define UMICOM_TESTING_REPORT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/testing/runner.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the test report summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_report_summary(const UmiTestRunSummary *summary,
                                  char *out_text,
                                  size_t capacity);
/**
 * Provide the test report results operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_report_results(const UmiTestResult *results,
                                  size_t result_count,
                                  char *out_text,
                                  size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
