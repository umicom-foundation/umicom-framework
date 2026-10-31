/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/watchlist.h
 *
 * PURPOSE:
 *   Keep named broker instrument selections separate from live observations and trading authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_WATCHLIST_H
#define UMICOM_BROKER_WATCHLIST_H
#include "umicom/broker_connectivity/quotes.h"
#define UMI_IBKR_WATCHLIST_LIMIT 64U
#ifdef __cplusplus
extern "C" {
#endif
/* A saved row preserves the exact provider identifier and route. It deliberately
 * carries no cash, quote, entitlement, password, or execution permission. */
typedef struct UmiIbkrWatchItem {
    UmiIbkrQuoteContract contract;
    char label[128];
} UmiIbkrWatchItem;
typedef struct UmiIbkrWatchlist {
    char name[128];
    size_t count;
    UmiIbkrWatchItem items[UMI_IBKR_WATCHLIST_LIMIT];
} UmiIbkrWatchlist;
UmiStatus UmiIbkrWatchlistValidate(const UmiIbkrWatchlist *list);
UmiStatus UmiIbkrWatchlistAdd(UmiIbkrWatchlist *list, const UmiIbkrWatchItem *item);
UmiStatus UmiIbkrWatchlistRemove(UmiIbkrWatchlist *list, size_t index);
#ifdef __cplusplus
}
#endif
#endif
