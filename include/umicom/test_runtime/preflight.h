/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/preflight.h
 *
 * PURPOSE:
 *   Run environment, executable and dependency checks before CTest launch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PREFLIGHT
#define UMICOM_TEST_RUNTIME_PREFLIGHT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime preflight data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimePreflight
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t check_count;
    uint64_t failure_count;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimePreflight;
/**
 * Initialise test runtime preflight from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_preflight_init(UmiTestRuntimePreflight *value,const char *id);
/**
 * Check that test runtime preflight satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_preflight_validate(const UmiTestRuntimePreflight *value);
/**
 * Provide the test runtime preflight set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_preflight_set_detail(UmiTestRuntimePreflight *value,const char *detail);
/**
 * Return the number of records represented by test runtime preflight set check without
 * changing their state.
 */
UmiStatus umi_test_runtime_preflight_set_check_count(UmiTestRuntimePreflight *value,uint64_t number);
/**
 * Return the number of records represented by test runtime preflight set failure without
 * changing their state.
 */
UmiStatus umi_test_runtime_preflight_set_failure_count(UmiTestRuntimePreflight *value,uint64_t number);
/**
 * Provide the test runtime preflight same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_preflight_same_identity(const UmiTestRuntimePreflight *left,const UmiTestRuntimePreflight *right);
#ifdef __cplusplus
}
#endif
#endif
