/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/start_failure.h
 *
 * PURPOSE:
 *   Describe process-creation failures before application code begins.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_START_FAILURE
#define UMICOM_TEST_RUNTIME_START_FAILURE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime start failure data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeStartFailure {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t native_status;
    uint64_t attempt_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeStartFailure;
/**
 * Initialise test runtime start failure from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_start_failure_init(UmiTestRuntimeStartFailure *value,const char *id);
/**
 * Check that test runtime start failure satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_start_failure_validate(const UmiTestRuntimeStartFailure *value);
/**
 * Provide the test runtime start failure set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_start_failure_set_category(UmiTestRuntimeStartFailure *value,const char *category);
/**
 * Provide the test runtime start failure set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_start_failure_set_detail(UmiTestRuntimeStartFailure *value,const char *detail);
/**
 * Provide the test runtime start failure set native status operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_start_failure_set_native_status(UmiTestRuntimeStartFailure *value,uint64_t number);
/**
 * Return the number of records represented by test runtime start failure set attempt
 * without changing their state.
 */
UmiStatus umi_test_runtime_start_failure_set_attempt_count(UmiTestRuntimeStartFailure *value,uint64_t number);
/**
 * Provide the test runtime start failure set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_start_failure_set_active(UmiTestRuntimeStartFailure *value,bool active);
/**
 * Provide the test runtime start failure same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_start_failure_same_identity(const UmiTestRuntimeStartFailure *left,const UmiTestRuntimeStartFailure *right);
#ifdef __cplusplus
}
#endif
#endif
