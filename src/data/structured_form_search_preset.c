/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/structured_form_search_preset.c
 *
 * PURPOSE:
 *   Compose query serialization with the same revision checks as other records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compose query serialization with the same revision checks as other records. */
#include "umicom/data/structured_form_search_preset.h"
#include "search_preset_internal.h"
#include <stdlib.h>
#include <string.h>
/* The original entity validator is retained for review. Shared readers and
 * discovery now use one private validator to preserve the same namespace and
 * key rules as saving. */
#if 0
static UmiStatus preset_entity(const char *id) {
  if (!id)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0;
  while (length < 64 && id[length])
    ++length;
  const size_t prefix = sizeof(UMI_FORM_SEARCH_PRESET_PREFIX) - 1;
  if (length <= prefix || length > prefix + 40 ||
      strncmp(id, UMI_FORM_SEARCH_PRESET_PREFIX, prefix))
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = prefix; i < length; ++i) {
    char c = id[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' ||
          c == '-'))
      return UMI_STATUS_INVALID_ARGUMENT;
  }
  return UMI_STATUS_OK;
}
#endif
static UmiStatus preset_entity(const char *id) {
    return umi_search_preset_entity_check(id);
}
UmiStatus umi_structured_form_search_preset_save(UmiDataServer *server,
                                                 const UmiRecordAmendment *metadata,
                                                 const UmiStructuredFormSchema *schema,
                                                 const UmiStructuredFormSearchPreset *preset,
                                                 uint64_t expected_revision,
                                                 UmiRecordRevision *out) {
  if (!server || !metadata || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = preset_entity(metadata->entity_id);
  if (status != UMI_STATUS_OK)
    return status;
  unsigned char *bytes = malloc(UMI_FORM_SEARCH_PRESET_MAX_BYTES);
  if (!bytes)
    return UMI_STATUS_OUT_OF_MEMORY;
  size_t length = 0;
  status = umi_structured_form_search_preset_encode(schema, preset, bytes,
                                                    UMI_FORM_SEARCH_PRESET_MAX_BYTES, &length);
  /* The initial append-only composition is retained for review. Validate the
   * existing preset before amendment so an unrelated or malformed document
   * cannot silently become a saved search. Append still checks the expected
   * head atomically if another writer advances it after this read. */
#if 0
    if (status == UMI_STATUS_OK) status = umi_record_document_append(server, metadata, bytes, length, expected_revision, out);
#endif
  if (status == UMI_STATUS_OK && expected_revision != 0) {
    UmiStructuredFormSearchPresetRecord *previous = malloc(sizeof *previous);
    if (!previous)
      status = UMI_STATUS_OUT_OF_MEMORY;
    else {
      status =
          umi_structured_form_search_preset_load(server, metadata->entity_id, 0, schema, previous);
      if (status == UMI_STATUS_OK && previous->record.revision != expected_revision)
        status = UMI_STATUS_INVALID_STATE;
      free(previous);
    }
  }
  if (status == UMI_STATUS_OK)
    status = umi_record_document_append(server, metadata, bytes, length, expected_revision, out);
  free(bytes);
  return status;
}
static UmiStatus preset_publish(const UmiRecordDocument *document,
                                const UmiStructuredFormSchema *schema,
                                UmiStructuredFormSearchPresetRecord *out) {
  UmiStructuredFormSearchPresetRecord *candidate = malloc(sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_structured_form_search_preset_decode(schema, document->content,
                                                              document->length, &candidate->preset);
  if (status == UMI_STATUS_OK) {
    candidate->record = document->record;
    *out = *candidate;
  }
  free(candidate);
  return status;
}
UmiStatus umi_structured_form_search_preset_load(UmiDataServer *server, const char *entity_id,
                                                 uint64_t revision,
                                                 const UmiStructuredFormSchema *schema,
                                                 UmiStructuredFormSearchPresetRecord *out) {
  if (!server || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = preset_entity(entity_id);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordDocument *document = malloc(sizeof *document);
  if (!document)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = revision ? umi_record_document_load(server, entity_id, revision, document)
                    : umi_record_document_latest(server, entity_id, document);
  if (status == UMI_STATUS_OK)
    status = preset_publish(document, schema, out);
  free(document);
  return status;
}
UmiStatus umi_structured_form_search_preset_open_reviewed(
    UmiDataServer *server, const UmiRecordRevision *reviewed, const UmiStructuredFormSchema *schema,
    UmiStructuredFormSearchPresetRecord *out) {
  if (!server || !reviewed || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = preset_entity(reviewed->amendment.entity_id);
  if (status != UMI_STATUS_OK)
    return status;
  UmiRecordDocument *document = malloc(sizeof *document);
  if (!document)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_record_document_open_reviewed(server, reviewed, document);
  if (status == UMI_STATUS_OK)
    status = preset_publish(document, schema, out);
  free(document);
  return status;
}
UmiStatus umi_structured_form_query_from_preset(const UmiStructuredFormSchema *schema,
                                                const UmiStructuredFormSearchPreset *preset,
                                                size_t limit, UmiStructuredFormQuery *out) {
  if (!out || !limit || limit > UMI_RECORD_CATALOGUE_PAGE_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_search_preset_validate(schema, preset);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormQuery *candidate = calloc(1, sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  strcpy(candidate->entity_prefix, preset->entity_prefix);
  strcpy(candidate->metadata_text, preset->metadata_text);
  strcpy(candidate->form_id, schema->base->form_id);
  candidate->schema_version = schema->version;
  candidate->filter = preset->filter;
  candidate->limit = limit;
  *out = *candidate;
  free(candidate);
  return UMI_STATUS_OK;
}
