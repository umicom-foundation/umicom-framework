/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/resource_lock.h
 *
 * PURPOSE:
 *   Describe an exclusive test resource lock and ownership evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESOURCE_LOCK
#define UMICOM_TEST_RUNTIME_RESOURCE_LOCK
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeResourceLock {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t acquired;
    uint64_t waiter_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeResourceLock;
void umi_test_runtime_resource_lock_init(UmiTestRuntimeResourceLock *value,const char *id);
UmiStatus umi_test_runtime_resource_lock_validate(const UmiTestRuntimeResourceLock *value);
UmiStatus umi_test_runtime_resource_lock_set_category(UmiTestRuntimeResourceLock *value,const char *category);
UmiStatus umi_test_runtime_resource_lock_set_detail(UmiTestRuntimeResourceLock *value,const char *detail);
UmiStatus umi_test_runtime_resource_lock_set_acquired(UmiTestRuntimeResourceLock *value,uint64_t number);
UmiStatus umi_test_runtime_resource_lock_set_waiter_count(UmiTestRuntimeResourceLock *value,uint64_t number);
UmiStatus umi_test_runtime_resource_lock_set_active(UmiTestRuntimeResourceLock *value,bool active);
bool umi_test_runtime_resource_lock_same_identity(const UmiTestRuntimeResourceLock *left,const UmiTestRuntimeResourceLock *right);
#ifdef __cplusplus
}
#endif
#endif
