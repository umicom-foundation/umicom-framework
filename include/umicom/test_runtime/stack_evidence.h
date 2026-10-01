/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/stack_evidence.h
 *
 * PURPOSE:
 *   Retain stack exhaustion and recursion depth evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_STACK_EVIDENCE
#define UMICOM_TEST_RUNTIME_STACK_EVIDENCE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime stack evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeStackEvidence {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t estimated_bytes;
    uint64_t depth;
    uint64_t revision;
    bool active;
} UmiTestRuntimeStackEvidence;
/**
 * Initialise test runtime stack evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_stack_evidence_init(UmiTestRuntimeStackEvidence *value,const char *id);
/**
 * Check that test runtime stack evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_stack_evidence_validate(const UmiTestRuntimeStackEvidence *value);
/**
 * Provide the test runtime stack evidence set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_category(UmiTestRuntimeStackEvidence *value,const char *category);
/**
 * Provide the test runtime stack evidence set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_detail(UmiTestRuntimeStackEvidence *value,const char *detail);
/**
 * Provide the test runtime stack evidence set estimated bytes operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_estimated_bytes(UmiTestRuntimeStackEvidence *value,uint64_t number);
/**
 * Provide the test runtime stack evidence set depth operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_depth(UmiTestRuntimeStackEvidence *value,uint64_t number);
/**
 * Provide the test runtime stack evidence set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_stack_evidence_set_active(UmiTestRuntimeStackEvidence *value,bool active);
/**
 * Provide the test runtime stack evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_stack_evidence_same_identity(const UmiTestRuntimeStackEvidence *left,const UmiTestRuntimeStackEvidence *right);
/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

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
UmiStatus umi_test_runtime_stack_evidence_init_checked(UmiTestRuntimeStackEvidence *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
