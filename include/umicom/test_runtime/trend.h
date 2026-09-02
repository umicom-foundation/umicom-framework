/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/trend.h
 *
 * PURPOSE:
 *   Retain pass rate, duration and failure-category trends across runs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TREND
#define UMICOM_TEST_RUNTIME_TREND
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime trend data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeTrend {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t sample_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeTrend;
/**
 * Initialise test runtime trend from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_runtime_trend_init(UmiTestRuntimeTrend *value,const char *id);
/**
 * Check that test runtime trend satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_trend_validate(const UmiTestRuntimeTrend *value);
/**
 * Provide the test runtime trend set category operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_trend_set_category(UmiTestRuntimeTrend *value,const char *category);
/**
 * Provide the test runtime trend set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_trend_set_detail(UmiTestRuntimeTrend *value,const char *detail);
/**
 * Return the number of records represented by test runtime trend set sample without
 * changing their state.
 */
UmiStatus umi_test_runtime_trend_set_sample_count(UmiTestRuntimeTrend *value,uint64_t number);
/**
 * Provide the test runtime trend set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_trend_set_generation(UmiTestRuntimeTrend *value,uint64_t number);
/**
 * Provide the test runtime trend set active operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_trend_set_active(UmiTestRuntimeTrend *value,bool active);
/**
 * Provide the test runtime trend same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_trend_same_identity(const UmiTestRuntimeTrend *left,const UmiTestRuntimeTrend *right);
#ifdef __cplusplus
}
#endif
#endif
