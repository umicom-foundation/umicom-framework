/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form.c
 * PURPOSE: Validate and edit versioned answers through one toolkit-neutral service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_internal.h"
#include "umicom/application/experience/form_temporal.h"
#include <stdlib.h>
#include <string.h>
static int terminated(const char *text, size_t capacity) {
  return memchr(text, '\0', capacity) != NULL;
}
static int unit_valid(const char *text) {
  if (text == NULL)
    return 0;
  for (size_t i = 0U; i < UMI_FORM_UNIT_CAPACITY; ++i) {
    unsigned char c = (unsigned char)text[i];
    if (c == 0U)
      return 1;
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' ||
          c == '_' || c == '-' || c == '/' || c == '%'))
      return 0;
  }
  return 0;
}
static int answer_shape(const UmiFormAnswer *answer) {
  if (answer == NULL || answer->state < UMI_FORM_UNANSWERED ||
      answer->state > UMI_FORM_NOT_APPLICABLE ||
      !terminated(answer->unit_id, sizeof(answer->unit_id)) ||
      !terminated(answer->reason, sizeof(answer->reason)) ||
      umi_application_experience_value_validate(&answer->value) != UMI_STATUS_OK)
    return 0;
  if (answer->state == UMI_FORM_KNOWN)
    return answer->value.type != UMI_APPLICATION_EXPERIENCE_VALUE_NONE && answer->reason[0] == '\0';
  /* Missing is a first-class answer, never a stale numeric value with a flag.
   * Require empty inactive storage so a transport cannot accidentally expose it. */
  if (answer->value.type != UMI_APPLICATION_EXPERIENCE_VALUE_NONE ||
      answer->value.boolean_value != 0 || answer->value.integer_value != 0 ||
      answer->value.decimal_value != 0.0 || answer->unit_id[0] != '\0')
    return 0;
  for (size_t i = 0U; i < sizeof(answer->value.text_value); ++i)
    if (answer->value.text_value[i] != '\0')
      return 0;
  return answer->state == UMI_FORM_UNANSWERED ? answer->reason[0] == '\0'
                                              : answer->reason[0] != '\0';
}
const char *umi_form_answer_state_text(UmiFormAnswerState state) {
  switch (state) {
  case UMI_FORM_UNANSWERED:
    return "Unanswered";
  case UMI_FORM_KNOWN:
    return "Known";
  case UMI_FORM_UNKNOWN:
    return "Unknown";
  case UMI_FORM_NOT_APPLICABLE:
    return "Not applicable";
  default:
    return "Invalid answer state";
  }
}
int umi_form_answer_equal(const UmiFormAnswer *left, const UmiFormAnswer *right) {
  return answer_shape(left) && answer_shape(right) && left->state == right->state &&
         strcmp(left->unit_id, right->unit_id) == 0 && strcmp(left->reason, right->reason) == 0 &&
         umi_application_experience_value_equal(&left->value, &right->value);
}
UmiStatus umi_structured_form_schema_validate(const UmiStructuredFormSchema *schema) {
  if (schema == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_application_experience_form_schema_validate(schema->base);
  if (status != UMI_STATUS_OK)
    return status;
  if (schema->version == 0U || schema->rules == NULL ||
      schema->rule_count != schema->base->field_count)
    return UMI_STATUS_INVALID_STATE;
  for (size_t i = 0U; i < schema->rule_count; ++i) {
    const UmiStructuredFormField *rule = &schema->rules[i];
    const UmiApplicationExperienceField *field = &schema->base->fields[i];
    if (!umi_application_experience_identifier_valid(rule->field_id) ||
        strcmp(rule->field_id, field->field_id) != 0 || !unit_valid(rule->unit_id) ||
        (rule->allow_unknown != 0 && rule->allow_unknown != 1) ||
        (rule->allow_not_applicable != 0 && rule->allow_not_applicable != 1) ||
        rule->text_format < UMI_FORM_TEXT_PLAIN ||
        rule->text_format > UMI_FORM_TEXT_OFFSET_TIMESTAMP)
      return UMI_STATUS_INVALID_STATE;
    if (rule->unit_id[0] != '\0' && field->value_type != UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER &&
        field->value_type != UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL)
      return UMI_STATUS_INVALID_STATE;
    if (rule->text_format != UMI_FORM_TEXT_PLAIN &&
        field->value_type != UMI_APPLICATION_EXPERIENCE_VALUE_TEXT)
      return UMI_STATUS_INVALID_STATE;
  }
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_answer_validate(const UmiStructuredFormSchema *schema, size_t index,
                                              const UmiFormAnswer *answer, int submitting) {
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (index >= schema->rule_count || answer == NULL || (submitting != 0 && submitting != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!answer_shape(answer))
    return UMI_STATUS_INVALID_STATE;
  const UmiStructuredFormField *rule = &schema->rules[index];
  const UmiApplicationExperienceField *field = &schema->base->fields[index];
  switch (answer->state) {
  case UMI_FORM_UNANSWERED:
    return submitting && field->required ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
  case UMI_FORM_UNKNOWN:
    return rule->allow_unknown ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
  case UMI_FORM_NOT_APPLICABLE:
    return rule->allow_not_applicable ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
  case UMI_FORM_KNOWN:
    break;
  default:
    return UMI_STATUS_INVALID_STATE;
  }
  if (strcmp(answer->unit_id, rule->unit_id) != 0 || answer->value.type != field->value_type)
    return UMI_STATUS_INVALID_STATE;
  status = umi_application_experience_field_accepts(field, &answer->value);
  if (status != UMI_STATUS_OK)
    return status;
  if (rule->text_format == UMI_FORM_TEXT_DATE)
    return umi_form_date_validate(answer->value.text_value, strlen(answer->value.text_value));
  if (rule->text_format == UMI_FORM_TEXT_OFFSET_TIMESTAMP) {
    UmiFormTimestamp timestamp;
    return umi_form_timestamp_parse(answer->value.text_value, strlen(answer->value.text_value),
                                    &timestamp);
  }
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_identity_check(const UmiStructuredFormSchema *schema,
                                             const UmiStructuredForm *form) {
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (form == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!terminated(form->form_id, sizeof(form->form_id)) ||
      strcmp(form->form_id, schema->base->form_id) != 0 ||
      form->schema_version != schema->version || form->count != schema->rule_count ||
      form->revision == 0U || form->saved_revision > form->revision)
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_draft_check(const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *form) {
  UmiStatus status = umi_structured_form_identity_check(schema, form);
  if (status != UMI_STATUS_OK)
    return status;
  for (size_t i = 0U; i < form->count; ++i) {
    status = umi_structured_form_answer_validate(schema, i, &form->answers[i], 0);
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_init(const UmiStructuredFormSchema *schema, UmiStructuredForm *out) {
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredForm *candidate = calloc(1U, sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(candidate->form_id, schema->base->form_id, strlen(schema->base->form_id) + 1U);
  candidate->schema_version = schema->version;
  candidate->revision = candidate->saved_revision = 1U;
  candidate->count = schema->rule_count;
  for (size_t i = 0U; i < candidate->count; ++i) {
    if (schema->base->fields[i].default_value.type != UMI_APPLICATION_EXPERIENCE_VALUE_NONE) {
      candidate->answers[i].state = UMI_FORM_KNOWN;
      candidate->answers[i].value = schema->base->fields[i].default_value;
      memcpy(candidate->answers[i].unit_id, schema->rules[i].unit_id,
             strlen(schema->rules[i].unit_id) + 1U);
    }
  }
  status = umi_structured_form_draft_check(schema, candidate);
  if (status == UMI_STATUS_OK)
    *out = *candidate;
  free(candidate);
  return status;
}
UmiStatus umi_structured_form_validate(const UmiStructuredFormSchema *schema,
                                       const UmiStructuredForm *form, int submitting,
                                       UmiApplicationExperienceValidationReport *report) {
  UmiStatus status = umi_structured_form_identity_check(schema, form);
  if (status != UMI_STATUS_OK)
    return status;
  if (report == NULL || (submitting != 0 && submitting != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiApplicationExperienceValidationReport candidate = {0};
  for (size_t i = 0U; i < form->count; ++i) {
    status = umi_structured_form_answer_validate(schema, i, &form->answers[i], submitting);
    if (status == UMI_STATUS_OK)
      continue;
    UmiApplicationExperienceValidationIssue *issue = &candidate.issues[candidate.count++];
    issue->severity = UMI_APPLICATION_EXPERIENCE_ISSUE_ERROR;
    memcpy(issue->field_id, schema->base->fields[i].field_id,
           strlen(schema->base->fields[i].field_id) + 1U);
    const char *message =
        form->answers[i].state == UMI_FORM_UNANSWERED
            ? "Provide a value or an explicitly permitted missing answer."
            : "Check the answer type, missing reason, unit and required date or time offset.";
    memcpy(issue->message, message, strlen(message) + 1U);
    ++candidate.error_count;
  }
  *report = candidate;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_set(const UmiStructuredFormSchema *schema, UmiStructuredForm *form,
                                  uint64_t expected_revision, const char *field_id,
                                  const UmiFormAnswer *answer) {
  UmiStatus status = umi_structured_form_draft_check(schema, form);
  if (status != UMI_STATUS_OK)
    return status;
  if (form->revision != expected_revision)
    return UMI_STATUS_INVALID_STATE;
  size_t index = umi_application_experience_form_schema_index(schema->base, field_id);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  if (schema->base->fields[index].read_only)
    return UMI_STATUS_PERMISSION_DENIED;
  status = umi_structured_form_answer_validate(schema, index, answer, 0);
  if (status != UMI_STATUS_OK)
    return status;
  if (umi_form_answer_equal(&form->answers[index], answer))
    return UMI_STATUS_OK;
  if (form->revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  form->answers[index] = *answer;
  ++form->revision;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_mark_saved(UmiStructuredForm *form, uint64_t saved_revision) {
  if (form == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (saved_revision == 0U || saved_revision != form->revision ||
      form->saved_revision > form->revision)
    return UMI_STATUS_INVALID_STATE;
  form->saved_revision = saved_revision;
  return UMI_STATUS_OK;
}
