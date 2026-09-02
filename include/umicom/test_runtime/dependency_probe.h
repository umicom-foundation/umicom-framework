/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/dependency_probe.h
 *
 * PURPOSE:
 *   Record runtime dependency discovery and missing-library evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DEPENDENCY_PROBE
#define UMICOM_TEST_RUNTIME_DEPENDENCY_PROBE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime dependency probe data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeDependencyProbe {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t dependency_count;
    uint64_t missing_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeDependencyProbe;

/**
 * Initialise test runtime dependency probe from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_dependency_probe_init(UmiTestRuntimeDependencyProbe *value, const char *id);
/**
 * Check that test runtime dependency probe satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_dependency_probe_validate(const UmiTestRuntimeDependencyProbe *value);
/**
 * Provide the test runtime dependency probe set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_dependency_probe_set_name(UmiTestRuntimeDependencyProbe *value, const char *name);
/**
 * Provide the test runtime dependency probe set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_dependency_probe_set_detail(UmiTestRuntimeDependencyProbe *value, const char *detail);
/**
 * Return the number of records represented by test runtime dependency probe set dependency
 * without changing their state.
 */
UmiStatus umi_test_runtime_dependency_probe_set_dependency_count(UmiTestRuntimeDependencyProbe *value, uint64_t number);
/**
 * Return the number of records represented by test runtime dependency probe set missing
 * without changing their state.
 */
UmiStatus umi_test_runtime_dependency_probe_set_missing_count(UmiTestRuntimeDependencyProbe *value, uint64_t number);
/**
 * Provide the test runtime dependency probe touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_dependency_probe_touch(UmiTestRuntimeDependencyProbe *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime dependency probe same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_dependency_probe_same_identity(const UmiTestRuntimeDependencyProbe *left, const UmiTestRuntimeDependencyProbe *right);

#ifdef __cplusplus
}
#endif
#endif
