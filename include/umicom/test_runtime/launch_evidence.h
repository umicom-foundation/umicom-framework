/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/launch_evidence.h
 *
 * PURPOSE:
 *   Retain child-process start, stop and native status evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_LAUNCH_EVIDENCE
#define UMICOM_TEST_RUNTIME_LAUNCH_EVIDENCE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime launch evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeLaunchEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t started;
    uint64_t completed;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeLaunchEvidence;

/**
 * Initialise test runtime launch evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_launch_evidence_init(UmiTestRuntimeLaunchEvidence *value, const char *id);
/**
 * Check that test runtime launch evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_launch_evidence_validate(const UmiTestRuntimeLaunchEvidence *value);
/**
 * Provide the test runtime launch evidence set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_launch_evidence_set_name(UmiTestRuntimeLaunchEvidence *value, const char *name);
/**
 * Provide the test runtime launch evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_launch_evidence_set_detail(UmiTestRuntimeLaunchEvidence *value, const char *detail);
/**
 * Provide the test runtime launch evidence set started operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_launch_evidence_set_started(UmiTestRuntimeLaunchEvidence *value, uint64_t number);
/**
 * Provide the test runtime launch evidence set completed operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_launch_evidence_set_completed(UmiTestRuntimeLaunchEvidence *value, uint64_t number);
/**
 * Provide the test runtime launch evidence touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_launch_evidence_touch(UmiTestRuntimeLaunchEvidence *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime launch evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_launch_evidence_same_identity(const UmiTestRuntimeLaunchEvidence *left, const UmiTestRuntimeLaunchEvidence *right);

#ifdef __cplusplus
}
#endif
#endif
