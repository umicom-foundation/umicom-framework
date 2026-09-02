/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/history.c
 *
 * PURPOSE:
 *   Implement append-only bounded completion execution history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/history.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise product execution history from caller-provided values so later operations
 * receive a known state.
 */
void umi_product_execution_history_init(UmiProductExecutionHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history != NULL) {
        (void)memset(history, 0, sizeof(*history));
        history->next_sequence = 1U;
    }
}
/*
 * Add product execution history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_product_execution_history_append(
    UmiProductExecutionHistory *history,
    size_t item_index,
    UmiProductExecutionEventKind kind,
    UmiStatus status,
    const char *detail)
{
    UmiProductExecutionEvent *event;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count >= UMI_PRODUCT_EXECUTION_MAX_EVENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    event = &history->events[history->count++];
    (void)memset(event, 0, sizeof(*event));
    event->sequence = history->next_sequence++;
    event->item_index = item_index;
    event->kind = kind;
    event->status = status;
    (void)snprintf(event->detail, sizeof(event->detail), "%s", detail);
    return UMI_STATUS_OK;
}
/*
 * Find product execution history while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiProductExecutionEvent *umi_product_execution_history_at(
    const UmiProductExecutionHistory *history, size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || index >= history->count) return NULL;
    return &history->events[index];
}
