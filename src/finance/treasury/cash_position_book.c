/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/cash_position_book.c
 *
 * PURPOSE:
 *   Implement maintain bounded cash positions with deterministic identifier lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_position_book.h"
#include <string.h>
void umi_treasury_cash_position_book_init(UmiTreasuryCashPositionBook *registry) { if (registry != NULL) memset(registry,0,sizeof *registry); }
UmiStatus umi_treasury_cash_position_book_add(UmiTreasuryCashPositionBook *registry, const UmiTreasuryCashPosition *item) {
    if (registry == NULL || item == NULL || !umi_treasury_cash_position_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_treasury_cash_position_book_find(registry, item->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_TREASURY_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *item;
    return UMI_STATUS_OK;
}
const UmiTreasuryCashPosition *umi_treasury_cash_position_book_find(const UmiTreasuryCashPositionBook *registry, const char *id) {
    if (registry == NULL || id == NULL) return NULL;
    for (size_t i=0U;i<registry->count;++i) if (strncmp(registry->items[i].id, id, UMI_TREASURY_ID_CAPACITY)==0) return &registry->items[i];
    return NULL;
}
