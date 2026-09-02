/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_form_history.c
 *
 * PURPOSE:
 *   Verify typed form changes can be recorded, undone and redone without
 *   frontend-specific event history.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  const UmiApplicationExperienceField field = {.field_id = "project.name",
                                               .label = "Name",
                                               .help_text = "Project name.",
                                               .kind = UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
                                               .value_type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
                                               .enforce_length_range = 1,
                                               .maximum_length = 80U};
  const UmiApplicationExperienceFormSchema schema = {sizeof(UmiApplicationExperienceFormSchema),
                                                     UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                     "umicom.form.project",
                                                     "Project",
                                                     "Project settings.",
                                                     "umicom.command.project.save",
                                                     &field,
                                                     1U};
  UmiApplicationExperienceFormModel before;
  UmiApplicationExperienceFormModel after;
  UmiApplicationExperienceFormChangeset changeset;
  UmiApplicationExperienceFormHistory history;
  UmiApplicationExperienceValue value;
  assert(umi_application_experience_form_model_init(&before, &schema) == UMI_STATUS_OK);
  after = before;
  assert(umi_application_experience_value_text(&value, "Umicom") == UMI_STATUS_OK);
  assert(umi_application_experience_form_model_set(&after, "project.name", &value) ==
         UMI_STATUS_OK);
  assert(umi_application_experience_form_changeset_create(&before, &after, &changeset) ==
         UMI_STATUS_OK);
  umi_application_experience_form_history_init(&history);
  assert(umi_application_experience_form_history_record(&history, &changeset) == UMI_STATUS_OK);
  assert(umi_application_experience_form_history_undo(&history, &after) == UMI_STATUS_OK);
  assert(umi_application_experience_form_model_get(&after, "project.name") == NULL);
  assert(umi_application_experience_form_history_redo(&history, &after) == UMI_STATUS_OK);
  assert(umi_application_experience_form_model_get(&after, "project.name") != NULL);
  return 0;
}
