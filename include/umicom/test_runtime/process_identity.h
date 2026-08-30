/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/process_identity.h
 *
 * PURPOSE:
 *   Retain process, parent-process and execution correlation identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PROCESS_IDENTITY
#define UMICOM_TEST_RUNTIME_PROCESS_IDENTITY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeProcessIdentity {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t process_id;
    uint64_t parent_process_id;
    uint64_t revision;
    bool active;
} UmiTestRuntimeProcessIdentity;
void umi_test_runtime_process_identity_init(UmiTestRuntimeProcessIdentity *value,const char *id);
UmiStatus umi_test_runtime_process_identity_validate(const UmiTestRuntimeProcessIdentity *value);
UmiStatus umi_test_runtime_process_identity_set_category(UmiTestRuntimeProcessIdentity *value,const char *category);
UmiStatus umi_test_runtime_process_identity_set_detail(UmiTestRuntimeProcessIdentity *value,const char *detail);
UmiStatus umi_test_runtime_process_identity_set_process_id(UmiTestRuntimeProcessIdentity *value,uint64_t number);
UmiStatus umi_test_runtime_process_identity_set_parent_process_id(UmiTestRuntimeProcessIdentity *value,uint64_t number);
UmiStatus umi_test_runtime_process_identity_set_active(UmiTestRuntimeProcessIdentity *value,bool active);
bool umi_test_runtime_process_identity_same_identity(const UmiTestRuntimeProcessIdentity *left,const UmiTestRuntimeProcessIdentity *right);
#ifdef __cplusplus
}
#endif
#endif
