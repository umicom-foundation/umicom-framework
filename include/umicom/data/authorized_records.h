/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/authorized_records.h
 *
 * PURPOSE:
 *   Protect bounded document operations with current identity and purpose policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Protect bounded document operations with current identity and purpose policy. */
#ifndef UMICOM_DATA_AUTHORIZED_RECORDS_H
#define UMICOM_DATA_AUTHORIZED_RECORDS_H
#include "umicom/data/record_document.h"
#include "umicom/security/record_access.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAuthorizedRecords UmiAuthorizedRecords;
typedef struct UmiAuthorizedRecordDocument {
  UmiRecordDocument document;
  UmiRecordAccessReceipt access;
} UmiAuthorizedRecordDocument;
typedef struct UmiAuthorizedRecordRevision {
  UmiRecordRevision record;
  UmiRecordAccessReceipt access;
} UmiAuthorizedRecordRevision;
/* Both dependencies are borrowed. Keep raw Data Server and policy mutation
 * outside untrusted reach. One trusted owner serializes these calls with
 * identity/session/policy changes, so authorization cannot change mid-operation.
 * No nested transaction, network authentication or background work is added. */
UmiStatus umi_authorized_records_create(UmiDataServer *server, UmiRecordAccessAuthority *authority,
                                        UmiAuthorizedRecords **out);
void umi_authorized_records_destroy(UmiAuthorizedRecords *records);
/* Every operation reauthorizes. The receipt describes that completed operation,
 * not a cache of permissions. No output changes on denial or storage failure.
 * latest/open_reviewed require READ_HEAD; load requires READ_HISTORY and a
 * nonzero revision. Reviewed metadata must match the request's entity exactly. */
UmiStatus umi_authorized_record_latest(UmiAuthorizedRecords *records,
                                       const UmiRecordAccessRequest *request,
                                       UmiAuthorizedRecordDocument *out);
UmiStatus umi_authorized_record_load(UmiAuthorizedRecords *records,
                                     const UmiRecordAccessRequest *request, uint64_t revision,
                                     UmiAuthorizedRecordDocument *out);
UmiStatus umi_authorized_record_open_reviewed(UmiAuthorizedRecords *records,
                                              const UmiRecordAccessRequest *request,
                                              const UmiRecordRevision *reviewed,
                                              UmiAuthorizedRecordDocument *out);
/* CREATE requires expected_revision=0; AMEND requires a positive reviewed head.
 * Actor comes from the current session identity, source from authority setup,
 * and recorded time from the trusted request clock. Callers supply only the
 * reason, optional observation time and bytes. Purpose is returned in access;
 * it is not silently inserted into the historical metadata format. Hosts that
 * need durable purpose audit must provide a separately qualified audit sink.
 * Raw lower-level document APIs remain available for trusted internal services. */
UmiStatus umi_authorized_record_append(UmiAuthorizedRecords *records,
                                       const UmiRecordAccessRequest *request, const char *reason,
                                       uint64_t observed_at_nanoseconds, const void *content,
                                       size_t length, uint64_t expected_revision,
                                       UmiAuthorizedRecordRevision *out);
#ifdef __cplusplus
}
#endif
#endif
