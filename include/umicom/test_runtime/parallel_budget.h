/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/parallel_budget.h
 *
 * PURPOSE:
 *   Define safe parallelism for constrained Windows and CI environments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_PARALLEL_BUDGET
#define UMICOM_TEST_RUNTIME_PARALLEL_BUDGET
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime parallel budget data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeParallelBudget {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t maximum_parallel;
    uint64_t processor_cost;
    uint64_t revision;
    bool active;
} UmiTestRuntimeParallelBudget;
/**
 * Initialise test runtime parallel budget from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_parallel_budget_init(UmiTestRuntimeParallelBudget *value,const char *id);
/**
 * Check that test runtime parallel budget satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_parallel_budget_validate(const UmiTestRuntimeParallelBudget *value);
/**
 * Provide the test runtime parallel budget set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_category(UmiTestRuntimeParallelBudget *value,const char *category);
/**
 * Provide the test runtime parallel budget set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_detail(UmiTestRuntimeParallelBudget *value,const char *detail);
/**
 * Provide the test runtime parallel budget set maximum parallel operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_maximum_parallel(UmiTestRuntimeParallelBudget *value,uint64_t number);
/**
 * Provide the test runtime parallel budget set processor cost operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_processor_cost(UmiTestRuntimeParallelBudget *value,uint64_t number);
/**
 * Provide the test runtime parallel budget set active operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_parallel_budget_set_active(UmiTestRuntimeParallelBudget *value,bool active);
/**
 * Provide the test runtime parallel budget same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_parallel_budget_same_identity(const UmiTestRuntimeParallelBudget *left,const UmiTestRuntimeParallelBudget *right);
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
UmiStatus umi_test_runtime_parallel_budget_init_checked(UmiTestRuntimeParallelBudget *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
