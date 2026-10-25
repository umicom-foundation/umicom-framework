/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_search_preset.c
 *
 * PURPOSE:
 *   A saved search retains scope and conditions without retaining pagination.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A saved search retains scope and conditions without retaining pagination. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/application/experience/structured_form_search_preset.h"
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
  const char *cases[] = {"roundtrip",
                         "title-empty",
                         "title-control",
                         "title-termination",
                         "prefix-empty",
                         "prefix-dot",
                         "prefix-character",
                         "prefix-termination",
                         "metadata-termination",
                         "filter-invalid",
                         "empty-filter",
                         "unicode",
                         "capacity",
                         "truncation",
                         "trailing",
                         "magic",
                         "embedded-nul",
                         "schema",
                         "alias-encode",
                         "alias-decode",
                         "null"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset), *out = malloc(sizeof *out),
                                *sentinel = malloc(sizeof *sentinel);
  unsigned char *bytes = malloc(UMI_FORM_SEARCH_PRESET_MAX_BYTES + 1),
                *alias = calloc(1, sizeof *preset + UMI_FORM_SEARCH_PRESET_MAX_BYTES);
  CHECK(f && preset && out && sentinel && bytes && alias);
  strcpy(preset->title, "Small parcels");
  strcpy(preset->entity_prefix, "parcel.record.");
  strcpy(preset->metadata_text, "Fictional source");
  preset->filter.count = 1;
  strcpy(preset->filter.terms[0].field_id, "parcel.mass");
  preset->filter.terms[0].operation = UMI_FORM_FILTER_LESS;
  preset->filter.terms[0].operand = form_mass(10);
  bool invalid = false;
  if (strcmp(name, "title-empty") == 0) {
    preset->title[0] = 0;
    invalid = true;
  }
  if (strcmp(name, "title-control") == 0) {
    preset->title[0] = '\n';
    invalid = true;
  }
  if (strcmp(name, "title-termination") == 0) {
    memset(preset->title, 'x', sizeof preset->title);
    invalid = true;
  }
  if (strcmp(name, "prefix-empty") == 0) {
    preset->entity_prefix[0] = 0;
    invalid = true;
  }
  if (strcmp(name, "prefix-dot") == 0) {
    strcpy(preset->entity_prefix, "parcel.record");
    invalid = true;
  }
  if (strcmp(name, "prefix-character") == 0) {
    strcpy(preset->entity_prefix, "../record.");
    invalid = true;
  }
  if (strcmp(name, "prefix-termination") == 0) {
    memset(preset->entity_prefix, 'x', sizeof preset->entity_prefix);
    invalid = true;
  }
  if (strcmp(name, "metadata-termination") == 0) {
    memset(preset->metadata_text, 'x', sizeof preset->metadata_text);
    invalid = true;
  }
  if (strcmp(name, "filter-invalid") == 0) {
    strcpy(preset->filter.terms[0].operand.unit_id, "mass.lb");
    invalid = true;
  }
  if (strcmp(name, "empty-filter") == 0)
    preset->filter.count = 0;
  if (strcmp(name, "unicode") == 0) {
    strcpy(preset->title, "Parcels \xc2\xb5");
    strcpy(preset->metadata_text, "\xe2\x82\xac");
  }
  memset(bytes, 0x5e, UMI_FORM_SEARCH_PRESET_MAX_BYTES + 1);
  size_t length = 123;
  UmiStatus status = umi_structured_form_search_preset_encode(
      &f->schema, preset, bytes, UMI_FORM_SEARCH_PRESET_MAX_BYTES, &length);
  if (invalid) {
    CHECK(status != UMI_STATUS_OK && length == 123);
    for (size_t i = 0; i < UMI_FORM_SEARCH_PRESET_MAX_BYTES; ++i)
      CHECK(bytes[i] == 0x5e);
    goto cleanup;
  }
  OK(status);
  CHECK(bytes[length] == 0x5e);
  if (strcmp(name, "capacity") == 0) {
    size_t unchanged = 321;
    memset(alias, 0x4c, UMI_FORM_SEARCH_PRESET_MAX_BYTES);
    CHECK(umi_structured_form_search_preset_encode(&f->schema, preset, alias, length - 1,
                                                   &unchanged) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(unchanged == 321);
    for (size_t i = 0; i < UMI_FORM_SEARCH_PRESET_MAX_BYTES; ++i)
      CHECK(alias[i] == 0x4c);
    goto cleanup;
  }
  memset(sentinel, 0x7f, sizeof *sentinel);
  *out = *sentinel;
  if (strcmp(name, "truncation") == 0) {
    for (size_t i = 0; i < length; ++i) {
      CHECK(umi_structured_form_search_preset_decode(&f->schema, bytes, i, out) != UMI_STATUS_OK);
      CHECK(memcmp(out, sentinel, sizeof *out) == 0);
    }
    goto cleanup;
  }
  if (strcmp(name, "null") == 0) {
    CHECK(umi_structured_form_search_preset_validate(&f->schema, NULL) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_preset_decode(&f->schema, bytes, length, NULL) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_search_preset_encode(&f->schema, preset, bytes, length, NULL) !=
          UMI_STATUS_OK);
    goto cleanup;
  }
  if (strcmp(name, "trailing") == 0) {
    bytes[length++] = 0;
    invalid = true;
  }
  if (strcmp(name, "magic") == 0) {
    bytes[0] = 'X';
    invalid = true;
  }
  if (strcmp(name, "embedded-nul") == 0) {
    bytes[10] = 0;
    invalid = true;
  }
  if (strcmp(name, "schema") == 0) {
    ++f->schema.version;
    invalid = true;
  }
  status = umi_structured_form_search_preset_decode(&f->schema, bytes, length, out);
  if (invalid) {
    CHECK(status != UMI_STATUS_OK && memcmp(out, sentinel, sizeof *out) == 0);
    goto cleanup;
  }
  OK(status);
  CHECK(strcmp(out->title, preset->title) == 0 &&
        strcmp(out->entity_prefix, preset->entity_prefix) == 0 &&
        strcmp(out->metadata_text, preset->metadata_text) == 0);
  char before[65], after[65];
  OK(umi_structured_form_filter_digest(&f->schema, &preset->filter, before));
  OK(umi_structured_form_filter_digest(&f->schema, &out->filter, after));
  CHECK(strcmp(before, after) == 0);
  if (strcmp(name, "alias-encode") == 0) {
    memcpy(alias, preset, sizeof *preset);
    size_t n = 0;
    OK(umi_structured_form_search_preset_encode(&f->schema, (UmiStructuredFormSearchPreset *)alias,
                                                alias, UMI_FORM_SEARCH_PRESET_MAX_BYTES, &n));
    CHECK(n == length && memcmp(bytes, alias, n) == 0);
  }
  if (strcmp(name, "alias-decode") == 0) {
    memcpy(alias, bytes, length);
    OK(umi_structured_form_search_preset_decode(&f->schema, alias, length,
                                                (UmiStructuredFormSearchPreset *)alias));
    CHECK(strcmp(((UmiStructuredFormSearchPreset *)alias)->title, preset->title) == 0);
  }
cleanup:
  free(f);
  free(preset);
  free(out);
  free(sentinel);
  free(bytes);
  free(alias);
  return result;
}
