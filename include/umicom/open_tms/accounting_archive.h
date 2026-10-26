/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/accounting_archive.h
 *
 * PURPOSE:
 *   Encode bounded local accounting batches with explicit portable fields.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_OPEN_TMS_ACCOUNTING_ARCHIVE_H
#define UMICOM_OPEN_TMS_ACCOUNTING_ARCHIVE_H
#include "umicom/open_tms/accounting_book.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Encode the active batch only: no padding, unused lines or text tails.
 * Currency syntax, bounded fields, finite values and boolean flags are checked.
 * Approved/posted batches must balance per currency; draft batches may not.
 * Imported flags/revisions are historical local values, not authenticated
 * approval or posting evidence. No I/O, encryption or external posting occurs.
 * The shared checksum detects accidental corruption, not malicious authors.
 *
 * Follow value_archive.h non-overlap/ownership rules. NULL/0 encode measures
 * required size. A short output buffer remains unchanged. Decode refusal
 * preserves the destination. Ordinary decode stages a book on the stack;
 * use the explicit workspace form for small-stack callers.
 *
 * Decode into a separate draft, review all rows, and explicitly replace local
 * state. Revoke any previously prepared action review after replacement.
 * Conditional decode compares the entire current canonical value with a
 * separate expected snapshot, then publishes atomically on the owning thread.
 * It does not enforce monotonic imported revisions or provide a transaction. */
size_t umi_open_tms_accounting_book_archive_limit(void);
UmiStatus umi_open_tms_accounting_book_archive_encode(const UmiOpenTmsAccountingBook *book,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_open_tms_accounting_book_archive_decode(const void *bytes, size_t count,
    UmiOpenTmsAccountingBook *out_book);
UmiStatus umi_open_tms_accounting_book_archive_decode_with_workspace(const void *bytes,
    size_t count, UmiOpenTmsAccountingBook *workspace, UmiOpenTmsAccountingBook *out_book);
UmiStatus umi_open_tms_accounting_book_archive_decode_if_unchanged(const void *bytes,
    size_t count, const UmiOpenTmsAccountingBook *expected, UmiOpenTmsAccountingBook *out_book);
#ifdef __cplusplus
}
#endif
#endif
