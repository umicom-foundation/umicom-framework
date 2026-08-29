/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/position_book.h
 *
 * PURPOSE:
 *   Provide bounded position books.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_POSITION_BOOK_H
#define UMICOM_FINANCE_CORE_POSITION_BOOK_H

#include "umicom/finance/core/position.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinancialPositionBook {
  UmiPosition items[UMI_FINANCIAL_CORE_MAX_ITEMS];
  size_t count;
} UmiFinancialPositionBook;
/* Reset collection. */ void umi_financial_position_book_init(UmiFinancialPositionBook *book);
/* Append valid item. */ UmiStatus umi_financial_position_book_add(UmiFinancialPositionBook *book,
                                                                   const UmiPosition *item);
/* Return item count. */ size_t
umi_financial_position_book_count(const UmiFinancialPositionBook *book);
#ifdef __cplusplus
}
#endif

#endif
