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

void umi_order_store_init(UmiOrderStore *store)
{
    if (store != NULL) {
        store->count = 0U;
    }
}

UmiStatus umi_order_store_add(UmiOrderStore *store, const UmiOrder *order)
{
    if (store == NULL || order == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (store->count >= UMI_TRADING_MAX_ORDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    store->orders[store->count] = *order;
    store->count++;
    return UMI_STATUS_OK;
}

UmiStatus umi_order_store_find(UmiOrderStore *store,
                               const char *client_order_id,
                               UmiOrder **out_order)
{
    if (store == NULL || client_order_id == NULL || out_order == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_order = NULL;
    for (size_t index = 0U; index < store->count; ++index) {
        if (strcmp(store->orders[index].request.client_order_id.value,
                   client_order_id) == 0) {
            *out_order = &store->orders[index];
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
