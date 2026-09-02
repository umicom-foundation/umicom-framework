/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/result.h
 *
 * PURPOSE:
 *   Represent one test result including status, duration, exit code and captured output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_RESULT_H
#define UMICOM_TESTING_RESULT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/testing/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test result data shared with callers of this public contract.
 */
typedef struct UmiTestResult {
    char test_id[UMI_TEST_ID_CAPACITY];
    char name[UMI_TEST_NAME_CAPACITY];
    UmiTestState state;
    UmiStatus status;
    int exit_code;
    uint64_t duration_ms;
    char output[UMI_TEST_OUTPUT_CAPACITY];
} UmiTestResult;

/**
 * Initialise test result from caller-provided values so later operations receive a known
 * state.
 */
void umi_test_result_init(UmiTestResult *result,
                          const char *test_id,
                          const char *name);
/**
 * Provide the test result finish operation used by this module and its client
 * applications.
 */
void umi_test_result_finish(UmiTestResult *result,
                            UmiStatus status,
                            int exit_code,
                            uint64_t duration_ms);
/**
 * Provide the test result summary operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_result_summary(const UmiTestResult *result,
                                  char *out_text,
                                  size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
