/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/environment_snapshot.h
 *
 * PURPOSE:
 *   Capture deterministic environment evidence for regression reproduction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ENVIRONMENT_SNAPSHOT
#define UMICOM_TEST_RUNTIME_ENVIRONMENT_SNAPSHOT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime environment snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeEnvironmentSnapshot {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeEnvironmentSnapshot;
/**
 * Initialise test runtime environment snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_environment_snapshot_init(UmiTestRuntimeEnvironmentSnapshot *value,const char *id);
/**
 * Check that test runtime environment snapshot satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_environment_snapshot_validate(const UmiTestRuntimeEnvironmentSnapshot *value);
/**
 * Provide the test runtime environment snapshot set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_environment_snapshot_set_category(UmiTestRuntimeEnvironmentSnapshot *value,const char *category);
/**
 * Provide the test runtime environment snapshot set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_environment_snapshot_set_detail(UmiTestRuntimeEnvironmentSnapshot *value,const char *detail);
/**
 * Return the number of records represented by test runtime environment snapshot set entry
 * without changing their state.
 */
UmiStatus umi_test_runtime_environment_snapshot_set_entry_count(UmiTestRuntimeEnvironmentSnapshot *value,uint64_t number);
/**
 * Provide the test runtime environment snapshot set generation operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_environment_snapshot_set_generation(UmiTestRuntimeEnvironmentSnapshot *value,uint64_t number);
/**
 * Provide the test runtime environment snapshot set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_environment_snapshot_set_active(UmiTestRuntimeEnvironmentSnapshot *value,bool active);
/**
 * Provide the test runtime environment snapshot same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_environment_snapshot_same_identity(const UmiTestRuntimeEnvironmentSnapshot *left,const UmiTestRuntimeEnvironmentSnapshot *right);
#ifdef __cplusplus
}
#endif
#endif
