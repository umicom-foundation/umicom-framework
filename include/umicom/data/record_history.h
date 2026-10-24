/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/record_history.h
 * PURPOSE: Share revision-checked amendment metadata across application hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_RECORD_HISTORY_H
#define UMICOM_DATA_RECORD_HISTORY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RECORD_REVISION_TEXT_CAPACITY 1024U

/* For content and metadata saved atomically, use record_document.h. Retain this
 * metadata-only API for separately managed content. Do not alternate the two
 * append APIs for the same entity: a metadata-only revision has no payload. */

/* Bounded identifiers and metadata, not a clinical note or a document payload.
 * The caller supplies the SHA-256 of its separately retained content. This
 * service records the digest but does not authenticate the actor, calculate
 * the digest, authorize a change or atomically save that external content. */
typedef struct UmiRecordAmendment {
    char entity_id[64];
    char source[160];
    char actor[128];
    char reason[256];
    char content_sha256[65];
    uint64_t observed_at_nanoseconds;
    uint64_t recorded_at_nanoseconds;
} UmiRecordAmendment;

typedef struct UmiRecordRevision {
    UmiRecordAmendment amendment;
    uint64_t revision;
    uint64_t previous_revision;
} UmiRecordRevision;

/* The canonical text codec rejects unknown schemas, control characters,
 * truncated fields and malformed integers. Outputs remain unchanged on error.
 * parse takes a byte length excluding the terminator; embedded NULs are invalid. */
UmiStatus umi_record_revision_format(const UmiRecordRevision *record,
                                    char *output, size_t capacity);
UmiStatus umi_record_revision_parse(const char *text, size_t length,
                                   UmiRecordRevision *output);

/* expected_revision is zero for a new entity, or the last reviewed revision.
 * A stale expected revision returns INVALID_STATE. Appending owns a Data Server
 * transaction, so existing caller transactions and competing owners return BUSY.
 * Row and head are committed together; output changes only after commit succeeds.
 * Nonzero recorded time cannot move backwards. observed time may be unknown (0).
 * Rollback failure returns IO_ERROR; retain the server and follow its recovery
 * contract before retrying. All callers must reserve repo.record-history.* for
 * this service. Raw store writes can defeat history integrity. */
UmiStatus umi_record_history_append(UmiDataServer *server,
                                   const UmiRecordAmendment *amendment,
                                   uint64_t expected_revision,
                                   UmiRecordRevision *output);
UmiStatus umi_record_history_load(const UmiDataServer *server, const char *entity_id,
                                 uint64_t revision, UmiRecordRevision *output);
UmiStatus umi_record_history_latest(const UmiDataServer *server,
                                   const char *entity_id, UmiRecordRevision *output);

/* Check every link from 1 through the recorded head under one transaction.
 * This detects malformed/missing links and time regressions, not authenticated
 * authorship, payload changes, deleted tails or malicious replacement of the
 * entire store. Empty history reports NOT_FOUND and leaves output unchanged. */
UmiStatus umi_record_history_verify(UmiDataServer *server, const char *entity_id,
                                   uint64_t *out_revisions);
#ifdef __cplusplus
}
#endif
#endif
