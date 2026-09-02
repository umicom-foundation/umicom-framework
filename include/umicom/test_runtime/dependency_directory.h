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
/**
 * Represent the test runtime dependency directory data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise test runtime dependency directory from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_dependency_directory_init(UmiTestRuntimeDependencyDirectory *value,const char *id);
/**
 * Check that test runtime dependency directory satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_dependency_directory_validate(const UmiTestRuntimeDependencyDirectory *value);
/**
 * Provide the test runtime dependency directory set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_dependency_directory_set_category(UmiTestRuntimeDependencyDirectory *value,const char *category);
/**
 * Provide the test runtime dependency directory set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_dependency_directory_set_detail(UmiTestRuntimeDependencyDirectory *value,const char *detail);
/**
 * Provide the test runtime dependency directory set priority operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_dependency_directory_set_priority(UmiTestRuntimeDependencyDirectory *value,uint64_t number);
/**
 * Return the number of records represented by test runtime dependency directory set
 * resolved without changing their state.
 */
UmiStatus umi_test_runtime_dependency_directory_set_resolved_count(UmiTestRuntimeDependencyDirectory *value,uint64_t number);
/**
 * Provide the test runtime dependency directory set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_dependency_directory_set_active(UmiTestRuntimeDependencyDirectory *value,bool active);
/**
 * Provide the test runtime dependency directory same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_dependency_directory_same_identity(const UmiTestRuntimeDependencyDirectory *left,const UmiTestRuntimeDependencyDirectory *right);
#ifdef __cplusplus
}
#endif
#endif
