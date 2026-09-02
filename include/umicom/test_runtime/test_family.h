/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/test_family.h
 *
 * PURPOSE:
 *   Group tests by durable subsystem identity for targeted execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TEST_FAMILY
#define UMICOM_TEST_RUNTIME_TEST_FAMILY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime test family data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeTestFamily
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t member_count;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeTestFamily;
/**
 * Initialise test runtime test family from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_test_family_init(UmiTestRuntimeTestFamily *value,const char *id);
/**
 * Check that test runtime test family satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_test_family_validate(const UmiTestRuntimeTestFamily *value);
/**
 * Provide the test runtime test family set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_family_set_detail(UmiTestRuntimeTestFamily *value,const char *detail);
/**
 * Return the number of records represented by test runtime test family set member without
 * changing their state.
 */
UmiStatus umi_test_runtime_test_family_set_member_count(UmiTestRuntimeTestFamily *value,uint64_t number);
/**
 * Provide the test runtime test family set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_test_family_set_generation(UmiTestRuntimeTestFamily *value,uint64_t number);
/**
 * Provide the test runtime test family same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_test_family_same_identity(const UmiTestRuntimeTestFamily *left,const UmiTestRuntimeTestFamily *right);
#ifdef __cplusplus
}
#endif
#endif
