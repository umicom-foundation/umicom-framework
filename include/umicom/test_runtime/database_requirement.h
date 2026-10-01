/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/database_requirement.h
 *
 * PURPOSE:
 *   Describe database provider and isolation requirements for a test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DATABASE_REQUIREMENT
#define UMICOM_TEST_RUNTIME_DATABASE_REQUIREMENT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime database requirement data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeDatabaseRequirement
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t available;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeDatabaseRequirement;
/**
 * Initialise test runtime database requirement from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_database_requirement_init(UmiTestRuntimeDatabaseRequirement *value,const char *id);
/**
 * Check that test runtime database requirement satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_database_requirement_validate(const UmiTestRuntimeDatabaseRequirement *value);
/**
 * Provide the test runtime database requirement set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_database_requirement_set_detail(UmiTestRuntimeDatabaseRequirement *value,const char *detail);
/**
 * Provide the test runtime database requirement set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_database_requirement_set_required(UmiTestRuntimeDatabaseRequirement *value,uint64_t number);
/**
 * Provide the test runtime database requirement set available operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_database_requirement_set_available(UmiTestRuntimeDatabaseRequirement *value,uint64_t number);
/**
 * Provide the test runtime database requirement same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_database_requirement_same_identity(const UmiTestRuntimeDatabaseRequirement *left,const UmiTestRuntimeDatabaseRequirement *right);
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
UmiStatus umi_test_runtime_database_requirement_init_checked(UmiTestRuntimeDatabaseRequirement *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
