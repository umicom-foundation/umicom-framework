/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bind result pages to query identity and checked revision heads. */
#include "umicom/data/structured_form_search_catalogue.h"
#include "umicom/base/sha256.h"
#include "umicom/data/structured_form_search_preset.h"
#include <stdlib.h>
#include <string.h>
static bool terminated(const char *text, size_t capacity) {
  return memchr(text, 0, capacity) != NULL;
}
static bool query_valid(const UmiStructuredFormSearchQuery *q) {
  if (!q || !terminated(q->form_id, sizeof q->form_id) ||
      !terminated(q->entity_prefix, sizeof q->entity_prefix) ||
      !terminated(q->text, sizeof q->text) ||
      !terminated(q->after_entity, sizeof q->after_entity) ||
      !terminated(q->expected_snapshot, sizeof q->expected_snapshot) || !q->form_id[0] ||
      !q->schema_version || !q->entity_prefix[0] || !q->limit ||
      q->limit > UMI_RECORD_CATALOGUE_PAGE_CAPACITY)
    return false;
  size_t n = strlen(q->expected_snapshot);
  if ((n && n != 64) || (q->after_entity[0] && n != 64))
    return false;
  for (size_t i = 0; i < n; ++i)
    if (!((q->expected_snapshot[i] >= '0' && q->expected_snapshot[i] <= '9') ||
          (q->expected_snapshot[i] >= 'a' && q->expected_snapshot[i] <= 'f')))
      return false;
  return true;
}
static UmiStatus query_snapshot(const char *heads, const UmiStructuredFormSearchQuery *q,
                                char out[65]) {
  UmiSha256 hash;
  UmiSha256Init(&hash);
  static const char domain[] = "Umicom saved search catalogue 1";
  UmiStatus status = UmiSha256Update(&hash, domain, sizeof domain);
  const char *parts[] = {heads, q->form_id, q->entity_prefix, q->text};
  for (size_t i = 0; i < sizeof parts / sizeof parts[0] && status == UMI_STATUS_OK; ++i)
    status = UmiSha256Update(&hash, parts[i], strlen(parts[i]) + 1);
  unsigned char version[8];
  for (size_t i = 0; i < 8; ++i)
    version[7 - i] = (unsigned char)(q->schema_version >> (8 * i));
  if (status == UMI_STATUS_OK)
    status = UmiSha256Update(&hash, version, sizeof version);
  unsigned char digest[UMI_SHA256_BYTES];
  if (status == UMI_STATUS_OK)
    status = UmiSha256Final(&hash, digest);
  if (status == UMI_STATUS_OK)
    UmiSha256Hex(digest, out);
  return status;
}
UmiStatus umi_structured_form_search_catalogue_page(UmiDataServer *server,
                                                    const UmiStructuredFormCatalogue *catalogue,
                                                    const UmiStructuredFormSearchQuery *query,
                                                    UmiStructuredFormSearchPage *out) {
  if (!server || !catalogue || !out || !query_valid(query))
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiStructuredFormSchema *schema = NULL;
  UmiStatus status =
      umi_structured_form_catalogue_find(catalogue, query->form_id, query->schema_version, &schema);
  if (status != UMI_STATUS_OK)
    return status;
  /* Validate namespace using the same portable preset contract, not a second
   * subtly different list of allowed characters. An empty filter is valid. */
  UmiStructuredFormSearchPreset *scope = calloc(1, sizeof *scope);
  if (!scope)
    return UMI_STATUS_OUT_OF_MEMORY;
  strcpy(scope->title, "Scope");
  strcpy(scope->entity_prefix, query->entity_prefix);
  status = umi_structured_form_search_preset_validate(schema, scope);
  free(scope);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormSearchPage *candidate = calloc(1, sizeof *candidate);
  UmiStructuredFormHistoricalSearch *record = malloc(sizeof *record);
  UmiRecordCataloguePage *metadata = malloc(sizeof *metadata);
  if (!candidate || !record || !metadata) {
    free(candidate);
    free(record);
    free(metadata);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiRecordCatalogueQuery scan = {.entity_prefix = UMI_FORM_SEARCH_PRESET_PREFIX,
                                  .limit = UMI_RECORD_CATALOGUE_PAGE_CAPACITY};
  char heads[65] = {0};
  bool cursor_found = !query->after_entity[0], more = false;
  size_t examined = 0;
  for (;;) {
    status = umi_record_catalogue_page(server, &scan, metadata);
    if (status != UMI_STATUS_OK)
      break;
    if (!heads[0]) {
      memcpy(heads, metadata->snapshot, sizeof heads);
      candidate->definition_count = metadata->matching_count;
      status = query_snapshot(heads, query, candidate->snapshot);
      if (status == UMI_STATUS_OK && query->expected_snapshot[0] &&
          strcmp(query->expected_snapshot, candidate->snapshot))
        status = UMI_STATUS_INVALID_STATE;
    }
    for (size_t i = 0; status == UMI_STATUS_OK && i < metadata->count; ++i) {
      if (++examined > UMI_RECORD_CATALOGUE_ENTITY_LIMIT) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        break;
      }
      status = umi_structured_form_search_reader_open_reviewed(server, catalogue,
                                                               &metadata->rows[i], record);
      if (status != UMI_STATUS_OK)
        break;
      const UmiStructuredFormSchema *stored = record->resolved.schema;
      const UmiStructuredFormSearchPreset *preset = &record->resolved.preset;
      const char *id = record->record.amendment.entity_id;
      if (strcmp(stored->base->form_id, query->form_id) ||
          stored->version != query->schema_version ||
          strcmp(preset->entity_prefix, query->entity_prefix) ||
          (!strstr(preset->title, query->text) && !strstr(id, query->text)))
        continue;
      ++candidate->matching_count;
      if (!strcmp(id, query->after_entity))
        cursor_found = true;
      if (strcmp(id, query->after_entity) <= 0)
        continue;
      if (candidate->count < query->limit) {
        UmiStructuredFormSearchRow *row = &candidate->rows[candidate->count++];
        row->record = record->record;
        strcpy(row->title, preset->title);
        row->term_count = preset->filter.count;
      } else
        more = true;
    }
    if (status != UMI_STATUS_OK || !metadata->next_after_entity[0])
      break;
    memcpy(scan.after_entity, metadata->next_after_entity, sizeof scan.after_entity);
    memcpy(scan.expected_snapshot, heads, sizeof scan.expected_snapshot);
  }
  if (status == UMI_STATUS_OK && (!cursor_found || examined != candidate->definition_count))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    scan.after_entity[0] = 0;
    memcpy(scan.expected_snapshot, heads, sizeof scan.expected_snapshot);
    status = umi_record_catalogue_page(server, &scan, metadata);
  }
  if (status == UMI_STATUS_OK) {
    if (more)
      strcpy(candidate->next_after_entity,
             candidate->rows[candidate->count - 1].record.amendment.entity_id);
    *out = *candidate;
  }
  free(candidate);
  free(record);
  free(metadata);
  return status;
}
