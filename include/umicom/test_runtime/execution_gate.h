/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_gate.h
 *
 * PURPOSE:
 *   Prevent launch when required runtime evidence is incomplete.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_GATE
#define UMICOM_TEST_RUNTIME_EXECUTION_GATE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_test_runtime_execution_gate_init(UmiTestRuntimeExecutionGate *value,const char *id);
UmiStatus umi_test_runtime_execution_gate_validate(const UmiTestRuntimeExecutionGate *value);
UmiStatus umi_test_runtime_execution_gate_set_detail(UmiTestRuntimeExecutionGate *value,const char *detail);
UmiStatus umi_test_runtime_execution_gate_set_required_count(UmiTestRuntimeExecutionGate *value,uint64_t number);
UmiStatus umi_test_runtime_execution_gate_set_blocked_count(UmiTestRuntimeExecutionGate *value,uint64_t number);
bool umi_test_runtime_execution_gate_same_identity(const UmiTestRuntimeExecutionGate *left,const UmiTestRuntimeExecutionGate *right);
#ifdef __cplusplus
}
#endif
#endif
