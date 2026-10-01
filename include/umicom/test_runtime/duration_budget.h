/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/duration_budget.h
 *
 * PURPOSE:
 *   Retain expected and observed duration budgets for one test family.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DURATION_BUDGET
#define UMICOM_TEST_RUNTIME_DURATION_BUDGET
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime duration budget data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeDurationBudget {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t expected_ms;
    uint64_t maximum_ms;
    uint64_t revision;
    bool active;
} UmiTestRuntimeDurationBudget;
/**
 * Initialise test runtime duration budget from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_duration_budget_init(UmiTestRuntimeDurationBudget *value,const char *id);
/**
 * Check that test runtime duration budget satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_duration_budget_validate(const UmiTestRuntimeDurationBudget *value);
/**
 * Provide the test runtime duration budget set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_duration_budget_set_category(UmiTestRuntimeDurationBudget *value,const char *category);
/**
 * Provide the test runtime duration budget set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_duration_budget_set_detail(UmiTestRuntimeDurationBudget *value,const char *detail);
/**
 * Provide the test runtime duration budget set expected ms operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_duration_budget_set_expected_ms(UmiTestRuntimeDurationBudget *value,uint64_t number);
/**
 * Provide the test runtime duration budget set maximum ms operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_duration_budget_set_maximum_ms(UmiTestRuntimeDurationBudget *value,uint64_t number);
/**
 * Provide the test runtime duration budget set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_duration_budget_set_active(UmiTestRuntimeDurationBudget *value,bool active);
/**
 * Provide the test runtime duration budget same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_duration_budget_same_identity(const UmiTestRuntimeDurationBudget *left,const UmiTestRuntimeDurationBudget *right);
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
UmiStatus umi_test_runtime_duration_budget_init_checked(UmiTestRuntimeDurationBudget *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
