/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/dependency_directory.h
 *
 * PURPOSE:
 *   Represent one directory scanned for runtime DLL or shared-library dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DEPENDENCY_DIRECTORY
#define UMICOM_TEST_RUNTIME_DEPENDENCY_DIRECTORY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeDependencyDirectory {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t priority;
    uint64_t resolved_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeDependencyDirectory;
void umi_test_runtime_dependency_directory_init(UmiTestRuntimeDependencyDirectory *value,const char *id);
UmiStatus umi_test_runtime_dependency_directory_validate(const UmiTestRuntimeDependencyDirectory *value);
UmiStatus umi_test_runtime_dependency_directory_set_category(UmiTestRuntimeDependencyDirectory *value,const char *category);
UmiStatus umi_test_runtime_dependency_directory_set_detail(UmiTestRuntimeDependencyDirectory *value,const char *detail);
UmiStatus umi_test_runtime_dependency_directory_set_priority(UmiTestRuntimeDependencyDirectory *value,uint64_t number);
UmiStatus umi_test_runtime_dependency_directory_set_resolved_count(UmiTestRuntimeDependencyDirectory *value,uint64_t number);
UmiStatus umi_test_runtime_dependency_directory_set_active(UmiTestRuntimeDependencyDirectory *value,bool active);
bool umi_test_runtime_dependency_directory_same_identity(const UmiTestRuntimeDependencyDirectory *left,const UmiTestRuntimeDependencyDirectory *right);
#ifdef __cplusplus
}
#endif
#endif
