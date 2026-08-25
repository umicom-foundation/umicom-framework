/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/margin_call_book.h
 *
 * PURPOSE:
 *   Maintain margin calls and reject duplicate call identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARGIN_CALL_BOOK_H
#define UMICOM_FINANCE_TREASURY_MARGIN_CALL_BOOK_H
#include "umicom/finance/treasury/margin_call.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryMarginCallBook {
    UmiTreasuryMarginCall items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryMarginCallBook;
void umi_treasury_margin_call_book_init(UmiTreasuryMarginCallBook *registry);
UmiStatus umi_treasury_margin_call_book_add(UmiTreasuryMarginCallBook *registry, const UmiTreasuryMarginCall *item);
const UmiTreasuryMarginCall *umi_treasury_margin_call_book_find(const UmiTreasuryMarginCallBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
