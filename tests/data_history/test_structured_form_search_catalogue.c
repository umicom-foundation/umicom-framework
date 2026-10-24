/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Discovery validates excluded definitions and binds pagination to reviewed heads. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_search_catalogue.h"
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
  const char *cases[] = {"empty",
                         "all",
                         "pages",
                         "title",
                         "key",
                         "case-sensitive",
                         "namespace",
                         "version",
                         "foreign-form",
                         "unknown-version",
                         "malformed",
                         "excluded-damage",
                         "metadata-only",
                         "limit-zero",
                         "limit-large",
                         "bad-snapshot",
                         "cursor-without-snapshot",
                         "unclosed-query",
                         "bad-prefix",
                         "unregistered-query",
                         "nested",
                         "changed-title",
                         "changed-scope",
                         "changed-version",
                         "changed-head",
                         "new-head",
                         "cursor-missing",
                         "cursor-filtered",
                         "last-cursor",
                         "reviewed",
                         "reviewed-stale",
                         "capacity",
                         "alias",
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
  FormFixture *f = form_fixture_new(), *g = form_fixture_new();
  UmiDataServer *server = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormSearchQuery *query = calloc(1, sizeof *query);
  UmiStructuredFormSearchPage *page = malloc(sizeof *page), *before = malloc(sizeof *before);
  UmiStructuredFormHistoricalSearch *opened = malloc(sizeof *opened);
  void *alias = NULL;
  CHECK(f && g && preset && query && page && before && opened);
  g->schema.version = 2;
  if (!strcmp(name, "foreign-form"))
    g->base.form_id = "example.other";
  const UmiStructuredFormSchema *schemas[] = {&f->schema, &g->schema};
  OK(umi_structured_form_catalogue_create(schemas, !strcmp(name, "unknown-version") ? 1 : 2,
                                          &catalogue));
  if (argc == 3)
    OK(umi_data_server_create_sqlite(":memory:", &server));
  else
    OK(umi_data_server_create_memory(&server));
  strcpy(preset->entity_prefix, "parcel.record.");
  preset->filter.count = 1;
  strcpy(preset->filter.terms[0].field_id, "parcel.mass");
  preset->filter.terms[0].operation = UMI_FORM_FILTER_LESS;
  preset->filter.terms[0].operand = form_mass(10);
  UmiRecordAmendment metadata = {.source = "Fictional searches",
                                 .actor = "Example author",
                                 .reason = "Store query",
                                 .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  size_t count = !strcmp(name, "empty") ? 0 : !strcmp(name, "capacity") ? 257 : 20;
  for (size_t i = 1; i <= count; ++i) {
    (void)snprintf(metadata.entity_id, sizeof metadata.entity_id, "form.search.%03zu", i);
    strcpy(preset->title, i % 2 ? "Bulk parcels" : "Small parcels");
    if (!strcmp(name, "namespace") && i == 20)
      strcpy(preset->entity_prefix, "other.record.");
    bool other = i == 20 && (!strcmp(name, "version") || !strcmp(name, "foreign-form") ||
                             !strcmp(name, "unknown-version"));
    if ((!strcmp(name, "malformed") && i == 20) || (!strcmp(name, "excluded-damage") && i == 19))
      OK(umi_record_document_append(server, &metadata, "bad", 3, 0, &saved));
    else if (!strcmp(name, "metadata-only") && i == 20) {
      memset(metadata.content_sha256, 'a', 64);
      metadata.content_sha256[64] = 0;
      OK(umi_record_history_append(server, &metadata, 0, &saved));
    } else
      OK(umi_structured_form_search_preset_save(server, &metadata, other ? &g->schema : &f->schema,
                                                preset, 0, &saved));
  }
  strcpy(query->form_id, f->base.form_id);
  query->schema_version = !strcmp(name, "version") ? 2 : 1;
  strcpy(query->entity_prefix, "parcel.record.");
  query->limit = !strcmp(name, "pages") ? 3 : 16;
  if (!strcmp(name, "title") || !strcmp(name, "excluded-damage") ||
      !strcmp(name, "cursor-filtered"))
    strcpy(query->text, "Small");
  if (!strcmp(name, "key"))
    strcpy(query->text, "003");
  if (!strcmp(name, "case-sensitive"))
    strcpy(query->text, "small");
  bool invalid = !strcmp(name, "unknown-version") || !strcmp(name, "malformed") ||
                 !strcmp(name, "excluded-damage") || !strcmp(name, "metadata-only") ||
                 !strcmp(name, "capacity");
  if (!strcmp(name, "limit-zero")) {
    query->limit = 0;
    invalid = true;
  }
  if (!strcmp(name, "limit-large")) {
    query->limit = 17;
    invalid = true;
  }
  if (!strcmp(name, "bad-snapshot")) {
    strcpy(query->expected_snapshot, "bad");
    invalid = true;
  }
  if (!strcmp(name, "cursor-without-snapshot")) {
    strcpy(query->after_entity, "form.search.001");
    invalid = true;
  }
  if (!strcmp(name, "unclosed-query")) {
    memset(query->text, 'x', sizeof query->text);
    invalid = true;
  }
  if (!strcmp(name, "bad-prefix")) {
    strcpy(query->entity_prefix, "invalid/");
    invalid = true;
  }
  if (!strcmp(name, "unregistered-query")) {
    query->schema_version = 99;
    invalid = true;
  }
  if (!strcmp(name, "nested")) {
    OK(umi_data_server_begin(server));
    invalid = true;
  }
  memset(before, 0x49, sizeof *before);
  *page = *before;
  if (!strcmp(name, "null")) {
    CHECK(umi_structured_form_search_catalogue_page(NULL, catalogue, query, page) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_catalogue_page(server, NULL, query, page) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_catalogue_page(server, catalogue, NULL, page) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_search_catalogue_page(server, catalogue, query, NULL) !=
          UMI_STATUS_OK);
    CHECK(!memcmp(page, before, sizeof *page));
    goto cleanup;
  }
  UmiStatus status = umi_structured_form_search_catalogue_page(server, catalogue, query, page);
  if (invalid) {
    CHECK(status != UMI_STATUS_OK && !memcmp(page, before, sizeof *page));
    if (!strcmp(name, "nested"))
      CHECK(status == UMI_STATUS_BUSY && umi_data_server_in_transaction(server));
    goto cleanup;
  }
  OK(status);
  size_t expected = !strcmp(name, "empty") || !strcmp(name, "case-sensitive")     ? 0
                    : !strcmp(name, "key") || !strcmp(name, "version")            ? 1
                    : !strcmp(name, "namespace") || !strcmp(name, "foreign-form") ? 19
                    : query->text[0]                                              ? 10
                                                                                  : 20;
  CHECK(page->matching_count == expected && page->definition_count == count &&
        page->count == (expected < query->limit ? expected : query->limit));
  for (size_t i = 0; i < page->count; ++i)
    CHECK(page->rows[i].term_count == 1 && page->rows[i].title[0]);
  if (!strcmp(name, "pages")) {
    size_t total = page->count;
    while (page->next_after_entity[0]) {
      char previous[64];
      strcpy(previous, page->next_after_entity);
      strcpy(query->after_entity, previous);
      strcpy(query->expected_snapshot, page->snapshot);
      OK(umi_structured_form_search_catalogue_page(server, catalogue, query, page));
      CHECK(page->count && strcmp(page->rows[0].record.amendment.entity_id, previous) > 0);
      total += page->count;
      CHECK(total <= 20);
    }
    CHECK(total == 20);
  }
  if (!strcmp(name, "reviewed") || !strcmp(name, "reviewed-stale")) {
    if (!strcmp(name, "reviewed-stale")) {
      strcpy(metadata.entity_id, page->rows[0].record.amendment.entity_id);
      strcpy(preset->title, "Amended");
      metadata.recorded_at_nanoseconds = 2;
      OK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset, 1, &saved));
    }
    status = umi_structured_form_search_reader_open_reviewed(server, catalogue,
                                                             &page->rows[0].record, opened);
    if (!strcmp(name, "reviewed-stale"))
      CHECK(status == UMI_STATUS_INVALID_STATE);
    else {
      OK(status);
      CHECK(!strcmp(opened->resolved.preset.title, page->rows[0].title));
    }
  }
  if (!strncmp(name, "changed-", 8) || !strcmp(name, "new-head") || !strncmp(name, "cursor-", 7) ||
      !strcmp(name, "last-cursor")) {
    strcpy(query->after_entity, page->next_after_entity);
    strcpy(query->expected_snapshot, page->snapshot);
    if (!strcmp(name, "changed-title"))
      strcpy(query->text, "Small");
    if (!strcmp(name, "changed-scope"))
      strcpy(query->entity_prefix, "other.record.");
    if (!strcmp(name, "changed-version"))
      query->schema_version = 2;
    if (!strcmp(name, "changed-head") || !strcmp(name, "new-head")) {
      strcpy(metadata.entity_id, !strcmp(name, "new-head") ? "form.search.021" : "form.search.001");
      strcpy(preset->title, "Changed query");
      metadata.recorded_at_nanoseconds = 2;
      OK(umi_structured_form_search_preset_save(server, &metadata, &f->schema, preset,
                                                !strcmp(name, "new-head") ? 0 : 1, &saved));
    }
    if (!strcmp(name, "cursor-missing"))
      strcpy(query->after_entity, "form.search.missing");
    if (!strcmp(name, "cursor-filtered"))
      strcpy(query->after_entity, "form.search.001");
    if (!strcmp(name, "last-cursor"))
      strcpy(query->after_entity, "form.search.020");
    *before = *page;
    status = umi_structured_form_search_catalogue_page(server, catalogue, query, page);
    if (!strcmp(name, "last-cursor")) {
      OK(status);
      CHECK(!page->count && !page->next_after_entity[0] && page->matching_count == 20);
    } else
      CHECK(status == UMI_STATUS_INVALID_STATE && !memcmp(page, before, sizeof *page));
  }
  if (!strcmp(name, "alias")) {
    size_t size = sizeof *query > sizeof *page ? sizeof *query : sizeof *page;
    alias = malloc(size);
    CHECK(alias);
    memcpy(alias, query, sizeof *query);
    OK(umi_structured_form_search_catalogue_page(server, catalogue, alias, alias));
    CHECK(((UmiStructuredFormSearchPage *)alias)->matching_count == 20);
  }
cleanup:
  if (server && umi_data_server_in_transaction(server))
    (void)umi_data_server_rollback(server);
  umi_data_server_destroy(server);
  umi_structured_form_catalogue_destroy(catalogue);
  free(f);
  free(g);
  free(preset);
  free(query);
  free(page);
  free(before);
  free(opened);
  free(alias);
  return result;
}
