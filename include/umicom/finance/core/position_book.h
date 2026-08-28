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
typedef struct UmiPositionBook { UmiPosition items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiPositionBook;
/* Reset collection. */ void umi_position_book_init(UmiPositionBook *c);
/* Append valid item. */ UmiStatus umi_position_book_add(UmiPositionBook *c,const UmiPosition *item);
/* Return item count. */ size_t umi_position_book_count(const UmiPositionBook *c);
#ifdef __cplusplus
}
#endif

#endif
