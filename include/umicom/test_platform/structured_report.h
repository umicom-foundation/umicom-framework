/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/structured_report.h
 *
 * PURPOSE:
 *   Format stable JSON records for test results and run-session summaries so
 *   CI, Studio panes and automation can consume the same structured output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_STRUCTURED_REPORT_H
#define UMICOM_TEST_PLATFORM_STRUCTURED_REPORT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/result.h"
#include "umicom/test_platform/run_session.h"

#define UMI_TEST_PLATFORM_RESULT_JSON_CAPACITY 24576U
#define UMI_TEST_PLATFORM_SESSION_JSON_CAPACITY 4096U

/* These capacities include NUL and cover maximally escaped snapshot fields.
 * Both exporters preserve output on every refusal, including short buffers.
 * Text fields must be terminated inside their arrays; exported fields must be valid UTF-8.
 * All JSON control bytes are escaped; result durations must be finite and
 * nonnegative and retain three decimal places with a dot decimal separator.
 * Serialize calls against locale changes and keep inputs separate from output.
 * Export does not authenticate outcomes or turn skipped coverage into passes. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the test platform report result json operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_report_result_json(
    const UmiTestPlatformResultSnapshot *result,
    char *out_json,
    size_t capacity
);
/**
 * Provide the test platform report session json operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_report_session_json(
    const UmiTestPlatformRunSessionSnapshot *session,
    char *out_json,
    size_t capacity
);

#ifdef __cplusplus
}
#endif
#endif
