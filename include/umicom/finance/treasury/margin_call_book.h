/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/margin_call_book.h
 *
 * PURPOSE:
 *   Maintain margin calls and reject duplicate call identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_MARGIN_CALL_BOOK_H
#define UMICOM_FINANCE_TREASURY_MARGIN_CALL_BOOK_H
#include "umicom/finance/treasury/margin_call.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury margin call book data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryMarginCallBook {
    UmiTreasuryMarginCall items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryMarginCallBook;
/**
 * Initialise treasury margin call book from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_margin_call_book_init(UmiTreasuryMarginCallBook *registry);
/**
 * Add treasury margin call book only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_margin_call_book_add(UmiTreasuryMarginCallBook *registry, const UmiTreasuryMarginCall *item);
/**
 * Find treasury margin call book while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTreasuryMarginCall *umi_treasury_margin_call_book_find(const UmiTreasuryMarginCallBook *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
