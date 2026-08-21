/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/execution_order.h
 *
 * PURPOSE:
 *   Retain deterministic execution order independently of parallel scheduling.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTION_ORDER
#define UMICOM_TEST_RUNTIME_EXECUTION_ORDER
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_test_runtime_execution_order_init(UmiTestRuntimeExecutionOrder *value,const char *id);
UmiStatus umi_test_runtime_execution_order_validate(const UmiTestRuntimeExecutionOrder *value);
UmiStatus umi_test_runtime_execution_order_set_detail(UmiTestRuntimeExecutionOrder *value,const char *detail);
UmiStatus umi_test_runtime_execution_order_set_position(UmiTestRuntimeExecutionOrder *value,uint64_t number);
UmiStatus umi_test_runtime_execution_order_set_total(UmiTestRuntimeExecutionOrder *value,uint64_t number);
bool umi_test_runtime_execution_order_same_identity(const UmiTestRuntimeExecutionOrder *left,const UmiTestRuntimeExecutionOrder *right);
#ifdef __cplusplus
}
#endif
#endif
