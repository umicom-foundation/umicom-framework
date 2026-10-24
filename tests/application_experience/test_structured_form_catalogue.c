/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Historical lookup must never reinterpret an unknown version as the latest. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/application/experience/structured_form_catalogue.h"
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
      "empty",       "invalid",     "duplicate",        "ordered",        "exact",
      "latest",      "array-copy",  "capacity",         "overflow",       "unknown-version",
      "unknown-id",  "roundtrip",   "trailing",         "body-truncated", "header-truncated",
      "magic",       "id-null",     "id-token",         "zero-version",   "zero-revision",
      "saved-ahead", "field-count", "output-unchanged", "alias",          "bad-index"};
  if (argc != 2)
    return 2;
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected)
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new(), *g = form_fixture_new();
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredFormResolved *resolved = malloc(sizeof(*resolved));
  unsigned char *bytes = malloc(UMI_FORM_SNAPSHOT_MAX_BYTES), *saved = malloc(sizeof(*resolved));
  UmiStructuredFormSchema versions[UMI_FORM_CATALOGUE_CAPACITY];
  const UmiStructuredFormSchema *list[UMI_FORM_CATALOGUE_CAPACITY + 1], *found = NULL;
  CHECK(f && g && resolved && bytes && saved);
  g->schema.version = 2;
  list[0] = &g->schema;
  list[1] = &f->schema;
  if (strcmp(name, "empty") == 0) {
    OK(umi_structured_form_catalogue_create(NULL, 0, &catalogue));
    CHECK(umi_structured_form_catalogue_count(catalogue) == 0);
    CHECK(umi_structured_form_catalogue_find(catalogue, "example.parcel", 1, &found) ==
              UMI_STATUS_NOT_FOUND &&
          found == NULL);
    goto cleanup;
  }
  if (strcmp(name, "invalid") == 0) {
    g->schema.rule_count = 1;
    CHECK(umi_structured_form_catalogue_create(list, 2, &catalogue) != UMI_STATUS_OK &&
          catalogue == NULL);
    goto cleanup;
  }
  if (strcmp(name, "duplicate") == 0) {
    g->schema.version = 1;
    CHECK(umi_structured_form_catalogue_create(list, 2, &catalogue) == UMI_STATUS_ALREADY_EXISTS &&
          catalogue == NULL);
    goto cleanup;
  }
  if (strcmp(name, "capacity") == 0 || strcmp(name, "overflow") == 0) {
    for (size_t i = 0; i < UMI_FORM_CATALOGUE_CAPACITY; ++i) {
      versions[i] = f->schema;
      versions[i].version = i + 1;
      list[i] = &versions[i];
    }
    if (strcmp(name, "overflow") == 0) {
      CHECK(umi_structured_form_catalogue_create(list, UMI_FORM_CATALOGUE_CAPACITY + 1,
                                                 &catalogue) == UMI_STATUS_CAPACITY_EXCEEDED &&
            catalogue == NULL);
      goto cleanup;
    }
    OK(umi_structured_form_catalogue_create(list, UMI_FORM_CATALOGUE_CAPACITY, &catalogue));
    CHECK(umi_structured_form_catalogue_count(catalogue) == UMI_FORM_CATALOGUE_CAPACITY);
    OK(umi_structured_form_catalogue_latest(catalogue, "example.parcel", &found));
    CHECK(found->version == UMI_FORM_CATALOGUE_CAPACITY);
    goto cleanup;
  }
  OK(umi_structured_form_catalogue_create(list, 2, &catalogue));
  CHECK(umi_structured_form_catalogue_count(catalogue) == 2);
  if (strcmp(name, "ordered") == 0) {
    OK(umi_structured_form_catalogue_at(catalogue, 0, &found));
    CHECK(found == &f->schema);
    OK(umi_structured_form_catalogue_at(catalogue, 1, &found));
    CHECK(found == &g->schema);
    goto cleanup;
  }
  if (strcmp(name, "exact") == 0) {
    OK(umi_structured_form_catalogue_find(catalogue, "example.parcel", 1, &found));
    CHECK(found == &f->schema);
    goto cleanup;
  }
  if (strcmp(name, "latest") == 0) {
    OK(umi_structured_form_catalogue_latest(catalogue, "example.parcel", &found));
    CHECK(found == &g->schema);
    goto cleanup;
  }
  if (strcmp(name, "array-copy") == 0) {
    list[0] = list[1] = NULL;
    OK(umi_structured_form_catalogue_find(catalogue, "example.parcel", 2, &found));
    CHECK(found == &g->schema);
    goto cleanup;
  }
  if (strcmp(name, "bad-index") == 0) {
    found = &f->schema;
    CHECK(umi_structured_form_catalogue_at(catalogue, 2, &found) == UMI_STATUS_NOT_FOUND &&
          found == &f->schema);
    goto cleanup;
  }
  UmiFormAnswer mass = form_mass(9);
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &mass));
  size_t length = 0;
  OK(umi_structured_form_snapshot_encode(&f->schema, &f->before, bytes, UMI_FORM_SNAPSHOT_MAX_BYTES,
                                         &length));
  size_t version_at = 10 + strlen(f->base.form_id), header_end = version_at + 26;
  memset(resolved, 0xa5, sizeof(*resolved));
  memcpy(saved, resolved, sizeof(*resolved));
  UmiStructuredFormSnapshotIdentity identity;
  memset(&identity, 0x5a, sizeof(identity));
  if (strcmp(name, "header-truncated") == 0) {
    for (size_t n = 0; n < header_end; ++n) {
      UmiStructuredFormSnapshotIdentity before = identity;
      CHECK(umi_structured_form_snapshot_identity(bytes, n, &identity) != UMI_STATUS_OK);
      CHECK(memcmp(&before, &identity, sizeof(identity)) == 0);
    }
    goto cleanup;
  }
  if (strcmp(name, "unknown-version") == 0)
    bytes[version_at + 7] = 3;
  if (strcmp(name, "unknown-id") == 0)
    bytes[10] = 'a';
  if (strcmp(name, "trailing") == 0)
    bytes[length++] = 0;
  if (strcmp(name, "body-truncated") == 0)
    --length;
  if (strcmp(name, "magic") == 0)
    bytes[0] = 'X';
  if (strcmp(name, "id-null") == 0)
    bytes[10] = 0;
  if (strcmp(name, "id-token") == 0)
    bytes[10] = '/';
  if (strcmp(name, "zero-version") == 0)
    memset(bytes + version_at, 0, 8);
  if (strcmp(name, "zero-revision") == 0)
    memset(bytes + version_at + 8, 0, 8);
  if (strcmp(name, "saved-ahead") == 0)
    bytes[version_at + 23] = 99;
  if (strcmp(name, "field-count") == 0)
    bytes[version_at + 25] = UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY + 1;
  if (strcmp(name, "output-unchanged") == 0) {
    bytes[length++] = 42;
    CHECK(umi_structured_form_catalogue_decode(catalogue, bytes, length, resolved) !=
          UMI_STATUS_OK);
    CHECK(memcmp(saved, resolved, sizeof(*resolved)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "roundtrip") != 0 && strcmp(name, "alias") != 0) {
    CHECK(umi_structured_form_catalogue_decode(catalogue, bytes, length, resolved) !=
          UMI_STATUS_OK);
    CHECK(memcmp(saved, resolved, sizeof(*resolved)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "alias") == 0) {
    CHECK(length <= sizeof(*resolved));
    memcpy(resolved, bytes, length);
    OK(umi_structured_form_catalogue_decode(catalogue, resolved, length, resolved));
  } else
    OK(umi_structured_form_catalogue_decode(catalogue, bytes, length, resolved));
  CHECK(resolved->schema == &f->schema && resolved->form.answers[0].value.decimal_value == 9 &&
        resolved->form.schema_version == 1);
cleanup:
  umi_structured_form_catalogue_destroy(catalogue);
  free(saved);
  free(bytes);
  free(resolved);
  free(g);
  free(f);
  return result;
}
