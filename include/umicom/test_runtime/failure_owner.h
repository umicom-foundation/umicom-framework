/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_owner.h
 *
 * PURPOSE:
 *   Assign failing subsystems to durable component identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_OWNER
#define UMICOM_TEST_RUNTIME_FAILURE_OWNER
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime failure owner data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeFailureOwner
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t priority;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeFailureOwner;
/**
 * Initialise test runtime failure owner from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_failure_owner_init(UmiTestRuntimeFailureOwner *value,const char *id);
/**
 * Check that test runtime failure owner satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_failure_owner_validate(const UmiTestRuntimeFailureOwner *value);
/**
 * Provide the test runtime failure owner set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_failure_owner_set_detail(UmiTestRuntimeFailureOwner *value,const char *detail);
/**
 * Provide the test runtime failure owner set priority operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_failure_owner_set_priority(UmiTestRuntimeFailureOwner *value,uint64_t number);
/**
 * Provide the test runtime failure owner set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_failure_owner_set_generation(UmiTestRuntimeFailureOwner *value,uint64_t number);
/**
 * Provide the test runtime failure owner same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_failure_owner_same_identity(const UmiTestRuntimeFailureOwner *left,const UmiTestRuntimeFailureOwner *right);
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
UmiStatus umi_test_runtime_failure_owner_init_checked(UmiTestRuntimeFailureOwner *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
