/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_order.h
 *
 * PURPOSE:
 *   Retain deterministic execution order independently of parallel scheduling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_ORDER
#define UMICOM_TEST_RUNTIME_EXECUTION_ORDER
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime execution order data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeExecutionOrder
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t position;
    uint64_t total;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeExecutionOrder;
/**
 * Initialise test runtime execution order from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_execution_order_init(UmiTestRuntimeExecutionOrder *value,const char *id);
/**
 * Check that test runtime execution order satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_execution_order_validate(const UmiTestRuntimeExecutionOrder *value);
/**
 * Provide the test runtime execution order set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_order_set_detail(UmiTestRuntimeExecutionOrder *value,const char *detail);
/**
 * Provide the test runtime execution order set position operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_execution_order_set_position(UmiTestRuntimeExecutionOrder *value,uint64_t number);
/**
 * Provide the test runtime execution order set total operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_execution_order_set_total(UmiTestRuntimeExecutionOrder *value,uint64_t number);
/**
 * Provide the test runtime execution order same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_execution_order_same_identity(const UmiTestRuntimeExecutionOrder *left,const UmiTestRuntimeExecutionOrder *right);
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
UmiStatus umi_test_runtime_execution_order_init_checked(UmiTestRuntimeExecutionOrder *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
