/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_call_book.c
 *
 * PURPOSE:
 *   Implement maintain margin calls and reject duplicate call identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_call_book.h"
#include <string.h>
/*
 * Initialise treasury margin call book from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_margin_call_book_init(UmiTreasuryMarginCallBook *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) memset(registry,0,sizeof *registry); }
/*
 * Add treasury margin call book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_margin_call_book_add(UmiTreasuryMarginCallBook *registry, const UmiTreasuryMarginCall *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || !umi_treasury_margin_call_valid(item)) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_treasury_margin_call_book_find(registry, item->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_TREASURY_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *item;
    return UMI_STATUS_OK;
}
/*
 * Find treasury margin call book while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTreasuryMarginCall *umi_treasury_margin_call_book_find(const UmiTreasuryMarginCallBook *registry, const char *id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (size_t i=0U;i<registry->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strncmp(registry->items[i].id, id, UMI_TREASURY_ID_CAPACITY)==0) return &registry->items[i];
    return NULL;
}
