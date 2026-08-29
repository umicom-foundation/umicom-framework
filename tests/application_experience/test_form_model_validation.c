/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_form_model_validation.c
 *
 * PURPOSE:
 *   Verify reusable form schemas initialise typed models, track dirty changes
 *   and report required fields in plain language.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/experience.h"

#include <assert.h>

int main(void) {
  const UmiApplicationExperienceField fields[] = {
      {.field_id = "person.name",
       .label = "Name",
       .help_text = "Enter the person's full name.",
       .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
       .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
       .required = 1,
       .enforce_length_range = 1,
       .minimum_length = 1U,
       .maximum_length = 80U},
      {.field_id = "person.age",
       .label = "Age",
       .help_text = "Enter an age between zero and 130.",
       .kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER,
       .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER,
       .enforce_number_range = 1,
       .minimum_number = 0.0,
       .maximum_number = 130.0}};
  const UmiApplicationExperienceFormSchema schema = {sizeof(UmiApplicationExperienceFormSchema),
                                                     UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                     "umicom.form.person",
                                                     "Person",
                                                     "Edit a person's details.",
                                                     "umicom.command.person.save",
                                                     fields,
                                                     2U};
  UmiApplicationExperienceFormModel model;
  UmiApplicationExperienceValidationReport report;
  UmiApplicationExperienceValue value;
  assert(umi_application_experience_form_model_init(&model, &schema) == UMI_STATUS_OK);
  assert(umi_application_experience_form_validate(&model, &report) == UMI_STATUS_OK);
  assert(!umi_application_experience_validation_report_passed(&report));
  assert(report.error_count == 1U);
  assert(umi_application_experience_value_text(&value, "Sammy") == UMI_STATUS_OK);
  assert(umi_application_experience_form_model_set(&model, "person.name", &value) == UMI_STATUS_OK);
  assert(umi_application_experience_form_model_dirty(&model));
  assert(umi_application_experience_form_validate(&model, &report) == UMI_STATUS_OK);
  assert(umi_application_experience_validation_report_passed(&report));
  return 0;
}
