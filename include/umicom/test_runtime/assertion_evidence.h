/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/assertion_evidence.h
 *
 * PURPOSE:
 *   Retain assertion failure identity and source evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ASSERTION_EVIDENCE
#define UMICOM_TEST_RUNTIME_ASSERTION_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime assertion evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeAssertionEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t line;
    uint64_t occurrence_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeAssertionEvidence;
/**
 * Initialise test runtime assertion evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_assertion_evidence_init(UmiTestRuntimeAssertionEvidence *value,const char *id);
/**
 * Check that test runtime assertion evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_assertion_evidence_validate(const UmiTestRuntimeAssertionEvidence *value);
/**
 * Provide the test runtime assertion evidence set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_assertion_evidence_set_category(UmiTestRuntimeAssertionEvidence *value,const char *category);
/**
 * Provide the test runtime assertion evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_assertion_evidence_set_detail(UmiTestRuntimeAssertionEvidence *value,const char *detail);
/**
 * Provide the test runtime assertion evidence set line operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_assertion_evidence_set_line(UmiTestRuntimeAssertionEvidence *value,uint64_t number);
/**
 * Return the number of records represented by test runtime assertion evidence set
 * occurrence without changing their state.
 */
UmiStatus umi_test_runtime_assertion_evidence_set_occurrence_count(UmiTestRuntimeAssertionEvidence *value,uint64_t number);
/**
 * Provide the test runtime assertion evidence set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_assertion_evidence_set_active(UmiTestRuntimeAssertionEvidence *value,bool active);
/**
 * Provide the test runtime assertion evidence same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_assertion_evidence_same_identity(const UmiTestRuntimeAssertionEvidence *left,const UmiTestRuntimeAssertionEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
