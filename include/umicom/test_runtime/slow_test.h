/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/slow_test.h
 *
 * PURPOSE:
 *   Identify tests whose duration exceeds their historical operating envelope.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SLOW_TEST
#define UMICOM_TEST_RUNTIME_SLOW_TEST
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime slow test data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeSlowTest {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t duration_ms;
    uint64_t threshold_ms;
    uint64_t revision;
    bool active;
} UmiTestRuntimeSlowTest;
/**
 * Initialise test runtime slow test from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_slow_test_init(UmiTestRuntimeSlowTest *value,const char *id);
/**
 * Check that test runtime slow test satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_slow_test_validate(const UmiTestRuntimeSlowTest *value);
/**
 * Provide the test runtime slow test set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_slow_test_set_category(UmiTestRuntimeSlowTest *value,const char *category);
/**
 * Provide the test runtime slow test set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_slow_test_set_detail(UmiTestRuntimeSlowTest *value,const char *detail);
/**
 * Provide the test runtime slow test set duration ms operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_slow_test_set_duration_ms(UmiTestRuntimeSlowTest *value,uint64_t number);
/**
 * Provide the test runtime slow test set threshold ms operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_slow_test_set_threshold_ms(UmiTestRuntimeSlowTest *value,uint64_t number);
/**
 * Provide the test runtime slow test set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_slow_test_set_active(UmiTestRuntimeSlowTest *value,bool active);
/**
 * Provide the test runtime slow test same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_slow_test_same_identity(const UmiTestRuntimeSlowTest *left,const UmiTestRuntimeSlowTest *right);
#ifdef __cplusplus
}
#endif
#endif
