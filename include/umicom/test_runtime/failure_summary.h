/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_summary.h
 *
 * PURPOSE:
 *   Aggregate failure categories without losing individual test identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_SUMMARY
#define UMICOM_TEST_RUNTIME_FAILURE_SUMMARY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime failure summary data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeFailureSummary {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t failure_count;
    uint64_t category_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeFailureSummary;
/**
 * Initialise test runtime failure summary from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_failure_summary_init(UmiTestRuntimeFailureSummary *value,const char *id);
/**
 * Check that test runtime failure summary satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_failure_summary_validate(const UmiTestRuntimeFailureSummary *value);
/**
 * Provide the test runtime failure summary set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_failure_summary_set_category(UmiTestRuntimeFailureSummary *value,const char *category);
/**
 * Provide the test runtime failure summary set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_failure_summary_set_detail(UmiTestRuntimeFailureSummary *value,const char *detail);
/**
 * Return the number of records represented by test runtime failure summary set failure
 * without changing their state.
 */
UmiStatus umi_test_runtime_failure_summary_set_failure_count(UmiTestRuntimeFailureSummary *value,uint64_t number);
/**
 * Return the number of records represented by test runtime failure summary set category
 * without changing their state.
 */
UmiStatus umi_test_runtime_failure_summary_set_category_count(UmiTestRuntimeFailureSummary *value,uint64_t number);
/**
 * Provide the test runtime failure summary set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_failure_summary_set_active(UmiTestRuntimeFailureSummary *value,bool active);
/**
 * Provide the test runtime failure summary same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_failure_summary_same_identity(const UmiTestRuntimeFailureSummary *left,const UmiTestRuntimeFailureSummary *right);
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
UmiStatus umi_test_runtime_failure_summary_init_checked(UmiTestRuntimeFailureSummary *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
