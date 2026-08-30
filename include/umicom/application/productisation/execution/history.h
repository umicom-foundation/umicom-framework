/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/history.h
 *
 * PURPOSE:
 *   Capture a bounded audit history of completion execution transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_HISTORY_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_HISTORY_H
#include "umicom/application/productisation/execution/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionEvent {
    uint64_t sequence;
    size_t item_index;
    UmiProductExecutionEventKind kind;
    UmiStatus status;
    char detail[UMI_PRODUCT_EXECUTION_DETAIL_CAPACITY];
} UmiProductExecutionEvent;
typedef struct UmiProductExecutionHistory {
    UmiProductExecutionEvent events[UMI_PRODUCT_EXECUTION_MAX_EVENTS];
    size_t count;
    uint64_t next_sequence;
} UmiProductExecutionHistory;
void umi_product_execution_history_init(UmiProductExecutionHistory *history);
UmiStatus umi_product_execution_history_append(
    UmiProductExecutionHistory *history,
    size_t item_index,
    UmiProductExecutionEventKind kind,
    UmiStatus status,
    const char *detail);
const UmiProductExecutionEvent *umi_product_execution_history_at(
    const UmiProductExecutionHistory *history, size_t index);
#ifdef __cplusplus
}
#endif
#endif
