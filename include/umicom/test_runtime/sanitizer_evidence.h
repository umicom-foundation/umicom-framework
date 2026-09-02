/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/sanitizer_evidence.h
 *
 * PURPOSE:
 *   Retain sanitizer type, finding count and first failing location.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SANITIZER_EVIDENCE
#define UMICOM_TEST_RUNTIME_SANITIZER_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime sanitizer evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeSanitizerEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t finding_count;
    uint64_t suppressed_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeSanitizerEvidence;
/**
 * Initialise test runtime sanitizer evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_sanitizer_evidence_init(UmiTestRuntimeSanitizerEvidence *value,const char *id);
/**
 * Check that test runtime sanitizer evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_validate(const UmiTestRuntimeSanitizerEvidence *value);
/**
 * Provide the test runtime sanitizer evidence set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_category(UmiTestRuntimeSanitizerEvidence *value,const char *category);
/**
 * Provide the test runtime sanitizer evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_detail(UmiTestRuntimeSanitizerEvidence *value,const char *detail);
/**
 * Return the number of records represented by test runtime sanitizer evidence set finding
 * without changing their state.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_finding_count(UmiTestRuntimeSanitizerEvidence *value,uint64_t number);
/**
 * Return the number of records represented by test runtime sanitizer evidence set
 * suppressed without changing their state.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_suppressed_count(UmiTestRuntimeSanitizerEvidence *value,uint64_t number);
/**
 * Provide the test runtime sanitizer evidence set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_sanitizer_evidence_set_active(UmiTestRuntimeSanitizerEvidence *value,bool active);
/**
 * Provide the test runtime sanitizer evidence same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_sanitizer_evidence_same_identity(const UmiTestRuntimeSanitizerEvidence *left,const UmiTestRuntimeSanitizerEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
