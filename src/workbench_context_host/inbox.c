/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/inbox.c
 *
 * PURPOSE:
 *   Implement bounded FIFO context delivery with explicit drop accounting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/inbox.h"

#include <stdlib.h>
#include <string.h>

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiWorkbenchContextHostInbox *inbox)
{
    UmiWorkbenchContextHostDelivery *items;
    size_t next;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (inbox->count < inbox->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (inbox->capacity >= UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (inbox->count > 0U) {
            memmove(
                &inbox->items[0],
                &inbox->items[1],
                (inbox->count - 1U) * sizeof(inbox->items[0]));
            --inbox->count;
            ++inbox->dropped_count;
        }
        return UMI_STATUS_OK;
    }

    next = inbox->capacity == 0U ? 8U : inbox->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS) {
        next = UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS;
    }
    items = (UmiWorkbenchContextHostDelivery *)realloc(
        inbox->items, next * sizeof(inbox->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (next > inbox->capacity) {
        memset(
            items + inbox->capacity,
            0,
            (next - inbox->capacity) * sizeof(items[0]));
    }
    inbox->items = items;
    inbox->capacity = next;
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench context host inbox from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_host_inbox_init(
    UmiWorkbenchContextHostInbox *inbox,
    const char *endpoint_id,
    const char *panel_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL) return;
    memset(inbox, 0, sizeof(*inbox));
    inbox->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            inbox->endpoint_id, sizeof(inbox->endpoint_id), endpoint_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (panel_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            inbox->panel_id, sizeof(inbox->panel_id), panel_id);
    }
}

/*
 * Release or reset state held by workbench context host inbox so the same storage can be
 * reused safely.
 */
void umi_workbench_context_host_inbox_destroy(
    UmiWorkbenchContextHostInbox *inbox)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL) return;
    free(inbox->items);
    memset(inbox, 0, sizeof(*inbox));
}

/*
 * Provide the workbench context host inbox push operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_inbox_push(
    UmiWorkbenchContextHostInbox *inbox,
    const UmiWorkbenchContextHostDelivery *delivery)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL || delivery == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_host_delivery_validate(delivery);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = ensure_capacity(inbox);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    inbox->items[inbox->count++] = *delivery;
    ++inbox->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host inbox pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_host_inbox_pop(
    UmiWorkbenchContextHostInbox *inbox,
    UmiWorkbenchContextHostDelivery *out_delivery)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL || out_delivery == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (inbox->count == 0U) return UMI_STATUS_NOT_FOUND;
    *out_delivery = inbox->items[0];
    out_delivery->disposition =
        UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_CONSUMED;
    ++out_delivery->revision;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (inbox->count > 1U) {
        memmove(
            &inbox->items[0],
            &inbox->items[1],
            (inbox->count - 1U) * sizeof(inbox->items[0]));
    }
    --inbox->count;
    memset(&inbox->items[inbox->count], 0, sizeof(inbox->items[0]));
    ++inbox->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host inbox peek operation used by this module and its
 * client applications.
 */
const UmiWorkbenchContextHostDelivery *
umi_workbench_context_host_inbox_peek(
    const UmiWorkbenchContextHostInbox *inbox)
{
    return inbox != NULL && inbox->count > 0U
        ? &inbox->items[0]
        : NULL;
}

/*
 * Release or reset state held by workbench context host inbox so the same storage can be
 * reused safely.
 */
void umi_workbench_context_host_inbox_clear(
    UmiWorkbenchContextHostInbox *inbox)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inbox->items != NULL && inbox->capacity > 0U) {
        memset(
            inbox->items,
            0,
            inbox->capacity * sizeof(inbox->items[0]));
    }
    inbox->count = 0U;
    ++inbox->revision;
}
