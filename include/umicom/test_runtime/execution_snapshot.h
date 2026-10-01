/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_snapshot.h
 *
 * PURPOSE:
 *   Capture immutable launch, runtime and result evidence for one test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_SNAPSHOT
#define UMICOM_TEST_RUNTIME_EXECUTION_SNAPSHOT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime execution snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeExecutionSnapshot {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t evidence_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeExecutionSnapshot;
/**
 * Initialise test runtime execution snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_execution_snapshot_init(UmiTestRuntimeExecutionSnapshot *value,const char *id);
/**
 * Check that test runtime execution snapshot satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_execution_snapshot_validate(const UmiTestRuntimeExecutionSnapshot *value);
/**
 * Provide the test runtime execution snapshot set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_category(UmiTestRuntimeExecutionSnapshot *value,const char *category);
/**
 * Provide the test runtime execution snapshot set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_detail(UmiTestRuntimeExecutionSnapshot *value,const char *detail);
/**
 * Return the number of records represented by test runtime execution snapshot set evidence
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_evidence_count(UmiTestRuntimeExecutionSnapshot *value,uint64_t number);
/**
 * Provide the test runtime execution snapshot set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_generation(UmiTestRuntimeExecutionSnapshot *value,uint64_t number);
/**
 * Provide the test runtime execution snapshot set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_snapshot_set_active(UmiTestRuntimeExecutionSnapshot *value,bool active);
/**
 * Provide the test runtime execution snapshot same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_execution_snapshot_same_identity(const UmiTestRuntimeExecutionSnapshot *left,const UmiTestRuntimeExecutionSnapshot *right);
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
UmiStatus umi_test_runtime_execution_snapshot_init_checked(UmiTestRuntimeExecutionSnapshot *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
