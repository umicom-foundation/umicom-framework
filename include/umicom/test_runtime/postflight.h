/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/postflight.h
 *
 * PURPOSE:
 *   Verify cleanup, evidence capture and resource release after tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_POSTFLIGHT
#define UMICOM_TEST_RUNTIME_POSTFLIGHT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime postflight data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimePostflight
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t check_count;
    uint64_t failure_count;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimePostflight;
/**
 * Initialise test runtime postflight from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_postflight_init(UmiTestRuntimePostflight *value,const char *id);
/**
 * Check that test runtime postflight satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_postflight_validate(const UmiTestRuntimePostflight *value);
/**
 * Provide the test runtime postflight set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_postflight_set_detail(UmiTestRuntimePostflight *value,const char *detail);
/**
 * Return the number of records represented by test runtime postflight set check without
 * changing their state.
 */
UmiStatus umi_test_runtime_postflight_set_check_count(UmiTestRuntimePostflight *value,uint64_t number);
/**
 * Return the number of records represented by test runtime postflight set failure without
 * changing their state.
 */
UmiStatus umi_test_runtime_postflight_set_failure_count(UmiTestRuntimePostflight *value,uint64_t number);
/**
 * Provide the test runtime postflight same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_postflight_same_identity(const UmiTestRuntimePostflight *left,const UmiTestRuntimePostflight *right);
#ifdef __cplusplus
}
#endif
#endif
