/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_feed/archive.h
 * PURPOSE: Reopen owned bank observations while retaining explicit source and pagination limits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_BANK_FEED_ARCHIVE_H
#define UMICOM_BANK_FEED_ARCHIVE_H
#include "umicom/bank_feed/transactions.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiBankFeedSourceFormat {
    UMI_BANK_FEED_SOURCE_UNKNOWN = 0,
    UMI_BANK_FEED_SOURCE_OPEN_BANKING_JSON = 1,
    UMI_BANK_FEED_SOURCE_STATEMENT_CSV = 2,
    UMI_BANK_FEED_SOURCE_MAPPED_CSV = 3
} UmiBankFeedSourceFormat;
typedef struct UmiBankFeedArchiveInfo {
    UmiBankFeedSourceFormat sourceFormat;
    char sourceSha256[65];
} UmiBankFeedArchiveInfo;
/* Validate only the source claim (known format and empty/lowercase SHA-256).
 * This does not read/encode a feed or establish source authenticity. */
UmiStatus UmiBankFeedArchiveInfoValidate(const UmiBankFeedArchiveInfo *info);

/* Encode an owned, NUL-terminated UTF-8 JSON snapshot of retained observations.
 * Preserves row order, identifiers, exact amount spelling, timestamp offsets,
 * multiline information, pending/mutable flags and page hints. Unknown provider
 * extensions and page URLs were discarded during import and cannot be restored.
 * info may be NULL (unknown source). Its digest is empty or 64 lowercase hex
 * characters describing the original input bytes; it is caller-supplied evidence,
 * not authentication. Mapping settings and source paths are not stored.
 * Maximum encoded bytes, excluding NUL, is UMI_BANK_FEED_MAX_BYTES. Escaping
 * can exceed this bound even when the input feed fits; no partial result escapes.
 * Failure clears supplied outputs. No file, network or ledger operation occurs.
 * Calls on the borrowed feed must be serialised with other uses/destruction. */
UmiStatus UmiBankFeedArchiveEncode(const UmiBankFeed *feed,
    const UmiBankFeedArchiveInfo *info, char **outBytes, size_t *outLength);
void UmiBankFeedArchiveFree(char *bytes);
/* Decode only the versioned Umicom observation format. Strict root/row schemas
 * reject duplicate, missing and unknown fields. Financial rows use the shared
 * amount and timestamp rules. Missing IDs remain separate observations; duplicate
 * nonempty IDs in an account are rejected. Pending rows must remain mutable.
 * Exact validated bytes are copied into a new feed; input can then be released.
 * Failure clears *out and leaves optional outInfo unchanged. Source digests in
 * an archive are untrusted claims until compared with independently kept input.
 * The same byte/row limits as bank import apply. No I/O or posting occurs. */
UmiStatus UmiBankFeedArchiveDecode(const void *bytes, size_t length,
    UmiBankFeed **out, UmiBankFeedArchiveInfo *outInfo);
#ifdef __cplusplus
}
#endif
#endif
