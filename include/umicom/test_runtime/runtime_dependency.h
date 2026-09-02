/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/runtime_dependency.h
 *
 * PURPOSE:
 *   Describe one dynamic runtime dependency and where it was resolved.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUNTIME_DEPENDENCY
#define UMICOM_TEST_RUNTIME_RUNTIME_DEPENDENCY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime runtime dependency data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeRuntimeDependency {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t resolved;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeRuntimeDependency;

/**
 * Initialise test runtime runtime dependency from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_runtime_dependency_init(UmiTestRuntimeRuntimeDependency *value, const char *id);
/**
 * Check that test runtime runtime dependency satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_dependency_validate(const UmiTestRuntimeRuntimeDependency *value);
/**
 * Provide the test runtime runtime dependency set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_dependency_set_name(UmiTestRuntimeRuntimeDependency *value, const char *name);
/**
 * Provide the test runtime runtime dependency set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_runtime_dependency_set_detail(UmiTestRuntimeRuntimeDependency *value, const char *detail);
/**
 * Provide the test runtime runtime dependency set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_runtime_dependency_set_required(UmiTestRuntimeRuntimeDependency *value, uint64_t number);
/**
 * Provide the test runtime runtime dependency set resolved operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_runtime_dependency_set_resolved(UmiTestRuntimeRuntimeDependency *value, uint64_t number);
/**
 * Provide the test runtime runtime dependency touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_dependency_touch(UmiTestRuntimeRuntimeDependency *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime runtime dependency same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_runtime_dependency_same_identity(const UmiTestRuntimeRuntimeDependency *left, const UmiTestRuntimeRuntimeDependency *right);

#ifdef __cplusplus
}
#endif
#endif
