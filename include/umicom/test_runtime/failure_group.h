/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_group.h
 *
 * PURPOSE:
 *   Group failures by common launch, timeout, crash or functional cause.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_GROUP
#define UMICOM_TEST_RUNTIME_FAILURE_GROUP
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeFailureGroup {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t member_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeFailureGroup;
void umi_test_runtime_failure_group_init(UmiTestRuntimeFailureGroup *value,const char *id);
UmiStatus umi_test_runtime_failure_group_validate(const UmiTestRuntimeFailureGroup *value);
UmiStatus umi_test_runtime_failure_group_set_category(UmiTestRuntimeFailureGroup *value,const char *category);
UmiStatus umi_test_runtime_failure_group_set_detail(UmiTestRuntimeFailureGroup *value,const char *detail);
UmiStatus umi_test_runtime_failure_group_set_member_count(UmiTestRuntimeFailureGroup *value,uint64_t number);
UmiStatus umi_test_runtime_failure_group_set_generation(UmiTestRuntimeFailureGroup *value,uint64_t number);
UmiStatus umi_test_runtime_failure_group_set_active(UmiTestRuntimeFailureGroup *value,bool active);
bool umi_test_runtime_failure_group_same_identity(const UmiTestRuntimeFailureGroup *left,const UmiTestRuntimeFailureGroup *right);
#ifdef __cplusplus
}
#endif
#endif
