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
void umi_test_runtime_baseline_comparison_init(UmiTestRuntimeBaselineComparison *value,const char *id);
UmiStatus umi_test_runtime_baseline_comparison_validate(const UmiTestRuntimeBaselineComparison *value);
UmiStatus umi_test_runtime_baseline_comparison_set_category(UmiTestRuntimeBaselineComparison *value,const char *category);
UmiStatus umi_test_runtime_baseline_comparison_set_detail(UmiTestRuntimeBaselineComparison *value,const char *detail);
UmiStatus umi_test_runtime_baseline_comparison_set_new_failures(UmiTestRuntimeBaselineComparison *value,uint64_t number);
UmiStatus umi_test_runtime_baseline_comparison_set_resolved_failures(UmiTestRuntimeBaselineComparison *value,uint64_t number);
UmiStatus umi_test_runtime_baseline_comparison_set_active(UmiTestRuntimeBaselineComparison *value,bool active);
bool umi_test_runtime_baseline_comparison_same_identity(const UmiTestRuntimeBaselineComparison *left,const UmiTestRuntimeBaselineComparison *right);
#ifdef __cplusplus
}
#endif
#endif
