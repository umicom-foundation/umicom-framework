/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/record_document.h
 * PURPOSE: Save bounded record content with its amendment history atomically.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_RECORD_DOCUMENT_H
#define UMICOM_DATA_RECORD_DOCUMENT_H
#include "umicom/data/record_history.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RECORD_DOCUMENT_MAX_BYTES 16384U

/* A copied snapshot, never borrowed storage. Content is arbitrary bytes and
 * length excludes any caller-supplied text terminator. Empty content is valid.
 * These records are not encrypted and actor labels are not authentication. */
typedef struct UmiRecordDocument {
    UmiRecordRevision record;
    size_t length;
    unsigned char content[UMI_RECORD_DOCUMENT_MAX_BYTES];
} UmiRecordDocument;

/* Create with expected_revision=0; amend using the last reviewed revision.
 * Metadata is copied and content_sha256 is replaced by the computed digest.
 * The caller keeps input buffers stable until return. NULL content requires
 * length=0. The previous content must be readable and intact before amendment.
 * Metadata-only history cannot silently become a document: choose a new entity
 * or explicitly migrate with a separately reviewed migration service.
 * On success output contains committed metadata; all errors leave it unchanged.
 * This operation owns a transaction; nested/competing transactions return BUSY.
 * Failed writes roll back content, revision and head together. Rollback failure
 * returns IO_ERROR; follow Data Server recovery before doing more work. */
UmiStatus umi_record_document_append(UmiDataServer *server,
                                    const UmiRecordAmendment *metadata,
                                    const void *content, size_t length,
                                    uint64_t expected_revision,
                                    UmiRecordRevision *output);

/* Reads take a transaction for a consistent metadata/content snapshot and
 * verify length, canonical chunk encoding and SHA-256 before returning bytes.
 * Historical revisions remain readable. Outputs are unchanged on any failure.
 * NOT_FOUND means a required row is absent; PARSE_ERROR means malformed data or
 * a digest mismatch. Neither condition authorizes silently recreating history. */
UmiStatus umi_record_document_load(UmiDataServer *server, const char *entity_id,
                                  uint64_t revision, UmiRecordDocument *output);
UmiStatus umi_record_document_latest(UmiDataServer *server, const char *entity_id,
                                    UmiRecordDocument *output);
UmiStatus umi_record_document_verify(UmiDataServer *server, const char *entity_id,
                                    uint64_t *out_revisions);

/* Reserve repo.record-history.* and repo.record-content.* for these services.
 * Raw store writes bypass integrity. Hash checking detects accidental changes,
 * not malicious replacement, missing tails or a trusted signer. Large files,
 * attachments, permissions and clinical/payment decisions belong to separate
 * services; this API is a bounded record-storage primitive. */
#ifdef __cplusplus
}
#endif
#endif
