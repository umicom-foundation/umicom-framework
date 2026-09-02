/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/result.h
 *
 * PURPOSE:
 *   Represent one test result independently of frontend or CTest text format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESULT
#define UMICOM_TEST_RUNTIME_RESULT

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime result data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeResult {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t duration_ms;
    uint64_t exit_code;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeResult;

/**
 * Initialise test runtime result from caller-provided values so later operations receive a
 * known state.
 */
void umi_test_runtime_result_init(UmiTestRuntimeResult *value, const char *id);
/**
 * Check that test runtime result satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_result_validate(const UmiTestRuntimeResult *value);
/**
 * Provide the test runtime result set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_result_set_name(UmiTestRuntimeResult *value, const char *name);
/**
 * Provide the test runtime result set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_result_set_detail(UmiTestRuntimeResult *value, const char *detail);
/**
 * Provide the test runtime result set duration ms operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_result_set_duration_ms(UmiTestRuntimeResult *value, uint64_t number);
/**
 * Provide the test runtime result set exit code operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_result_set_exit_code(UmiTestRuntimeResult *value, uint64_t number);
/**
 * Provide the test runtime result touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_result_touch(UmiTestRuntimeResult *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime result same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_result_same_identity(const UmiTestRuntimeResult *left, const UmiTestRuntimeResult *right);

#ifdef __cplusplus
}
#endif
#endif
