/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/structured_forms/main.c
 * PURPOSE: Demonstrate typed units, explicit unknowns and stale-edit refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/structured_form.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void) {
  const UmiApplicationExperienceField fields[] = {
      {.field_id = "parcel.mass",
       .label = "Mass",
       .help_text = "Fictional parcel mass.",
       .kind = UMI_APPLICATION_EXPERIENCE_FIELD_DECIMAL,
       .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL,
       .required = 1,
       .enforce_number_range = 1,
       .minimum_number = 0.0,
       .maximum_number = 100.0}};
  const UmiApplicationExperienceFormSchema base = {sizeof(base),
                                                   UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                   "example.parcel",
                                                   "Parcel",
                                                   "A synthetic example",
                                                   "example.save",
                                                   fields,
                                                   1U};
  const UmiStructuredFormField rules[] = {{"parcel.mass", "mass.kg", UMI_FORM_TEXT_PLAIN, 1, 0}};
  const UmiStructuredFormSchema schema = {&base, 1U, rules, 1U};
  UmiStructuredForm *form = malloc(sizeof(*form));
  UmiApplicationExperienceValidationReport *report = malloc(sizeof(*report));
  if (form == NULL || report == NULL) {
    free(form);
    free(report);
    return 1;
  }
  UmiFormAnswer answer = {0};
  answer.state = UMI_FORM_UNKNOWN;
  memcpy(answer.reason, "Not measured", 13U);
  UmiStatus status = umi_structured_form_init(&schema, form);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_set(&schema, form, 1U, "parcel.mass", &answer);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_validate(&schema, form, 1, report);
  if (status != UMI_STATUS_OK || report->error_count != 0U) {
    free(report);
    free(form);
    return 1;
  }
  if (umi_structured_form_set(&schema, form, 1U, "parcel.mass", &answer) !=
      UMI_STATUS_INVALID_STATE) {
    free(report);
    free(form);
    return 1;
  }
  printf("Mass: %s (%s). Stale edit refused.\n", umi_form_answer_state_text(form->answers[0].state),
         form->answers[0].reason);
  free(report);
  free(form);
  return 0;
}
