/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/ctest_result.h
 *
 * PURPOSE:
 *   Represent one CTest outcome with native process evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CTEST_RESULT
#define UMICOM_TEST_RUNTIME_CTEST_RESULT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime ctest result data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeCtestResult {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t duration_ms;
    uint64_t native_status;
    uint64_t revision;
    bool active;
} UmiTestRuntimeCtestResult;
/**
 * Initialise test runtime ctest result from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_ctest_result_init(UmiTestRuntimeCtestResult *value,const char *id);
/**
 * Check that test runtime ctest result satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_ctest_result_validate(const UmiTestRuntimeCtestResult *value);
/**
 * Provide the test runtime ctest result set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_category(UmiTestRuntimeCtestResult *value,const char *category);
/**
 * Provide the test runtime ctest result set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_detail(UmiTestRuntimeCtestResult *value,const char *detail);
/**
 * Provide the test runtime ctest result set duration ms operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_duration_ms(UmiTestRuntimeCtestResult *value,uint64_t number);
/**
 * Provide the test runtime ctest result set native status operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_native_status(UmiTestRuntimeCtestResult *value,uint64_t number);
/**
 * Provide the test runtime ctest result set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_ctest_result_set_active(UmiTestRuntimeCtestResult *value,bool active);
/**
 * Provide the test runtime ctest result same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_ctest_result_same_identity(const UmiTestRuntimeCtestResult *left,const UmiTestRuntimeCtestResult *right);
#ifdef __cplusplus
}
#endif
#endif
