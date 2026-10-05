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
#include "umicom/base/value_archive.h"
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
UmiStatus umi_test_runtime_assertion_evidence_init_checked(UmiTestRuntimeAssertionEvidence *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_test_runtime_assertion_evidence_archive_encode(const UmiTestRuntimeAssertionEvidence *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_runtime_assertion_evidence_archive_decode(const void *bytes, size_t byte_count,
    UmiTestRuntimeAssertionEvidence *value);

#ifdef __cplusplus
}
#endif
#endif
