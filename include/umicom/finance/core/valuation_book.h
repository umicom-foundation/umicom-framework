/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/valuation_book.h
 *
 * PURPOSE:
 *   Provide bounded valuation books.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_VALUATION_BOOK_H
#define UMICOM_FINANCE_CORE_VALUATION_BOOK_H

#include "umicom/finance/core/valuation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the valuation book data shared with callers of this public contract.
 */
typedef struct UmiValuationBook { UmiValuation items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiValuationBook;
/* Reset collection. */ void umi_valuation_book_init(UmiValuationBook *c);
/* Append valid item. */ UmiStatus umi_valuation_book_add(UmiValuationBook *c,const UmiValuation *item);
/* Return item count. */ size_t umi_valuation_book_count(const UmiValuationBook *c);
#ifdef __cplusplus
}
#endif

#endif
