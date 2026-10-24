/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reject partial and semantically reinterpreted saved filters before publication. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/application/experience/structured_form_filter_snapshot.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static size_t text_end(const unsigned char *b, size_t at) {
  return at + 2 + ((size_t)b[at] << 8) + b[at + 1];
}
int main(int argc, char **argv) {
  const char *cases[] = {
      "roundtrip",    "empty",        "states",        "integer-min", "integer-max", "boolean",
      "text",         "maximum",      "outside-range", "inactive",    "capacity",    "truncation",
      "trailing",     "magic",        "version",       "identity",    "unit",        "type",
      "format",       "field",        "count",         "operator",    "state",       "embedded-nul",
      "noncanonical", "alias-encode", "alias-decode",  "null"};
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
  UmiStructuredFormFilter *filter = calloc(1, sizeof *filter), *decoded = malloc(sizeof *decoded),
                          *sentinel = malloc(sizeof *sentinel);
  unsigned char *bytes = malloc(UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES + 1),
                *second = malloc(UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES + 1),
                *alias = calloc(1, sizeof *filter + UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES);
  CHECK(f && filter && decoded && sentinel && bytes && second && alias);
  filter->count = 1;
  strcpy(filter->terms[0].field_id, "parcel.mass");
  filter->terms[0].operation = UMI_FORM_FILTER_EQUAL;
  filter->terms[0].operand = form_mass(5.25);
  if (strcmp(name, "empty") == 0)
    filter->count = 0;
  if (strcmp(name, "states") == 0) {
    filter->count = 4;
    for (size_t i = 0; i < 4; ++i) {
      strcpy(filter->terms[i].field_id, "parcel.mass");
      filter->terms[i].operation = UMI_FORM_FILTER_STATE;
      filter->terms[i].operand = (UmiFormAnswer){.state = (UmiFormAnswerState)i};
    }
  }
  if (strncmp(name, "integer-", 8) == 0) {
    f->fields[0].kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER;
    f->fields[0].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    f->fields[0].enforce_number_range = 0;
    OK(umi_application_experience_value_integer(
        &filter->terms[0].operand.value, strcmp(name, "integer-min") == 0 ? INT64_MIN : INT64_MAX));
  }
  if (strcmp(name, "boolean") == 0) {
    f->fields[0].kind = UMI_APPLICATION_EXPERIENCE_FIELD_BOOLEAN;
    f->fields[0].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN;
    f->fields[0].enforce_number_range = 0;
    f->rules[0].unit_id = "";
    filter->terms[0].operand.unit_id[0] = 0;
    OK(umi_application_experience_value_boolean(&filter->terms[0].operand.value, 1));
  }
  if (strcmp(name, "text") == 0 || strcmp(name, "format") == 0 || strcmp(name, "maximum") == 0) {
    strcpy(filter->terms[0].field_id, "parcel.note");
    filter->terms[0].operand = form_text("example|line\nsecond");
    if (strcmp(name, "maximum") == 0) {
      memset(filter->terms[0].operand.value.text_value, 'x',
             UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY - 1);
      filter->terms[0].operand.value.text_value[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY - 1] = 0;
      for (size_t i = 1; i < UMI_FORM_FILTER_CAPACITY; ++i)
        filter->terms[i] = filter->terms[0];
      filter->count = UMI_FORM_FILTER_CAPACITY;
    }
  }
  if (strcmp(name, "outside-range") == 0) {
    filter->terms[0].operation = UMI_FORM_FILTER_LESS;
    filter->terms[0].operand = form_mass(1000);
  }
  memset(bytes, 0x7b, UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES + 1);
  size_t length = 0;
  OK(umi_structured_form_filter_snapshot_encode(&f->schema, filter, bytes,
                                                UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES, &length));
  CHECK(bytes[length] == 0x7b);
  if (strcmp(name, "capacity") == 0) {
    size_t unchanged = 123;
    memset(second, 0x4a, UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES);
    CHECK(umi_structured_form_filter_snapshot_encode(&f->schema, filter, second, length - 1,
                                                     &unchanged) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(unchanged == 123);
    for (size_t i = 0; i < UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES; ++i)
      CHECK(second[i] == 0x4a);
    goto cleanup;
  }
  if (strcmp(name, "inactive") == 0) {
    memset(filter->terms[1].field_id, 'x', sizeof filter->terms[1].field_id);
    memset(filter->terms[0].operand.value.text_value, 'z',
           sizeof filter->terms[0].operand.value.text_value);
    size_t n = 0;
    OK(umi_structured_form_filter_snapshot_encode(&f->schema, filter, second,
                                                  UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES, &n));
    CHECK(n == length && memcmp(bytes, second, n) == 0);
  }
  memset(sentinel, 0x6d, sizeof *sentinel);
  *decoded = *sentinel;
  if (strcmp(name, "truncation") == 0) {
    for (size_t i = 0; i < length; ++i) {
      CHECK(umi_structured_form_filter_snapshot_decode(&f->schema, bytes, i, decoded) !=
            UMI_STATUS_OK);
      CHECK(memcmp(decoded, sentinel, sizeof *decoded) == 0);
    }
    goto cleanup;
  }
  if (strcmp(name, "null") == 0) {
    CHECK(umi_structured_form_filter_snapshot_decode(NULL, bytes, length, decoded) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_filter_snapshot_decode(&f->schema, NULL, length, decoded) !=
          UMI_STATUS_OK);
    CHECK(umi_structured_form_filter_snapshot_encode(&f->schema, filter, bytes, length, NULL) !=
          UMI_STATUS_OK);
    CHECK(memcmp(decoded, sentinel, sizeof *decoded) == 0);
    goto cleanup;
  }
  size_t count_at = text_end(bytes, 8) + 8, field_at = count_at + 1,
         operator_at = text_end(bytes, field_at);
  bool invalid = false;
  if (strcmp(name, "trailing") == 0) {
    bytes[length++] = 0;
    invalid = true;
  }
  if (strcmp(name, "magic") == 0) {
    bytes[0] = 'X';
    invalid = true;
  }
  if (strcmp(name, "version") == 0) {
    ++f->schema.version;
    invalid = true;
  }
  if (strcmp(name, "identity") == 0) {
    f->base.form_id = "example.other";
    invalid = true;
  }
  if (strcmp(name, "unit") == 0) {
    f->rules[0].unit_id = "mass.lb";
    invalid = true;
  }
  if (strcmp(name, "type") == 0) {
    f->fields[0].kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER;
    f->fields[0].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    invalid = true;
  }
  if (strcmp(name, "format") == 0) {
    f->rules[2].text_format = UMI_FORM_TEXT_DATE;
    invalid = true;
  }
  if (strcmp(name, "field") == 0) {
    bytes[field_at + 2] = 'x';
    invalid = true;
  }
  if (strcmp(name, "count") == 0) {
    bytes[count_at] = UMI_FORM_FILTER_CAPACITY + 1;
    invalid = true;
  }
  if (strcmp(name, "operator") == 0) {
    bytes[operator_at] = 255;
    invalid = true;
  }
  if (strcmp(name, "state") == 0) {
    bytes[operator_at + 1] = 255;
    invalid = true;
  }
  if (strcmp(name, "embedded-nul") == 0) {
    bytes[field_at + 2] = 0;
    invalid = true;
  }
  if (strcmp(name, "noncanonical") == 0) {
    size_t value_at = text_end(bytes, operator_at + 4), start = value_at + 2;
    memmove(bytes + start + 1, bytes + start, length - start);
    bytes[start] = '0';
    ++bytes[value_at + 1];
    ++length;
    invalid = true;
  }
  UmiStatus status = umi_structured_form_filter_snapshot_decode(&f->schema, bytes, length, decoded);
  if (invalid) {
    CHECK(status != UMI_STATUS_OK && memcmp(decoded, sentinel, sizeof *decoded) == 0);
    goto cleanup;
  }
  OK(status);
  CHECK(decoded->count == filter->count);
  char before[65], after[65];
  OK(umi_structured_form_filter_digest(&f->schema, filter, before));
  OK(umi_structured_form_filter_digest(&f->schema, decoded, after));
  CHECK(strcmp(before, after) == 0);
  if (strcmp(name, "alias-encode") == 0) {
    memcpy(alias, filter, sizeof *filter);
    size_t n = 0;
    OK(umi_structured_form_filter_snapshot_encode(&f->schema, (UmiStructuredFormFilter *)alias,
                                                  alias, UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES, &n));
    CHECK(n == length && memcmp(alias, bytes, n) == 0);
  }
  if (strcmp(name, "alias-decode") == 0) {
    memcpy(alias, bytes, length);
    OK(umi_structured_form_filter_snapshot_decode(&f->schema, alias, length,
                                                  (UmiStructuredFormFilter *)alias));
    OK(umi_structured_form_filter_digest(&f->schema, (UmiStructuredFormFilter *)alias, after));
    CHECK(strcmp(before, after) == 0);
  }
cleanup:
  free(f);
  free(filter);
  free(decoded);
  free(sentinel);
  free(bytes);
  free(second);
  free(alias);
  return result;
}
