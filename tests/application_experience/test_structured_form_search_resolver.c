/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A bounded hint selects a schema but never bypasses complete validation. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/application/experience/structured_form_search_resolver.h"
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
  const char *cases[] = {"roundtrip",       "old-version",   "unknown-version", "unknown-form",
                         "empty-catalogue", "outer-magic",   "inner-magic",     "truncation",
                         "trailing",        "oversized",     "title-nul",       "title-empty",
                         "prefix-invalid",  "filter-length", "identity-length", "identity-nul",
                         "zero-version",    "term-count",    "unit-change",     "operand-invalid",
                         "alias",           "null"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  if (!known)
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new(), *g = form_fixture_new();
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormResolvedSearch *out = malloc(sizeof *out), *before = malloc(sizeof *before);
  unsigned char *bytes = malloc(UMI_FORM_SEARCH_PRESET_MAX_BYTES + 1);
  void *alias = NULL;
  CHECK(f && g && preset && out && before && bytes);
  g->schema.version = 2;
  if (!strcmp(name, "unknown-form"))
    g->base.form_id = "example.other";
  const UmiStructuredFormSchema *schemas[] = {&f->schema, &g->schema};
  size_t schemas_count = !strcmp(name, "empty-catalogue")                                    ? 0
                         : !strcmp(name, "unknown-version") || !strcmp(name, "unknown-form") ? 1
                                                                                             : 2;
  OK(umi_structured_form_catalogue_create(schemas, schemas_count, &catalogue));
  strcpy(preset->title, "Small parcels");
  strcpy(preset->entity_prefix, "parcel.record.");
  strcpy(preset->metadata_text, "example");
  preset->filter.count = 1;
  strcpy(preset->filter.terms[0].field_id, "parcel.mass");
  preset->filter.terms[0].operation = UMI_FORM_FILTER_LESS;
  preset->filter.terms[0].operand = form_mass(10);
  const UmiStructuredFormSchema *encoded =
      (!strcmp(name, "roundtrip") || !strcmp(name, "unknown-version") ||
       !strcmp(name, "unknown-form"))
          ? &g->schema
          : &f->schema;
  size_t length = 0;
  OK(umi_structured_form_search_preset_encode(encoded, preset, bytes,
                                              UMI_FORM_SEARCH_PRESET_MAX_BYTES, &length));
  size_t title = 10, prefix = title + strlen(preset->title) + 2,
         metadata = prefix + strlen(preset->entity_prefix) + 2;
  size_t size_offset = metadata + strlen(preset->metadata_text), filter = size_offset + 2,
         id = filter + 10;
  memset(before, 0x65, sizeof *before);
  *out = *before;
  if (!strcmp(name, "null")) {
    CHECK(umi_structured_form_search_resolve(NULL, bytes, length, out) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_resolve(catalogue, NULL, length, out) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_resolve(catalogue, bytes, length, NULL) != UMI_STATUS_OK);
    CHECK(!memcmp(out, before, sizeof *out));
    goto cleanup;
  }
  if (!strcmp(name, "truncation")) {
    for (size_t n = 0; n < length; ++n) {
      CHECK(umi_structured_form_search_resolve(catalogue, bytes, n, out) != UMI_STATUS_OK);
      CHECK(!memcmp(out, before, sizeof *out));
    }
    goto cleanup;
  }
  if (!strcmp(name, "outer-magic"))
    bytes[0] = 'X';
  if (!strcmp(name, "inner-magic"))
    bytes[filter] = 'X';
  if (!strcmp(name, "trailing"))
    bytes[length++] = 0;
  if (!strcmp(name, "oversized"))
    length = UMI_FORM_SEARCH_PRESET_MAX_BYTES + 1;
  if (!strcmp(name, "title-nul"))
    bytes[title] = 0;
  if (!strcmp(name, "title-empty")) {
    bytes[8] = 0;
    bytes[9] = 0;
  }
  if (!strcmp(name, "prefix-invalid"))
    bytes[prefix] = '/';
  if (!strcmp(name, "filter-length"))
    bytes[size_offset + 1] ^= 1;
  if (!strcmp(name, "identity-length"))
    bytes[filter + 8] = 255;
  if (!strcmp(name, "identity-nul"))
    bytes[id] = 0;
  if (!strcmp(name, "zero-version"))
    memset(bytes + id + strlen(encoded->base->form_id), 0, 8);
  if (!strcmp(name, "term-count"))
    bytes[id + strlen(encoded->base->form_id) + 8] = 9;
  if (!strcmp(name, "unit-change"))
    f->rules[0].unit_id = "mass.g";
  if (!strcmp(name, "operand-invalid"))
    bytes[length - 1] = 'x';
  UmiStatus status = umi_structured_form_search_resolve(catalogue, bytes, length, out);
  bool valid = !strcmp(name, "roundtrip") || !strcmp(name, "old-version") || !strcmp(name, "alias");
  if (!valid) {
    CHECK(status != UMI_STATUS_OK && !memcmp(out, before, sizeof *out));
    goto cleanup;
  }
  OK(status);
  CHECK(out->schema == encoded && out->schema->version == encoded->version &&
        !strcmp(out->preset.title, "Small parcels") &&
        out->preset.filter.terms[0].operand.value.decimal_value == 10);
  if (!strcmp(name, "alias")) {
    size_t capacity = sizeof *out > length ? sizeof *out : length;
    alias = malloc(capacity);
    CHECK(alias);
    memcpy(alias, bytes, length);
    OK(umi_structured_form_search_resolve(catalogue, alias, length, alias));
    CHECK(((UmiStructuredFormResolvedSearch *)alias)->schema == &f->schema);
  }
cleanup:
  umi_structured_form_catalogue_destroy(catalogue);
  free(preset);
  free(out);
  free(before);
  free(bytes);
  free(alias);
  free(f);
  free(g);
  return result;
}
