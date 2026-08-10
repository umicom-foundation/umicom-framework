/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/result.c
 *
 * PURPOSE:
 *   Implement test-result state transitions and summaries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/result.h"

#include <stdio.h>
#include <string.h>

void umi_test_result_init(UmiTestResult *result,
                          const char *test_id,
                          const char *name)
{
    if (result == NULL) {
        return;
    }
    (void)memset(result, 0, sizeof(*result));
    result->state = UMI_TEST_STATE_NOT_RUN;
    result->status = UMI_STATUS_OK;
    if (test_id != NULL) {
        (void)snprintf(result->test_id,
                       sizeof(result->test_id),
                       "%s",
                       test_id);
    }
    if (name != NULL) {
        (void)snprintf(result->name,
                       sizeof(result->name),
                       "%s",
                       name);
    }
}

void umi_test_result_finish(UmiTestResult *result,
                            UmiStatus status,
                            int exit_code,
                            uint64_t duration_ms)
{
    if (result == NULL) {
        return;
    }
    result->status = status;
    result->exit_code = exit_code;
    result->duration_ms = duration_ms;
    if (status == UMI_STATUS_OK && exit_code == 0) {
        result->state = UMI_TEST_STATE_PASSED;
    } else if (status == UMI_STATUS_CANCELLED) {
        result->state = UMI_TEST_STATE_CANCELLED;
    } else if (status == UMI_STATUS_TIMEOUT) {
        result->state = UMI_TEST_STATE_TIMED_OUT;
    } else {
        result->state = UMI_TEST_STATE_FAILED;
    }
}

UmiStatus umi_test_result_summary(const UmiTestResult *result,
                                  char *out_text,
                                  size_t capacity)
{
    int written;
    if (result == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_text,
                       capacity,
                       "%s: %s, exit=%d, duration=%llums",
                       result->test_id,
                       umi_test_state_text(result->state),
                       result->exit_code,
                       (unsigned long long)result->duration_ms);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}
