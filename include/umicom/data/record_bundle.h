/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/record_bundle.h
 *
 * PURPOSE:
 *   Portable bounded history transfer without replacing an existing entity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Portable bounded history transfer without replacing an existing entity. */
#ifndef UMICOM_DATA_RECORD_BUNDLE_H
#define UMICOM_DATA_RECORD_BUNDLE_H
#include "umicom/data/record_document.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RECORD_BUNDLE_MAX_REVISIONS 16U
#define UMI_RECORD_BUNDLE_SCAN_LIMIT 32768U
#define UMI_RECORD_BUNDLE_MAX_BYTES                                                                \
  (80U + UMI_RECORD_BUNDLE_MAX_REVISIONS *                                                         \
             (8U + UMI_RECORD_REVISION_TEXT_CAPACITY + UMI_RECORD_DOCUMENT_MAX_BYTES))
typedef struct UmiRecordBundle UmiRecordBundle;
/* Capture the complete history of one entity in one owned read transaction.
 * Refuse more than MAX_REVISIONS instead of silently exporting a partial chain.
 * Nested transactions remain caller-owned and return BUSY. Output is unchanged
 * on error. The bundle owns immutable copies and may be read by other threads. */
UmiStatus umi_record_bundle_capture(UmiDataServer *server, const char *entity_id,
                                    UmiRecordBundle **out);
void umi_record_bundle_destroy(UmiRecordBundle *bundle);
size_t umi_record_bundle_count(const UmiRecordBundle *bundle);
/* Zero-based history in ascending revision order; returns a copied document. */
UmiStatus umi_record_bundle_at(const UmiRecordBundle *bundle, size_t index, UmiRecordDocument *out);
/* Canonical versioned framing includes complete original metadata and content.
 * A SHA-256 covers the entire frame; individual content digests are checked too.
 * This detects corruption, not authenticity. No encryption or compression.
 * Encode allocates bytes; release through bytes_free. Decode copies valid input.
 * Both output arguments remain unchanged on failure. */
UmiStatus umi_record_bundle_encode(const UmiRecordBundle *bundle, unsigned char **out,
                                   size_t *out_length);
UmiStatus umi_record_bundle_decode(const void *bytes, size_t length, UmiRecordBundle **out);
void umi_record_bundle_bytes_free(void *bytes);
/* Restore the original entity and revisions only where no such history or
 * content rows exist. All revisions are staged in ONE transaction; collision,
 * corrupt input or storage failure rolls it back. Other entities are retained.
 * Collision review visits at most SCAN_LIMIT total destination rows; a larger
 * store returns CAPACITY_EXCEEDED instead of accepting a partial occupancy scan.
 * This never merges, overwrites, truncates or chooses a replacement identity.
 * Rollback failure is IO_ERROR; inspect storage before any retry. On success
 * out receives the final original revision. Caller owns restore authorization.
 * This bounded entity transfer is not a whole-database backup service. */
UmiStatus umi_record_bundle_restore_new(UmiDataServer *server, const UmiRecordBundle *bundle,
                                        UmiRecordRevision *out);
#ifdef __cplusplus
}
#endif
#endif
