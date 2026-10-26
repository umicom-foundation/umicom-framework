/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/accounting_archive.c
 *
 * PURPOSE:
 *   Reuse the checked value envelope for local accounting batches and atomic replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/accounting_archive.h"
#include "../base/value_archive_internal.h"
#include "../base/conditional_archive_internal.h"

static uint64_t AccountingArchiveSchema(void) {
    uint64_t schema = UINT64_C(0xc2b94069ed738af1);
    schema = (schema ^ UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY) * UINT64_C(1099511628211);
    schema = (schema ^ sizeof(((UmiOpenTmsAccountingLine *)0)->accountId)) * UINT64_C(1099511628211);
    schema = (schema ^ sizeof(((UmiOpenTmsAccountingLine *)0)->currency)) * UINT64_C(1099511628211);
    schema = (schema ^ sizeof(((UmiOpenTmsAccountingLine *)0)->referenceId)) * UINT64_C(1099511628211);
    return schema;
}
static size_t AccountingArchiveBound(void) {
    /* Four owner scalars, then three length-prefixed fields and two binary64
     * amounts per row. Structure padding never enters this representation. */
    return UMI_VALUE_ARCHIVE_HEADER_SIZE + 32U + UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY *
        (24U + sizeof(((UmiOpenTmsAccountingLine *)0)->accountId) - 1U +
         sizeof(((UmiOpenTmsAccountingLine *)0)->currency) - 1U +
         sizeof(((UmiOpenTmsAccountingLine *)0)->referenceId) - 1U + 16U);
}
size_t umi_open_tms_accounting_book_archive_limit(void) {
    return AccountingArchiveBound();
}
static UmiStatus AccountingArchiveValidate(const UmiOpenTmsAccountingBook *book) {
    UmiStatus status = umi_open_tms_accounting_book_validate(book);
    if (status == UMI_STATUS_OK && book->approved)
        status = umi_open_tms_accounting_book_check_balance(book);
    return status;
}
static void AccountingArchiveWrite(UmiArchiveWriter *writer, const UmiOpenTmsAccountingBook *book) {
    UmiArchiveWriteUnsigned(writer, book->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)book->count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)book->approved);
    UmiArchiveWriteUnsigned(writer, (uint64_t)book->posted);
    for (size_t i = 0U; i < book->count && writer->status == UMI_STATUS_OK; ++i) {
        const UmiOpenTmsAccountingLine *line = &book->lines[i];
        UmiArchiveWriteText(writer, line->accountId, sizeof line->accountId);
        UmiArchiveWriteText(writer, line->currency, sizeof line->currency);
        UmiArchiveWriteText(writer, line->referenceId, sizeof line->referenceId);
        UmiArchiveWriteDouble(writer, line->debit);
        UmiArchiveWriteDouble(writer, line->credit);
    }
}
static void AccountingArchiveRead(UmiArchiveReader *reader, UmiOpenTmsAccountingBook *book) {
    book->revision = UmiArchiveReadUnsigned(reader, UINT64_MAX);
    book->count = (size_t)UmiArchiveReadUnsigned(reader, UMI_OPEN_TMS_ACCOUNTING_LINE_CAPACITY);
    book->approved = (int)UmiArchiveReadUnsigned(reader, 1U);
    book->posted = (int)UmiArchiveReadUnsigned(reader, 1U);
    for (size_t i = 0U; i < book->count && reader->status == UMI_STATUS_OK; ++i) {
        UmiOpenTmsAccountingLine *line = &book->lines[i];
        UmiArchiveReadText(reader, line->accountId, sizeof line->accountId);
        UmiArchiveReadText(reader, line->currency, sizeof line->currency);
        UmiArchiveReadText(reader, line->referenceId, sizeof line->referenceId);
        line->debit = UmiArchiveReadDouble(reader);
        line->credit = UmiArchiveReadDouble(reader);
    }
}
UMI_DEFINE_VALUE_ARCHIVE(umi_open_tms_accounting_book_archive_encode,
    umi_open_tms_accounting_book_archive_decode, UmiOpenTmsAccountingBook,
    AccountingArchiveSchema, AccountingArchiveBound, AccountingArchiveWrite, AccountingArchiveRead,
    AccountingArchiveValidate)
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_open_tms_accounting_book_archive_decode_with_workspace,
    UmiOpenTmsAccountingBook, AccountingArchiveSchema, AccountingArchiveBound, AccountingArchiveRead,
    AccountingArchiveValidate)
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_open_tms_accounting_book_archive_decode_if_unchanged,
    umi_open_tms_accounting_book_archive_encode, UmiOpenTmsAccountingBook,
    AccountingArchiveSchema, AccountingArchiveBound, AccountingArchiveRead, AccountingArchiveValidate)
