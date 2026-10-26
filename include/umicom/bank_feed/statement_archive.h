/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_feed/statement_archive.h
 *
 * PURPOSE:
 *   Retain and reopen statement comparison assumptions with owned observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_BANK_FEED_STATEMENT_ARCHIVE_H
#define UMICOM_BANK_FEED_STATEMENT_ARCHIVE_H
#include "umicom/bank_feed/archive.h"
#include "umicom/bank_feed/statement.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES (UMI_BANK_FEED_MAX_BYTES + 4096U)
typedef struct UmiBankStatementArchive UmiBankStatementArchive;
/* Fixed owned values can be copied without retaining pointers into an archive.
 * Dates and balances are the user's assumptions, never verified provider facts. */
typedef struct UmiBankStatementArchiveSnapshot {
    char account[161], currency[4];
    UmiFinancialDate firstDate, lastDate;
    UmiDecimal openingBalance, closingBalance;
    UmiBankStatementResult result;
    UmiBankFeedArchiveInfo source;
} UmiBankStatementArchiveSnapshot;

/* Create an immutable review by copying all imported accounts and calculating
 * the selected account/currency/period through UmiBankStatementReview.
 * Mismatched balances are valid review evidence. Invalid input, absent scope
 * or arithmetic overflow refuses creation and clears *out. info may be NULL.
 * Unknown source extensions, source paths and mapping settings remain absent.
 * Serialise use of the borrowed feed during creation. No I/O or posting occurs. */
UmiStatus UmiBankStatementArchiveCreate(const UmiBankFeed *feed,
    const UmiBankStatementRequest *request, const UmiBankFeedArchiveInfo *info,
    UmiBankStatementArchive **out);
void UmiBankStatementArchiveDestroy(UmiBankStatementArchive *archive);
/* Copy the reviewed scope, exact decimal scales, result and untrusted source
 * provenance. Failure leaves out unchanged; no pointers are borrowed. */
UmiStatus UmiBankStatementArchiveRead(const UmiBankStatementArchive *archive,
    UmiBankStatementArchiveSnapshot *out);
/* Return an independent feed for an editor. Failure clears *out. */
UmiStatus UmiBankStatementArchiveCloneFeed(const UmiBankStatementArchive *archive,
    UmiBankFeed **out);

/* Encode strict versioned UTF-8 JSON with scope, canonical decimal strings and
 * the complete existing observation archive. Result flags/totals are deliberately
 * not serialised: decode recalculates them from the retained observations.
 * Decimal scales are preserved; input spacing/sign/leading zeros are not.
 * No signature or integrity guarantee is implied. Every account in the original
 * import is included, so save privately. Destroy successful bytes using
 * UmiBankFeedArchiveFree. Failure clears supplied outputs, never returns a prefix. */
UmiStatus UmiBankStatementArchiveEncode(const UmiBankStatementArchive *archive,
    char **outBytes, size_t *outLength);
/* Strict root and nested observation schemas reject missing/duplicate/unknown
 * fields, malformed dates, noncanonical decimal spelling, unsupported versions
 * and out-of-scope accounts. The input need not be NUL terminated.
 * Limits bound allocation; no file access, clock, ledger write or network call.
 * Source digests are untrusted claims, not authentication. Failure clears *out. */
UmiStatus UmiBankStatementArchiveDecode(const void *bytes, size_t length,
    UmiBankStatementArchive **out);
#ifdef __cplusplus
}
#endif
#endif
