/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward_archive.c
 *
 * PURPOSE:
 *   Bind FX-forward books to the existing checked portable value archive.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_archive.h"
#include "../base/value_archive_internal.h"
#include "../base/conditional_archive_internal.h"

/* Field serialization reuses the Framework envelope, integer byte order,
 * finite binary64 contract and atomic decode. The trade owner remains the
 * domain validator; a second, weaker persistence admission rule is not added. */
static uint64_t ForwardArchiveSchema(void) {
    uint64_t schema = UINT64_C(0x9f3e1c1460d2a73b);
    schema = (schema ^ UMI_OPEN_TMS_FX_FORWARD_CAPACITY) * UINT64_C(1099511628211);
    schema = (schema ^ sizeof(((UmiOpenTmsFxForward *)0)->tradeId)) * UINT64_C(1099511628211);
    schema = (schema ^ sizeof(((UmiOpenTmsFxForward *)0)->accountId)) * UINT64_C(1099511628211);
    schema = (schema ^ sizeof(((UmiOpenTmsFxForward *)0)->counterpartyId)) * UINT64_C(1099511628211);
    schema = (schema ^ sizeof(((UmiOpenTmsFxForward *)0)->baseCurrency)) * UINT64_C(1099511628211);
    schema = (schema ^ sizeof(((UmiOpenTmsFxForward *)0)->quoteCurrency)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ForwardArchiveBound(void) {
    /* Five length-prefixed strings plus six 8-byte numeric fields per row.
     * Bound includes maximum string lengths, never structure padding. */
    return UMI_VALUE_ARCHIVE_HEADER_SIZE + 16U + UMI_OPEN_TMS_FX_FORWARD_CAPACITY *
        (40U + sizeof(((UmiOpenTmsFxForward *)0)->tradeId) - 1U +
         sizeof(((UmiOpenTmsFxForward *)0)->accountId) - 1U +
         sizeof(((UmiOpenTmsFxForward *)0)->counterpartyId) - 1U +
         sizeof(((UmiOpenTmsFxForward *)0)->baseCurrency) - 1U +
         sizeof(((UmiOpenTmsFxForward *)0)->quoteCurrency) - 1U + 48U);
}
size_t umi_open_tms_fx_forward_book_archive_limit(void) {
    return ForwardArchiveBound();
}
static void ForwardArchiveWrite(UmiArchiveWriter *writer, const UmiOpenTmsFxForwardBook *book) {
    UmiArchiveWriteUnsigned(writer, book->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)book->count);
    for (size_t i = 0U; i < book->count && writer->status == UMI_STATUS_OK; ++i) {
        const UmiOpenTmsFxForward *trade = &book->trades[i];
        UmiArchiveWriteText(writer, trade->tradeId, sizeof trade->tradeId);
        UmiArchiveWriteText(writer, trade->accountId, sizeof trade->accountId);
        UmiArchiveWriteText(writer, trade->counterpartyId, sizeof trade->counterpartyId);
        UmiArchiveWriteText(writer, trade->baseCurrency, sizeof trade->baseCurrency);
        UmiArchiveWriteText(writer, trade->quoteCurrency, sizeof trade->quoteCurrency);
        UmiArchiveWriteDouble(writer, trade->baseNotional);
        UmiArchiveWriteDouble(writer, trade->forwardRate);
        UmiArchiveWriteDouble(writer, trade->marketForwardRate);
        UmiArchiveWriteUnsigned(writer, trade->maturityMilliseconds);
        UmiArchiveWriteUnsigned(writer, (uint64_t)trade->state);
        UmiArchiveWriteUnsigned(writer, trade->version);
    }
}
static void ForwardArchiveRead(UmiArchiveReader *reader, UmiOpenTmsFxForwardBook *book) {
    book->revision = UmiArchiveReadUnsigned(reader, UINT64_MAX);
    book->count = (size_t)UmiArchiveReadUnsigned(reader, UMI_OPEN_TMS_FX_FORWARD_CAPACITY);
    for (size_t i = 0U; i < book->count && reader->status == UMI_STATUS_OK; ++i) {
        UmiOpenTmsFxForward *trade = &book->trades[i];
        UmiArchiveReadText(reader, trade->tradeId, sizeof trade->tradeId);
        UmiArchiveReadText(reader, trade->accountId, sizeof trade->accountId);
        UmiArchiveReadText(reader, trade->counterpartyId, sizeof trade->counterpartyId);
        UmiArchiveReadText(reader, trade->baseCurrency, sizeof trade->baseCurrency);
        UmiArchiveReadText(reader, trade->quoteCurrency, sizeof trade->quoteCurrency);
        trade->baseNotional = UmiArchiveReadDouble(reader);
        trade->forwardRate = UmiArchiveReadDouble(reader);
        trade->marketForwardRate = UmiArchiveReadDouble(reader);
        trade->maturityMilliseconds = UmiArchiveReadUnsigned(reader, UINT64_MAX);
        trade->state = (UmiOpenTmsFxForwardState)UmiArchiveReadUnsigned(reader, UMI_OPEN_TMS_FX_FORWARD_CANCELLED);
        trade->version = UmiArchiveReadUnsigned(reader, UINT64_MAX);
    }
}
UMI_DEFINE_VALUE_ARCHIVE(umi_open_tms_fx_forward_book_archive_encode,
    umi_open_tms_fx_forward_book_archive_decode, UmiOpenTmsFxForwardBook,
    ForwardArchiveSchema, ForwardArchiveBound, ForwardArchiveWrite, ForwardArchiveRead,
    umi_open_tms_fx_forward_book_validate)

UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_open_tms_fx_forward_book_archive_decode_with_workspace,
    UmiOpenTmsFxForwardBook, ForwardArchiveSchema, ForwardArchiveBound, ForwardArchiveRead,
    umi_open_tms_fx_forward_book_validate)

UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_open_tms_fx_forward_book_archive_decode_if_unchanged,
    umi_open_tms_fx_forward_book_archive_encode, UmiOpenTmsFxForwardBook,
    ForwardArchiveSchema, ForwardArchiveBound, ForwardArchiveRead,
    umi_open_tms_fx_forward_book_validate)
