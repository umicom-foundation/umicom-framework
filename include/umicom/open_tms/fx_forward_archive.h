/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/fx_forward_archive.h
 *
 * PURPOSE:
 *   Persist bounded local treasury observations without conferring trade authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OPEN_TMS_FX_FORWARD_ARCHIVE_H
#define UMICOM_OPEN_TMS_FX_FORWARD_ARCHIVE_H
#include "umicom/open_tms/fx_forward_book.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Store the complete active book as explicit portable fields using Framework's
 * checked value envelope. No padding, unused rows or string tails are saved.
 * Decode validates bounded unique identities, lifecycle states and finite values.
 * Historical versions/revisions remain imported evidence, not execution tokens.
 * Imported Confirmed or Settled records are not proof of external approval.
 *
 * Follow value_archive.h ownership and non-overlap rules. Encode can measure
 * with NULL/0 and reports required size without writing a short buffer. Every
 * decode failure leaves the destination unchanged. The ordinary decoder stages
 * one whole book on its stack; use the workspace form on small-stack threads.
 * The conditional form allocates bounded temporary storage and compares the
 * current canonical value with the caller's independent reviewed snapshot.
 *
 * Decode into a separate draft, review, and explicitly replace local practice
 * state. Revoke all prior transition reviews on replacement. This is a value
 * codec, not an atomic database transaction or a monotonic history service.
 * Checksum protects against accidental damage, not malicious authors. Treat
 * archives as private financial data; no encryption or authentication is added. */
/* Upper byte bound for admission before allocating or reading an archive. */
size_t umi_open_tms_fx_forward_book_archive_limit(void);
UmiStatus umi_open_tms_fx_forward_book_archive_encode(
    const UmiOpenTmsFxForwardBook *book, void *bytes, size_t capacity, size_t *outSize);
UmiStatus umi_open_tms_fx_forward_book_archive_decode(
    const void *bytes, size_t byteCount, UmiOpenTmsFxForwardBook *outBook);
UmiStatus umi_open_tms_fx_forward_book_archive_decode_with_workspace(
    const void *bytes, size_t byteCount, UmiOpenTmsFxForwardBook *workspace,
    UmiOpenTmsFxForwardBook *outBook);
UmiStatus umi_open_tms_fx_forward_book_archive_decode_if_unchanged(
    const void *bytes, size_t byteCount, const UmiOpenTmsFxForwardBook *expected,
    UmiOpenTmsFxForwardBook *outBook);
#ifdef __cplusplus
}
#endif
#endif
