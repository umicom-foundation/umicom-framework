/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/structured_form_record.c
 * PURPOSE: Compose typed form snapshots with existing atomic record transactions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/structured_form_record.h"
#include <stdlib.h>
UmiStatus umi_structured_form_record_save(UmiDataServer *server, const UmiRecordAmendment *metadata,
                                          const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *form,
                                          uint64_t expected_record_revision,
                                          UmiRecordRevision *out) {
  if (server == NULL || metadata == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  unsigned char *bytes = malloc(UMI_RECORD_DOCUMENT_MAX_BYTES);
  if (bytes == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  size_t length = 0U;
  UmiStatus status = umi_structured_form_snapshot_encode(schema, form, bytes,
                                                         UMI_RECORD_DOCUMENT_MAX_BYTES, &length);
  if (status == UMI_STATUS_OK)
    status =
        umi_record_document_append(server, metadata, bytes, length, expected_record_revision, out);
  free(bytes);
  return status;
}
static UmiStatus publish(const UmiRecordDocument *document, const UmiStructuredFormSchema *schema,
                         UmiStructuredFormRecord *out) {
  UmiStructuredFormRecord *candidate = malloc(sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_structured_form_snapshot_decode(schema, document->content,
                                                         document->length, &candidate->form);
  if (status == UMI_STATUS_OK) {
    candidate->record = document->record;
    candidate->form.saved_revision = candidate->form.revision;
    *out = *candidate;
  }
  free(candidate);
  return status;
}
UmiStatus umi_structured_form_record_load(UmiDataServer *server, const char *entity_id,
                                          uint64_t record_revision,
                                          const UmiStructuredFormSchema *schema,
                                          UmiStructuredFormRecord *out) {
  if (server == NULL || entity_id == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDocument *document = malloc(sizeof(*document));
  if (document == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = record_revision == 0U
                         ? umi_record_document_latest(server, entity_id, document)
                         : umi_record_document_load(server, entity_id, record_revision, document);
  if (status == UMI_STATUS_OK)
    status = publish(document, schema, out);
  free(document);
  return status;
}
UmiStatus umi_structured_form_record_open_reviewed(UmiDataServer *server,
                                                   const UmiRecordRevision *reviewed,
                                                   const UmiStructuredFormSchema *schema,
                                                   UmiStructuredFormRecord *out) {
  if (server == NULL || reviewed == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDocument *document = malloc(sizeof(*document));
  if (document == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_record_document_open_reviewed(server, reviewed, document);
  if (status == UMI_STATUS_OK)
    status = publish(document, schema, out);
  free(document);
  return status;
}
