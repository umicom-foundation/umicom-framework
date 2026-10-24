/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reuse canonical typed snapshots and the shared authorization/storage boundary. */
#include "umicom/data/authorized_form_records.h"
#include <stdlib.h>
static UmiStatus publish_form(const UmiAuthorizedRecordDocument *document,
                              const UmiStructuredFormSchema *schema, UmiAuthorizedFormRecord *out) {
  UmiAuthorizedFormRecord *candidate = malloc(sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_structured_form_snapshot_decode(
      schema, document->document.content, document->document.length, &candidate->form);
  if (status == UMI_STATUS_OK) {
    candidate->record = document->document.record;
    candidate->access = document->access;
    candidate->form.saved_revision = candidate->form.revision;
    *out = *candidate;
  }
  free(candidate);
  return status;
}
UmiStatus umi_authorized_form_record_load(UmiAuthorizedRecords *records,
                                          const UmiRecordAccessRequest *request,
                                          uint64_t record_revision,
                                          const UmiStructuredFormSchema *schema,
                                          UmiAuthorizedFormRecord *out) {
  if (!records || !request || !schema || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiAuthorizedRecordDocument *document = malloc(sizeof *document);
  if (!document)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = record_revision
                         ? umi_authorized_record_load(records, request, record_revision, document)
                         : umi_authorized_record_latest(records, request, document);
  if (status == UMI_STATUS_OK)
    status = publish_form(document, schema, out);
  free(document);
  return status;
}
UmiStatus umi_authorized_form_record_open_reviewed(UmiAuthorizedRecords *records,
                                                   const UmiRecordAccessRequest *request,
                                                   const UmiRecordRevision *reviewed,
                                                   const UmiStructuredFormSchema *schema,
                                                   UmiAuthorizedFormRecord *out) {
  if (!records || !request || !reviewed || !schema || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiAuthorizedRecordDocument *document = malloc(sizeof *document);
  if (!document)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_authorized_record_open_reviewed(records, request, reviewed, document);
  if (status == UMI_STATUS_OK)
    status = publish_form(document, schema, out);
  free(document);
  return status;
}
UmiStatus umi_authorized_form_record_save(UmiAuthorizedRecords *records,
                                          const UmiRecordAccessRequest *request,
                                          const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *form, const char *reason,
                                          uint64_t observed_at_nanoseconds,
                                          uint64_t expected_record_revision,
                                          UmiAuthorizedRecordRevision *out) {
  if (!records || !request || !schema || !form || !reason || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (expected_record_revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (request->operation !=
      (expected_record_revision ? UMI_RECORD_ACCESS_AMEND : UMI_RECORD_ACCESS_CREATE))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_access_request_validate(request);
  if (status != UMI_STATUS_OK)
    return status;
  /* An amendment must not repurpose an unrelated document or silently migrate
   * its schema. Read through the same authority; optimistic append rechecks
   * the head after validation if another store connection has written it. */
  if (expected_record_revision) {
    UmiRecordAccessRequest read = *request;
    read.operation = UMI_RECORD_ACCESS_READ_HEAD;
    UmiAuthorizedFormRecord *previous = malloc(sizeof *previous);
    if (!previous)
      return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_authorized_form_record_load(records, &read, 0, schema, previous);
    if (status == UMI_STATUS_OK && previous->record.revision != expected_record_revision)
      status = UMI_STATUS_INVALID_STATE;
    free(previous);
    if (status != UMI_STATUS_OK)
      return status;
  }
  unsigned char *bytes = malloc(UMI_RECORD_DOCUMENT_MAX_BYTES);
  if (!bytes)
    return UMI_STATUS_OUT_OF_MEMORY;
  size_t length = 0;
  status = umi_structured_form_snapshot_encode(schema, form, bytes, UMI_RECORD_DOCUMENT_MAX_BYTES,
                                               &length);
  if (status == UMI_STATUS_OK)
    status = umi_authorized_record_append(records, request, reason, observed_at_nanoseconds, bytes,
                                          length, expected_record_revision, out);
  free(bytes);
  return status;
}
