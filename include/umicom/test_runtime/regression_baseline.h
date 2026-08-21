/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/regression_baseline.h
 *
 * PURPOSE:
 *   Describe the expected pass/fail/timeout/crash baseline for comparison.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_REGRESSION_BASELINE
#define UMICOM_TEST_RUNTIME_REGRESSION_BASELINE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeRegressionBaseline {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t expected_passed;
    uint64_t expected_failed;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeRegressionBaseline;

void umi_test_runtime_regression_baseline_init(UmiTestRuntimeRegressionBaseline *value, const char *id);
UmiStatus umi_test_runtime_regression_baseline_validate(const UmiTestRuntimeRegressionBaseline *value);
UmiStatus umi_test_runtime_regression_baseline_set_name(UmiTestRuntimeRegressionBaseline *value, const char *name);
UmiStatus umi_test_runtime_regression_baseline_set_detail(UmiTestRuntimeRegressionBaseline *value, const char *detail);
UmiStatus umi_test_runtime_regression_baseline_set_expected_passed(UmiTestRuntimeRegressionBaseline *value, uint64_t number);
UmiStatus umi_test_runtime_regression_baseline_set_expected_failed(UmiTestRuntimeRegressionBaseline *value, uint64_t number);
UmiStatus umi_test_runtime_regression_baseline_touch(UmiTestRuntimeRegressionBaseline *value, uint64_t updated_at_ms);
bool umi_test_runtime_regression_baseline_same_identity(const UmiTestRuntimeRegressionBaseline *left, const UmiTestRuntimeRegressionBaseline *right);

#ifdef __cplusplus
}
#endif
#endif
