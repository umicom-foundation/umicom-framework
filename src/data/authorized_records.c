/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/authorized_records.c
 *
 * PURPOSE:
 *   Reauthorize at the storage boundary and derive provenance from trusted state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reauthorize at the storage boundary and derive provenance from trusted state. */
#include "umicom/data/authorized_records.h"
#include <stdlib.h>
#include <string.h>
struct UmiAuthorizedRecords {
  UmiDataServer *server;
  UmiRecordAccessAuthority *authority;
};
UmiStatus umi_authorized_records_create(UmiDataServer *server, UmiRecordAccessAuthority *authority,
                                        UmiAuthorizedRecords **out) {
  if (!server || !authority || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiAuthorizedRecords *candidate = malloc(sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  candidate->server = server;
  candidate->authority = authority;
  *out = candidate;
  return UMI_STATUS_OK;
}
void umi_authorized_records_destroy(UmiAuthorizedRecords *records) { free(records); }
static UmiStatus read_document(UmiAuthorizedRecords *records, const UmiRecordAccessRequest *request,
                               uint64_t revision, const UmiRecordRevision *reviewed, int mode,
                               UmiAuthorizedRecordDocument *out) {
  if (!records || !out || !request)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_access_request_validate(request);
  if (status != UMI_STATUS_OK)
    return status;
  if (request->operation !=
          (mode == 1 ? UMI_RECORD_ACCESS_READ_HISTORY : UMI_RECORD_ACCESS_READ_HEAD) ||
      (mode == 1 && !revision))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (mode == 2 &&
      (!reviewed ||
       !memchr(reviewed->amendment.entity_id, 0, sizeof reviewed->amendment.entity_id) ||
       strcmp(reviewed->amendment.entity_id, request->entity_id)))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiAuthorizedRecordDocument *candidate = calloc(1, sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_record_access_authorize(records->authority, request, &candidate->access);
  if (status == UMI_STATUS_OK) {
    if (mode == 1)
      status = umi_record_document_load(records->server, request->entity_id, revision,
                                        &candidate->document);
    else if (mode == 2)
      status = umi_record_document_open_reviewed(records->server, reviewed, &candidate->document);
    else
      status =
          umi_record_document_latest(records->server, request->entity_id, &candidate->document);
  }
  if (status == UMI_STATUS_OK)
    *out = *candidate;
  free(candidate);
  return status;
}
UmiStatus umi_authorized_record_latest(UmiAuthorizedRecords *records,
                                       const UmiRecordAccessRequest *request,
                                       UmiAuthorizedRecordDocument *out) {
  return read_document(records, request, 0, NULL, 0, out);
}
UmiStatus umi_authorized_record_load(UmiAuthorizedRecords *records,
                                     const UmiRecordAccessRequest *request, uint64_t revision,
                                     UmiAuthorizedRecordDocument *out) {
  return read_document(records, request, revision, NULL, 1, out);
}
UmiStatus umi_authorized_record_open_reviewed(UmiAuthorizedRecords *records,
                                              const UmiRecordAccessRequest *request,
                                              const UmiRecordRevision *reviewed,
                                              UmiAuthorizedRecordDocument *out) {
  return read_document(records, request, 0, reviewed, 2, out);
}
UmiStatus umi_authorized_record_append(UmiAuthorizedRecords *records,
                                       const UmiRecordAccessRequest *request, const char *reason,
                                       uint64_t observed_at_nanoseconds, const void *content,
                                       size_t length, uint64_t expected_revision,
                                       UmiAuthorizedRecordRevision *out) {
  if (!records || !request || !out || !reason || (!content && length))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length > UMI_RECORD_DOCUMENT_MAX_BYTES || expected_revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (request->operation !=
      (expected_revision ? UMI_RECORD_ACCESS_AMEND : UMI_RECORD_ACCESS_CREATE))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t reason_length = 0;
  while (reason_length < 256 && reason[reason_length]) {
    unsigned char c = (unsigned char)reason[reason_length];
    if (c < 32 || c == 127)
      return UMI_STATUS_INVALID_ARGUMENT;
    ++reason_length;
  }
  if (!reason_length || reason_length == 256)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiAuthorizedRecordRevision candidate = {0};
  UmiStatus status = umi_record_access_authorize(records->authority, request, &candidate.access);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordAmendment metadata = {0};
  strcpy(metadata.entity_id, candidate.access.entity_id);
  strcpy(metadata.source, candidate.access.source);
  strcpy(metadata.actor, candidate.access.principal_id);
  memcpy(metadata.reason, reason, reason_length + 1);
  metadata.recorded_at_nanoseconds = candidate.access.checked_at_nanoseconds;
  metadata.observed_at_nanoseconds = observed_at_nanoseconds;
  status = umi_record_document_append(records->server, &metadata, content, length,
                                      expected_revision, &candidate.record);
  if (status == UMI_STATUS_OK)
    *out = candidate;
  return status;
}
