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
/**
 * Represent the test runtime run summary data shared with callers of this public contract.
 */
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
/**
 * Initialise test runtime run summary from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_run_summary_init(UmiTestRuntimeRunSummary *value,const char *id);
/**
 * Check that test runtime run summary satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_run_summary_validate(const UmiTestRuntimeRunSummary *value);
/**
 * Provide the test runtime run summary set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_run_summary_set_detail(UmiTestRuntimeRunSummary *value,const char *detail);
/**
 * Return the number of records represented by test runtime run summary set passed without
 * changing their state.
 */
UmiStatus umi_test_runtime_run_summary_set_passed_count(UmiTestRuntimeRunSummary *value,uint64_t number);
/**
 * Return the number of records represented by test runtime run summary set failed without
 * changing their state.
 */
UmiStatus umi_test_runtime_run_summary_set_failed_count(UmiTestRuntimeRunSummary *value,uint64_t number);
/**
 * Provide the test runtime run summary same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_run_summary_same_identity(const UmiTestRuntimeRunSummary *left,const UmiTestRuntimeRunSummary *right);
#ifdef __cplusplus
}
#endif
#endif
