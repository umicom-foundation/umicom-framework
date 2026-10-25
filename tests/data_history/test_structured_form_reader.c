/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_structured_form_reader.c
 *
 * PURPOSE:
 *   A trusted historical schema and stored integrity must both match before open.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A trusted historical schema and stored integrity must both match before open. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_reader.h"
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
  const char *cases[] = {"history", "latest",   "unknown",   "corrupt", "reviewed",
                         "stale",   "metadata", "unchanged", "alias",   "missing"};
  if (argc != 2 && argc != 3)
    return 2;
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected || (argc == 3 && strcmp(argv[2], "sqlite") != 0))
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new(), *g = form_fixture_new();
  UmiDataServer *store = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredFormHistorical *out = malloc(sizeof(*out));
  UmiRecordDocument *document = malloc(sizeof(*document));
  unsigned char *before = malloc(sizeof(*out));
  CHECK(f && g && out && document && before);
  g->schema.version = 2;
  OK(umi_structured_form_init(&g->schema, &g->before));
  const UmiStructuredFormSchema *schemas[] = {&g->schema, &f->schema};
  OK(umi_structured_form_catalogue_create(schemas, strcmp(name, "unknown") == 0 ? 1 : 2,
                                          &catalogue));
  if (argc == 3)
    OK(umi_data_server_create_sqlite(":memory:", &store));
  else
    OK(umi_data_server_create_memory(&store));
  UmiRecordAmendment metadata = {.entity_id = "read.parcel",
                                 .source = "Synthetic fixture",
                                 .actor = "Example author",
                                 .reason = "Original",
                                 .recorded_at_nanoseconds = 1U};
  UmiRecordRevision first, second;
  UmiFormAnswer answer = form_mass(9);
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
  OK(umi_structured_form_record_save(store, &metadata, &f->schema, &f->before, 0, &first));
  OK(umi_record_document_latest(store, "read.parcel", document));
  memset(out, 0xa5, sizeof(*out));
  memcpy(before, out, sizeof(*out));
  if (strcmp(name, "unknown") == 0) {
    CHECK(umi_structured_form_reader_decode(catalogue, document, out) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(before, out, sizeof(*out)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "corrupt") == 0) {
    document->content[0] ^= 1;
    CHECK(umi_structured_form_reader_decode(catalogue, document, out) != UMI_STATUS_OK);
    CHECK(memcmp(before, out, sizeof(*out)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "metadata") == 0) {
    document->record.revision = 0;
    CHECK(umi_structured_form_reader_decode(catalogue, document, out) != UMI_STATUS_OK);
    CHECK(memcmp(before, out, sizeof(*out)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "missing") == 0) {
    CHECK(umi_structured_form_reader_load(store, catalogue, "read.absent", 0, out) ==
          UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(before, out, sizeof(*out)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "alias") == 0) {
    CHECK(sizeof(*out) >= sizeof(*document));
    memcpy(out, document, sizeof(*document));
    OK(umi_structured_form_reader_decode(catalogue, (const UmiRecordDocument *)out, out));
    CHECK(out->resolved.form.answers[0].value.decimal_value == 9);
    goto cleanup;
  }
  if (strcmp(name, "reviewed") == 0) {
    OK(umi_structured_form_reader_open_reviewed(store, catalogue, &first, out));
    CHECK(out->resolved.schema == &f->schema && out->record.revision == 1);
    goto cleanup;
  }
  if (strcmp(name, "unchanged") == 0) {
    CHECK(umi_structured_form_reader_load(store, catalogue, "read.parcel", 99, out) ==
          UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(before, out, sizeof(*out)) == 0);
    goto cleanup;
  }
  answer = form_mass(15);
  OK(umi_structured_form_set(&g->schema, &g->before, g->before.revision, "parcel.mass", &answer));
  OK(umi_structured_form_record_save(store, &metadata, &g->schema, &g->before, 1, &second));
  if (strcmp(name, "stale") == 0) {
    CHECK(umi_structured_form_reader_open_reviewed(store, catalogue, &first, out) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(before, out, sizeof(*out)) == 0);
    goto cleanup;
  }
  OK(umi_structured_form_reader_load(store, catalogue, "read.parcel",
                                     strcmp(name, "history") == 0 ? 1 : 0, out));
  CHECK(out->record.revision == (strcmp(name, "history") == 0 ? 1U : 2U));
  CHECK(out->resolved.schema == (strcmp(name, "history") == 0 ? &f->schema : &g->schema));
  CHECK(out->resolved.form.answers[0].value.decimal_value ==
        (strcmp(name, "history") == 0 ? 9 : 15));
  CHECK(out->resolved.form.saved_revision == out->resolved.form.revision);
cleanup:
  umi_data_server_destroy(store);
  umi_structured_form_catalogue_destroy(catalogue);
  free(before);
  free(document);
  free(out);
  free(g);
  free(f);
  return result;
}
