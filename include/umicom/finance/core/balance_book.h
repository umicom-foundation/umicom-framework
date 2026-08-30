/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/balance_book.h
 *
 * PURPOSE:
 *   Provide bounded balance books.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_BALANCE_BOOK_H
#define UMICOM_FINANCE_CORE_BALANCE_BOOK_H

#include "umicom/finance/core/balance.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBalanceBook { UmiFinancialBalance items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiBalanceBook;
/* Reset collection. */ void umi_balance_book_init(UmiBalanceBook *c);
/* Append valid item. */ UmiStatus umi_balance_book_add(UmiBalanceBook *c,const UmiFinancialBalance *item);
/* Return item count. */ size_t umi_balance_book_count(const UmiBalanceBook *c);
#ifdef __cplusplus
}
#endif

#endif
