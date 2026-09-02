/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_gate.h
 *
 * PURPOSE:
 *   Prevent launch when required runtime evidence is incomplete.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_GATE
#define UMICOM_TEST_RUNTIME_EXECUTION_GATE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime execution gate data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeExecutionGate
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required_count;
    uint64_t blocked_count;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeExecutionGate;
/**
 * Initialise test runtime execution gate from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_execution_gate_init(UmiTestRuntimeExecutionGate *value,const char *id);
/**
 * Check that test runtime execution gate satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_execution_gate_validate(const UmiTestRuntimeExecutionGate *value);
/**
 * Provide the test runtime execution gate set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_execution_gate_set_detail(UmiTestRuntimeExecutionGate *value,const char *detail);
/**
 * Return the number of records represented by test runtime execution gate set required
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_gate_set_required_count(UmiTestRuntimeExecutionGate *value,uint64_t number);
/**
 * Return the number of records represented by test runtime execution gate set blocked
 * without changing their state.
 */
UmiStatus umi_test_runtime_execution_gate_set_blocked_count(UmiTestRuntimeExecutionGate *value,uint64_t number);
/**
 * Provide the test runtime execution gate same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_execution_gate_same_identity(const UmiTestRuntimeExecutionGate *left,const UmiTestRuntimeExecutionGate *right);
#ifdef __cplusplus
}
#endif
#endif
