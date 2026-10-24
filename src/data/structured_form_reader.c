/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * History decoding keeps storage integrity and form semantics in one result. */
#include "umicom/data/structured_form_reader.h"
#include "umicom/data/record_draft.h"
#include <stdlib.h>
UmiStatus umi_structured_form_reader_decode(const UmiStructuredFormCatalogue *catalogue,
                                            const UmiRecordDocument *document,
                                            UmiStructuredFormHistorical *out) {
  if (catalogue == NULL || document == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* The draft's existing open check validates metadata, bounded bytes and the
   * canonical digest without adding a second public document validator. */
  UmiRecordDraft *check = NULL;
  UmiStatus status = umi_record_draft_create(&check);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_open(check, document);
  UmiStructuredFormHistorical *candidate = NULL;
  if (status == UMI_STATUS_OK) {
    candidate = malloc(sizeof(*candidate));
    if (candidate == NULL)
      status = UMI_STATUS_OUT_OF_MEMORY;
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_catalogue_decode(catalogue, document->content, document->length,
                                                  &candidate->resolved);
  if (status == UMI_STATUS_OK) {
    candidate->record = document->record;
    candidate->resolved.form.saved_revision = candidate->resolved.form.revision;
    *out = *candidate;
  }
  free(candidate);
  umi_record_draft_destroy(check);
  return status;
}
UmiStatus umi_structured_form_reader_load(UmiDataServer *server,
                                          const UmiStructuredFormCatalogue *catalogue,
                                          const char *entity_id, uint64_t record_revision,
                                          UmiStructuredFormHistorical *out) {
  if (server == NULL || catalogue == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDocument *document = malloc(sizeof(*document));
  if (document == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = record_revision == 0
                         ? umi_record_document_latest(server, entity_id, document)
                         : umi_record_document_load(server, entity_id, record_revision, document);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_reader_decode(catalogue, document, out);
  free(document);
  return status;
}
UmiStatus umi_structured_form_reader_open_reviewed(UmiDataServer *server,
                                                   const UmiStructuredFormCatalogue *catalogue,
                                                   const UmiRecordRevision *reviewed,
                                                   UmiStructuredFormHistorical *out) {
  if (server == NULL || catalogue == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDocument *document = malloc(sizeof(*document));
  if (document == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_record_document_open_reviewed(server, reviewed, document);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_reader_decode(catalogue, document, out);
  free(document);
  return status;
}
