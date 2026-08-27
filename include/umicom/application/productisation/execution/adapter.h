/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/adapter.h
 *
 * PURPOSE:
 *   Define host callbacks for controlled execution without embedding application side effects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_ADAPTER_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_ADAPTER_H
#include "umicom/application/productisation/execution/work_item.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef UmiStatus (*UmiProductExecutionInvoke)(
    void *context,
    const UmiProductExecutionWorkItem *item,
    char *evidence,
    size_t evidence_capacity,
    UmiProductExecutionOutcome *out_outcome);
typedef struct UmiProductExecutionAdapter {
    void *context;
    UmiProductExecutionInvoke framework;
    UmiProductExecutionInvoke application;
    UmiProductExecutionInvoke external_adapter;
    UmiProductExecutionInvoke acceptance;
} UmiProductExecutionAdapter;
UmiStatus umi_product_execution_adapter_validate(
    const UmiProductExecutionAdapter *adapter);
UmiStatus umi_product_execution_adapter_invoke(
    const UmiProductExecutionAdapter *adapter,
    const UmiProductExecutionWorkItem *item,
    char *evidence,
    size_t evidence_capacity,
    UmiProductExecutionOutcome *out_outcome);
#ifdef __cplusplus
}
#endif
#endif
