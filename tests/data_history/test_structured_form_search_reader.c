/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_structured_form_search_reader.c
 *
 * PURPOSE:
 *   Historical query readers retain exact schemas and verified revision identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Historical query readers retain exact schemas and verified revision identity. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_search_preset.h"
#include "umicom/data/structured_form_search_reader.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  const char *cases[] = {
      "decode",   "latest",    "historical",     "reviewed",  "reviewed-stale", "reviewed-metadata",
      "missing",  "namespace", "unknown-schema", "malformed", "metadata-only",  "digest",
      "metadata", "length",    "entity",         "alias",     "nested-load",    "nested-reviewed",
      "null"};
  if (argc != 2 && argc != 3)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  if (!known || (argc == 3 && strcmp(argv[2], "sqlite")))
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  UmiDataServer *server = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormHistoricalSearch *out = malloc(sizeof *out), *before = malloc(sizeof *before);
  UmiRecordDocument *document = malloc(sizeof *document);
  void *alias = NULL;
  CHECK(f && preset && out && before && document);
  if (argc == 3)
    OK(umi_data_server_create_sqlite(":memory:", &server));
  else
    OK(umi_data_server_create_memory(&server));
  const UmiStructuredFormSchema *schemas[] = {&f->schema};
  OK(umi_structured_form_catalogue_create(schemas, 1, &catalogue));
  strcpy(preset->title, "First search");
  strcpy(preset->entity_prefix, "parcel.record.");
  UmiRecordAmendment metadata = {.entity_id = "form.search.example",
                                 .source = "Fictional catalogue",
                                 .actor = "Example author",
                                 .reason = "Store query",
                                 .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved, first;
  if (!strcmp(name, "unknown-schema"))
    f->schema.version = 2;
  if (!strcmp(name, "malformed"))
    OK(umi_record_document_append(server, &metadata, "bad", 3, 0, &saved));
  else if (!strcmp(name, "metadata-only")) {
    memset(metadata.content_sha256, 'a', 64);
    metadata.content_sha256[64] = 0;
    OK(umi_record_history_append(server, &metadata, 0, &saved));
  } else
    OK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 0, &saved));
  f->schema.version = 1;
  first = saved;
  if (!strcmp(name, "historical") || !strcmp(name, "latest") || !strcmp(name, "reviewed-stale")) {
    strcpy(preset->title, "Second search");
    metadata.recorded_at_nanoseconds = 2;
    OK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 1, &saved));
  }
  memset(before, 0x75, sizeof *before);
  *out = *before;
  if (!strcmp(name, "null")) {
    CHECK(umi_structured_form_search_reader_load(NULL, catalogue, metadata.entity_id, 0, out) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_search_reader_load(server, NULL, metadata.entity_id, 0, out) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_search_reader_load(server, catalogue, NULL, 0, out) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_reader_load(server, catalogue, metadata.entity_id, 0, NULL) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_search_reader_decode(catalogue, NULL, out) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_reader_open_reviewed(server, catalogue, NULL, out) !=
          UMI_STATUS_OK);
    CHECK(!memcmp(out, before, sizeof *out));
    goto cleanup;
  }
  UmiStatus status;
  bool direct = !strcmp(name, "decode") || !strcmp(name, "digest") || !strcmp(name, "metadata") ||
                !strcmp(name, "length") || !strcmp(name, "entity") || !strcmp(name, "alias");
  if (direct) {
    OK(umi_record_document_latest(server, metadata.entity_id, document));
    if (!strcmp(name, "digest"))
      document->content[0] ^= 1;
    if (!strcmp(name, "metadata"))
      document->record.amendment.recorded_at_nanoseconds = 0;
    if (!strcmp(name, "length"))
      document->length = UMI_RECORD_DOCUMENT_MAX_BYTES + 1;
    if (!strcmp(name, "entity"))
      strcpy(document->record.amendment.entity_id, "other.example");
    status = umi_structured_form_search_reader_decode(catalogue, document, out);
  } else if (!strncmp(name, "reviewed", 8) || !strcmp(name, "nested-reviewed")) {
    if (!strcmp(name, "reviewed-metadata"))
      strcpy(first.amendment.actor, "Changed actor");
    if (!strcmp(name, "nested-reviewed"))
      OK(umi_data_server_begin(server));
    status = umi_structured_form_search_reader_open_reviewed(server, catalogue, &first, out);
  } else {
    if (!strcmp(name, "nested-load"))
      OK(umi_data_server_begin(server));
    status =
        umi_structured_form_search_reader_load(server, catalogue,
                                               !strcmp(name, "missing")     ? "form.search.absent"
                                               : !strcmp(name, "namespace") ? "other.example"
                                                                            : metadata.entity_id,
                                               !strcmp(name, "historical") ? 1 : 0, out);
  }
  bool valid = !strcmp(name, "decode") || !strcmp(name, "latest") || !strcmp(name, "historical") ||
               !strcmp(name, "reviewed") || !strcmp(name, "alias");
  if (!valid) {
    CHECK(status != UMI_STATUS_OK && !memcmp(out, before, sizeof *out));
    if (!strncmp(name, "nested-", 7))
      CHECK(status == UMI_STATUS_BUSY && umi_data_server_in_transaction(server));
    goto cleanup;
  }
  OK(status);
  CHECK(out->resolved.schema == &f->schema &&
        out->record.revision == (!strcmp(name, "latest") ? 2 : 1));
  CHECK(!strcmp(out->resolved.preset.title,
                !strcmp(name, "latest") ? "Second search" : "First search"));
  if (!strcmp(name, "alias")) {
    size_t size = sizeof *document > sizeof *out ? sizeof *document : sizeof *out;
    alias = malloc(size);
    CHECK(alias);
    memcpy(alias, document, sizeof *document);
    OK(umi_structured_form_search_reader_decode(catalogue, alias, alias));
    CHECK(((UmiStructuredFormHistoricalSearch *)alias)->resolved.schema == &f->schema);
  }
cleanup:
  if (server && umi_data_server_in_transaction(server))
    (void)umi_data_server_rollback(server);
  umi_data_server_destroy(server);
  umi_structured_form_catalogue_destroy(catalogue);
  free(preset);
  free(out);
  free(before);
  free(document);
  free(alias);
  free(f);
  return result;
}
