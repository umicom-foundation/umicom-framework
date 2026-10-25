/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/structured_form_search_draft_gtk4.c
 *
 * PURPOSE:
 *   Keep incomplete native text local while portable sessions own valid drafts.
 *   Complete replacement controls are built before changing the active model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Keep incomplete native text local while portable sessions own valid drafts.
 * Complete replacement controls are built before changing the active model. */
#include "umicom/base/text.h"
#include "umicom/ui/gtk4/structured_form_filter.h"
#include "umicom/ui/gtk4/structured_form_search_draft.h"
#include <string.h>
typedef struct UmiSearchDraftRoot {
  GtkBox parent;
  const UmiStructuredFormSchema *schema;
  UmiStructuredFormSearchSession *session;
  GtkWidget *body, *title, *metadata, *filter;
  bool retired, working;
} UmiSearchDraftRoot;
typedef struct UmiSearchDraftRootClass {
  GtkBoxClass parent;
} UmiSearchDraftRootClass;
GType umi_search_draft_root_get_type(void) G_GNUC_CONST;
G_DEFINE_TYPE(UmiSearchDraftRoot, umi_search_draft_root, GTK_TYPE_BOX)
static UmiSearchDraftRoot *root_from(GtkWidget *widget) {
  return widget && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_search_draft_root_get_type())
             ? (UmiSearchDraftRoot *)widget
             : NULL;
}
static void retire(UmiSearchDraftRoot *self) {
  self->retired = true;
  if (self->filter)
    UmiStructuredFormFilterGtk4Retire(self->filter);
}
static void unroot(GtkWidget *widget) {
  retire((UmiSearchDraftRoot *)widget);
  GTK_WIDGET_CLASS(umi_search_draft_root_parent_class)->unroot(widget);
}
static void dispose(GObject *object) {
  UmiSearchDraftRoot *self = (UmiSearchDraftRoot *)object;
  retire(self);
  self->body = self->title = self->metadata = self->filter = NULL;
  G_OBJECT_CLASS(umi_search_draft_root_parent_class)->dispose(object);
}
static void finalize(GObject *object) {
  UmiSearchDraftRoot *self = (UmiSearchDraftRoot *)object;
  umi_structured_form_search_session_destroy(self->session);
  G_OBJECT_CLASS(umi_search_draft_root_parent_class)->finalize(object);
}
static void umi_search_draft_root_class_init(UmiSearchDraftRootClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = unroot;
  G_OBJECT_CLASS(klass)->dispose = dispose;
  G_OBJECT_CLASS(klass)->finalize = finalize;
}
static void umi_search_draft_root_init(UmiSearchDraftRoot *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 8);
}
static UmiStatus enter(UmiSearchDraftRoot *self, bool completing) {
  if (!self)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired && !completing)
    return UMI_STATUS_INVALID_STATE;
  if (self->working)
    return UMI_STATUS_BUSY;
  g_object_ref(self);
  self->working = true;
  return UMI_STATUS_OK;
}
static UmiStatus leave(UmiSearchDraftRoot *self, UmiStatus status) {
  self->working = false;
  g_object_unref(self);
  return status;
}
static UmiStatus native_values(UmiSearchDraftRoot *self, UmiStructuredFormSearchPreset *out) {
  UmiStructuredFormSearchSessionView *view = g_try_new0(UmiStructuredFormSearchSessionView, 1);
  if (!view)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_structured_form_search_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK && !view->record.ready)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    strcpy(out->entity_prefix, view->preset.entity_prefix);
    status = umi_text_replace(out->title, sizeof out->title,
                              gtk_editable_get_text(GTK_EDITABLE(self->title)));
  }
  if (status == UMI_STATUS_OK)
    status = umi_text_replace(out->metadata_text, sizeof out->metadata_text,
                              gtk_editable_get_text(GTK_EDITABLE(self->metadata)));
  uint64_t revision = 0;
  if (status == UMI_STATUS_OK)
    status = UmiStructuredFormFilterGtk4Read(self->filter, &out->filter, &revision);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_preset_validate(self->schema, out);
  g_free(view);
  return status;
}
static UmiStatus accept_visible(UmiSearchDraftRoot *self) {
  UmiStructuredFormSearchSessionView *view = g_try_new0(UmiStructuredFormSearchSessionView, 1);
  UmiStructuredFormSearchPreset *preset = g_try_new0(UmiStructuredFormSearchPreset, 1);
  unsigned char *a = g_try_malloc(UMI_FORM_SEARCH_PRESET_MAX_BYTES),
                *b = g_try_malloc(UMI_FORM_SEARCH_PRESET_MAX_BYTES);
  if (!view || !preset || !a || !b) {
    g_free(view);
    g_free(preset);
    g_free(a);
    g_free(b);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiStatus status = umi_structured_form_search_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK && view->record.ready) {
    status = native_values(self, preset);
    size_t na = 0, nb = 0;
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_search_preset_encode(self->schema, &view->preset, a,
                                                        UMI_FORM_SEARCH_PRESET_MAX_BYTES, &na);
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_search_preset_encode(self->schema, preset, b,
                                                        UMI_FORM_SEARCH_PRESET_MAX_BYTES, &nb);
    if (status == UMI_STATUS_OK && (na != nb || memcmp(a, b, na)))
      status = umi_structured_form_search_session_edit(self->session, view->generation,
                                                       view->edit_revision, preset);
  }
  g_free(view);
  g_free(preset);
  g_free(a);
  g_free(b);
  return status;
}
static void discard_clicked(GtkButton *button, gpointer data) {
  UmiSearchDraftRoot *self = data;
  if (!self->retired && self->body && gtk_widget_is_ancestor(GTK_WIDGET(button), self->body))
    (void)UmiStructuredFormSearchDraftGtk4Discard(GTK_WIDGET(self));
}
static UmiStatus make_body(UmiSearchDraftRoot *self, const char *entity,
                           const UmiStructuredFormSearchPreset *preset, GtkWidget **out) {
  /* Bound record identifiers before asking GTK to inspect UTF-8 text. */
  size_t entity_length = 0;
  while (entity_length < 64 && entity[entity_length])
    ++entity_length;
  if (entity_length == 64)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!g_utf8_validate(preset->title, -1, NULL) ||
      !g_utf8_validate(preset->metadata_text, -1, NULL) || !g_utf8_validate(entity, -1, NULL))
    return UMI_STATUS_PARSE_ERROR;
  GtkWidget *filter = NULL;
  UmiStatus status = UmiStructuredFormFilterGtk4New(self->schema, &filter);
  if (status != UMI_STATUS_OK)
    return status;
  g_object_ref_sink(filter);
  status = UmiStructuredFormFilterGtk4Set(filter, &preset->filter,
                                          UmiStructuredFormFilterGtk4Revision(filter));
  if (status != UMI_STATUS_OK) {
    g_object_unref(filter);
    return status;
  }
  GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  g_object_ref_sink(body);
  gtk_box_append(GTK_BOX(body), gtk_label_new(entity));
  GtkWidget *title = gtk_entry_new(), *metadata = gtk_entry_new();
  gtk_widget_set_name(title, "search-draft-title");
  gtk_widget_set_name(metadata, "search-draft-metadata");
  gtk_entry_set_placeholder_text(GTK_ENTRY(title), "Search title");
  gtk_entry_set_placeholder_text(GTK_ENTRY(metadata), "Record metadata text");
  gtk_editable_set_text(GTK_EDITABLE(title), preset->title);
  gtk_editable_set_text(GTK_EDITABLE(metadata), preset->metadata_text);
  gtk_box_append(GTK_BOX(body), title);
  gtk_box_append(GTK_BOX(body), metadata);
  gtk_widget_set_name(filter, "search-draft-filter");
  gtk_box_append(GTK_BOX(body), filter);
  g_object_unref(filter);
  GtkWidget *discard = gtk_button_new_with_label("Discard search edits");
  gtk_widget_set_name(discard, "search-draft-discard");
  gtk_box_append(GTK_BOX(body), discard);
  g_signal_connect_object(discard, "clicked", G_CALLBACK(discard_clicked), self, 0);
  g_object_set_data(G_OBJECT(body), "title", title);
  g_object_set_data(G_OBJECT(body), "metadata", metadata);
  g_object_set_data(G_OBJECT(body), "filter", filter);
  *out = body;
  return UMI_STATUS_OK;
}
static UmiStatus install_body(UmiSearchDraftRoot *self, GtkWidget *body) {
  GtkWidget *old = self->body;
  if (old)
    g_object_ref(old);
  self->body = body;
  self->title = body ? g_object_get_data(G_OBJECT(body), "title") : NULL;
  self->metadata = body ? g_object_get_data(G_OBJECT(body), "metadata") : NULL;
  self->filter = body ? g_object_get_data(G_OBJECT(body), "filter") : NULL;
  if (body)
    gtk_box_append(GTK_BOX(self), body);
  if (!self->retired && old && gtk_widget_get_parent(old) == GTK_WIDGET(self))
    gtk_box_remove(GTK_BOX(self), old);
  if (old)
    g_object_unref(old);
  return self->retired ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
UmiStatus UmiStructuredFormSearchDraftGtk4New(const UmiStructuredFormSchema *schema,
                                              const char *entity_prefix, GtkWidget **out) {
  if (!out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormSearchSession *session = NULL;
  UmiStatus status = umi_structured_form_search_session_create(schema, entity_prefix, &session);
  if (status != UMI_STATUS_OK)
    return status;
  /* Refuse unsupported filter schemas before exposing an empty editor. */
  GtkWidget *probe = NULL;
  status = UmiStructuredFormFilterGtk4New(schema, &probe);
  if (status == UMI_STATUS_OK) {
    g_object_ref_sink(probe);
    g_object_unref(probe);
  }
  if (status != UMI_STATUS_OK) {
    umi_structured_form_search_session_destroy(session);
    return status;
  }
  UmiSearchDraftRoot *self = g_object_new(umi_search_draft_root_get_type(), NULL);
  self->schema = schema;
  self->session = session;
  *out = GTK_WIDGET(self);
  return UMI_STATUS_OK;
}
UmiStatus UmiStructuredFormSearchDraftGtk4Begin(GtkWidget *widget, const char *entity_id,
                                                const UmiStructuredFormSearchPreset *preset) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, false);
  if (status != UMI_STATUS_OK)
    return status;
  GtkWidget *body = NULL;
  if (!entity_id || !preset)
    status = UMI_STATUS_INVALID_ARGUMENT;
  else
    status = umi_structured_form_search_preset_validate(self->schema, preset);
  if (status == UMI_STATUS_OK)
    status = make_body(self, entity_id, preset, &body);
  if (status == UMI_STATUS_OK)
    status = accept_visible(self);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_begin(self->session, entity_id, preset);
  if (status == UMI_STATUS_OK)
    status = install_body(self, body);
  if (body)
    g_object_unref(body);
  return leave(self, status);
}
UmiStatus UmiStructuredFormSearchDraftGtk4Open(GtkWidget *widget,
                                               const UmiRecordDocument *document) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, false);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormSearchPreset *preset = g_try_new0(UmiStructuredFormSearchPreset, 1);
  GtkWidget *body = NULL;
  if (!document)
    status = UMI_STATUS_INVALID_ARGUMENT;
  else if (!preset)
    status = UMI_STATUS_OUT_OF_MEMORY;
  else if (document->length > UMI_RECORD_DOCUMENT_MAX_BYTES)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  else
    status = umi_structured_form_search_preset_decode(self->schema, document->content,
                                                      document->length, preset);
  if (status == UMI_STATUS_OK &&
      !memchr(document->record.amendment.entity_id, 0, sizeof document->record.amendment.entity_id))
    status = UMI_STATUS_INVALID_ARGUMENT;
  if (status == UMI_STATUS_OK)
    status = make_body(self, document->record.amendment.entity_id, preset, &body);
  if (status == UMI_STATUS_OK)
    status = accept_visible(self);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_open(self->session, document);
  if (status == UMI_STATUS_OK)
    status = install_body(self, body);
  if (body)
    g_object_unref(body);
  g_free(preset);
  return leave(self, status);
}
UmiStatus UmiStructuredFormSearchDraftGtk4Accept(GtkWidget *widget) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, false);
  if (status != UMI_STATUS_OK)
    return status;
  return leave(self, accept_visible(self));
}
UmiStatus UmiStructuredFormSearchDraftGtk4State(GtkWidget *widget,
                                                UmiStructuredFormSearchSessionView *out) {
  UmiSearchDraftRoot *self = root_from(widget);
  if (!self || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->working)
    return UMI_STATUS_BUSY;
  return umi_structured_form_search_session_snapshot(self->session, out);
}
bool UmiStructuredFormSearchDraftGtk4CanClose(GtkWidget *widget) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, false);
  if (status != UMI_STATUS_OK)
    return false;
  status = accept_visible(self);
  UmiStructuredFormSearchSessionView *view = g_try_new0(UmiStructuredFormSearchSessionView, 1);
  if (!view)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_snapshot(self->session, view);
  bool clean = status == UMI_STATUS_OK && !view->record.dirty && !view->record.saving;
  g_free(view);
  (void)leave(self, status);
  return clean;
}
UmiStatus UmiStructuredFormSearchDraftGtk4Discard(GtkWidget *widget) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, false);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormSearchSessionView *view = g_try_new0(UmiStructuredFormSearchSessionView, 1);
  UmiRecordDraftSnapshot *snapshot = g_try_new0(UmiRecordDraftSnapshot, 1);
  UmiStructuredFormSearchPreset *baseline = g_try_new0(UmiStructuredFormSearchPreset, 1);
  GtkWidget *body = NULL;
  if (!view || !snapshot || !baseline)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_capture(self->session, snapshot);
  if (status == UMI_STATUS_OK && !view->record.is_new) {
    status = umi_structured_form_search_preset_decode(self->schema, snapshot->baseline.content,
                                                      snapshot->baseline.length, baseline);
    if (status == UMI_STATUS_OK)
      status = make_body(self, snapshot->baseline.record.amendment.entity_id, baseline, &body);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_discard(self->session, view->generation,
                                                        view->edit_revision);
  if (status == UMI_STATUS_OK)
    status = install_body(self, body);
  if (body)
    g_object_unref(body);
  g_free(view);
  g_free(snapshot);
  g_free(baseline);
  return leave(self, status);
}
UmiStatus UmiStructuredFormSearchDraftGtk4PrepareSave(GtkWidget *widget,
                                                      const UmiRecordDraftAttribution *attribution,
                                                      UmiRecordSaveRequest *out) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, false);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormSearchSessionView *view = g_try_new0(UmiStructuredFormSearchSessionView, 1);
  if (!out || !attribution)
    status = UMI_STATUS_INVALID_ARGUMENT;
  else if (!view)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = accept_visible(self);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_prepare_save(self->session, view->generation,
                                                             view->edit_revision, attribution, out);
  g_free(view);
  return leave(self, status);
}
UmiStatus UmiStructuredFormSearchDraftGtk4FinishSave(GtkWidget *widget, uint64_t request_id,
                                                     UmiStatus result,
                                                     const UmiRecordRevision *saved) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, true);
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_structured_form_search_session_finish_save(self->session, request_id, result, saved);
  return leave(self, status);
}
UmiStatus UmiStructuredFormSearchDraftGtk4Capture(GtkWidget *widget, UmiRecordDraftSnapshot *out) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, false);
  if (status != UMI_STATUS_OK)
    return status;
  if (!out)
    status = UMI_STATUS_INVALID_ARGUMENT;
  else
    status = accept_visible(self);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_capture(self->session, out);
  return leave(self, status);
}
UmiStatus UmiStructuredFormSearchDraftGtk4Restore(GtkWidget *widget,
                                                  const UmiRecordDraftSnapshot *snapshot) {
  UmiSearchDraftRoot *self = root_from(widget);
  UmiStatus status = enter(self, false);
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_record_draft_snapshot_validate(snapshot);
  UmiStructuredFormSearchPreset *preset = g_try_new0(UmiStructuredFormSearchPreset, 1);
  GtkWidget *body = NULL;
  if (status == UMI_STATUS_OK && !preset)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_preset_decode(self->schema, snapshot->content,
                                                      snapshot->length, preset);
  if (status == UMI_STATUS_OK)
    status = make_body(self, snapshot->baseline.record.amendment.entity_id, preset, &body);
  if (status == UMI_STATUS_OK)
    status = accept_visible(self);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_session_restore(self->session, snapshot);
  if (status == UMI_STATUS_OK)
    status = install_body(self, body);
  if (body)
    g_object_unref(body);
  g_free(preset);
  return leave(self, status);
}
void UmiStructuredFormSearchDraftGtk4Retire(GtkWidget *widget) {
  UmiSearchDraftRoot *self = root_from(widget);
  if (self)
    retire(self);
}
