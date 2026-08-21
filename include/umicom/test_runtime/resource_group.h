/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/resource_group.h
 *
 * PURPOSE:
 *   Describe tests sharing constrained ports, databases or temporary state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESOURCE_GROUP
#define UMICOM_TEST_RUNTIME_RESOURCE_GROUP
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_test_runtime_resource_group_init(UmiTestRuntimeResourceGroup *value,const char *id);
UmiStatus umi_test_runtime_resource_group_validate(const UmiTestRuntimeResourceGroup *value);
UmiStatus umi_test_runtime_resource_group_set_category(UmiTestRuntimeResourceGroup *value,const char *category);
UmiStatus umi_test_runtime_resource_group_set_detail(UmiTestRuntimeResourceGroup *value,const char *detail);
UmiStatus umi_test_runtime_resource_group_set_member_count(UmiTestRuntimeResourceGroup *value,uint64_t number);
UmiStatus umi_test_runtime_resource_group_set_capacity(UmiTestRuntimeResourceGroup *value,uint64_t number);
UmiStatus umi_test_runtime_resource_group_set_active(UmiTestRuntimeResourceGroup *value,bool active);
bool umi_test_runtime_resource_group_same_identity(const UmiTestRuntimeResourceGroup *left,const UmiTestRuntimeResourceGroup *right);
#ifdef __cplusplus
}
#endif
#endif
