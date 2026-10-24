/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/structured_form_fixture.h
 * PURPOSE: Share bounded fictional schemas across structured form regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STRUCTURED_FORM_FIXTURE_H
#define UMICOM_STRUCTURED_FORM_FIXTURE_H
#include "umicom/application/experience/structured_form.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition)                                                                           \
  do {                                                                                             \
    if (!(condition)) {                                                                            \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition);                              \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
typedef struct FormFixture {
  UmiApplicationExperienceField fields[3];
  UmiStructuredFormField rules[3];
  UmiApplicationExperienceFormSchema base;
  UmiStructuredFormSchema schema;
  UmiStructuredForm before, after, saved;
} FormFixture;
static inline FormFixture *form_fixture_new(void) {
  FormFixture *f = calloc(1U, sizeof(*f));
  if (f == NULL)
    return NULL;
  f->fields[0] =
      (UmiApplicationExperienceField){.field_id = "parcel.mass",
                                      .label = "Mass",
                                      .help_text = "Fictional parcel mass.",
                                      .kind = UMI_APPLICATION_EXPERIENCE_FIELD_DECIMAL,
                                      .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL,
                                      .required = 1,
                                      .enforce_number_range = 1,
                                      .minimum_number = 0.0,
                                      .maximum_number = 100.0};
  f->fields[1] =
      (UmiApplicationExperienceField){.field_id = "parcel.observed",
                                      .label = "Observed",
                                      .help_text = "Explicit offset.",
                                      .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
                                      .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
                                      .required = 1};
  f->fields[2] =
      (UmiApplicationExperienceField){.field_id = "parcel.note",
                                      .label = "Note",
                                      .help_text = "Optional note.",
                                      .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
                                      .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT};
  f->rules[0] = (UmiStructuredFormField){"parcel.mass", "mass.kg", UMI_FORM_TEXT_PLAIN, 1, 0};
  f->rules[1] =
      (UmiStructuredFormField){"parcel.observed", "", UMI_FORM_TEXT_OFFSET_TIMESTAMP, 0, 0};
  f->rules[2] = (UmiStructuredFormField){"parcel.note", "", UMI_FORM_TEXT_PLAIN, 1, 1};
  f->base = (UmiApplicationExperienceFormSchema){
      sizeof(f->base),     UMI_APPLICATION_EXPERIENCE_API_VERSION,
      "example.parcel",    "Parcel",
      "Fictional fixture", "example.save",
      f->fields,           3U};
  f->schema = (UmiStructuredFormSchema){&f->base, 1U, f->rules, 3U};
  if (umi_structured_form_init(&f->schema, &f->before) != UMI_STATUS_OK) {
    free(f);
    return NULL;
  }
  f->after = f->saved = f->before;
  return f;
}
static inline UmiFormAnswer form_mass(double number) {
  UmiFormAnswer a = {0};
  a.state = UMI_FORM_KNOWN;
  (void)umi_application_experience_value_decimal(&a.value, number);
  memcpy(a.unit_id, "mass.kg", 8U);
  return a;
}
static inline UmiFormAnswer form_text(const char *text) {
  UmiFormAnswer a = {0};
  a.state = UMI_FORM_KNOWN;
  (void)umi_application_experience_value_text(&a.value, text);
  return a;
}
static inline UmiFormAnswer form_missing(UmiFormAnswerState state, const char *reason) {
  UmiFormAnswer a = {0};
  a.state = state;
  if (strlen(reason) < sizeof(a.reason))
    memcpy(a.reason, reason, strlen(reason) + 1U);
  return a;
}
#endif
