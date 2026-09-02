/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/timeout_evidence.h
 *
 * PURPOSE:
 *   Retain timeout budgets, elapsed time and cancellation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TIMEOUT_EVIDENCE
#define UMICOM_TEST_RUNTIME_TIMEOUT_EVIDENCE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime timeout evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeTimeoutEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t elapsed_ms;
    uint64_t budget_ms;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeTimeoutEvidence;

/**
 * Initialise test runtime timeout evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_timeout_evidence_init(UmiTestRuntimeTimeoutEvidence *value, const char *id);
/**
 * Check that test runtime timeout evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_timeout_evidence_validate(const UmiTestRuntimeTimeoutEvidence *value);
/**
 * Provide the test runtime timeout evidence set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_set_name(UmiTestRuntimeTimeoutEvidence *value, const char *name);
/**
 * Provide the test runtime timeout evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_set_detail(UmiTestRuntimeTimeoutEvidence *value, const char *detail);
/**
 * Provide the test runtime timeout evidence set elapsed ms operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_set_elapsed_ms(UmiTestRuntimeTimeoutEvidence *value, uint64_t number);
/**
 * Provide the test runtime timeout evidence set budget ms operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_set_budget_ms(UmiTestRuntimeTimeoutEvidence *value, uint64_t number);
/**
 * Provide the test runtime timeout evidence touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_touch(UmiTestRuntimeTimeoutEvidence *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime timeout evidence same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_timeout_evidence_same_identity(const UmiTestRuntimeTimeoutEvidence *left, const UmiTestRuntimeTimeoutEvidence *right);

#ifdef __cplusplus
}
#endif
#endif
