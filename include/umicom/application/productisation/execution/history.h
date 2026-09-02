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
/**
 * Represent the product execution event data shared with callers of this public contract.
 */
typedef struct UmiProductExecutionEvent {
    uint64_t sequence;
    size_t item_index;
    UmiProductExecutionEventKind kind;
    UmiStatus status;
    char detail[UMI_PRODUCT_EXECUTION_DETAIL_CAPACITY];
} UmiProductExecutionEvent;
/**
 * Represent the product execution history data shared with callers of this public
 * contract.
 */
typedef struct UmiProductExecutionHistory {
    UmiProductExecutionEvent events[UMI_PRODUCT_EXECUTION_MAX_EVENTS];
    size_t count;
    uint64_t next_sequence;
} UmiProductExecutionHistory;
/** Clear the bounded event history before execution starts. */
void umi_product_execution_history_init(UmiProductExecutionHistory *history);
/** Append one ordered event with time, item identity, state and detail. */
UmiStatus umi_product_execution_history_append(
    UmiProductExecutionHistory *history,
    size_t item_index,
    UmiProductExecutionEventKind kind,
    UmiStatus status,
    const char *detail);
/** Borrow an event by index, or return NULL when the index is invalid. */
const UmiProductExecutionEvent *umi_product_execution_history_at(
    const UmiProductExecutionHistory *history, size_t index);
#ifdef __cplusplus
}
#endif
#endif
