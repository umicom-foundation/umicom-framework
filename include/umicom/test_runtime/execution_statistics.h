/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_statistics.h
 *
 * PURPOSE:
 *   Aggregate deterministic counts and duration percentiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_STATISTICS
#define UMICOM_TEST_RUNTIME_EXECUTION_STATISTICS
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime execution statistics data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeExecutionStatistics {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t sample_count;
    uint64_t total_duration_ms;
    uint64_t revision;
    bool active;
} UmiTestRuntimeExecutionStatistics;
/**
 * Initialise test runtime execution statistics from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_execution_statistics_init(UmiTestRuntimeExecutionStatistics *value,const char *id);
/**
 * Check that test runtime execution statistics satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_execution_statistics_validate(const UmiTestRuntimeExecutionStatistics *value);
/**
 * Provide the test runtime execution statistics set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_execution_statistics_set_category(UmiTestRuntimeExecutionStatistics *value,const char *category);
/**
 * Provide the test runtime execution statistics set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_execution_statistics_set_detail(UmiTestRuntimeExecutionStatistics *value,const char *detail);
/**
 * Return the number of records represented by test runtime execution statistics set sample
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_statistics_set_sample_count(UmiTestRuntimeExecutionStatistics *value,uint64_t number);
/**
 * Provide the test runtime execution statistics set total duration ms operation used by
 * this module and its client applications.
 */
UmiStatus umi_test_runtime_execution_statistics_set_total_duration_ms(UmiTestRuntimeExecutionStatistics *value,uint64_t number);
/**
 * Provide the test runtime execution statistics set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_execution_statistics_set_active(UmiTestRuntimeExecutionStatistics *value,bool active);
/**
 * Provide the test runtime execution statistics same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_execution_statistics_same_identity(const UmiTestRuntimeExecutionStatistics *left,const UmiTestRuntimeExecutionStatistics *right);
#ifdef __cplusplus
}
#endif
#endif
