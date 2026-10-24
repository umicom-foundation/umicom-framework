/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/structured_form_gtk4.c
 * PURPOSE: Keep native input capture thin and validation in the shared C23 model.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/structured_form_edit.h"
#include "umicom/application/experience/structured_form_text.h"
#include "umicom/ui/gtk4/structured_form.h"
#include "structured_form_value.h"
#include <errno.h>
#include <math.h>
#include <string.h>
typedef struct FormRow {
  GtkWidget *value, *state, *reason;
} FormRow;
typedef struct UmiStructuredFormRoot {
  GtkBox parent;
  const UmiStructuredFormSchema *schema;
  UmiStructuredForm *form;
  FormRow rows[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
  GtkWidget *feedback;
  int retired, applying, staged;
} UmiStructuredFormRoot;
typedef struct UmiStructuredFormRootClass {
  GtkBoxClass parent;
} UmiStructuredFormRootClass;
GType umi_structured_form_root_get_type(void) G_GNUC_CONST;
G_DEFINE_TYPE(UmiStructuredFormRoot, umi_structured_form_root, GTK_TYPE_BOX)
static UmiStructuredFormRoot *root_from(GtkWidget *widget) {
  return widget != NULL && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_structured_form_root_get_type())
             ? (UmiStructuredFormRoot *)widget
             : NULL;
}
static void form_unroot(GtkWidget *widget) {
  ((UmiStructuredFormRoot *)widget)->retired = 1;
  GTK_WIDGET_CLASS(umi_structured_form_root_parent_class)->unroot(widget);
}
static void form_dispose(GObject *object) {
  ((UmiStructuredFormRoot *)object)->retired = 1;
  G_OBJECT_CLASS(umi_structured_form_root_parent_class)->dispose(object);
}
static void form_finalize(GObject *object) {
  g_free(((UmiStructuredFormRoot *)object)->form);
  G_OBJECT_CLASS(umi_structured_form_root_parent_class)->finalize(object);
}
static void umi_structured_form_root_class_init(UmiStructuredFormRootClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = form_unroot;
  G_OBJECT_CLASS(klass)->dispose = form_dispose;
  G_OBJECT_CLASS(klass)->finalize = form_finalize;
}
static void umi_structured_form_root_init(UmiStructuredFormRoot *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 12);
}
static void text_changed(GtkEditable *editable, gpointer data) {
  (void)editable;
  UmiStructuredFormRoot *self = data;
  if (!self->retired)
    self->staged = 1;
}
static void buffer_changed(GtkTextBuffer *buffer, gpointer data) {
  (void)buffer;
  UmiStructuredFormRoot *self = data;
  if (!self->retired) self->staged = 1;
}
static void state_changed(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  UmiStructuredFormRoot *self = data;
  if (!self->retired)
    self->staged = 1;
}
static void feedback(UmiStructuredFormRoot *self, const char *message) {
  if (self->retired)
    return;
  GtkLabel *label = g_object_ref(GTK_LABEL(self->feedback));
  gtk_label_set_text(label, message);
  g_object_unref(label);
}
/* Numeric spelling and answer policy now live in the shared C23 text adapter,
 * so native and browser clients cannot disagree. The former GLib conversion
 * remains disabled below for review of its original behavior. */
#if 0
/* Restrict native numeric entry to decimal spelling before using GLib's
 * locale-independent conversions. Hex, NaN, whitespace and suffixes are not
 * accepted as accidental alternate ways to enter an answer. */
static int numeric_spelling(const char *text, int integer) {
  size_t at = 0U, digits = 0U;
  if (text[at] == '+' || text[at] == '-')
    ++at;
  while (text[at] >= '0' && text[at] <= '9') {
    ++at;
    ++digits;
  }
  if (digits == 0U)
    return 0;
  if (!integer && text[at] == '.') {
    ++at;
    digits = 0U;
    while (text[at] >= '0' && text[at] <= '9') {
      ++at;
      ++digits;
    }
    if (digits == 0U)
      return 0;
  }
  if (!integer && (text[at] == 'e' || text[at] == 'E')) {
    ++at;
    if (text[at] == '+' || text[at] == '-')
      ++at;
    digits = 0U;
    while (text[at] >= '0' && text[at] <= '9') {
      ++at;
      ++digits;
    }
    if (digits == 0U)
      return 0;
  }
  return text[at] == '\0';
}
static UmiStatus read_answer(UmiStructuredFormRoot *self, size_t index, UmiFormAnswer *out) {
  UmiFormAnswer answer = {0};
  const FormRow *row = &self->rows[index];
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(row->state));
  if (selected > (guint)UMI_FORM_NOT_APPLICABLE)
    return UMI_STATUS_INVALID_STATE;
  answer.state = (UmiFormAnswerState)selected;
  if (answer.state == UMI_FORM_UNKNOWN || answer.state == UMI_FORM_NOT_APPLICABLE) {
    const char *reason = gtk_editable_get_text(GTK_EDITABLE(row->reason));
    if (strlen(reason) >= sizeof(answer.reason))
      return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(answer.reason, reason, strlen(reason) + 1U);
  } else if (answer.state == UMI_FORM_KNOWN) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(row->value));
    UmiStatus status = UMI_STATUS_PARSE_ERROR;
    switch (self->schema->base->fields[index].value_type) {
    case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
      status = umi_application_experience_value_text(&answer.value, text);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
      if (strcmp(text, "true") == 0 || strcmp(text, "false") == 0)
        status = umi_application_experience_value_boolean(&answer.value, strcmp(text, "true") == 0);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER:
      if (numeric_spelling(text, 1)) {
        char *end = NULL;
        errno = 0;
        gint64 value = g_ascii_strtoll(text, &end, 10);
        if (errno == 0 && end != NULL && *end == '\0')
          status = umi_application_experience_value_integer(&answer.value, (int64_t)value);
      }
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL:
      if (numeric_spelling(text, 0)) {
        char *end = NULL;
        errno = 0;
        double value = g_ascii_strtod(text, &end);
        if (errno == 0 && end != NULL && *end == '\0' && isfinite(value))
          status = umi_application_experience_value_decimal(&answer.value, value);
      }
      break;
    default:
      break;
    }
    if (status != UMI_STATUS_OK)
      return status;
    const char *unit = self->schema->rules[index].unit_id;
    memcpy(answer.unit_id, unit, strlen(unit) + 1U);
  }
  UmiStatus status = umi_structured_form_answer_validate(self->schema, index, &answer, 0);
  if (status == UMI_STATUS_OK)
    *out = answer;
  return status;
}
#endif
static UmiStatus read_answer(UmiStructuredFormRoot *self, size_t index, UmiFormAnswer *out) {
  UmiFormTextAnswer text = {0};
  const FormRow *row = &self->rows[index];
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(row->state));
  if (selected > (guint)UMI_FORM_NOT_APPLICABLE) return UMI_STATUS_INVALID_STATE;
  text.state = (UmiFormAnswerState)selected;
  if (text.state == UMI_FORM_KNOWN) {
    UmiStatus captured = umi_structured_form_value_read(row->value, text.value, sizeof(text.value));
    if (captured != UMI_STATUS_OK) return captured;
    const char *unit = self->schema->rules[index].unit_id;
    memcpy(text.unit_id, unit, strlen(unit) + 1U);
  } else if (text.state == UMI_FORM_UNKNOWN || text.state == UMI_FORM_NOT_APPLICABLE) {
    const char *reason = gtk_editable_get_text(GTK_EDITABLE(row->reason));
    if (strlen(reason) >= sizeof(text.reason)) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(text.reason, reason, strlen(reason) + 1U);
  }
  UmiStatus status = umi_structured_form_answer_parse(self->schema, index, &text, out);
  /* Retain the native entry's existing numeric refusal status for callers. */
  UmiApplicationExperienceValueType type = self->schema->base->fields[index].value_type;
  if (status == UMI_STATUS_CAPACITY_EXCEEDED && text.state == UMI_FORM_KNOWN &&
      (type == UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER || type == UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL))
    return UMI_STATUS_PARSE_ERROR;
  return status;
}
UmiStatus UmiStructuredFormGtk4Apply(GtkWidget *widget, int submitting) {
  UmiStructuredFormRoot *self = root_from(widget);
  if (self == NULL || (submitting != 0 && submitting != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired)
    return UMI_STATUS_INVALID_STATE;
  if (self->applying)
    return UMI_STATUS_BUSY;
  g_object_ref(self);
  self->applying = 1;
  UmiStructuredForm *candidate = g_try_malloc(sizeof(*candidate));
  UmiStructuredFormEdit *edit = g_try_malloc(sizeof(*edit));
  UmiApplicationExperienceValidationReport *report = g_try_malloc(sizeof(*report));
  UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
  const char *message = "Unable to allocate the form review.";
  if (candidate != NULL && edit != NULL && report != NULL) {
    *candidate = *self->form;
    status = UMI_STATUS_OK;
    for (size_t i = 0U; i < candidate->count; ++i) {
      if (self->schema->base->fields[i].read_only)
        continue;
      UmiFormAnswer answer;
      status = read_answer(self, i, &answer);
      if (status == UMI_STATUS_OK)
        status = umi_structured_form_set(self->schema, candidate, candidate->revision,
                                         self->schema->base->fields[i].field_id, &answer);
      if (status != UMI_STATUS_OK) {
        message = "Check answer types, missing reasons, units and explicit time offsets. No "
                  "changes applied.";
        break;
      }
    }
    if (status == UMI_STATUS_OK) {
      status = umi_structured_form_validate(self->schema, candidate, submitting, report);
      if (status == UMI_STATUS_OK && report->error_count != 0U) {
        status = UMI_STATUS_INVALID_STATE;
        message = "Complete the required answers before submission. No changes applied.";
      }
    }
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_edit_capture(self->schema, self->form, candidate, edit);
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_edit_apply(self->schema, self->form, edit);
    if (status == UMI_STATUS_OK) {
      self->staged = 0;
      message = submitting
                    ? "Answers validated and applied. Save through the application to retain them."
                    : "Draft applied. Save through the application to retain it.";
    } else if (strcmp(message, "Unable to allocate the form review.") == 0)
      message = "The reviewed form changed or cannot accept this edit. No changes applied.";
  }
  g_free(report);
  g_free(edit);
  g_free(candidate);
  /* Publication precedes native notification. A status observer can type a
   * later draft or close the host; neither action redirects this accepted edit. */
  feedback(self, message);
  self->applying = 0;
  g_object_unref(self);
  return status;
}
UmiStatus UmiStructuredFormGtk4Snapshot(GtkWidget *widget, UmiStructuredForm *out) {
  UmiStructuredFormRoot *self = root_from(widget);
  if (self == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired)
    return UMI_STATUS_INVALID_STATE;
  *out = *self->form;
  return UMI_STATUS_OK;
}
UmiStatus UmiStructuredFormGtk4MarkSaved(GtkWidget *widget, uint64_t revision) {
  UmiStructuredFormRoot *self = root_from(widget);
  if (self == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired)
    return UMI_STATUS_INVALID_STATE;
  return umi_structured_form_mark_saved(self->form, revision);
}
int UmiStructuredFormGtk4CanClose(GtkWidget *widget) {
  UmiStructuredFormRoot *self = root_from(widget);
  return self != NULL && !self->retired && !self->staged &&
         self->form->revision == self->form->saved_revision;
}
void UmiStructuredFormGtk4Retire(GtkWidget *widget) {
  UmiStructuredFormRoot *self = root_from(widget);
  if (self != NULL)
    self->retired = 1;
}
static void apply_clicked(GtkButton *button, gpointer data) {
  (void)button;
  (void)UmiStructuredFormGtk4Apply(GTK_WIDGET(data), 0);
}
static void validate_clicked(GtkButton *button, gpointer data) {
  (void)button;
  (void)UmiStructuredFormGtk4Apply(GTK_WIDGET(data), 1);
}
static void name_control(GtkWidget *widget, const char *field, const char *suffix) {
  char *name = g_strdup_printf("%s.%s", field, suffix);
  gtk_widget_set_name(widget, name);
  g_free(name);
}
GtkWidget *UmiStructuredFormGtk4New(const UmiStructuredFormSchema *schema,
                                    const UmiStructuredForm *initial) {
  if (umi_structured_form_schema_validate(schema) != UMI_STATUS_OK)
    return NULL;
  UmiStructuredForm *form = g_try_malloc(sizeof(*form));
  UmiApplicationExperienceValidationReport *report = g_try_malloc(sizeof(*report));
  if (form == NULL || report == NULL) {
    g_free(form);
    g_free(report);
    return NULL;
  }
  UmiStatus status;
  if (initial != NULL) {
    *form = *initial;
    status = umi_structured_form_validate(schema, form, 0, report);
    if (status == UMI_STATUS_OK && report->error_count != 0U)
      status = UMI_STATUS_INVALID_STATE;
  } else
    status = umi_structured_form_init(schema, form);
  g_free(report);
  if (status != UMI_STATUS_OK || !g_utf8_validate(schema->base->title, -1, NULL)) {
    g_free(form);
    return NULL;
  }
  for (size_t i = 0U; i < form->count; ++i) {
    if (!g_utf8_validate(schema->base->fields[i].field_id, -1, NULL) ||
        !g_utf8_validate(schema->base->fields[i].label, -1, NULL) ||
        !g_utf8_validate(schema->base->fields[i].help_text, -1, NULL) ||
        !g_utf8_validate(form->answers[i].reason, -1, NULL) ||
        (form->answers[i].value.type == UMI_APPLICATION_EXPERIENCE_VALUE_TEXT &&
         !g_utf8_validate(form->answers[i].value.text_value, -1, NULL))) {
      g_free(form);
      return NULL;
    }
  }
  UmiStructuredFormRoot *self = g_object_new(umi_structured_form_root_get_type(), NULL);
  self->schema = schema;
  self->form = form;
  gtk_box_append(GTK_BOX(self), gtk_label_new(schema->base->title));
  const char *states[] = {"Unanswered", "Known", "Unknown", "Not applicable", NULL};
  for (size_t i = 0U; i < form->count; ++i) {
    const UmiApplicationExperienceField *field = &schema->base->fields[i];
    const UmiFormAnswer *answer = &form->answers[i];
    FormRow *row = &self->rows[i];
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *label = gtk_label_new(field->label);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    row->state = gtk_drop_down_new_from_strings(states);
    name_control(row->state, field->field_id, "state");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(row->state), (guint)answer->state);
    gtk_box_append(GTK_BOX(box), row->state);
    /* Field kinds now choose native controls and share C23 text conversion.
     * Retain the earlier entry-only composition for engineering comparison. */
#if 0
    row->value = gtk_entry_new();
    name_control(row->value, field->field_id, "value");
    gtk_widget_set_tooltip_text(row->value, field->help_text);
    if (field->kind == UMI_APPLICATION_EXPERIENCE_FIELD_SECRET)
      gtk_entry_set_visibility(GTK_ENTRY(row->value), FALSE);
    char number[G_ASCII_DTOSTR_BUF_SIZE];
    const char *text = "";
    if (answer->state == UMI_FORM_KNOWN) {
      switch (answer->value.type) {
      case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
        text = answer->value.text_value;
        break;
      case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
        text = answer->value.boolean_value ? "true" : "false";
        break;
      case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER:
        g_snprintf(number, sizeof(number), "%" G_GINT64_FORMAT,
                   (gint64)answer->value.integer_value);
        text = number;
        break;
      case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL:
        text = g_ascii_dtostr(number, sizeof(number), answer->value.decimal_value);
        break;
      default:
        break;
      }
    }
    gtk_editable_set_text(GTK_EDITABLE(row->value), text);
    gtk_box_append(GTK_BOX(box), row->value);
#endif
    row->value = umi_structured_form_value_new(schema, i, answer);
    if (row->value == NULL) {
      g_object_ref_sink(box); g_object_unref(box);
      g_object_ref_sink(self); g_object_unref(self); return NULL;
    }
    name_control(row->value, field->field_id, "value");
    gtk_widget_set_tooltip_text(row->value, field->help_text);
    if (GTK_IS_TEXT_VIEW(row->value)) {
      GtkWidget *scroll = gtk_scrolled_window_new();
      gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 96);
      gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), row->value);
      gtk_box_append(GTK_BOX(box), scroll);
    } else gtk_box_append(GTK_BOX(box), row->value);
    if (schema->rules[i].unit_id[0] != '\0')
      gtk_box_append(GTK_BOX(box), gtk_label_new(schema->rules[i].unit_id));
    row->reason = gtk_entry_new();
    name_control(row->reason, field->field_id, "reason");
    gtk_entry_set_placeholder_text(GTK_ENTRY(row->reason), "Reason when unknown or not applicable");
    gtk_editable_set_text(GTK_EDITABLE(row->reason), answer->reason);
    gtk_box_append(GTK_BOX(box), row->reason);
    gtk_widget_set_sensitive(box, !field->read_only);
    gtk_box_append(GTK_BOX(self), box);
    if (GTK_IS_TEXT_VIEW(row->value))
      g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(row->value)), "changed", G_CALLBACK(buffer_changed), self, 0);
    else if (GTK_IS_DROP_DOWN(row->value))
      g_signal_connect_object(row->value, "notify::selected", G_CALLBACK(state_changed), self, 0);
    else g_signal_connect_object(row->value, "changed", G_CALLBACK(text_changed), self, 0);
    g_signal_connect_object(row->reason, "changed", G_CALLBACK(text_changed), self, 0);
    g_signal_connect_object(row->state, "notify::selected", G_CALLBACK(state_changed), self, 0);
  }
  GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  GtkWidget *apply = gtk_button_new_with_label("Apply draft"),
            *validate = gtk_button_new_with_label("Validate for submission");
  gtk_box_append(GTK_BOX(buttons), apply);
  gtk_box_append(GTK_BOX(buttons), validate);
  gtk_box_append(GTK_BOX(self), buttons);
  g_signal_connect_object(apply, "clicked", G_CALLBACK(apply_clicked), self, 0);
  g_signal_connect_object(validate, "clicked", G_CALLBACK(validate_clicked), self, 0);
  self->feedback =
      gtk_label_new("Edit answers, then apply the complete draft. No data is sent automatically.");
  gtk_widget_set_name(self->feedback, "structured-form-feedback");
  gtk_label_set_wrap(GTK_LABEL(self->feedback), TRUE);
  gtk_box_append(GTK_BOX(self), self->feedback);
  return GTK_WIDGET(self);
}
