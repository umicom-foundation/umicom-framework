/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compose existing integrity readers; never search raw unvalidated payloads. */
#include "umicom/data/structured_form_query.h"
#include "umicom/base/sha256.h"
#include <stdlib.h>
#include <string.h>
static bool bounded(const char *value, size_t capacity) {
  const char *end = memchr(value, '\0', capacity);
  if (!end)
    return false;
  for (const char *p = value; p != end; ++p)
    if ((unsigned char)*p < 32 || (unsigned char)*p == 127)
      return false;
  return true;
}
static bool query_valid(const UmiStructuredFormQuery *query) {
  if (!query || !bounded(query->entity_prefix, sizeof query->entity_prefix) ||
      !query->entity_prefix[0] || !bounded(query->metadata_text, sizeof query->metadata_text) ||
      !bounded(query->form_id, sizeof query->form_id) ||
      !bounded(query->after_entity, sizeof query->after_entity) ||
      !bounded(query->expected_snapshot, sizeof query->expected_snapshot) || !query->form_id[0] ||
      !query->schema_version || !query->limit || query->limit > UMI_RECORD_CATALOGUE_PAGE_CAPACITY)
    return false;
  size_t n = strlen(query->expected_snapshot);
  if ((n != 0 && n != 64) || (query->after_entity[0] && n != 64))
    return false;
  for (size_t i = 0; i < n; ++i) {
    char ch = query->expected_snapshot[i];
    if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')))
      return false;
  }
  return true;
}
static UmiStatus snapshot(const char *heads, const char *filter, char out[65]) {
  UmiSha256 hash;
  UmiSha256Init(&hash);
  static const char domain[] = "Umicom typed record query 1";
  UmiStatus status = UmiSha256Update(&hash, domain, sizeof domain);
  if (status == UMI_STATUS_OK)
    status = UmiSha256Update(&hash, heads, 65);
  if (status == UMI_STATUS_OK)
    status = UmiSha256Update(&hash, filter, 65);
  unsigned char bytes[UMI_SHA256_BYTES];
  if (status == UMI_STATUS_OK)
    status = UmiSha256Final(&hash, bytes);
  if (status == UMI_STATUS_OK)
    UmiSha256Hex(bytes, out);
  return status;
}
UmiStatus umi_structured_form_query_page(UmiDataServer *server,
                                         const UmiStructuredFormCatalogue *catalogue,
                                         const UmiStructuredFormQuery *query,
                                         UmiStructuredFormQueryPage *out) {
  if (!server || !catalogue || !out || !query_valid(query))
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiStructuredFormSchema *schema = NULL;
  UmiStatus status =
      umi_structured_form_catalogue_find(catalogue, query->form_id, query->schema_version, &schema);
  char filter_digest[65];
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_filter_digest(schema, &query->filter, filter_digest);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormQueryPage *candidate = calloc(1, sizeof *candidate);
  UmiStructuredFormHistorical *record = malloc(sizeof *record);
  UmiRecordCataloguePage *metadata = malloc(sizeof *metadata);
  if (!candidate || !record || !metadata) {
    free(candidate);
    free(record);
    free(metadata);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiRecordCatalogueQuery scan = {.limit = UMI_RECORD_CATALOGUE_PAGE_CAPACITY};
  memcpy(scan.entity_prefix, query->entity_prefix, sizeof scan.entity_prefix);
  memcpy(scan.text, query->metadata_text, sizeof scan.text);
  char initial_heads[65] = {0};
  bool cursor_found = query->after_entity[0] == '\0', more = false;
  size_t examined = 0;
  for (;;) {
    status = umi_record_catalogue_page(server, &scan, metadata);
    if (status != UMI_STATUS_OK)
      break;
    if (!initial_heads[0]) {
      memcpy(initial_heads, metadata->snapshot, sizeof initial_heads);
      candidate->metadata_count = metadata->matching_count;
      candidate->namespace_count = metadata->namespace_count;
      status = snapshot(initial_heads, filter_digest, candidate->snapshot);
      if (status == UMI_STATUS_OK && query->expected_snapshot[0] &&
          strcmp(query->expected_snapshot, candidate->snapshot) != 0)
        status = UMI_STATUS_INVALID_STATE;
    }
    for (size_t i = 0; status == UMI_STATUS_OK && i < metadata->count; ++i) {
      if (++examined > UMI_RECORD_CATALOGUE_ENTITY_LIMIT) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        break;
      }
      status =
          umi_structured_form_reader_open_reviewed(server, catalogue, &metadata->rows[i], record);
      if (status != UMI_STATUS_OK)
        break;
      if (strcmp(record->resolved.form.form_id, query->form_id) != 0 ||
          record->resolved.form.schema_version != query->schema_version)
        continue;
      bool match = false;
      status =
          umi_structured_form_filter_match(schema, &record->resolved.form, &query->filter, &match);
      if (status != UMI_STATUS_OK || !match)
        continue;
      ++candidate->matching_count;
      const char *id = record->record.amendment.entity_id;
      if (strcmp(id, query->after_entity) == 0)
        cursor_found = true;
      if (strcmp(id, query->after_entity) <= 0)
        continue;
      if (candidate->count < query->limit)
        candidate->rows[candidate->count++] = record->record;
      else
        more = true;
    }
    if (status != UMI_STATUS_OK || !metadata->next_after_entity[0])
      break;
    memcpy(scan.after_entity, metadata->next_after_entity, sizeof scan.after_entity);
    memcpy(scan.expected_snapshot, initial_heads, sizeof scan.expected_snapshot);
  }
  if (status == UMI_STATUS_OK && (!cursor_found || examined != candidate->metadata_count))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    /* Re-read the first catalogue page after decoding all selected records.
     * A namespace change anywhere during those separate reads refuses output. */
    scan.after_entity[0] = '\0';
    memcpy(scan.expected_snapshot, initial_heads, sizeof scan.expected_snapshot);
    status = umi_record_catalogue_page(server, &scan, metadata);
  }
  if (status == UMI_STATUS_OK) {
    if (more)
      memcpy(candidate->next_after_entity,
             candidate->rows[candidate->count - 1].amendment.entity_id,
             sizeof candidate->next_after_entity);
    *out = *candidate;
  }
  free(metadata);
  free(record);
  free(candidate);
  return status;
}
