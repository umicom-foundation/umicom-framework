/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/artifact_locator.h
 *
 * PURPOSE:
 *   Resolve test executables, logs and generated evidence from build roots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ARTIFACT_LOCATOR
#define UMICOM_TEST_RUNTIME_ARTIFACT_LOCATOR

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime artifact locator data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeArtifactLocator {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t resolved;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeArtifactLocator;

/**
 * Initialise test runtime artifact locator from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_artifact_locator_init(UmiTestRuntimeArtifactLocator *value, const char *id);
/**
 * Check that test runtime artifact locator satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_artifact_locator_validate(const UmiTestRuntimeArtifactLocator *value);
/**
 * Provide the test runtime artifact locator set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_artifact_locator_set_name(UmiTestRuntimeArtifactLocator *value, const char *name);
/**
 * Provide the test runtime artifact locator set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_artifact_locator_set_detail(UmiTestRuntimeArtifactLocator *value, const char *detail);
/**
 * Return the number of records represented by test runtime artifact locator set candidate
 * without changing their state.
 */
UmiStatus umi_test_runtime_artifact_locator_set_candidate_count(UmiTestRuntimeArtifactLocator *value, uint64_t number);
/**
 * Provide the test runtime artifact locator set resolved operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_artifact_locator_set_resolved(UmiTestRuntimeArtifactLocator *value, uint64_t number);
/**
 * Provide the test runtime artifact locator touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_artifact_locator_touch(UmiTestRuntimeArtifactLocator *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime artifact locator same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_artifact_locator_same_identity(const UmiTestRuntimeArtifactLocator *left, const UmiTestRuntimeArtifactLocator *right);

#ifdef __cplusplus
}
#endif
#endif
