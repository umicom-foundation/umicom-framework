/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/run_summary.h
 *
 * PURPOSE:
 *   Summarise one run without losing native failure categories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUN_SUMMARY
#define UMICOM_TEST_RUNTIME_RUN_SUMMARY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeRunSummary
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t passed_count;
    uint64_t failed_count;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeRunSummary;
void umi_test_runtime_run_summary_init(UmiTestRuntimeRunSummary *value,const char *id);
UmiStatus umi_test_runtime_run_summary_validate(const UmiTestRuntimeRunSummary *value);
UmiStatus umi_test_runtime_run_summary_set_detail(UmiTestRuntimeRunSummary *value,const char *detail);
UmiStatus umi_test_runtime_run_summary_set_passed_count(UmiTestRuntimeRunSummary *value,uint64_t number);
UmiStatus umi_test_runtime_run_summary_set_failed_count(UmiTestRuntimeRunSummary *value,uint64_t number);
bool umi_test_runtime_run_summary_same_identity(const UmiTestRuntimeRunSummary *left,const UmiTestRuntimeRunSummary *right);
#ifdef __cplusplus
}
#endif
#endif
