/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_summary.h
 *
 * PURPOSE:
 *   Aggregate failure categories without losing individual test identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_SUMMARY
#define UMICOM_TEST_RUNTIME_FAILURE_SUMMARY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeFailureSummary {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t failure_count;
    uint64_t category_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeFailureSummary;
void umi_test_runtime_failure_summary_init(UmiTestRuntimeFailureSummary *value,const char *id);
UmiStatus umi_test_runtime_failure_summary_validate(const UmiTestRuntimeFailureSummary *value);
UmiStatus umi_test_runtime_failure_summary_set_category(UmiTestRuntimeFailureSummary *value,const char *category);
UmiStatus umi_test_runtime_failure_summary_set_detail(UmiTestRuntimeFailureSummary *value,const char *detail);
UmiStatus umi_test_runtime_failure_summary_set_failure_count(UmiTestRuntimeFailureSummary *value,uint64_t number);
UmiStatus umi_test_runtime_failure_summary_set_category_count(UmiTestRuntimeFailureSummary *value,uint64_t number);
UmiStatus umi_test_runtime_failure_summary_set_active(UmiTestRuntimeFailureSummary *value,bool active);
bool umi_test_runtime_failure_summary_same_identity(const UmiTestRuntimeFailureSummary *left,const UmiTestRuntimeFailureSummary *right);
#ifdef __cplusplus
}
#endif
#endif
