/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/structured_report.h
 *
 * PURPOSE:
 *   Format stable JSON records for test results and run-session summaries so
 *   CI, Studio panes and automation can consume the same structured output.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_STRUCTURED_REPORT_H
#define UMICOM_TEST_PLATFORM_STRUCTURED_REPORT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/result.h"
#include "umicom/test_platform/run_session.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_test_platform_report_result_json(
    const UmiTestPlatformResultSnapshot *result,
    char *out_json,
    size_t capacity
);
UmiStatus umi_test_platform_report_session_json(
    const UmiTestPlatformRunSessionSnapshot *session,
    char *out_json,
    size_t capacity
);

#ifdef __cplusplus
}
#endif
#endif
