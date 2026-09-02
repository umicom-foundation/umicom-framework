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
/**
 * Represent the test runtime dependency closure data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise test runtime dependency closure from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_dependency_closure_init(UmiTestRuntimeDependencyClosure *value,const char *id);
/**
 * Check that test runtime dependency closure satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_dependency_closure_validate(const UmiTestRuntimeDependencyClosure *value);
/**
 * Provide the test runtime dependency closure set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_dependency_closure_set_category(UmiTestRuntimeDependencyClosure *value,const char *category);
/**
 * Provide the test runtime dependency closure set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_dependency_closure_set_detail(UmiTestRuntimeDependencyClosure *value,const char *detail);
/**
 * Return the number of records represented by test runtime dependency closure set
 * dependency without changing their state.
 */
UmiStatus umi_test_runtime_dependency_closure_set_dependency_count(UmiTestRuntimeDependencyClosure *value,uint64_t number);
/**
 * Return the number of records represented by test runtime dependency closure set missing
 * without changing their state.
 */
UmiStatus umi_test_runtime_dependency_closure_set_missing_count(UmiTestRuntimeDependencyClosure *value,uint64_t number);
/**
 * Provide the test runtime dependency closure set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_dependency_closure_set_active(UmiTestRuntimeDependencyClosure *value,bool active);
/**
 * Provide the test runtime dependency closure same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_dependency_closure_same_identity(const UmiTestRuntimeDependencyClosure *left,const UmiTestRuntimeDependencyClosure *right);
#ifdef __cplusplus
}
#endif
#endif
