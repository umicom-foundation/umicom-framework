/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/runner.h
 *
 * PURPOSE:
 *   Execute individual test cases or complete suites using the portable process service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_RUNNER_H
#define UMICOM_TESTING_RUNNER_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#include "umicom/testing/result.h"
#include "umicom/testing/suite.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRunSummary {
    size_t total;
    size_t passed;
    size_t failed;
    size_t skipped;
    size_t cancelled;
    size_t timed_out;
    uint64_t duration_ms;
} UmiTestRunSummary;

UmiStatus umi_test_runner_run_case(const UmiTestCase *test_case,
                                   UmiCancellationToken *cancellation,
                                   UmiTestResult *out_result);
UmiStatus umi_test_runner_run_suite(const UmiTestSuite *suite,
                                    UmiCancellationToken *cancellation,
                                    UmiTestResult *results,
                                    size_t result_capacity,
                                    UmiTestRunSummary *out_summary);

#ifdef __cplusplus
}
#endif

#endif
