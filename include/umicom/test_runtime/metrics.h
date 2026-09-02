/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/metrics.h
 *
 * PURPOSE:
 *   Aggregate launch, pass, failure, timeout and crash counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_METRICS
#define UMICOM_TEST_RUNTIME_METRICS

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime metrics data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeMetrics {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t launched;
    uint64_t passed;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeMetrics;

/**
 * Initialise test runtime metrics from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_runtime_metrics_init(UmiTestRuntimeMetrics *value, const char *id);
/**
 * Check that test runtime metrics satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_metrics_validate(const UmiTestRuntimeMetrics *value);
/**
 * Provide the test runtime metrics set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_metrics_set_name(UmiTestRuntimeMetrics *value, const char *name);
/**
 * Provide the test runtime metrics set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_metrics_set_detail(UmiTestRuntimeMetrics *value, const char *detail);
/**
 * Provide the test runtime metrics set launched operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_metrics_set_launched(UmiTestRuntimeMetrics *value, uint64_t number);
/**
 * Provide the test runtime metrics set passed operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_metrics_set_passed(UmiTestRuntimeMetrics *value, uint64_t number);
/**
 * Provide the test runtime metrics touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_metrics_touch(UmiTestRuntimeMetrics *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime metrics same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_metrics_same_identity(const UmiTestRuntimeMetrics *left, const UmiTestRuntimeMetrics *right);

#ifdef __cplusplus
}
#endif
#endif
