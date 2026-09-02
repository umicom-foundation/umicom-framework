/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/memory_evidence.h
 *
 * PURPOSE:
 *   Retain allocation, bounds and ownership diagnostics for test failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_MEMORY_EVIDENCE
#define UMICOM_TEST_RUNTIME_MEMORY_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime memory evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeMemoryEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t allocation_count;
    uint64_t failure_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeMemoryEvidence;
/**
 * Initialise test runtime memory evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_memory_evidence_init(UmiTestRuntimeMemoryEvidence *value,const char *id);
/**
 * Check that test runtime memory evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_memory_evidence_validate(const UmiTestRuntimeMemoryEvidence *value);
/**
 * Provide the test runtime memory evidence set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_memory_evidence_set_category(UmiTestRuntimeMemoryEvidence *value,const char *category);
/**
 * Provide the test runtime memory evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_memory_evidence_set_detail(UmiTestRuntimeMemoryEvidence *value,const char *detail);
/**
 * Return the number of records represented by test runtime memory evidence set allocation
 * without changing their state.
 */
UmiStatus umi_test_runtime_memory_evidence_set_allocation_count(UmiTestRuntimeMemoryEvidence *value,uint64_t number);
/**
 * Return the number of records represented by test runtime memory evidence set failure
 * without changing their state.
 */
UmiStatus umi_test_runtime_memory_evidence_set_failure_count(UmiTestRuntimeMemoryEvidence *value,uint64_t number);
/**
 * Provide the test runtime memory evidence set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_memory_evidence_set_active(UmiTestRuntimeMemoryEvidence *value,bool active);
/**
 * Provide the test runtime memory evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_memory_evidence_same_identity(const UmiTestRuntimeMemoryEvidence *left,const UmiTestRuntimeMemoryEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
