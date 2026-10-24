/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Historical inspection uses copied labels so changing a schema or current
 * draft later cannot turn the displayed history into another editable record. */
#include "umicom/application/experience/structured_form_text.h"
#include "umicom/ui/gtk4/structured_form_inspect.h"
#include <inttypes.h>

static GtkWidget *label(GtkWidget *parent, const char *name, const char *text) {
  GtkWidget *item = gtk_label_new(text);
  gtk_widget_set_name(item, name);
  gtk_label_set_xalign(GTK_LABEL(item), 0);
  gtk_label_set_wrap(GTK_LABEL(item), TRUE);
  gtk_label_set_selectable(GTK_LABEL(item), TRUE);
  gtk_box_append(GTK_BOX(parent), item);
  return item;
}
UmiStatus UmiStructuredFormInspectGtk4New(const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *form, GtkWidget **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiApplicationExperienceValidationReport *report =
      g_try_new0(UmiApplicationExperienceValidationReport, 1);
  if (report == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_structured_form_validate(schema, form, 0, report);
  if (status == UMI_STATUS_OK && report->error_count != 0)
    status = UMI_STATUS_INVALID_STATE;
  g_free(report);
  if (status != UMI_STATUS_OK)
    return status;
  UmiFormTextAnswer *answers = g_try_new0(UmiFormTextAnswer, form->count);
  if (answers == NULL && form->count != 0)
    return UMI_STATUS_OUT_OF_MEMORY;
  if (!g_utf8_validate(schema->base->title, -1, NULL))
    status = UMI_STATUS_PARSE_ERROR;
  for (size_t i = 0; status == UMI_STATUS_OK && i < form->count; ++i) {
    const UmiApplicationExperienceField *field = &schema->base->fields[i];
    if (field->kind == UMI_APPLICATION_EXPERIENCE_FIELD_SECRET)
      status = UMI_STATUS_PERMISSION_DENIED;
    else if (!g_utf8_validate(field->label, -1, NULL))
      status = UMI_STATUS_PARSE_ERROR;
    else
      status = umi_structured_form_answer_format(schema, i, &form->answers[i], &answers[i]);
    if (status == UMI_STATUS_OK && (!g_utf8_validate(answers[i].value, -1, NULL) ||
                                    !g_utf8_validate(answers[i].reason, -1, NULL) ||
                                    !g_utf8_validate(answers[i].unit_id, -1, NULL)))
      status = UMI_STATUS_PARSE_ERROR;
  }
  if (status != UMI_STATUS_OK) {
    g_free(answers);
    return status;
  }
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  (void)label(box, "form-inspect-title", schema->base->title);
  char *identity =
      g_strdup_printf("Schema %" PRIu64 " | form edit revision %" PRIu64 " | Read-only snapshot",
                      form->schema_version, form->revision);
  (void)label(box, "form-inspect-identity", identity);
  g_free(identity);
  for (size_t i = 0; i < form->count; ++i) {
    const UmiApplicationExperienceField *field = &schema->base->fields[i];
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_append(GTK_BOX(box), row);
    (void)label(row, "form-inspect-field-label", field->label);
    bool known = answers[i].state == UMI_FORM_KNOWN;
    char *answer = g_strdup_printf("%s%s%s%s%s", umi_form_answer_state_text(answers[i].state),
                                   known || answers[i].reason[0] != '\0' ? " — " : "",
                                   known ? answers[i].value : answers[i].reason,
                                   known && answers[i].unit_id[0] != '\0' ? " " : "",
                                   known ? answers[i].unit_id : "");
    char *name = g_strdup_printf("form-inspect.%s.value", field->field_id);
    (void)label(row, name, answer);
    g_free(name);
    g_free(answer);
  }
  g_free(answers);
  *out = box;
  return UMI_STATUS_OK;
}
