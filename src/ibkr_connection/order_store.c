/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/order_store.c
 * PURPOSE: Publish complete order observations atomically and distinguish repeated status callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiIbkrOrdersStore(UmiIbkrConnection *c, const UmiIbkrRecoveredOrder *incoming,
                             const unsigned char *payload, size_t length, uint64_t now)
{
    UmiIbkrOrderStore *store = &c->orders;
    UmiIbkrOrdersObserveId(c, incoming->key.orderId);
    UmiIbkrOrdersExpire(c, now);
    if (!store->snapshot.requested || store->snapshot.failed)
        return UMI_STATUS_OK;
    if (store->snapshot.scope == UMI_IBKR_ORDERS_THIS_CLIENT &&
        incoming->key.clientId != c->options.adapter.clientId)
        return UMI_STATUS_OK;
    size_t index = store->snapshot.count;
    bool unbound = incoming->key.clientId == 0 && incoming->key.orderId == 0;
    /* Unbound manual orders can share the zero API identity. Require the
     * broker permanent identity to distinguish them without binding them. */
    if (unbound && incoming->permanentId == 0U)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < store->snapshot.count; ++i)
        if (store->rows[i].key.clientId == incoming->key.clientId &&
            store->rows[i].key.orderId == incoming->key.orderId &&
            (!unbound || store->rows[i].permanentId == incoming->permanentId))
        {
            index = i;
            break;
        }
    if (index == UMI_IBKR_ORDER_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiIbkrRecoveredOrder candidate = store->rows[index];
    /* API order IDs are scoped by client; permanent IDs are additional broker
     * identity. A conflicting nonzero permanent ID is not silently merged.
     * Different client bindings remain separate rows even with the same permId. */
    if (candidate.permanentId && incoming->permanentId && candidate.permanentId != incoming->permanentId)
        return UMI_STATUS_PARSE_ERROR;
    candidate.key = incoming->key;
    if (incoming->permanentId)
        candidate.permanentId = incoming->permanentId;
    bool duplicate = false;
    unsigned char *owned = NULL;
    if (incoming->hasOpenOrder)
    {
        duplicate = candidate.hasOpenOrder && store->lengths[index] == length &&
                    memcmp(store->payloads[index], payload, length) == 0;
        if (!duplicate)
        {
            owned = malloc(length);
            if (!owned)
                return UMI_STATUS_OUT_OF_MEMORY;
            memcpy(owned, payload, length);
        }
        candidate.open = incoming->open;
        candidate.hasOpenOrder = true;
        candidate.openReceivedAtMilliseconds = now;
    }
    else
    {
        /* Cumulative filled and remaining quantities replace the last report.
         * They are never added together across callbacks, nor are statuses
         * promoted to executions. Corrections and unknown states stay visible. */
        duplicate =
            candidate.hasStatus && candidate.status.parentId == incoming->status.parentId &&
            strcmp(candidate.status.status, incoming->status.status) == 0 &&
            strcmp(candidate.status.whyHeld, incoming->status.whyHeld) == 0 &&
            strcmp(candidate.status.filled.reportedText, incoming->status.filled.reportedText) == 0 &&
            strcmp(candidate.status.remaining.reportedText, incoming->status.remaining.reportedText) == 0 &&
            strcmp(candidate.status.averageFillPrice.reportedText,
                   incoming->status.averageFillPrice.reportedText) == 0 &&
            strcmp(candidate.status.lastFillPrice.reportedText,
                   incoming->status.lastFillPrice.reportedText) == 0 &&
            strcmp(candidate.status.marketCapPrice.reportedText,
                   incoming->status.marketCapPrice.reportedText) == 0 &&
            candidate.permanentId == store->rows[index].permanentId;
        candidate.status = incoming->status;
        candidate.hasStatus = true;
        candidate.statusReceivedAtMilliseconds = now;
        if (duplicate)
        {
            if (candidate.duplicateStatuses == UINT64_MAX)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            ++candidate.duplicateStatuses;
        }
    }
    if (!duplicate)
    {
        if (candidate.revision == UINT64_MAX || store->snapshot.revision == UINT64_MAX)
        {
            free(owned);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        ++candidate.revision;
        ++store->snapshot.revision;
    }
    /* All validation and allocation precede publication, so readers never see
     * new metadata paired with an old or partially copied payload. */
    if (owned)
    {
        free(store->payloads[index]);
        store->payloads[index] = owned;
        store->lengths[index] = length;
    }
    store->rows[index] = candidate;
    if (index == store->snapshot.count)
        ++store->snapshot.count;
    return UMI_STATUS_OK;
}
