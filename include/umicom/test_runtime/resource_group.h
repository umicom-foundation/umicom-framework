/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/resource_group.h
 *
 * PURPOSE:
 *   Describe tests sharing constrained ports, databases or temporary state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESOURCE_GROUP
#define UMICOM_TEST_RUNTIME_RESOURCE_GROUP
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime resource group data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeResourceGroup {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t member_count;
    uint64_t capacity;
    uint64_t revision;
    bool active;
} UmiTestRuntimeResourceGroup;
/**
 * Initialise test runtime resource group from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_resource_group_init(UmiTestRuntimeResourceGroup *value,const char *id);
/**
 * Check that test runtime resource group satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_resource_group_validate(const UmiTestRuntimeResourceGroup *value);
/**
 * Provide the test runtime resource group set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_resource_group_set_category(UmiTestRuntimeResourceGroup *value,const char *category);
/**
 * Provide the test runtime resource group set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_resource_group_set_detail(UmiTestRuntimeResourceGroup *value,const char *detail);
/**
 * Return the number of records represented by test runtime resource group set member
 * without changing their state.
 */
UmiStatus umi_test_runtime_resource_group_set_member_count(UmiTestRuntimeResourceGroup *value,uint64_t number);
/**
 * Provide the test runtime resource group set capacity operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_resource_group_set_capacity(UmiTestRuntimeResourceGroup *value,uint64_t number);
/**
 * Provide the test runtime resource group set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_resource_group_set_active(UmiTestRuntimeResourceGroup *value,bool active);
/**
 * Provide the test runtime resource group same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_resource_group_same_identity(const UmiTestRuntimeResourceGroup *left,const UmiTestRuntimeResourceGroup *right);
#ifdef __cplusplus
}
#endif
#endif
