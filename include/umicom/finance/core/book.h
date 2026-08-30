/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/book.h
 *
 * PURPOSE:
 *   Define legal-entity-owned financial books.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_BOOK_H
#define UMICOM_FINANCE_CORE_BOOK_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinancialBook { UmiFinancialId book_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; bool active; } UmiFinancialBook;
/* Initialize the typed financial record. */ UmiStatus umi_book_init(UmiFinancialBook *item,const char *id,const char *name,const char *parent_id);
/* Validate the typed financial record. */ bool umi_book_is_valid(const UmiFinancialBook *item);
#ifdef __cplusplus
}
#endif

#endif
