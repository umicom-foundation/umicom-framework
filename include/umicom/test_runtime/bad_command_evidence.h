/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/bad_command_evidence.h
 *
 * PURPOSE:
 *   Retain evidence when CTest cannot start a registered test command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_BAD_COMMAND_EVIDENCE
#define UMICOM_TEST_RUNTIME_BAD_COMMAND_EVIDENCE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime bad command evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeBadCommandEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t missing_dependency_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeBadCommandEvidence;

/**
 * Initialise test runtime bad command evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_bad_command_evidence_init(UmiTestRuntimeBadCommandEvidence *value, const char *id);
/**
 * Check that test runtime bad command evidence satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_bad_command_evidence_validate(const UmiTestRuntimeBadCommandEvidence *value);
/**
 * Provide the test runtime bad command evidence set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_bad_command_evidence_set_name(UmiTestRuntimeBadCommandEvidence *value, const char *name);
/**
 * Provide the test runtime bad command evidence set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_bad_command_evidence_set_detail(UmiTestRuntimeBadCommandEvidence *value, const char *detail);
/**
 * Return the number of records represented by test runtime bad command evidence set
 * candidate without changing their state.
 */
UmiStatus umi_test_runtime_bad_command_evidence_set_candidate_count(UmiTestRuntimeBadCommandEvidence *value, uint64_t number);
/**
 * Return the number of records represented by test runtime bad command evidence set
 * missing dependency without changing their state.
 */
UmiStatus umi_test_runtime_bad_command_evidence_set_missing_dependency_count(UmiTestRuntimeBadCommandEvidence *value, uint64_t number);
/**
 * Provide the test runtime bad command evidence touch operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_bad_command_evidence_touch(UmiTestRuntimeBadCommandEvidence *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime bad command evidence same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_bad_command_evidence_same_identity(const UmiTestRuntimeBadCommandEvidence *left, const UmiTestRuntimeBadCommandEvidence *right);

#ifdef __cplusplus
}
#endif
#endif
