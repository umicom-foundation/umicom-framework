/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/structured_form_filter_gtk4.c
 *
 * PURPOSE:
 *   Rows keep local text; only the shared parser publishes typed filter operands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Rows keep local text; only the shared parser publishes typed filter operands. */
#include "umicom/ui/gtk4/structured_form_filter.h"
#include <string.h>
typedef struct UmiFormFilterEditor {
  GtkBox parent;
  const UmiStructuredFormSchema *schema;
  GtkWidget *heading, *rows;
  uint64_t revision;
  size_t count;
  bool working, emitting, retired;
  guint deferred_signal;
} UmiFormFilterEditor;
typedef struct UmiFormFilterEditorClass {
  GtkBoxClass parent;
} UmiFormFilterEditorClass;
GType umi_form_filter_editor_get_type(void);
G_DEFINE_TYPE(UmiFormFilterEditor, umi_form_filter_editor, GTK_TYPE_BOX)
static guint changed_signal;
typedef struct FilterRow {
  GWeakRef editor;
  GtkWidget *field, *operation, *value, *unit;
} FilterRow;
static const char *operators[] = {
    "Equals",   "Contains",      "Less than", "At most",    "Greater than",
    "At least", "Is unanswered", "Is known",  "Is unknown", "Is not applicable",
    NULL};
static void row_free(gpointer data) {
  FilterRow *row = data;
  g_weak_ref_clear(&row->editor);
  g_free(row);
}
static UmiFormFilterEditor *instance(GtkWidget *widget) {
  return widget && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_form_filter_editor_get_type())
             ? (UmiFormFilterEditor *)widget
             : NULL;
}
static void editor_unroot(GtkWidget *widget) {
  ((UmiFormFilterEditor *)widget)->retired = true;
  GTK_WIDGET_CLASS(umi_form_filter_editor_parent_class)->unroot(widget);
}
static void editor_dispose(GObject *object) {
  ((UmiFormFilterEditor *)object)->retired = true;
  G_OBJECT_CLASS(umi_form_filter_editor_parent_class)->dispose(object);
}
static void umi_form_filter_editor_class_init(UmiFormFilterEditorClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = editor_unroot;
  G_OBJECT_CLASS(klass)->dispose = editor_dispose;
  changed_signal = g_signal_new("filter-changed", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST, 0,
                                NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_UINT64);
}
static void umi_form_filter_editor_init(UmiFormFilterEditor *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 8);
  self->revision = 1;
}
static UmiStatus enter(UmiFormFilterEditor *self, uint64_t expected) {
  if (!self)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired)
    return UMI_STATUS_INVALID_STATE;
  if (self->working || self->emitting)
    return UMI_STATUS_BUSY;
  if (expected != self->revision)
    return UMI_STATUS_INVALID_STATE;
  if (self->revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  g_object_ref(self);
  self->working = true;
  return UMI_STATUS_OK;
}
/* Native controls can be edited by a change observer. Advance identity at once
 * and coalesce the recursive notification onto the main loop. The previous
 * callback must never leave changed query text sharing its earlier revision. */
static gboolean deferred_changed(gpointer context) {
  UmiFormFilterEditor *self = context;
  if (!self->retired && (self->working || self->emitting))
    return G_SOURCE_CONTINUE;
  self->deferred_signal = 0;
  if (!self->retired) {
    self->emitting = true;
    g_signal_emit(self, changed_signal, 0, self->revision);
    self->emitting = false;
  }
  return G_SOURCE_REMOVE;
}
static void advance(UmiFormFilterEditor *self) {
  if (self->retired)
    return;
  if (self->revision == UINT64_MAX) {
    self->retired = true;
    return;
  }
  ++self->revision;
  if (self->emitting) {
    if (!self->deferred_signal)
      self->deferred_signal = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, deferred_changed,
                                              g_object_ref(self), g_object_unref);
    return;
  }
  self->emitting = true;
  g_signal_emit(self, changed_signal, 0, self->revision);
  self->emitting = false;
}
static UmiStatus leave(UmiFormFilterEditor *self, UmiStatus status, bool changed) {
  self->working = false;
  if (self->retired)
    status = UMI_STATUS_CANCELLED;
  if (status == UMI_STATUS_OK && changed) {
    /* Retain the direct emission path for comparison. advance() also records
     * native edits made by an observer without recursive signal emission. */
#if 0
        ++self->revision; self->emitting = true;
        g_signal_emit(self, changed_signal, 0, self->revision);
        self->emitting = false;
#endif
    advance(self);
  }
  g_object_unref(self);
  return status;
}
static UmiFormFilterEditor *row_owner(GtkWidget *widget) {
  FilterRow *row = g_object_get_data(G_OBJECT(widget), "form-filter-row");
  if (!row)
    return NULL;
  UmiFormFilterEditor *self = g_weak_ref_get(&row->editor);
  if (!self)
    return NULL;
  if (self->retired || gtk_widget_get_parent(widget) != self->rows) {
    g_object_unref(self);
    return NULL;
  }
  return self;
}
static void row_labels(UmiFormFilterEditor *self, FilterRow *row) {
  guint field = gtk_drop_down_get_selected(GTK_DROP_DOWN(row->field));
  guint operation = gtk_drop_down_get_selected(GTK_DROP_DOWN(row->operation));
  gtk_label_set_text(GTK_LABEL(row->unit),
                     field < self->schema->rule_count ? self->schema->rules[field].unit_id : "");
  if (!self->retired)
    gtk_widget_set_sensitive(row->value, operation < 6);
}
/* The former re-entry guard ignored native edits during signal emission. Keep
 * it for review; the replacement records those edits and defers notification. */
#if 0
static void native_changed(GtkWidget *widget) {
    UmiFormFilterEditor *self = row_owner(widget);
    if (!self) return;
    if (!self->working && !self->emitting) {
        UmiStatus status = enter(self, self->revision);
        if (status == UMI_STATUS_OK) {
            /* An external property observer may retire the editor while labels
             * change. The temporary row reference keeps its children available. */
            g_object_ref(widget);
            row_labels(self, g_object_get_data(G_OBJECT(widget), "form-filter-row"));
            g_object_unref(widget);
            (void)leave(self, UMI_STATUS_OK, true);
        } else if (status == UMI_STATUS_CAPACITY_EXCEEDED) self->retired = true;
    }
    g_object_unref(self);
}
#endif
static void native_changed(GtkWidget *widget) {
  UmiFormFilterEditor *self = row_owner(widget);
  if (!self)
    return;
  if (!self->working) {
    self->working = true;
    g_object_ref(widget);
    row_labels(self, g_object_get_data(G_OBJECT(widget), "form-filter-row"));
    g_object_unref(widget);
    self->working = false;
    advance(self);
  }
  g_object_unref(self);
}
static void selected(GObject *source, GParamSpec *spec, gpointer context) {
  (void)source;
  (void)spec;
  native_changed(GTK_WIDGET(context));
}
static void typed(GtkEditable *source, gpointer context) {
  (void)source;
  native_changed(GTK_WIDGET(context));
}
static void remove_clicked(GtkButton *source, gpointer context) {
  (void)source;
  GtkWidget *widget = context;
  UmiFormFilterEditor *self = row_owner(widget);
  if (!self)
    return;
  size_t index = 0;
  GtkWidget *child = gtk_widget_get_first_child(self->rows);
  while (child && child != widget) {
    child = gtk_widget_get_next_sibling(child);
    ++index;
  }
  if (child)
    (void)UmiStructuredFormFilterGtk4Remove(GTK_WIDGET(self), index, self->revision);
  g_object_unref(self);
}
static GtkWidget *row_new(UmiFormFilterEditor *self, const UmiFormFilterTextTerm *initial) {
  GtkWidget *widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_set_name(widget, "form-filter-term");
  FilterRow *row = g_new0(FilterRow, 1);
  g_weak_ref_init(&row->editor, G_OBJECT(self));
  g_object_set_data_full(G_OBJECT(widget), "form-filter-row", row, row_free);
  const char *labels[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY + 1] = {0};
  for (size_t i = 0; i < self->schema->rule_count; ++i)
    labels[i] = self->schema->base->fields[i].label;
  row->field = gtk_drop_down_new_from_strings(labels);
  row->operation = gtk_drop_down_new_from_strings(operators);
  row->value = gtk_entry_new();
  row->unit = gtk_label_new("");
  GtkWidget *remove = gtk_button_new_with_label("Remove");
  gtk_widget_set_name(row->field, "form-filter-field");
  gtk_widget_set_name(row->operation, "form-filter-operation");
  gtk_widget_set_name(row->value, "form-filter-value");
  gtk_widget_set_name(row->unit, "form-filter-unit");
  gtk_widget_set_name(remove, "form-filter-remove");
  gtk_widget_set_hexpand(row->value, TRUE);
  gtk_box_append(GTK_BOX(widget), row->field);
  gtk_box_append(GTK_BOX(widget), row->operation);
  gtk_box_append(GTK_BOX(widget), row->value);
  gtk_box_append(GTK_BOX(widget), row->unit);
  gtk_box_append(GTK_BOX(widget), remove);
  if (initial) {
    size_t index =
        umi_application_experience_form_schema_index(self->schema->base, initial->field_id);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(row->field), (guint)index);
    guint operation = initial->operation == UMI_FORM_FILTER_STATE ? 6U + (guint)initial->state
                                                                  : (guint)initial->operation - 1U;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(row->operation), operation);
    gtk_editable_set_text(GTK_EDITABLE(row->value), initial->value);
  } else
    gtk_drop_down_set_selected(GTK_DROP_DOWN(row->operation), 7);
  row_labels(self, row);
  /* Signal contexts are the row object, with automatic disconnection when it
   * dies. A separately retained removed button cannot use a freed row state. */
  g_signal_connect_object(row->field, "notify::selected", G_CALLBACK(selected), widget, 0);
  g_signal_connect_object(row->operation, "notify::selected", G_CALLBACK(selected), widget, 0);
  g_signal_connect_object(row->value, "changed", G_CALLBACK(typed), widget, 0);
  g_signal_connect_object(remove, "clicked", G_CALLBACK(remove_clicked), widget, 0);
  return widget;
}
UmiStatus UmiStructuredFormFilterGtk4Add(GtkWidget *widget, uint64_t expected) {
  UmiFormFilterEditor *self = instance(widget);
  UmiStatus status = enter(self, expected);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->count == UMI_FORM_FILTER_CAPACITY)
    return leave(self, UMI_STATUS_CAPACITY_EXCEEDED, false);
  GtkWidget *row = row_new(self, NULL);
  ++self->count;
  gtk_box_append(GTK_BOX(self->rows), row);
  return leave(self, UMI_STATUS_OK, true);
}
UmiStatus UmiStructuredFormFilterGtk4Remove(GtkWidget *widget, size_t index, uint64_t expected) {
  UmiFormFilterEditor *self = instance(widget);
  UmiStatus status = enter(self, expected);
  if (status != UMI_STATUS_OK)
    return status;
  if (index >= self->count)
    return leave(self, UMI_STATUS_NOT_FOUND, false);
  GtkWidget *row = gtk_widget_get_first_child(self->rows);
  for (size_t i = 0; i < index; ++i)
    row = gtk_widget_get_next_sibling(row);
  --self->count;
  gtk_box_remove(GTK_BOX(self->rows), row);
  return leave(self, UMI_STATUS_OK, true);
}
UmiStatus UmiStructuredFormFilterGtk4Set(GtkWidget *widget, const UmiStructuredFormFilter *filter,
                                         uint64_t expected) {
  UmiFormFilterEditor *self = instance(widget);
  UmiStatus status = enter(self, expected);
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_structured_form_filter_validate(self->schema, filter);
  UmiFormFilterTextTerm *text = NULL;
  if (status == UMI_STATUS_OK) {
    text = g_try_new0(UmiFormFilterTextTerm, filter->count ? filter->count : 1);
    if (!text)
      status = UMI_STATUS_OUT_OF_MEMORY;
  }
  for (size_t i = 0; status == UMI_STATUS_OK && i < filter->count; ++i) {
    status = umi_structured_form_filter_term_format(self->schema, &filter->terms[i], &text[i]);
    if (status == UMI_STATUS_OK && !g_utf8_validate(text[i].value, -1, NULL))
      status = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (status == UMI_STATUS_OK) {
    GtkWidget *rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_ref_sink(rows);
    gtk_widget_set_name(rows, "form-filter-rows");
    for (size_t i = 0; i < filter->count; ++i)
      gtk_box_append(GTK_BOX(rows), row_new(self, &text[i]));
    GtkWidget *old = g_object_ref(self->rows);
    self->rows = rows;
    self->count = filter->count;
    gtk_box_insert_child_after(GTK_BOX(self), rows, self->heading);
    if (gtk_widget_get_parent(old) == GTK_WIDGET(self))
      gtk_box_remove(GTK_BOX(self), old);
    g_object_unref(old);
    g_object_unref(rows);
  }
  g_free(text);
  return leave(self, status, status == UMI_STATUS_OK);
}
UmiStatus UmiStructuredFormFilterGtk4Read(GtkWidget *widget, UmiStructuredFormFilter *out,
                                          uint64_t *revision) {
  UmiFormFilterEditor *self = instance(widget);
  if (!self || !out || !revision)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired)
    return UMI_STATUS_INVALID_STATE;
  if (self->working)
    return UMI_STATUS_BUSY;
  UmiStructuredFormFilter *filter = g_try_new0(UmiStructuredFormFilter, 1);
  if (!filter)
    return UMI_STATUS_OUT_OF_MEMORY;
  filter->count = self->count;
  UmiStatus status = UMI_STATUS_OK;
  size_t i = 0;
  for (GtkWidget *widget_row = gtk_widget_get_first_child(self->rows);
       widget_row && status == UMI_STATUS_OK;
       widget_row = gtk_widget_get_next_sibling(widget_row)) {
    if (i >= filter->count) {
      status = UMI_STATUS_INVALID_STATE;
      break;
    }
    FilterRow *row = g_object_get_data(G_OBJECT(widget_row), "form-filter-row");
    guint field = gtk_drop_down_get_selected(GTK_DROP_DOWN(row->field)),
          operation = gtk_drop_down_get_selected(GTK_DROP_DOWN(row->operation));
    if (field >= self->schema->rule_count || operation >= 10) {
      status = UMI_STATUS_INVALID_STATE;
      break;
    }
    UmiFormFilterTextTerm text = {0};
    strcpy(text.field_id, self->schema->base->fields[field].field_id);
    text.operation = operation < 6 ? (UmiFormFilterOperator)(operation + 1) : UMI_FORM_FILTER_STATE;
    text.state = operation < 6 ? UMI_FORM_KNOWN : (UmiFormAnswerState)(operation - 6);
    if (operation < 6) {
      const char *value = gtk_editable_get_text(GTK_EDITABLE(row->value));
      if (strlen(value) >= sizeof text.value) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        break;
      }
      memcpy(text.value, value, strlen(value) + 1);
    }
    status = umi_structured_form_filter_term_parse(self->schema, &text, &filter->terms[i++]);
  }
  if (status == UMI_STATUS_OK && i != filter->count)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    *out = *filter;
    *revision = self->revision;
  }
  g_free(filter);
  return status;
}
uint64_t UmiStructuredFormFilterGtk4Revision(GtkWidget *widget) {
  UmiFormFilterEditor *self = instance(widget);
  return self && !self->retired ? self->revision : 0;
}
void UmiStructuredFormFilterGtk4Retire(GtkWidget *widget) {
  UmiFormFilterEditor *self = instance(widget);
  if (self)
    self->retired = true;
}
static void add_clicked(GtkButton *button, gpointer context) {
  (void)button;
  UmiFormFilterEditor *self = context;
  (void)UmiStructuredFormFilterGtk4Add(GTK_WIDGET(self), self->revision);
}
UmiStatus UmiStructuredFormFilterGtk4New(const UmiStructuredFormSchema *schema, GtkWidget **out) {
  if (out)
    *out = NULL;
  if (!out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (!schema->rule_count)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < schema->rule_count; ++i) {
    if (schema->base->fields[i].kind == UMI_APPLICATION_EXPERIENCE_FIELD_SECRET ||
        !g_utf8_validate(schema->base->fields[i].label, -1, NULL))
      return UMI_STATUS_INVALID_ARGUMENT;
  }
  UmiFormFilterEditor *self = g_object_new(umi_form_filter_editor_get_type(), NULL);
  self->schema = schema;
  self->heading = gtk_label_new("Match every condition. No conditions matches every valid record.");
  gtk_label_set_wrap(GTK_LABEL(self->heading), TRUE);
  gtk_box_append(GTK_BOX(self), self->heading);
  self->rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_set_name(self->rows, "form-filter-rows");
  gtk_box_append(GTK_BOX(self), self->rows);
  GtkWidget *add = gtk_button_new_with_label("Add condition");
  gtk_widget_set_name(add, "form-filter-add");
  gtk_box_append(GTK_BOX(self), add);
  g_signal_connect_object(add, "clicked", G_CALLBACK(add_clicked), self, 0);
  *out = GTK_WIDGET(self);
  return UMI_STATUS_OK;
}
