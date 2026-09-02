/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/resolution_evidence.h
 *
 * PURPOSE:
 *   Retain evidence that a previously failing test now passes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESOLUTION_EVIDENCE
#define UMICOM_TEST_RUNTIME_RESOLUTION_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime resolution evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeResolutionEvidence
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t verification_count;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeResolutionEvidence;
/**
 * Initialise test runtime resolution evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_resolution_evidence_init(UmiTestRuntimeResolutionEvidence *value,const char *id);
/**
 * Check that test runtime resolution evidence satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_resolution_evidence_validate(const UmiTestRuntimeResolutionEvidence *value);
/**
 * Provide the test runtime resolution evidence set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_resolution_evidence_set_detail(UmiTestRuntimeResolutionEvidence *value,const char *detail);
/**
 * Return the number of records represented by test runtime resolution evidence set
 * verification without changing their state.
 */
UmiStatus umi_test_runtime_resolution_evidence_set_verification_count(UmiTestRuntimeResolutionEvidence *value,uint64_t number);
/**
 * Provide the test runtime resolution evidence set generation operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_resolution_evidence_set_generation(UmiTestRuntimeResolutionEvidence *value,uint64_t number);
/**
 * Provide the test runtime resolution evidence same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_resolution_evidence_same_identity(const UmiTestRuntimeResolutionEvidence *left,const UmiTestRuntimeResolutionEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
