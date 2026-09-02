/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/health.h
 *
 * PURPOSE:
 *   Summarise whether the test execution environment is ready or degraded.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_HEALTH
#define UMICOM_TEST_RUNTIME_HEALTH

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime health data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeHealth {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t ready_tests;
    uint64_t blocked_tests;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeHealth;

/**
 * Initialise test runtime health from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_runtime_health_init(UmiTestRuntimeHealth *value, const char *id);
/**
 * Check that test runtime health satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_health_validate(const UmiTestRuntimeHealth *value);
/**
 * Provide the test runtime health set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_health_set_name(UmiTestRuntimeHealth *value, const char *name);
/**
 * Provide the test runtime health set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_health_set_detail(UmiTestRuntimeHealth *value, const char *detail);
/**
 * Provide the test runtime health set ready tests operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_health_set_ready_tests(UmiTestRuntimeHealth *value, uint64_t number);
/**
 * Provide the test runtime health set blocked tests operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_health_set_blocked_tests(UmiTestRuntimeHealth *value, uint64_t number);
/**
 * Provide the test runtime health touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_health_touch(UmiTestRuntimeHealth *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime health same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_health_same_identity(const UmiTestRuntimeHealth *left, const UmiTestRuntimeHealth *right);

#ifdef __cplusplus
}
#endif
#endif
