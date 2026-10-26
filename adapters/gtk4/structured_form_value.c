/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/structured_form_value.c
 * PURPOSE: Use native multiline, boolean and choice controls without duplicating validation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_value.h"
#include "umicom/base/text.h"
#include <string.h>
#define VALUE_IDS "umicom-structured-form-option-identifiers"
GtkWidget *umi_structured_form_value_new(const UmiStructuredFormSchema *schema, size_t index,
                                         const UmiFormAnswer *answer) {
  UmiFormTextAnswer text;
  if (umi_structured_form_answer_format(schema, index, answer, &text) != UMI_STATUS_OK)
    return NULL;
  const UmiApplicationExperienceField *field = &schema->base->fields[index];
  if (!g_utf8_validate(text.value, -1, NULL))
    return NULL;
  if (field->kind == UMI_APPLICATION_EXPERIENCE_FIELD_MULTILINE) {
    GtkWidget *view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), text.value, -1);
    return view;
  }
  if (field->value_type == UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN ||
      field->kind == UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE) {
    /* GTK may select the first item of a populated dropdown automatically.
     * Keep the former index mapping for review. A non-answer row now occupies
     * that position so an unknown answer cannot become false or an option ID
     * merely because the user marks the answer as known. */
#if 0
    char **ids = g_new0(char *, UMI_APPLICATION_EXPERIENCE_OPTION_CAPACITY + 1U);
    const char *labels[UMI_APPLICATION_EXPERIENCE_OPTION_CAPACITY + 1U] = {0};
    size_t count = 0U;
    guint selected = GTK_INVALID_LIST_POSITION;
    if (field->value_type == UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN) {
      ids[0] = g_strdup("false");
      ids[1] = g_strdup("true");
      labels[0] = "False";
      labels[1] = "True";
      count = 2U;
    } else {
      for (size_t i = 0U; i < field->option_count; ++i) {
        if (field->options[i].disabled)
          continue;
        if (!g_utf8_validate(field->options[i].label, -1, NULL) ||
            !g_utf8_validate(field->options[i].option_id, -1, NULL)) {
          g_strfreev(ids);
          return NULL;
        }
        ids[count] = g_strdup(field->options[i].option_id);
        labels[count++] = field->options[i].label;
      }
    }
    for (size_t i = 0U; i < count; ++i)
      if (text.state == UMI_FORM_KNOWN && strcmp(ids[i], text.value) == 0)
        selected = (guint)i;
    GtkWidget *dropdown = gtk_drop_down_new_from_strings(labels);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), selected);
    /* Own identifiers with the control; display labels never become stored IDs. */
    g_object_set_data_full(G_OBJECT(dropdown), VALUE_IDS, ids, (GDestroyNotify)g_strfreev);
    return dropdown;
#endif
    char **ids = g_new0(char *, UMI_APPLICATION_EXPERIENCE_OPTION_CAPACITY + 2U);
    const char *labels[UMI_APPLICATION_EXPERIENCE_OPTION_CAPACITY + 2U] = {0};
    ids[0] = g_strdup("");
    labels[0] = "Select a value";
    size_t count = 1U;
    guint selected = 0U;
    if (field->value_type == UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN) {
      ids[1] = g_strdup("false");
      ids[2] = g_strdup("true");
      labels[1] = "False";
      labels[2] = "True";
      count = 3U;
    } else {
      for (size_t i = 0U; i < field->option_count; ++i) {
        if (field->options[i].disabled)
          continue;
        if (!g_utf8_validate(field->options[i].label, -1, NULL) ||
            !g_utf8_validate(field->options[i].option_id, -1, NULL)) {
          g_strfreev(ids);
          return NULL;
        }
        ids[count] = g_strdup(field->options[i].option_id);
        labels[count++] = field->options[i].label;
      }
    }
    for (size_t i = 1U; i < count; ++i)
      if (text.state == UMI_FORM_KNOWN && strcmp(ids[i], text.value) == 0)
        selected = (guint)i;
    GtkWidget *dropdown = gtk_drop_down_new_from_strings(labels);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), selected);
    /* Own identifiers with the control; display labels never become stored IDs. */
    g_object_set_data_full(G_OBJECT(dropdown), VALUE_IDS, ids, (GDestroyNotify)g_strfreev);
    return dropdown;
  }
  GtkWidget *entry = gtk_entry_new();
  if (field->kind == UMI_APPLICATION_EXPERIENCE_FIELD_SECRET)
    gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
  if (field->value_type == UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER ||
      field->value_type == UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL)
    gtk_entry_set_input_purpose(GTK_ENTRY(entry), GTK_INPUT_PURPOSE_NUMBER);
  if (schema->rules[index].text_format == UMI_FORM_TEXT_OFFSET_TIMESTAMP)
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "2024-02-29T12:00:00+01:00");
  else if (schema->rules[index].text_format == UMI_FORM_TEXT_DATE)
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "2024-02-29");
  gtk_editable_set_text(GTK_EDITABLE(entry), text.value);
  return entry;
}
UmiStatus umi_structured_form_value_read(GtkWidget *widget, char *out, size_t capacity) {
  if (widget == NULL || out == NULL || capacity == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (GTK_IS_TEXT_VIEW(widget)) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    char *text = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
    if (text == NULL)
      return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_text_replace(out, capacity, text);
    g_free(text);
    return status;
  }
  if (GTK_IS_DROP_DOWN(widget)) {
    char **ids = g_object_get_data(G_OBJECT(widget), VALUE_IDS);
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(widget));
    /* Retain the previous check; row zero now represents no chosen value. */
#if 0
    if (ids == NULL || selected == GTK_INVALID_LIST_POSITION || selected >= g_strv_length(ids))
      return UMI_STATUS_INVALID_STATE;
#endif
    if (ids == NULL || selected == 0U || selected == GTK_INVALID_LIST_POSITION ||
        selected >= g_strv_length(ids))
      return UMI_STATUS_INVALID_STATE;
    return umi_text_replace(out, capacity, ids[selected]);
  }
  if (GTK_IS_EDITABLE(widget))
    return umi_text_replace(out, capacity, gtk_editable_get_text(GTK_EDITABLE(widget)));
  return UMI_STATUS_INVALID_ARGUMENT;
}
