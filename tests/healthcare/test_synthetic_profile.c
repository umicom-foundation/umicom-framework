/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/healthcare/test_synthetic_profile.c
 *
 * PURPOSE:
 *   Qualify fictional profile semantics independently of any UI or database.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Qualify fictional profile semantics independently of any UI or database. */
#include "umicom/application/experience/structured_form_snapshot.h"
#include "umicom/healthcare/synthetic_profile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto done;                                                                                   \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static UmiFormAnswer text_answer(const char *text) {
  UmiFormAnswer result = {.state = UMI_FORM_KNOWN};
  (void)umi_application_experience_value_text(&result.value, text);
  return result;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  int result = 0;
  const char *name = argv[1];
  const UmiStructuredFormSchema *schema = umi_healthcare_synthetic_profile_schema();
  UmiStructuredForm *form = calloc(1, sizeof(*form)), *restored = calloc(1, sizeof(*restored));
  UmiApplicationExperienceValidationReport *report = calloc(1, sizeof(*report));
  unsigned char *bytes = malloc(UMI_FORM_SNAPSHOT_MAX_BYTES);
  CHECK(form && restored && report && bytes);
  OK(umi_structured_form_schema_validate(schema));
  OK(umi_structured_form_init(schema, form));
  if (strcmp(name, "schema") == 0) {
    CHECK(schema == umi_healthcare_synthetic_profile_schema() && schema->version == 1 &&
          form->count == 6);
    goto done;
  }
  if (strcmp(name, "keys") == 0) {
    OK(umi_healthcare_synthetic_profile_key_validate("SYN-000001"));
    OK(umi_healthcare_synthetic_profile_key_validate("SYN-999999"));
    const char *bad[] = {"SYN-12345", "SYN-1234567", "syn-000001", "SYN-00000x", "REAL-00001", ""};
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); ++i)
      CHECK(umi_healthcare_synthetic_profile_key_validate(bad[i]) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_healthcare_synthetic_profile_key_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto done;
  }
  if (strcmp(name, "required") == 0) {
    OK(umi_structured_form_validate(schema, form, 0, report));
    CHECK(report->error_count == 0);
    OK(umi_structured_form_validate(schema, form, 1, report));
    CHECK(report->error_count == 3);
    goto done;
  }
  if (strcmp(name, "date") == 0) {
    UmiFormAnswer answer = text_answer("2024-02-29");
    OK(umi_structured_form_set(schema, form, form->revision, "profile.birth-date", &answer));
    *restored = *form;
    answer = text_answer("2023-02-29");
    CHECK(umi_structured_form_set(schema, form, form->revision, "profile.birth-date", &answer) !=
          UMI_STATUS_OK);
    CHECK(memcmp(form, restored, sizeof(*form)) == 0);
    goto done;
  }
  if (strcmp(name, "choice") == 0) {
    UmiFormAnswer answer = text_answer("telephone");
    OK(umi_structured_form_set(schema, form, form->revision, "profile.contact-method", &answer));
    answer = text_answer("Telephone");
    CHECK(umi_structured_form_set(schema, form, form->revision, "profile.contact-method",
                                  &answer) != UMI_STATUS_OK);
    goto done;
  }
  if (strcmp(name, "missing") == 0) {
    UmiFormAnswer answer = {.state = UMI_FORM_UNKNOWN,
                            .reason = "Fictional source did not specify a date"};
    OK(umi_structured_form_set(schema, form, form->revision, "profile.birth-date", &answer));
    CHECK(umi_structured_form_set(schema, form, form->revision, "profile.contact-permission",
                                  &answer) != UMI_STATUS_OK);
    goto done;
  }
  if (strcmp(name, "length") == 0) {
    char large[162];
    memset(large, 'x', 161);
    large[161] = '\0';
    UmiFormAnswer answer = text_answer(large);
    CHECK(umi_structured_form_set(schema, form, form->revision, "profile.source", &answer) !=
          UMI_STATUS_OK);
    large[160] = '\0';
    answer = text_answer(large);
    OK(umi_structured_form_set(schema, form, form->revision, "profile.source", &answer));
    goto done;
  }
  if (strcmp(name, "roundtrip") != 0) {
    result = 2;
    goto done;
  }
  UmiFormAnswer answer = text_answer("Fictional Example");
  OK(umi_structured_form_set(schema, form, form->revision, "profile.label", &answer));
  answer = text_answer("Synthetic training scenario");
  OK(umi_structured_form_set(schema, form, form->revision, "profile.source", &answer));
  answer = (UmiFormAnswer){.state = UMI_FORM_KNOWN};
  OK(umi_application_experience_value_boolean(&answer.value, 0));
  OK(umi_structured_form_set(schema, form, form->revision, "profile.contact-permission", &answer));
  OK(umi_structured_form_validate(schema, form, 1, report));
  CHECK(report->error_count == 0);
  size_t length = 0;
  OK(umi_structured_form_snapshot_encode(schema, form, bytes, UMI_FORM_SNAPSHOT_MAX_BYTES,
                                         &length));
  OK(umi_structured_form_snapshot_decode(schema, bytes, length, restored));
  CHECK(restored->answers[3].state == UMI_FORM_KNOWN && !restored->answers[3].value.boolean_value);
  CHECK(strcmp(restored->answers[0].value.text_value, "Fictional Example") == 0);
done:
  free(bytes);
  free(report);
  free(form);
  free(restored);
  return result;
}
