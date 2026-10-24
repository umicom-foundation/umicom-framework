/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Typed queries reject partial discovery and stale continuation identities. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_query.h"
#include "umicom/data/structured_form_record.h"
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
  const char *cases[] = {"all",
                         "filter",
                         "pages",
                         "empty",
                         "metadata",
                         "metadata-none",
                         "state",
                         "version",
                         "foreign-form",
                         "unknown-schema",
                         "unregistered-query",
                         "payload",
                         "metadata-only",
                         "excluded-damage",
                         "stale",
                         "changed-filter",
                         "changed-metadata",
                         "cursor",
                         "nested",
                         "empty-prefix",
                         "limit-zero",
                         "limit-large",
                         "bad-snapshot",
                         "cursor-without-snapshot",
                         "invalid-filter",
                         "null",
                         "reviewed-open",
                         "reopen-stale"};
  if (argc != 2 && argc != 3)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known || (argc == 3 && strcmp(argv[2], "sqlite") != 0))
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new(), *g = form_fixture_new();
  UmiDataServer *server = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredFormQuery *query = calloc(1, sizeof *query);
  UmiStructuredFormQueryPage *page = malloc(sizeof *page), *sentinel = malloc(sizeof *sentinel);
  UmiStructuredFormHistorical *opened = malloc(sizeof *opened);
  CHECK(f && g && query && page && sentinel && opened);
  memset(sentinel, 0x6a, sizeof *sentinel);
  *page = *sentinel;
  g->schema.version = strcmp(name, "unknown-schema") == 0 ? 3 : 2;
  if (strcmp(name, "foreign-form") == 0)
    g->base.form_id = "foreign.parcel";
  OK(umi_structured_form_init(&g->schema, &g->before));
  const UmiStructuredFormSchema *schemas[] = {&f->schema, &g->schema};
  OK(umi_structured_form_catalogue_create(schemas, strcmp(name, "unknown-schema") == 0 ? 1 : 2,
                                          &catalogue));
  if (argc == 3)
    OK(umi_data_server_create_sqlite(":memory:", &server));
  else
    OK(umi_data_server_create_memory(&server));
  UmiRecordAmendment metadata = {.source = "Fictional inventory",
                                 .actor = "keeper",
                                 .reason = "Example",
                                 .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  size_t count = strcmp(name, "empty") == 0 ? 0 : 20;
  bool alternate = strcmp(name, "version") == 0 || strcmp(name, "foreign-form") == 0 ||
                   strcmp(name, "unknown-schema") == 0;
  for (size_t i = 1; i <= count; ++i) {
    (void)snprintf(metadata.entity_id, sizeof metadata.entity_id, "search.parcel.%03zu", i);
    strcpy(metadata.actor, i % 2 == 0 ? "keeper" : "other");
    FormFixture *owner = alternate && i == 20 ? g : f;
    UmiFormAnswer answer = form_mass((double)i);
    OK(umi_structured_form_set(&owner->schema, &owner->before, owner->before.revision,
                               "parcel.mass", &answer));
    if ((strcmp(name, "payload") == 0 && i == 20) ||
        (strcmp(name, "excluded-damage") == 0 && i == 19))
      OK(umi_record_document_append(server, &metadata, "invalid form", 12, 0, &saved));
    else if (strcmp(name, "metadata-only") == 0 && i == 20) {
      memset(metadata.content_sha256, 'a', 64);
      metadata.content_sha256[64] = '\0';
      OK(umi_record_history_append(server, &metadata, 0, &saved));
    } else
      OK(umi_structured_form_record_save(server, &metadata, &owner->schema, &owner->before, 0,
                                         &saved));
  }
  strcpy(query->entity_prefix, "search.parcel.");
  strcpy(query->form_id, f->base.form_id);
  query->schema_version = 1;
  query->limit = 16;
  if (strcmp(name, "filter") == 0 || strcmp(name, "pages") == 0 ||
      strcmp(name, "changed-filter") == 0) {
    query->filter.count = 1;
    strcpy(query->filter.terms[0].field_id, "parcel.mass");
    query->filter.terms[0].operation = UMI_FORM_FILTER_GREATER;
    query->filter.terms[0].operand = form_mass(10);
  }
  if (strcmp(name, "state") == 0) {
    query->filter.count = 1;
    strcpy(query->filter.terms[0].field_id, "parcel.note");
    query->filter.terms[0].operation = UMI_FORM_FILTER_STATE;
    query->filter.terms[0].operand.state = UMI_FORM_UNANSWERED;
  }
  if (strcmp(name, "version") == 0)
    query->schema_version = 2;
  if (strcmp(name, "metadata") == 0 || strcmp(name, "excluded-damage") == 0)
    strcpy(query->metadata_text, "keeper");
  if (strcmp(name, "metadata-none") == 0)
    strcpy(query->metadata_text, "unmatched");
  if (strcmp(name, "pages") == 0)
    query->limit = 3;
  bool invalid = strcmp(name, "unknown-schema") == 0 || strcmp(name, "payload") == 0 ||
                 strcmp(name, "metadata-only") == 0;
  if (strcmp(name, "unregistered-query") == 0) {
    query->schema_version = 99;
    invalid = true;
  }
  if (strcmp(name, "empty-prefix") == 0) {
    query->entity_prefix[0] = '\0';
    invalid = true;
  }
  if (strcmp(name, "limit-zero") == 0) {
    query->limit = 0;
    invalid = true;
  }
  if (strcmp(name, "limit-large") == 0) {
    query->limit = 17;
    invalid = true;
  }
  if (strcmp(name, "bad-snapshot") == 0) {
    strcpy(query->expected_snapshot, "bad");
    invalid = true;
  }
  if (strcmp(name, "cursor-without-snapshot") == 0) {
    strcpy(query->after_entity, "search.parcel.001");
    invalid = true;
  }
  if (strcmp(name, "invalid-filter") == 0) {
    query->filter.count = 9;
    invalid = true;
  }
  if (strcmp(name, "nested") == 0) {
    OK(umi_data_server_begin(server));
    invalid = true;
  }
  if (strcmp(name, "null") == 0) {
    CHECK(umi_structured_form_query_page(NULL, catalogue, query, page) != UMI_STATUS_OK);
    CHECK(umi_structured_form_query_page(server, NULL, query, page) != UMI_STATUS_OK);
    CHECK(umi_structured_form_query_page(server, catalogue, NULL, page) != UMI_STATUS_OK);
    CHECK(umi_structured_form_query_page(server, catalogue, query, NULL) != UMI_STATUS_OK);
    CHECK(memcmp(page, sentinel, sizeof *page) == 0);
    goto cleanup;
  }
  UmiStatus status = umi_structured_form_query_page(server, catalogue, query, page);
  if (invalid) {
    CHECK(status != UMI_STATUS_OK && memcmp(page, sentinel, sizeof *page) == 0);
    if (strcmp(name, "nested") == 0) {
      CHECK(status == UMI_STATUS_BUSY && umi_data_server_in_transaction(server));
      OK(umi_data_server_rollback(server));
    }
    goto cleanup;
  }
  OK(status);
  size_t expected =
      strcmp(name, "empty") == 0 || strcmp(name, "metadata-none") == 0                   ? 0
      : strcmp(name, "version") == 0                                                     ? 1
      : strcmp(name, "foreign-form") == 0                                                ? 19
      : query->filter.count && query->filter.terms[0].operation != UMI_FORM_FILTER_STATE ? 10
      : query->metadata_text[0]                                                          ? 10
                                                                                         : 20;
  CHECK(page->matching_count == expected && page->namespace_count == count);
  CHECK(page->count == (expected < query->limit ? expected : query->limit));
  if (strcmp(name, "pages") == 0) {
    size_t total = page->count;
    char previous[64] = {0};
    while (page->next_after_entity[0]) {
      strcpy(previous, page->next_after_entity);
      strcpy(query->after_entity, previous);
      strcpy(query->expected_snapshot, page->snapshot);
      OK(umi_structured_form_query_page(server, catalogue, query, page));
      CHECK(page->count && strcmp(page->rows[0].amendment.entity_id, previous) > 0);
      CHECK(page->matching_count == 10);
      total += page->count;
      CHECK(total <= 10);
    }
    CHECK(total == 10);
  }
  if (strcmp(name, "reviewed-open") == 0) {
    OK(umi_structured_form_reader_open_reviewed(server, catalogue, &page->rows[0], opened));
    CHECK(opened->resolved.form.answers[0].value.decimal_value == 1);
  }
  bool stale = strcmp(name, "stale") == 0 || strcmp(name, "reopen-stale") == 0;
  if (stale || strcmp(name, "changed-filter") == 0 || strcmp(name, "changed-metadata") == 0 ||
      strcmp(name, "cursor") == 0) {
    UmiRecordRevision reviewed = page->rows[0];
    strcpy(query->expected_snapshot, page->snapshot);
    strcpy(query->after_entity, page->rows[0].amendment.entity_id);
    if (stale) {
      strcpy(metadata.entity_id, reviewed.amendment.entity_id);
      metadata.recorded_at_nanoseconds = 2;
      OK(umi_structured_form_record_save(server, &metadata, &f->schema, &f->before,
                                         reviewed.revision, &saved));
    }
    if (strcmp(name, "changed-filter") == 0)
      query->filter.terms[0].operand = form_mass(15);
    if (strcmp(name, "changed-metadata") == 0)
      strcpy(query->metadata_text, "keeper");
    if (strcmp(name, "cursor") == 0)
      strcpy(query->after_entity, "search.parcel.absent");
    *page = *sentinel;
    CHECK(umi_structured_form_query_page(server, catalogue, query, page) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(page, sentinel, sizeof *page) == 0);
    if (strcmp(name, "reopen-stale") == 0)
      CHECK(umi_structured_form_reader_open_reviewed(server, catalogue, &reviewed, opened) ==
            UMI_STATUS_INVALID_STATE);
  }
cleanup:
  if (server && umi_data_server_in_transaction(server))
    (void)umi_data_server_rollback(server);
  umi_data_server_destroy(server);
  umi_structured_form_catalogue_destroy(catalogue);
  free(opened);
  free(sentinel);
  free(page);
  free(query);
  free(g);
  free(f);
  return result;
}
