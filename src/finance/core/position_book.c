/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/position_book.c
 *
 * PURPOSE:
 *   Implement bounded position books.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/position_book.h"

#include <string.h>
/* Reset collection. */ void umi_financial_position_book_init(UmiFinancialPositionBook *book) {
  if (book != NULL)
    memset(book, 0, sizeof *book);
}
/* Append valid item. */ UmiStatus umi_financial_position_book_add(UmiFinancialPositionBook *book,
                                                                   const UmiPosition *item) {
  if (book == NULL || item == NULL || !umi_position_is_valid(item))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (book->count >= UMI_FINANCIAL_CORE_MAX_ITEMS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  book->items[book->count++] = *item;
  return UMI_STATUS_OK;
}
/* Return item count. */ size_t
umi_financial_position_book_count(const UmiFinancialPositionBook *book) {
  return book != NULL ? book->count : 0U;
}
