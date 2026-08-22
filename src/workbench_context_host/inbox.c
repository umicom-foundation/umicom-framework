/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/inbox.c
 *
 * PURPOSE:
 *   Implement bounded FIFO context delivery with explicit drop accounting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/inbox.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(UmiWorkbenchContextHostInbox *inbox)
{
    UmiWorkbenchContextHostDelivery *items;
    size_t next;

    if (inbox->count < inbox->capacity) return UMI_STATUS_OK;
    if (inbox->capacity >= UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS) {
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
    if (next > UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS) {
        next = UMI_WORKBENCH_CONTEXT_HOST_MAX_INBOX_ITEMS;
    }
    items = (UmiWorkbenchContextHostDelivery *)realloc(
        inbox->items, next * sizeof(inbox->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
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

void umi_workbench_context_host_inbox_init(
    UmiWorkbenchContextHostInbox *inbox,
    const char *endpoint_id,
    const char *panel_id)
{
    if (inbox == NULL) return;
    memset(inbox, 0, sizeof(*inbox));
    inbox->revision = 1U;
    if (endpoint_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            inbox->endpoint_id, sizeof(inbox->endpoint_id), endpoint_id);
    }
    if (panel_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            inbox->panel_id, sizeof(inbox->panel_id), panel_id);
    }
}

void umi_workbench_context_host_inbox_destroy(
    UmiWorkbenchContextHostInbox *inbox)
{
    if (inbox == NULL) return;
    free(inbox->items);
    memset(inbox, 0, sizeof(*inbox));
}

UmiStatus umi_workbench_context_host_inbox_push(
    UmiWorkbenchContextHostInbox *inbox,
    const UmiWorkbenchContextHostDelivery *delivery)
{
    UmiStatus status;
    if (inbox == NULL || delivery == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_host_delivery_validate(delivery);
    if (status != UMI_STATUS_OK) return status;
    status = ensure_capacity(inbox);
    if (status != UMI_STATUS_OK) return status;
    inbox->items[inbox->count++] = *delivery;
    ++inbox->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_inbox_pop(
    UmiWorkbenchContextHostInbox *inbox,
    UmiWorkbenchContextHostDelivery *out_delivery)
{
    if (inbox == NULL || out_delivery == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (inbox->count == 0U) return UMI_STATUS_NOT_FOUND;
    *out_delivery = inbox->items[0];
    out_delivery->disposition =
        UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_CONSUMED;
    ++out_delivery->revision;
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

const UmiWorkbenchContextHostDelivery *
umi_workbench_context_host_inbox_peek(
    const UmiWorkbenchContextHostInbox *inbox)
{
    return inbox != NULL && inbox->count > 0U
        ? &inbox->items[0]
        : NULL;
}

void umi_workbench_context_host_inbox_clear(
    UmiWorkbenchContextHostInbox *inbox)
{
    if (inbox == NULL) return;
    if (inbox->items != NULL && inbox->capacity > 0U) {
        memset(
            inbox->items,
            0,
            inbox->capacity * sizeof(inbox->items[0]));
    }
    inbox->count = 0U;
    ++inbox->revision;
}
