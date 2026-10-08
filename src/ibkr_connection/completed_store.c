/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/completed_store.c
 * PURPOSE: Retain one consistent completed record per broker permanent identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiIbkrCompletedStoreRow(UmiIbkrConnection *c, const UmiIbkrCompletedOrder *row,
                                   const unsigned char *body, size_t length, uint64_t now)
{
    UmiIbkrCompletedStore *store = &c->completed;
    for (size_t i = 0U; i < store->snapshot.count; ++i)
    {
        if (store->rows[i].permanentId != row->permanentId)
            continue;
        /* A repeated completion must agree with all retained fields, not only
         * the displayed prefix. Keep the first evidence on conflict; an
         * ambiguous correction must not silently replace a final record. */
        if (store->lengths[i] != length || memcmp(store->payloads[i], body, length))
            return UMI_STATUS_PARSE_ERROR;
        if (store->rows[i].duplicateCount == UINT64_MAX)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        ++store->rows[i].duplicateCount;
        return UMI_STATUS_OK;
    }
    if (store->snapshot.count == UMI_IBKR_COMPLETED_ORDER_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char *owned = malloc(length);
    if (!owned)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(owned, body, length);
    size_t index = store->snapshot.count;
    store->rows[index] = *row;
    store->rows[index].receivedAtMilliseconds = now;
    store->payloads[index] = owned;
    store->lengths[index] = length;
    ++store->snapshot.count;
    return UMI_STATUS_OK;
}
