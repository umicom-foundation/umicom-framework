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
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the financial book data shared with callers of this public contract.
 */
typedef struct UmiFinancialBook { UmiFinancialId book_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; bool active; } UmiFinancialBook;
/* Initialize the typed financial record. */ UmiStatus umi_book_init(UmiFinancialBook *item,const char *id,const char *name,const char *parent_id);
/* Validate the typed financial record. */ bool umi_book_is_valid(const UmiFinancialBook *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_book_archive_encode(const UmiFinancialBook *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_book_archive_decode(const void *bytes, size_t byte_count,
    UmiFinancialBook *value);

#ifdef __cplusplus
}
#endif

#endif
