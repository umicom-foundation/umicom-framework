/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/watchlist.c
 *
 * PURPOSE:
 *   Validate bounded watchlists without treating saved selections as broker observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/broker_connectivity/watchlist.h"
#include <limits.h>
#include <string.h>
static bool WatchText(const char *s, size_t size)
{
    if (!UmiIbkrText(s, size, false)) return false;
    for (size_t i = 0U; s[i]; ++i) if ((unsigned char)s[i] < 32U || s[i] == 127) return false;
    return true;
}
static bool WatchItem(const UmiIbkrWatchItem *item)
{
    return item && item->contract.contractId > 0U && item->contract.contractId <= INT32_MAX &&
        WatchText(item->contract.exchange, sizeof item->contract.exchange) &&
        WatchText(item->label, sizeof item->label);
}
UmiStatus UmiIbkrWatchlistValidate(const UmiIbkrWatchlist *list)
{
    if (!list || !WatchText(list->name, sizeof list->name)) return UMI_STATUS_INVALID_ARGUMENT;
    if (list->count > UMI_IBKR_WATCHLIST_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < list->count; ++i) {
        if (!WatchItem(&list->items[i])) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (list->items[i].contract.contractId == list->items[j].contract.contractId &&
                !strcmp(list->items[i].contract.exchange, list->items[j].contract.exchange))
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrWatchlistAdd(UmiIbkrWatchlist *list, const UmiIbkrWatchItem *item)
{
    UmiStatus status = UmiIbkrWatchlistValidate(list);
    if (status != UMI_STATUS_OK) return status;
    if (!WatchItem(item)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < list->count; ++i)
        if (list->items[i].contract.contractId == item->contract.contractId &&
            !strcmp(list->items[i].contract.exchange, item->contract.exchange))
            return UMI_STATUS_ALREADY_EXISTS;
    if (list->count == UMI_IBKR_WATCHLIST_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    list->items[list->count++] = *item;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrWatchlistRemove(UmiIbkrWatchlist *list, size_t index)
{
    UmiStatus status = UmiIbkrWatchlistValidate(list);
    if (status != UMI_STATUS_OK) return status;
    if (index >= list->count) return UMI_STATUS_NOT_FOUND;
    /* Shift only owned values. No provider cancellation or order is implied. */
    memmove(&list->items[index], &list->items[index + 1U],
        (list->count - index - 1U) * sizeof list->items[0]);
    memset(&list->items[--list->count], 0, sizeof list->items[0]);
    return UMI_STATUS_OK;
}
