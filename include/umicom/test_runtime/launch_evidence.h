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
UmiStatus umi_test_runtime_launch_evidence_replace_if_current(UmiTestRuntimeLaunchEvidence *value,
    uint64_t expected_revision, const UmiTestRuntimeLaunchEvidence *proposal);

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_test_runtime_launch_evidence_init_checked(UmiTestRuntimeLaunchEvidence *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
