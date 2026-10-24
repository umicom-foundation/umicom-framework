/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_snapshot.c
 * PURPOSE: Preserve answer semantics while refusing incomplete or incompatible frames.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
#include "umicom/application/experience/structured_form_snapshot.h"
static int unchanged(const void *memory, size_t length) {
  const unsigned char *p = memory;
  for (size_t i = 0U; i < length; ++i)
    if (p[i] != 0x5aU)
      return 0;
  return 1;
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  FormFixture *f = form_fixture_new();
  CHECK(f);
  const char *name = argv[1];
  unsigned char *bytes = malloc(UMI_FORM_SNAPSHOT_MAX_BYTES),
                *second = malloc(UMI_FORM_SNAPSHOT_MAX_BYTES);
  CHECK(bytes && second);
  UmiFormAnswer a = form_mass(3.5);
  CHECK(umi_structured_form_set(&f->schema, &f->before, 1U, "parcel.mass", &a) == UMI_STATUS_OK);
  a = form_text("2024-02-29T12:00:00.123456789-03:30");
  CHECK(umi_structured_form_set(&f->schema, &f->before, 2U, "parcel.observed", &a) ==
        UMI_STATUS_OK);
  a = form_missing(UMI_FORM_NOT_APPLICABLE, "No note needed");
  CHECK(umi_structured_form_set(&f->schema, &f->before, 3U, "parcel.note", &a) == UMI_STATUS_OK);
  size_t length = 0U;
  CHECK(umi_structured_form_snapshot_encode(&f->schema, &f->before, bytes,
                                            UMI_FORM_SNAPSHOT_MAX_BYTES, &length) == UMI_STATUS_OK);
  if (strcmp(name, "roundtrip") == 0) {
    CHECK(umi_structured_form_snapshot_decode(&f->schema, bytes, length, &f->after) ==
          UMI_STATUS_OK);
    CHECK(f->after.revision == 4U && f->after.saved_revision == 1U);
    for (size_t i = 0U; i < 3U; ++i)
      CHECK(umi_form_answer_equal(&f->before.answers[i], &f->after.answers[i]));
    size_t n = 0U;
    CHECK(umi_structured_form_snapshot_encode(&f->schema, &f->after, second,
                                              UMI_FORM_SNAPSHOT_MAX_BYTES, &n) == UMI_STATUS_OK &&
          n == length && memcmp(bytes, second, n) == 0);
  } else if (strcmp(name, "truncated") == 0) {
    for (size_t n = 0U; n < length; ++n) {
      memset(&f->after, 0x5a, sizeof(f->after));
      CHECK(umi_structured_form_snapshot_decode(&f->schema, bytes, n, &f->after) != UMI_STATUS_OK);
      CHECK(unchanged(&f->after, sizeof(f->after)));
    }
  } else if (strcmp(name, "capacity") == 0) {
    memset(second, 0x5a, UMI_FORM_SNAPSHOT_MAX_BYTES);
    size_t n = 77U;
    CHECK(umi_structured_form_snapshot_encode(&f->schema, &f->before, second, length - 1U, &n) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          n == 77U);
    CHECK(unchanged(second, UMI_FORM_SNAPSHOT_MAX_BYTES));
  } else if (strcmp(name, "version") == 0 || strcmp(name, "trailing") == 0 ||
             strcmp(name, "magic") == 0 || strcmp(name, "field") == 0 ||
             strcmp(name, "embedded-null") == 0) {
    if (strcmp(name, "version") == 0)
      f->schema.version = 2U;
    else if (strcmp(name, "trailing") == 0)
      bytes[length++] = 0U;
    else if (strcmp(name, "magic") == 0)
      bytes[6] = '2';
    else if (strcmp(name, "embedded-null") == 0)
      bytes[10] = 0U;
    else {
      size_t start = 8U + 2U + strlen(f->before.form_id) + 8U + 8U + 8U + 2U + 2U;
      bytes[start] = 'X';
    }
    memset(&f->after, 0x5a, sizeof(f->after));
    CHECK(umi_structured_form_snapshot_decode(&f->schema, bytes, length, &f->after) !=
          UMI_STATUS_OK);
    CHECK(unchanged(&f->after, sizeof(f->after)));
  } else if (strcmp(name, "integers") == 0) {
    f->fields[0].kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER;
    f->fields[0].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    f->fields[0].enforce_number_range = 0;
    const int64_t values[] = {INT64_MIN, INT64_MAX, -1, 0, 1};
    for (size_t i = 0U; i < sizeof(values) / sizeof(values[0]); ++i) {
      f->before.answers[0].value.type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
      f->before.answers[0].value.integer_value = values[i];
      CHECK(umi_structured_form_snapshot_encode(&f->schema, &f->before, bytes,
                                                UMI_FORM_SNAPSHOT_MAX_BYTES,
                                                &length) == UMI_STATUS_OK);
      CHECK(umi_structured_form_snapshot_decode(&f->schema, bytes, length, &f->after) ==
                UMI_STATUS_OK &&
            f->after.answers[0].value.integer_value == values[i]);
    }
  } else if (strcmp(name, "alias") == 0) {
    CHECK(length < sizeof(f->after));
    memcpy(&f->after, bytes, length);
    CHECK(umi_structured_form_snapshot_decode(&f->schema, &f->after, length, &f->after) ==
              UMI_STATUS_OK &&
          f->after.answers[0].value.decimal_value == 3.5);
  } else
    CHECK(0);
  free(second);
  free(bytes);
  free(f);
  return 0;
}
