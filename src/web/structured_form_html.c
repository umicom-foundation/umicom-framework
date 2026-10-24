/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/structured_form_html.c
 * PURPOSE: Emit accessible browser controls with shared schema and revision identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_internal.h"
typedef struct Html {
  char *bytes;
  size_t length;
  UmiStatus status;
} Html;
static void append(Html *html, const char *text) {
  if (html->status != UMI_STATUS_OK)
    return;
  size_t length = strlen(text);
  if (length >= UMI_WEB_FORM_HTML_MAX_BYTES - html->length) {
    html->status = UMI_STATUS_CAPACITY_EXCEEDED;
    return;
  }
  memcpy(html->bytes + html->length, text, length + 1U);
  html->length += length;
}
static void escaped(Html *html, const char *text) {
  if (html->status != UMI_STATUS_OK)
    return;
  if (!umi_form_web_text(text, 4096U)) {
    html->status = UMI_STATUS_PARSE_ERROR;
    return;
  }
  for (const unsigned char *at = (const unsigned char *)text; *at != 0U; ++at) {
    switch (*at) {
    case '&':
      append(html, "&amp;");
      break;
    case '<':
      append(html, "&lt;");
      break;
    case '>':
      append(html, "&gt;");
      break;
    case '"':
      append(html, "&quot;");
      break;
    case '\'':
      append(html, "&#39;");
      break;
    case '\r':
      append(html, "&#13;");
      break;
    case '\n':
      append(html, "&#10;");
      break;
    case '\t':
      append(html, "&#9;");
      break;
    default: {
      if (*at < 0x20U || *at == 0x7fU) {
        html->status = UMI_STATUS_PARSE_ERROR;
        return;
      }
      char byte[2] = {(char)*at, '\0'};
      append(html, byte);
      break;
    }
    }
  }
}
static void hidden(Html *html, const char *name, const char *value) {
  append(html, "<input type=\"hidden\" name=\"");
  escaped(html, name);
  append(html, "\" value=\"");
  escaped(html, value);
  append(html, "\">\n");
}
static void hidden_number(Html *html, const char *name, uint64_t value) {
  char text[32];
  int length = snprintf(text, sizeof(text), "%llu", (unsigned long long)value);
  if (length < 0 || (size_t)length >= sizeof(text)) {
    html->status = UMI_STATUS_INTERNAL_ERROR;
    return;
  }
  hidden(html, name, text);
}
static int action_valid(const char *action) {
  if (action == NULL || action[0] != '/' || action[1] == '/')
    return 0;
  for (size_t i = 1U; i < UMI_WEB_PATH_CAPACITY; ++i) {
    unsigned char c = (unsigned char)action[i];
    if (c == 0U)
      return 1;
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '/' ||
          c == '.' || c == '_' || c == '-'))
      return 0;
  }
  return 0;
}
static void labelled_name(Html *html, const char *prefix, const char *id, const char *label) {
  char name[UMI_WEB_FORM_NAME_CAPACITY];
  if (umi_form_web_control_name(name, sizeof(name), prefix, id) != UMI_STATUS_OK) {
    html->status = UMI_STATUS_CAPACITY_EXCEEDED;
    return;
  }
  append(html, "<label for=\"");
  escaped(html, name);
  append(html, "\">");
  escaped(html, label);
  append(html, "</label>\n");
}
static void field(Html *html, const UmiStructuredFormSchema *schema, const UmiStructuredForm *form,
                  size_t index) {
  const UmiApplicationExperienceField *descriptor = &schema->base->fields[index];
  const UmiStructuredFormField *rule = &schema->rules[index];
  UmiFormTextAnswer text;
  html->status = umi_structured_form_answer_format(schema, index, &form->answers[index], &text);
  if (html->status != UMI_STATUS_OK)
    return;
  /* Reject a model representation that HTML would normalize silently. */
  if (strchr(text.value, '\r') != NULL || strchr(text.reason, '\r') != NULL) {
    html->status = UMI_STATUS_INVALID_STATE;
    return;
  }
  if (descriptor->kind == UMI_APPLICATION_EXPERIENCE_FIELD_SECRET && text.state == UMI_FORM_KNOWN) {
    html->status = UMI_STATUS_PERMISSION_DENIED;
    return;
  }
  append(html, descriptor->read_only ? "<fieldset disabled>\n<legend>" : "<fieldset>\n<legend>");
  escaped(html, descriptor->label);
  append(html, "</legend>\n<p>");
  escaped(html, descriptor->help_text);
  append(html, "</p>\n");
  labelled_name(html, "state", rule->field_id, "Answer state");
  append(html, "<select id=\"state.");
  escaped(html, rule->field_id);
  append(html, "\" name=\"state.");
  escaped(html, rule->field_id);
  append(html, "\">\n");
  for (int state = 0; state <= 3; ++state) {
    char number[2] = {(char)('0' + state), '\0'};
    append(html, "<option value=\"");
    append(html, number);
    append(html, "\"");
    if (state == (int)text.state)
      append(html, " selected");
    if ((state == 2 && !rule->allow_unknown) || (state == 3 && !rule->allow_not_applicable))
      append(html, " disabled");
    append(html, ">");
    escaped(html, umi_form_answer_state_text((UmiFormAnswerState)state));
    append(html, "</option>\n");
  }
  append(html, "</select>\n");
  labelled_name(html, "value", rule->field_id, "Value");
  if (descriptor->kind == UMI_APPLICATION_EXPERIENCE_FIELD_SECRET) {
    append(html, "<input type=\"password\" autocomplete=\"off\" id=\"value.");
    escaped(html, rule->field_id);
    append(html, "\" name=\"value.");
    escaped(html, rule->field_id);
    append(html, "\" value=\"\">\n");
  } else {
    append(html, "<textarea id=\"value.");
    escaped(html, rule->field_id);
    append(html, "\" name=\"value.");
    escaped(html, rule->field_id);
    append(html, "\" rows=\"2\">\n");
    escaped(html, text.value);
    append(html, "</textarea>\n");
  }
  if (descriptor->value_type == UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN)
    append(html, "<p>Enter true or false.</p>\n");
  if (rule->text_format == UMI_FORM_TEXT_OFFSET_TIMESTAMP)
    append(html,
           "<p>Include Z or an explicit offset, for example 2024-02-29T12:00:00+01:00.</p>\n");
  if (descriptor->option_count != 0U) {
    append(html, "<p>Available choice identifiers:</p><ul>\n");
    for (size_t i = 0U; i < descriptor->option_count; ++i) {
      if (descriptor->options[i].disabled)
        continue;
      append(html, "<li>");
      escaped(html, descriptor->options[i].option_id);
      append(html, " — ");
      escaped(html, descriptor->options[i].label);
      append(html, "</li>\n");
    }
    append(html, "</ul>\n");
  }
  if (rule->unit_id[0] != '\0') {
    append(html, "<p>Unit: ");
    escaped(html, rule->unit_id);
    append(html, "</p>\n");
  }
  char unit_name[UMI_WEB_FORM_NAME_CAPACITY];
  if (umi_form_web_control_name(unit_name, sizeof(unit_name), "unit", rule->field_id) !=
      UMI_STATUS_OK) {
    html->status = UMI_STATUS_CAPACITY_EXCEEDED;
    return;
  }
  hidden(html, unit_name, rule->unit_id);
  labelled_name(html, "reason", rule->field_id, "Reason when unknown or not applicable");
  append(html, "<textarea id=\"reason.");
  escaped(html, rule->field_id);
  append(html, "\" name=\"reason.");
  escaped(html, rule->field_id);
  append(html, "\" rows=\"2\">\n");
  escaped(html, text.reason);
  append(html, "</textarea>\n</fieldset>\n");
}
UmiStatus umi_web_structured_form_render(const UmiStructuredFormSchema *schema,
                                         const UmiStructuredForm *form, const char *action,
                                         const char *csrf_token, char *out, size_t capacity,
                                         size_t *out_length) {
  if (out == NULL || out_length == NULL || !action_valid(action) || !umi_form_web_token(csrf_token))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiApplicationExperienceValidationReport report;
  UmiStatus status = umi_structured_form_validate(schema, form, 0, &report);
  if (status != UMI_STATUS_OK)
    return status;
  if (report.error_count != 0U)
    return UMI_STATUS_INVALID_STATE;
  Html html = {malloc(UMI_WEB_FORM_HTML_MAX_BYTES), 0U, UMI_STATUS_OK};
  if (html.bytes == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  html.bytes[0] = '\0';
  append(&html, "<form method=\"post\" accept-charset=\"UTF-8\" action=\"");
  escaped(&html, action);
  append(&html, "\">\n<h2>");
  escaped(&html, schema->base->title);
  append(&html, "</h2>\n<p>");
  escaped(&html, schema->base->description);
  append(&html, "</p>\n");
  hidden(&html, "form", form->form_id);
  hidden_number(&html, "version", form->schema_version);
  hidden_number(&html, "revision", form->revision);
  hidden(&html, "csrf", csrf_token);
  for (size_t i = 0U; i < form->count && html.status == UMI_STATUS_OK; ++i)
    field(&html, schema, form, i);
  append(&html, "<p>Only the selected answer state is submitted as an answer. Values and reasons "
                "for inactive states remain drafts.</p>\n");
  append(&html, "<button type=\"submit\" name=\"intent\" value=\"draft\">Apply draft</button>\n");
  append(&html, "<button type=\"submit\" name=\"intent\" value=\"submit\">Validate for "
                "submission</button>\n</form>\n");
  status = html.status;
  if (status == UMI_STATUS_OK && html.length >= capacity)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK) {
    memcpy(out, html.bytes, html.length + 1U);
    *out_length = html.length;
  }
  free(html.bytes);
  return status;
}
