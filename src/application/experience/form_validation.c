/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/form_validation.c
 *
 * PURPOSE:
 *   Validate a complete form model and explain each invalid field in plain
 *   language suitable for junior developers and application users.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/form_validation.h"

#include <string.h>

#include "umicom/base/text.h"

void umi_application_experience_validation_report_init(
    UmiApplicationExperienceValidationReport *report) {
  if (report != NULL)
    (void)memset(report, 0, sizeof(*report));
}

static UmiStatus add_issue(UmiApplicationExperienceValidationReport *report,
                           const UmiApplicationExperienceField *field, const char *message) {
  UmiApplicationExperienceValidationIssue *issue;
  if (report->count >= UMI_APPLICATION_EXPERIENCE_ISSUE_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  issue = &report->issues[report->count++];
  issue->severity = UMI_APPLICATION_EXPERIENCE_ISSUE_ERROR;
  report->error_count += 1U;
  if (umi_text_copy(issue->field_id, sizeof(issue->field_id), field->field_id) != UMI_STATUS_OK ||
      umi_text_format(issue->message, sizeof(issue->message), "%s: %s", field->label, message) !=
          UMI_STATUS_OK)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_experience_form_validate(const UmiApplicationExperienceFormModel *model,
                                         UmiApplicationExperienceValidationReport *report) {
  size_t index;
  if (model == NULL || model->schema == NULL || report == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_validation_report_init(report);
  for (index = 0U; index < model->schema->field_count; ++index) {
    const UmiApplicationExperienceField *field = &model->schema->fields[index];
    const UmiApplicationExperienceValue *value =
        model->present[index] ? &model->values[index] : &field->default_value;
    UmiStatus status = umi_application_experience_field_accepts(field, value);
    if (status != UMI_STATUS_OK) {
      const char *message = umi_application_experience_value_empty(value)
                                ? "a value is required"
                                : "the value does not meet this field's rules";
      status = add_issue(report, field, message);
      if (status != UMI_STATUS_OK)
        return status;
    }
  }
  return UMI_STATUS_OK;
}

const UmiApplicationExperienceValidationIssue *umi_application_experience_validation_report_at(
    const UmiApplicationExperienceValidationReport *report, size_t index) {
  return report != NULL && report->count <= UMI_APPLICATION_EXPERIENCE_ISSUE_CAPACITY &&
                 index < report->count
             ? &report->issues[index]
             : NULL;
}

int umi_application_experience_validation_report_passed(
    const UmiApplicationExperienceValidationReport *report) {
  return report != NULL && report->count <= UMI_APPLICATION_EXPERIENCE_ISSUE_CAPACITY &&
         report->error_count == 0U;
}
