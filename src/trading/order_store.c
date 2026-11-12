/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_store.c
 *
 * PURPOSE:
 *   Store a bounded set of orders for deterministic simulations and tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of order store. Product-specific UI and vendor details stay outside this file.
 */

#include <string.h>
#include "umicom/trading/order_store.h"
#include "umicom/finance/identifier.h"
#include "umicom/base/text.h"
#include "../base/storage_internal.h"

/*
 * Initialise order store from caller-provided values so later operations receive a known
 * state.
 */
void umi_order_store_init(UmiOrderStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) {
        store->count = 0U;
    }
}

/* Add order store only after its inputs and available capacity have been checked. */
/* Validate the complete active identifier set before duplicate lookup or append. The shared admission contract replaces early-match acceptance and retains the previous implementation for review. The previous implementation is retained for review. */
#if 0
UmiStatus umi_order_store_add(UmiOrderStore *store, const UmiOrder *order)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || order == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (store->count > UMI_TRADING_MAX_ORDERS) return UMI_STATUS_INVALID_STATE;
    if (!umi_financial_id_valid(&order->request.client_order_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < store->count; ++index) {
        if (!umi_financial_id_valid(&store->orders[index].request.client_order_id))
            return UMI_STATUS_INVALID_STATE;
        if (umi_financial_id_equal(&store->orders[index].request.client_order_id,
                                   &order->request.client_order_id))
            return UMI_STATUS_ALREADY_EXISTS;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_TRADING_MAX_ORDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    store->orders[store->count] = *order;
    store->count++;
    return UMI_STATUS_OK;
}
#endif
/* Storage admission is deliberately narrower than order submission: identifiers
 * and uniqueness make lookup safe, while request/risk/lifecycle policy remains
 * in its existing owners. Inspect every active row before returning a match. */
UmiStatus umi_order_store_validate(const UmiOrderStore *store)
{
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (store->count > UMI_TRADING_MAX_ORDERS) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < store->count; ++index) {
        const UmiFinancialId *id = &store->orders[index].request.client_order_id;
        if (!umi_financial_id_valid(id)) return UMI_STATUS_INVALID_STATE;
        for (size_t prior = 0U; prior < index; ++prior)
            if (umi_financial_id_equal(id, &store->orders[prior].request.client_order_id))
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* Copy query bytes before writing a caller's output. This also permits a
 * query borrowed from that separate output allocation. The shared text helper
 * bounds the scan to the declared identifier capacity. */
static UmiStatus OrderStoreIndex(const UmiOrderStore *store, const char *text,
                                size_t *out_index)
{
    UmiFinancialId query = {0};
    UmiStatus status = umi_text_replace(query.value, sizeof(query.value), text);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_financial_id_valid(&query)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_order_store_validate(store);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < store->count; ++index) {
        if (umi_financial_id_equal(&query, &store->orders[index].request.client_order_id)) {
            *out_index = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_order_store_add(UmiOrderStore *store, const UmiOrder *order)
{
    if (store == NULL || order == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_order_store_validate(store);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_financial_id_valid(&order->request.client_order_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < store->count; ++index)
        if (umi_financial_id_equal(&store->orders[index].request.client_order_id,
                                   &order->request.client_order_id))
            return UMI_STATUS_ALREADY_EXISTS;
    if (store->count == UMI_TRADING_MAX_ORDERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiOrder staged = *order;
    store->orders[store->count] = staged;
    store->count += 1U;
    return UMI_STATUS_OK;
}


/* Find order store while leaving the underlying catalogue or model owned by this module. */
/* Use bounded shared query admission and protect the owner from overlapping output storage. Mutable and const callers share one search; the old direct strcmp loop is retained for review. The previous implementation is retained for review. */
#if 0
UmiStatus umi_order_store_find(UmiOrderStore *store,
                               const char *client_order_id,
                               UmiOrder **out_order)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || client_order_id == NULL || out_order == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_order = NULL;
    if (store->count > UMI_TRADING_MAX_ORDERS) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (size_t index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!umi_financial_id_valid(&store->orders[index].request.client_order_id))
            return UMI_STATUS_INVALID_STATE;
        if (strcmp(store->orders[index].request.client_order_id.value,
                   client_order_id) == 0) {
            *out_order = &store->orders[index];
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
#endif
UmiStatus umi_order_store_find(UmiOrderStore *store, const char *client_order_id,
                               UmiOrder **out_order)
{
    if (store == NULL || client_order_id == NULL || out_order == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiStorageOverlaps(store, sizeof(*store), out_order, sizeof(*out_order)))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = 0U;
    const UmiStatus status = OrderStoreIndex(store, client_order_id, &index);
    /* Preserve the established NULL-on-lookup-failure contract, without ever
     * clearing pointer bytes inside the store itself. */
    *out_order = status == UMI_STATUS_OK ? &store->orders[index] : NULL;
    return status;
}

UmiStatus umi_order_store_find_const(const UmiOrderStore *store,
                                     const char *client_order_id,
                                     const UmiOrder **out_order)
{
    if (store == NULL || client_order_id == NULL || out_order == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiStorageOverlaps(store, sizeof(*store), out_order, sizeof(*out_order)))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = 0U;
    const UmiStatus status = OrderStoreIndex(store, client_order_id, &index);
    *out_order = status == UMI_STATUS_OK ? &store->orders[index] : NULL;
    return status;
}

