/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_reproduction.h
 *
 * PURPOSE:
 *   Retain the exact runtime context required to reproduce a failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_REPRODUCTION
#define UMICOM_TEST_RUNTIME_FAILURE_REPRODUCTION
#include "umicom/test_runtime/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime failure reproduction data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeFailureReproduction
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t evidence_count;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeFailureReproduction;
/**
 * Initialise test runtime failure reproduction from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_failure_reproduction_init(UmiTestRuntimeFailureReproduction *value,const char *id);
/**
 * Check that test runtime failure reproduction satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_failure_reproduction_validate(const UmiTestRuntimeFailureReproduction *value);
/**
 * Provide the test runtime failure reproduction set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_failure_reproduction_set_detail(UmiTestRuntimeFailureReproduction *value,const char *detail);
/**
 * Return the number of records represented by test runtime failure reproduction set
 * evidence without changing their state.
 */
UmiStatus umi_test_runtime_failure_reproduction_set_evidence_count(UmiTestRuntimeFailureReproduction *value,uint64_t number);
/**
 * Provide the test runtime failure reproduction set generation operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_failure_reproduction_set_generation(UmiTestRuntimeFailureReproduction *value,uint64_t number);
/**
 * Provide the test runtime failure reproduction same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_failure_reproduction_same_identity(const UmiTestRuntimeFailureReproduction *left,const UmiTestRuntimeFailureReproduction *right);
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
UmiStatus umi_test_runtime_failure_reproduction_init_checked(UmiTestRuntimeFailureReproduction *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_test_runtime_failure_reproduction_archive_encode(const UmiTestRuntimeFailureReproduction *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_test_runtime_failure_reproduction_archive_decode(const void *bytes, size_t byte_count,
    UmiTestRuntimeFailureReproduction *value);

#ifdef __cplusplus
}
#endif
#endif
