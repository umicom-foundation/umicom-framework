/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/executor.h
 *
 * PURPOSE:
 *   Execute one ready completion work item through policy, cancellation and adapter boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_EXECUTOR_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_EXECUTOR_H
#include "umicom/application/productisation/execution/adapter.h"
#include "umicom/application/productisation/execution/cancellation.h"
#include "umicom/application/productisation/execution/history.h"
#include "umicom/application/productisation/execution/policy.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Execute one ready item through policy, adapter, evidence and history services. */
UmiStatus umi_product_execution_execute_item(
    UmiProductExecutionWorkQueue *queue,
    size_t item_index,
    const UmiProductExecutionPolicy *policy,
    const UmiProductExecutionAdapter *adapter,
    const UmiProductExecutionCancellation *cancellation,
    UmiProductExecutionHistory *history);
#ifdef __cplusplus
}
#endif
#endif
