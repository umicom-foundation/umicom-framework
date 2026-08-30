/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/dependency_closure.h
 *
 * PURPOSE:
 *   Evaluate transitive runtime dependency closure before execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DEPENDENCY_CLOSURE
#define UMICOM_TEST_RUNTIME_DEPENDENCY_CLOSURE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeDependencyClosure {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t dependency_count;
    uint64_t missing_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeDependencyClosure;
void umi_test_runtime_dependency_closure_init(UmiTestRuntimeDependencyClosure *value,const char *id);
UmiStatus umi_test_runtime_dependency_closure_validate(const UmiTestRuntimeDependencyClosure *value);
UmiStatus umi_test_runtime_dependency_closure_set_category(UmiTestRuntimeDependencyClosure *value,const char *category);
UmiStatus umi_test_runtime_dependency_closure_set_detail(UmiTestRuntimeDependencyClosure *value,const char *detail);
UmiStatus umi_test_runtime_dependency_closure_set_dependency_count(UmiTestRuntimeDependencyClosure *value,uint64_t number);
UmiStatus umi_test_runtime_dependency_closure_set_missing_count(UmiTestRuntimeDependencyClosure *value,uint64_t number);
UmiStatus umi_test_runtime_dependency_closure_set_active(UmiTestRuntimeDependencyClosure *value,bool active);
bool umi_test_runtime_dependency_closure_same_identity(const UmiTestRuntimeDependencyClosure *left,const UmiTestRuntimeDependencyClosure *right);
#ifdef __cplusplus
}
#endif
#endif
