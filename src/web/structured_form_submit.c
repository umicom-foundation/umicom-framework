/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/structured_form_submit.c
 * PURPOSE: Prepare complete browser edits with server-owned identity and stale-state guards.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_internal.h"
static UmiStatus number_control(const UmiWebFormBody *body, const char *name, uint64_t expected) {
  const char *value = umi_web_form_body_get(body, name);
  if (value == NULL)
    return UMI_STATUS_NOT_FOUND;
  uint64_t parsed;
  UmiStatus status = umi_ascii_uint64_parse(value, strlen(value), &parsed);
  if (status != UMI_STATUS_OK)
    return status;
  return parsed == expected ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
/* Browser form submission expands line feeds to CRLF. The shared text model
 * uses LF, so remove only that transport expansion. Bare CR is refused; existing
 * CR-bearing model values cannot be rendered by this adapter without review. */
static UmiStatus browser_text(const char *source, char *out, size_t capacity) {
  size_t used = 0U;
  for (size_t i = 0U; source[i] != '\0'; ++i) {
    char value = source[i];
    if (value == '\r') {
      if (source[i + 1U] != '\n')
        return UMI_STATUS_PARSE_ERROR;
      ++i;
      value = '\n';
    }
    if (used + 1U >= capacity)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    out[used++] = value;
  }
  out[used] = '\0';
  return UMI_STATUS_OK;
}
static UmiStatus answer_controls(const UmiStructuredFormSchema *schema, size_t index,
                                 const UmiWebFormBody *body, UmiFormAnswer *out) {
  const char *prefixes[] = {"state", "value", "unit", "reason"};
  const char *values[4];
  for (size_t i = 0U; i < 4U; ++i) {
    char name[UMI_WEB_FORM_NAME_CAPACITY];
    UmiStatus status =
        umi_form_web_control_name(name, sizeof(name), prefixes[i], schema->rules[index].field_id);
    if (status != UMI_STATUS_OK)
      return status;
    values[i] = umi_web_form_body_get(body, name);
    if (values[i] == NULL)
      return UMI_STATUS_NOT_FOUND;
  }
  if (values[0][0] < '0' || values[0][0] > '3' || values[0][1] != '\0')
    return UMI_STATUS_PARSE_ERROR;
  /* The unit is always submitted from the rendered schema, even when the user
   * changes the state selector. It cannot silently redirect a numeric answer. */
  if (strcmp(values[2], schema->rules[index].unit_id) != 0)
    return UMI_STATUS_INVALID_STATE;
  UmiFormTextAnswer text = {0};
  text.state = (UmiFormAnswerState)(values[0][0] - '0');
  if (text.state == UMI_FORM_KNOWN) {
    UmiStatus status = browser_text(values[1], text.value, sizeof(text.value));
    if (status != UMI_STATUS_OK)
      return status;
    memcpy(text.unit_id, values[2], strlen(values[2]) + 1U);
  } else if (text.state == UMI_FORM_UNKNOWN || text.state == UMI_FORM_NOT_APPLICABLE) {
    UmiStatus status = browser_text(values[3], text.reason, sizeof(text.reason));
    if (status != UMI_STATUS_OK)
      return status;
  }
  return umi_structured_form_answer_parse(schema, index, &text, out);
}
UmiStatus umi_web_structured_form_prepare(const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *current, const void *bytes,
                                          size_t length, const char *expected_csrf_token,
                                          UmiStructuredFormEdit *out_edit, int *out_submitting) {
  if (out_edit == NULL || out_submitting == NULL || !umi_form_web_token(expected_csrf_token))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiApplicationExperienceValidationReport report;
  UmiStatus status = umi_structured_form_validate(schema, current, 0, &report);
  if (status != UMI_STATUS_OK)
    return status;
  if (report.error_count != 0U)
    return UMI_STATUS_INVALID_STATE;
  UmiWebFormBody *body = malloc(sizeof(*body));
  UmiStructuredFormEdit *edit = calloc(1U, sizeof(*edit));
  UmiStructuredForm *candidate = malloc(sizeof(*candidate));
  if (body == NULL || edit == NULL || candidate == NULL) {
    free(body);
    free(edit);
    free(candidate);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  int submitting = 0;
  status = umi_web_form_body_parse(bytes, length, body);
  if (status != UMI_STATUS_OK)
    goto done;
  const char *token = umi_web_form_body_get(body, "csrf");
  const char *form = umi_web_form_body_get(body, "form");
  const char *intent = umi_web_form_body_get(body, "intent");
  if (token == NULL || strcmp(token, expected_csrf_token) != 0) {
    status = UMI_STATUS_PERMISSION_DENIED;
    goto done;
  }
  if (form == NULL || strcmp(form, current->form_id) != 0) {
    status = UMI_STATUS_INVALID_STATE;
    goto done;
  }
  if (intent == NULL || (strcmp(intent, "draft") != 0 && strcmp(intent, "submit") != 0)) {
    status = UMI_STATUS_PARSE_ERROR;
    goto done;
  }
  submitting = strcmp(intent, "submit") == 0;
  status = number_control(body, "version", current->schema_version);
  if (status == UMI_STATUS_OK)
    status = number_control(body, "revision", current->revision);
  if (status != UMI_STATUS_OK)
    goto done;
  size_t editable = 0U;
  memcpy(edit->form_id, current->form_id, sizeof(edit->form_id));
  edit->schema_version = current->schema_version;
  edit->base_revision = current->revision;
  for (size_t i = 0U; i < current->count; ++i) {
    if (schema->base->fields[i].read_only)
      continue;
    ++editable;
    UmiFormAnswer answer;
    status = answer_controls(schema, i, body, &answer);
    if (status != UMI_STATUS_OK)
      goto done;
    if (!umi_form_answer_equal(&current->answers[i], &answer)) {
      UmiStructuredFormChange *change = &edit->changes[edit->count++];
      memcpy(change->field_id, schema->rules[i].field_id, strlen(schema->rules[i].field_id) + 1U);
      change->before = current->answers[i];
      change->after = answer;
    }
  }
  /* Every expected name was resolved and parser rejected duplicates. Exact
   * cardinality therefore rules out all unknown or read-only extra controls. */
  if (body->count != 5U + 4U * editable) {
    status = UMI_STATUS_INVALID_STATE;
    goto done;
  }
  *candidate = *current;
  status = umi_structured_form_edit_apply(schema, candidate, edit);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_validate(schema, candidate, submitting, &report);
  if (status == UMI_STATUS_OK && report.error_count != 0U)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    *out_edit = *edit;
    *out_submitting = submitting;
  }
done:
  free(candidate);
  free(edit);
  free(body);
  return status;
}
