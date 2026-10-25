/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/structured_form_search_reader.c
 *
 * PURPOSE:
 *   Reuse document integrity checks before interpreting stored query definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reuse document integrity checks before interpreting stored query definitions. */
#include "umicom/data/structured_form_search_reader.h"
#include "search_preset_internal.h"
#include "umicom/data/record_draft.h"
#include <stdlib.h>
UmiStatus umi_structured_form_search_reader_decode(const UmiStructuredFormCatalogue *catalogue,
                                                   const UmiRecordDocument *document,
                                                   UmiStructuredFormHistoricalSearch *out) {
  if (!catalogue || !document || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_search_preset_entity_check(document->record.amendment.entity_id);
  UmiRecordDraft *check = NULL;
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_create(&check);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_open(check, document);
  UmiStructuredFormHistoricalSearch *candidate = NULL;
  if (status == UMI_STATUS_OK) {
    candidate = malloc(sizeof *candidate);
    if (!candidate)
      status = UMI_STATUS_OUT_OF_MEMORY;
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_resolve(catalogue, document->content, document->length,
                                                &candidate->resolved);
  if (status == UMI_STATUS_OK) {
    candidate->record = document->record;
    *out = *candidate;
  }
  free(candidate);
  umi_record_draft_destroy(check);
  return status;
}
UmiStatus umi_structured_form_search_reader_load(UmiDataServer *server,
                                                 const UmiStructuredFormCatalogue *catalogue,
                                                 const char *entity_id, uint64_t revision,
                                                 UmiStructuredFormHistoricalSearch *out) {
  if (!server || !catalogue || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_search_preset_entity_check(entity_id);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordDocument *document = malloc(sizeof *document);
  if (!document)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = revision ? umi_record_document_load(server, entity_id, revision, document)
                    : umi_record_document_latest(server, entity_id, document);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_reader_decode(catalogue, document, out);
  free(document);
  return status;
}
UmiStatus umi_structured_form_search_reader_open_reviewed(
    UmiDataServer *server, const UmiStructuredFormCatalogue *catalogue,
    const UmiRecordRevision *reviewed, UmiStructuredFormHistoricalSearch *out) {
  if (!server || !catalogue || !reviewed || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_search_preset_entity_check(reviewed->amendment.entity_id);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordDocument *document = malloc(sizeof *document);
  if (!document)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_record_document_open_reviewed(server, reviewed, document);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_reader_decode(catalogue, document, out);
  free(document);
  return status;
}
