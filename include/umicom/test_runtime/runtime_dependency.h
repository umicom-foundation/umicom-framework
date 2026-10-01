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


/** Publish a complete reviewed value while expected_revision still matches.
 * Copy the live record, edit that copy with the ordinary setters, and retain
 * the revision observed before editing. Both values must validate and have
 * the same ID. A stale review returns INVALID_STATE; exhausted revision space
 * returns CAPACITY_EXCEEDED. All failures leave the live value unchanged.
 * Success copies every proposal field and assigns one new live revision;
 * the proposal's own revision does not control publication. Self-assignment
 * is allowed and also advances once. Other overlapping storage is unsupported.
 * Serialize access on the owner. This value operation allocates nothing and
 * does not perform I/O, authenticate evidence or run the described workflow. */
UmiStatus umi_test_runtime_runtime_dependency_replace_if_current(UmiTestRuntimeRuntimeDependency *value,
    uint64_t expected_revision, const UmiTestRuntimeRuntimeDependency *proposal);

#ifdef __cplusplus
}
#endif
#endif
