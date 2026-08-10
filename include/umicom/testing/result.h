/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/result.h
 *
 * PURPOSE:
 *   Represent one test result including status, duration, exit code and captured output.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiTestResult {
    char test_id[UMI_TEST_ID_CAPACITY];
    char name[UMI_TEST_NAME_CAPACITY];
    UmiTestState state;
    UmiStatus status;
    int exit_code;
    uint64_t duration_ms;
    char output[UMI_TEST_OUTPUT_CAPACITY];
} UmiTestResult;

void umi_test_result_init(UmiTestResult *result,
                          const char *test_id,
                          const char *name);
void umi_test_result_finish(UmiTestResult *result,
                            UmiStatus status,
                            int exit_code,
                            uint64_t duration_ms);
UmiStatus umi_test_result_summary(const UmiTestResult *result,
                                  char *out_text,
                                  size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
