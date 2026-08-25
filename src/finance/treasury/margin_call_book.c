/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_call_book.c
 *
 * PURPOSE:
 *   Implement maintain margin calls and reject duplicate call identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_call_book.h"
#include <string.h>
void umi_treasury_margin_call_book_init(UmiTreasuryMarginCallBook *registry) { if (registry != NULL) memset(registry,0,sizeof *registry); }
UmiStatus umi_treasury_margin_call_book_add(UmiTreasuryMarginCallBook *registry, const UmiTreasuryMarginCall *item) {
    if (registry == NULL || item == NULL || !umi_treasury_margin_call_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_treasury_margin_call_book_find(registry, item->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_TREASURY_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *item;
    return UMI_STATUS_OK;
}
const UmiTreasuryMarginCall *umi_treasury_margin_call_book_find(const UmiTreasuryMarginCallBook *registry, const char *id) {
    if (registry == NULL || id == NULL) return NULL;
    for (size_t i=0U;i<registry->count;++i) if (strncmp(registry->items[i].id, id, UMI_TREASURY_ID_CAPACITY)==0) return &registry->items[i];
    return NULL;
}
