/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Preset revisions and query scopes share the same checked record boundary. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_search_preset.h"
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
  const char *cases[] = {"create",         "amend",
                         "historical",     "stale",
                         "collision",      "missing",
                         "namespace",      "empty-key",
                         "key-character",  "key-length",
                         "unclosed-id",    "invalid-preset",
                         "wrong-schema",   "reviewed",
                         "reviewed-stale", "reviewed-metadata",
                         "malformed",      "malformed-amend",
                         "metadata-only",  "nested-save",
                         "nested-load",    "query",
                         "query-zero",     "query-large",
                         "query-alias",    "null"};
  if (argc != 2 && argc != 3)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known || (argc == 3 && strcmp(argv[2], "sqlite")))
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  UmiDataServer *server = NULL;
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormSearchPresetRecord *loaded = malloc(sizeof *loaded),
                                      *sentinel = malloc(sizeof *sentinel);
  UmiStructuredFormQuery *query = malloc(sizeof *query);
  void *alias = calloc(1, sizeof *preset + sizeof *query);
  CHECK(f && preset && loaded && sentinel && query && alias);
  if (argc == 3)
    OK(umi_data_server_create_sqlite(":memory:", &server));
  else
    OK(umi_data_server_create_memory(&server));
  strcpy(preset->title, "Small parcels");
  strcpy(preset->entity_prefix, "parcel.record.");
  strcpy(preset->metadata_text, "fictional");
  preset->filter.count = 1;
  strcpy(preset->filter.terms[0].field_id, "parcel.mass");
  preset->filter.terms[0].operation = UMI_FORM_FILTER_LESS;
  preset->filter.terms[0].operand = form_mass(10);
  UmiRecordAmendment metadata = {.entity_id = "form.search.small",
                                 .source = "Synthetic presets",
                                 .actor = "Example author",
                                 .reason = "Store reusable search",
                                 .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved, before;
  memset(&before, 0x6c, sizeof before);
  saved = before;
  memset(sentinel, 0x3d, sizeof *sentinel);
  *loaded = *sentinel;
  if (strncmp(name, "query", 5) == 0) {
    memset(query, 0x7a, sizeof *query);
    UmiStructuredFormQuery *original = malloc(sizeof *original);
    CHECK(original);
    *original = *query;
    size_t limit = strcmp(name, "query-zero") == 0 ? 0 : strcmp(name, "query-large") == 0 ? 17 : 4;
    UmiStatus status = umi_structured_form_query_from_preset(&f->schema, preset, limit, query);
    bool valid = limit > 0 && limit <= 16;
    if (!valid) {
      bool unchanged = memcmp(query, original, sizeof *query) == 0;
      free(original);
      CHECK(status != UMI_STATUS_OK && unchanged);
    } else {
      free(original);
      OK(status);
      CHECK(query->limit == 4 && !query->after_entity[0] && !query->expected_snapshot[0]);
      CHECK(strcmp(query->entity_prefix, preset->entity_prefix) == 0 &&
            strcmp(query->metadata_text, preset->metadata_text) == 0 &&
            query->schema_version == f->schema.version);
    }
    if (strcmp(name, "query-alias") == 0) {
      memcpy(alias, preset, sizeof *preset);
      OK(umi_structured_form_query_from_preset(&f->schema, alias, 4, alias));
      CHECK(strcmp(((UmiStructuredFormQuery *)alias)->entity_prefix, "parcel.record.") == 0);
    }
    goto cleanup;
  }
  if (strcmp(name, "null") == 0) {
    CHECK(umi_structured_form_search_preset_save(NULL, &metadata, &f->schema, preset, 0, &saved) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_search_preset_load(server, NULL, 0, &f->schema, loaded) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_search_preset_open_reviewed(server, NULL, &f->schema, loaded) !=
          UMI_STATUS_OK);
    CHECK(memcmp(loaded, sentinel, sizeof *loaded) == 0);
    goto cleanup;
  }
  bool invalid = false;
  if (strcmp(name, "namespace") == 0) {
    strcpy(metadata.entity_id, "parcel.record.small");
    invalid = true;
  }
  if (strcmp(name, "empty-key") == 0) {
    strcpy(metadata.entity_id, UMI_FORM_SEARCH_PRESET_PREFIX);
    invalid = true;
  }
  if (strcmp(name, "key-character") == 0) {
    strcpy(metadata.entity_id, "form.search.a.b");
    invalid = true;
  }
  if (strcmp(name, "key-length") == 0) {
    strcpy(metadata.entity_id, UMI_FORM_SEARCH_PRESET_PREFIX);
    memset(metadata.entity_id + sizeof(UMI_FORM_SEARCH_PRESET_PREFIX) - 1, 'x', 41);
    metadata.entity_id[sizeof(UMI_FORM_SEARCH_PRESET_PREFIX) - 1 + 41] = 0;
    invalid = true;
  }
  if (strcmp(name, "unclosed-id") == 0) {
    memset(metadata.entity_id, 'x', sizeof metadata.entity_id);
    invalid = true;
  }
  if (strcmp(name, "invalid-preset") == 0) {
    preset->title[0] = 0;
    invalid = true;
  }
  if (strcmp(name, "missing") == 0) {
    CHECK(umi_structured_form_search_preset_load(server, metadata.entity_id, 0, &f->schema,
                                                 loaded) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(loaded, sentinel, sizeof *loaded) == 0);
    goto cleanup;
  }
  if (strcmp(name, "nested-save") == 0) {
    OK(umi_data_server_begin(server));
    invalid = true;
  }
  if (strcmp(name, "malformed") == 0 || strcmp(name, "malformed-amend") == 0) {
    OK(umi_record_document_append(server, &metadata, "other document", 14, 0, &saved));
    if (strcmp(name, "malformed-amend") == 0) {
      saved = before;
      CHECK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 1,
                                                   &saved) != UMI_STATUS_OK);
      CHECK(memcmp(&saved, &before, sizeof saved) == 0);
    }
    CHECK(umi_structured_form_search_preset_load(server, metadata.entity_id, 0, &f->schema,
                                                 loaded) != UMI_STATUS_OK);
    CHECK(memcmp(loaded, sentinel, sizeof *loaded) == 0);
    goto cleanup;
  }
  if (strcmp(name, "metadata-only") == 0) {
    memset(metadata.content_sha256, '0', 64);
    metadata.content_sha256[64] = 0;
    OK(umi_record_history_append(server, &metadata, 0, &saved));
    saved = before;
    CHECK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 1,
                                                 &saved) != UMI_STATUS_OK);
    CHECK(memcmp(&saved, &before, sizeof saved) == 0);
    goto cleanup;
  }
  UmiStatus status =
      umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 0, &saved);
  if (invalid) {
    CHECK(status != UMI_STATUS_OK && memcmp(&saved, &before, sizeof saved) == 0);
    if (strcmp(name, "nested-save") == 0) {
      CHECK(status == UMI_STATUS_BUSY && umi_data_server_in_transaction(server));
      OK(umi_data_server_rollback(server));
    }
    goto cleanup;
  }
  OK(status);
  CHECK(saved.revision == 1);
  UmiRecordRevision reviewed = saved;
  if (strcmp(name, "collision") == 0) {
    CHECK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 0,
                                                 &saved) != UMI_STATUS_OK);
    CHECK(saved.revision == 1);
    goto cleanup;
  }
  if (strcmp(name, "nested-load") == 0) {
    OK(umi_data_server_begin(server));
    CHECK(umi_structured_form_search_preset_load(server, metadata.entity_id, 0, &f->schema,
                                                 loaded) == UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(server));
    OK(umi_data_server_rollback(server));
    CHECK(memcmp(loaded, sentinel, sizeof *loaded) == 0);
    goto cleanup;
  }
  if (strcmp(name, "amend") == 0 || strcmp(name, "historical") == 0 || strcmp(name, "stale") == 0 ||
      strcmp(name, "reviewed-stale") == 0) {
    strcpy(preset->title, "Changed search");
    OK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 1, &saved));
    CHECK(saved.revision == 2);
  }
  if (strcmp(name, "stale") == 0) {
    saved = before;
    CHECK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 1,
                                                 &saved) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&saved, &before, sizeof saved) == 0);
    goto cleanup;
  }
  if (strcmp(name, "wrong-schema") == 0)
    ++f->schema.version;
  if (strcmp(name, "reviewed-metadata") == 0)
    strcpy(reviewed.amendment.reason, "Unreviewed replacement");
  status =
      strncmp(name, "reviewed", 8) == 0
          ? umi_structured_form_search_preset_open_reviewed(server, &reviewed, &f->schema, loaded)
          : umi_structured_form_search_preset_load(server, metadata.entity_id,
                                                   strcmp(name, "historical") == 0 ? 1 : 0,
                                                   &f->schema, loaded);
  if (strcmp(name, "wrong-schema") == 0 || strcmp(name, "reviewed-stale") == 0 ||
      strcmp(name, "reviewed-metadata") == 0) {
    CHECK(status != UMI_STATUS_OK && memcmp(loaded, sentinel, sizeof *loaded) == 0);
    goto cleanup;
  }
  OK(status);
  CHECK(strcmp(loaded->preset.title,
               strcmp(name, "amend") == 0 ? "Changed search" : "Small parcels") == 0);
  uint64_t revisions = 0;
  OK(umi_record_document_verify(server, metadata.entity_id, &revisions));
  CHECK(revisions == (strcmp(name, "amend") == 0 || strcmp(name, "historical") == 0 ? 2U : 1U));
cleanup:
  if (server && umi_data_server_in_transaction(server))
    (void)umi_data_server_rollback(server);
  umi_data_server_destroy(server);
  free(f);
  free(preset);
  free(loaded);
  free(sentinel);
  free(query);
  free(alias);
  return result;
}
