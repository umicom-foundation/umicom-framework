/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/baseline_comparison.h
 *
 * PURPOSE:
 *   Compare a new regression run with an accepted baseline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_BASELINE_COMPARISON
#define UMICOM_TEST_RUNTIME_BASELINE_COMPARISON
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime baseline comparison data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeBaselineComparison {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t new_failures;
    uint64_t resolved_failures;
    uint64_t revision;
    bool active;
} UmiTestRuntimeBaselineComparison;
/**
 * Initialise test runtime baseline comparison from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_baseline_comparison_init(UmiTestRuntimeBaselineComparison *value,const char *id);
/**
 * Check that test runtime baseline comparison satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_baseline_comparison_validate(const UmiTestRuntimeBaselineComparison *value);
/**
 * Provide the test runtime baseline comparison set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_baseline_comparison_set_category(UmiTestRuntimeBaselineComparison *value,const char *category);
/**
 * Provide the test runtime baseline comparison set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_baseline_comparison_set_detail(UmiTestRuntimeBaselineComparison *value,const char *detail);
/**
 * Provide the test runtime baseline comparison set new failures operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_baseline_comparison_set_new_failures(UmiTestRuntimeBaselineComparison *value,uint64_t number);
/**
 * Provide the test runtime baseline comparison set resolved failures operation used by
 * this module and its client applications.
 */
UmiStatus umi_test_runtime_baseline_comparison_set_resolved_failures(UmiTestRuntimeBaselineComparison *value,uint64_t number);
/**
 * Provide the test runtime baseline comparison set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_baseline_comparison_set_active(UmiTestRuntimeBaselineComparison *value,bool active);
/**
 * Provide the test runtime baseline comparison same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_baseline_comparison_same_identity(const UmiTestRuntimeBaselineComparison *left,const UmiTestRuntimeBaselineComparison *right);
#ifdef __cplusplus
}
#endif
#endif
