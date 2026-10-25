/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/form_record_editor_gtk4.c
 *
 * PURPOSE:
 *   Keep database work away from GTK and give every worker copied input. Typed
 *   sessions own revision checks; products supply only schema and namespace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Keep database work away from GTK and give every worker copied input. Typed
 * sessions own revision checks; products supply only schema and namespace. */
#include "umicom/application/experience/structured_form_snapshot.h"
#include "umicom/application/experience/structured_form_text.h"
#include "umicom/base/text.h"
#include "umicom/data/record_bundle_files.h"
#include "umicom/data/record_catalogue.h"
#include "umicom/data/record_draft_files.h"
#include "umicom/data/record_history_page.h"
#include "umicom/data/structured_form_conflict.h"
#include "umicom/data/structured_form_reader.h"
#include "umicom/data/structured_form_query.h"
#include "umicom/data/structured_form_search_preset.h"
#include "umicom/ui/gtk4/form_record_editor.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/ui/gtk4/structured_form.h"
#include "umicom/ui/gtk4/structured_form_inspect.h"
#include "umicom/ui/gtk4/structured_form_filter.h"
#include "umicom/ui/gtk4/structured_form_search_draft.h"
#include <inttypes.h>
#include <string.h>

typedef struct UmiFormRecordRoot {
  GtkBox parent;
  const UmiStructuredFormSchema *schema;
  const UmiStructuredFormCatalogue *history_schemas;
  UmiStructuredFormSession *session;
  UmiDataServer *server;
  UmiRecordCataloguePage *catalogue;
  UmiRecordHistoryPage *history;
  uint64_t history_generation;
  GtkWidget *search, *results, *history_results, *history_preview;
  GtkWidget *filter;
  uint64_t catalogue_filter_revision;
  bool catalogue_is_typed;
  GtkWidget *preset_key, *preset_title;
  GtkWidget *search_draft, *search_recovery;
  GtkWidget *preset_find, *preset_results;
  GListModel *preset_model;
  UmiStructuredFormSearchPage *preset_page;
  UmiStructuredFormSearchQuery preset_query;
  UmiRecordRevision saved_search;
  bool has_saved_search;
  UmiStructuredFormConflict *conflict;
  GtkWidget *review_box;
  uint64_t review_serial, review_generation, review_native_revision;
  GtkWidget *body, *form, *feedback, *path, *key, *actor, *reason, *recovery;
  GtkWidget *export_path;
  char prefix[49], source[160];
  UmiStatus (*key_validator)(const char *key);
  bool retired, busy, working;
  UmiStatus last;
} UmiFormRecordRoot;
typedef struct UmiFormRecordRootClass {
  GtkBoxClass parent;
} UmiFormRecordRootClass;
GType umi_form_record_root_get_type(void) G_GNUC_CONST;
G_DEFINE_TYPE(UmiFormRecordRoot, umi_form_record_root, GTK_TYPE_BOX)
typedef enum FormRecordOperation {
  FORM_RECORD_CONNECT,
  FORM_RECORD_OPEN,
  FORM_RECORD_SAVE,
  FORM_RECORD_KEEP,
  FORM_RECORD_RECOVER,
  FORM_RECORD_REVIEW,
  FORM_RECORD_FIND,
  FORM_RECORD_FIND_TYPED,
  FORM_RECORD_SAVE_SEARCH,
  FORM_RECORD_OPEN_SEARCH,
  FORM_RECORD_FIND_SEARCH,
  FORM_RECORD_EDIT_SEARCH_DRAFT,
  FORM_RECORD_SAVE_SEARCH_DRAFT,
  FORM_RECORD_KEEP_SEARCH_DRAFT,
  FORM_RECORD_RECOVER_SEARCH_DRAFT,
  FORM_RECORD_OPEN_LISTED,
  FORM_RECORD_HISTORY,
  FORM_RECORD_INSPECT,
  FORM_RECORD_EXPORT
} FormRecordOperation;
typedef struct FormRecordJob {
  FormRecordOperation operation;
  UmiDataServer *server, *opened;
  GtkWidget *draft_target;
  UmiRecordSaveRequest *request;
  UmiRecordDocument *document;
  UmiRecordDraftSnapshot *snapshot;
  UmiRecordRevision saved, selected;
  UmiRecordCatalogueQuery query;
  UmiStructuredFormQuery *typed_query;
  UmiStructuredFormQueryPage *typed_page;
  const UmiStructuredFormCatalogue *schemas;
  UmiStructuredFormCatalogue *owned_schemas;
  uint64_t filter_revision;
  bool selected_typed;
  UmiStructuredFormSearchPreset *preset;
  UmiStructuredFormSearchPresetRecord *preset_loaded;
  UmiStructuredFormSearchQuery search_query;
  UmiStructuredFormSearchPage *search_page;
  bool reviewed_search;
  guint preset_index;
  UmiRecordAmendment preset_metadata;
  const UmiStructuredFormSchema *schema;
  uint64_t preset_expected;
  char search_key[41], search_title[160];
  UmiRecordCataloguePage *catalogue;
  UmiRecordHistoryPage *history;
  uint64_t history_head, history_before;
  uint64_t generation, native_revision;
  char path[1024], entity[64], prefix[49];
  UmiStatus status;
} FormRecordJob;
static UmiFormRecordRoot *root_from(GtkWidget *widget) {
  return widget != NULL && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_form_record_root_get_type())
             ? (UmiFormRecordRoot *)widget
             : NULL;
}
static void retire(UmiFormRecordRoot *self) {
  self->retired = true;
  if (self->search_draft) UmiStructuredFormSearchDraftGtk4Retire(self->search_draft);
  if (self->filter != NULL)
    UmiStructuredFormFilterGtk4Retire(self->filter);
  if (self->form != NULL)
    UmiStructuredFormGtk4Retire(self->form);
}
static void unroot(GtkWidget *widget) {
  retire((UmiFormRecordRoot *)widget);
  GTK_WIDGET_CLASS(umi_form_record_root_parent_class)->unroot(widget);
}
static void dispose(GObject *object) {
  retire((UmiFormRecordRoot *)object);
  /* Child pointers cease to be usable after chaining to GtkBox disposal.
   * Keep the session/store until finalization: GTask retains its owner. */
  ((UmiFormRecordRoot *)object)->form = NULL;
  ((UmiFormRecordRoot *)object)->filter = NULL;
  G_OBJECT_CLASS(umi_form_record_root_parent_class)->dispose(object);
}
static void finalize(GObject *object) {
  UmiFormRecordRoot *self = (UmiFormRecordRoot *)object;
  g_clear_object(&self->search_draft);
  g_free(self->preset_page);
  g_clear_object(&self->preset_model);
  g_free(self->catalogue);
  g_free(self->history);
  umi_structured_form_conflict_destroy(self->conflict);
  umi_structured_form_session_destroy(self->session);
  umi_data_server_destroy(self->server);
  G_OBJECT_CLASS(umi_form_record_root_parent_class)->finalize(object);
}
static void umi_form_record_root_class_init(UmiFormRecordRootClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = unroot;
  G_OBJECT_CLASS(klass)->dispose = dispose;
  G_OBJECT_CLASS(klass)->finalize = finalize;
}
static void umi_form_record_root_init(UmiFormRecordRoot *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 10);
}
static void show_status(UmiFormRecordRoot *self, UmiStatus status, const char *success) {
  self->last = status;
  if (self->retired)
    return;
  GtkWidget *label = g_object_ref(self->feedback);
  gtk_label_set_text(GTK_LABEL(label), status == UMI_STATUS_OK ? success : umi_status_text(status));
  g_object_unref(label);
}
static UmiStatus enter(UmiFormRecordRoot *self) {
  if (self == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired)
    return UMI_STATUS_INVALID_STATE;
  if (self->working || self->busy)
    return UMI_STATUS_BUSY;
  g_object_ref(self);
  self->working = true;
  return UMI_STATUS_OK;
}
static UmiStatus leave(UmiFormRecordRoot *self, UmiStatus status, const char *message) {
  show_status(self, status, message);
  self->working = false;
  g_object_unref(self);
  return status;
}
static bool ascii_key_char(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' ||
         c == '-';
}
static UmiStatus entity_for(UmiFormRecordRoot *self, const char *key, char out[64]) {
  if (key == NULL || key[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0, prefix = strlen(self->prefix);
  while (length < 64 && key[length] != '\0') {
    if (!ascii_key_char(key[length]))
      return UMI_STATUS_INVALID_ARGUMENT;
    ++length;
  }
  if (length + prefix >= 64)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (self->key_validator != NULL) {
    UmiStatus status = self->key_validator(key);
    if (status != UMI_STATUS_OK)
      return status;
    if (self->retired)
      return UMI_STATUS_INVALID_STATE;
  }
  memcpy(out, self->prefix, prefix);
  memcpy(out + prefix, key, length + 1);
  return UMI_STATUS_OK;
}
static bool clean(UmiFormRecordRoot *self) {
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  if (view == NULL)
    return false;
  bool okay = umi_structured_form_session_snapshot(self->session, view) == UMI_STATUS_OK &&
              !view->record.dirty && !view->record.saving &&
              (self->form == NULL || UmiStructuredFormGtk4CanClose(self->form));
  g_free(view);
  return okay;
}
static void clear_review(UmiFormRecordRoot *self) {
  umi_structured_form_conflict_destroy(self->conflict);
  self->conflict = NULL;
  if (self->review_serial != UINT64_MAX)
    ++self->review_serial;
  if (self->retired || self->review_box == NULL)
    return;
  GtkWidget *box = g_object_ref(self->review_box);
  GtkWidget *child;
  while (!self->retired && (child = gtk_widget_get_first_child(box)) != NULL)
    gtk_box_remove(GTK_BOX(box), child);
  g_object_unref(box);
}
/* The new form is fully constructed before changing record selection. Holding
 * its reference through parent notifications also handles synchronous unroot. */
/* Navigation remains read-only. Replacing the selected form invalidates its
 * historical view, so a previous record cannot masquerade as current history. */
static void clear_history(UmiFormRecordRoot *self) {
  g_clear_pointer(&self->history, g_free);
  if (self->retired)
    return;
  if (self->history_results != NULL) {
    GtkWidget *control = g_object_ref(self->history_results);
    gtk_drop_down_set_model(GTK_DROP_DOWN(control), NULL);
    g_object_unref(control);
  }
  if (!self->retired && self->history_preview != NULL) {
    GtkWidget *box = g_object_ref(self->history_preview), *child;
    while (!self->retired && (child = gtk_widget_get_first_child(box)) != NULL)
      gtk_box_remove(GTK_BOX(box), child);
    g_object_unref(box);
  }
}
static void install_form(UmiFormRecordRoot *self, GtkWidget *form) {
  clear_history(self);
  clear_review(self);
  if (self->retired)
    return;
  GtkWidget *old = self->form;
  self->form = NULL;
  if (old != NULL) {
    UmiStructuredFormGtk4Retire(old);
    gtk_box_remove(GTK_BOX(self->body), old);
  }
  if (!self->retired && form != NULL) {
    self->form = form;
    gtk_box_append(GTK_BOX(self->body), form);
  }
}
static void job_free(gpointer data) {
  FormRecordJob *job = data;
  if (job == NULL)
    return;
  umi_data_server_destroy(job->opened);
  g_free(job->request);
  g_clear_object(&job->draft_target);
  g_free(job->document);
  g_free(job->snapshot);
  g_free(job->catalogue);
  g_free(job->history);
  g_free(job->typed_query);
  g_free(job->typed_page);
  g_free(job->preset);
  g_free(job->preset_loaded);
  g_free(job->search_page);
  umi_structured_form_catalogue_destroy(job->owned_schemas);
  g_free(job);
}
/* Preserve the original three-operation worker for review. Recovery adds
 * exclusive private-file operations using the existing shared file service. */
#if 0
static void worker(GTask *task, gpointer source, gpointer data, GCancellable *cancellable) {
    (void)source; (void)cancellable;
    FormRecordJob *job = data;
    if (job->operation == FORM_RECORD_CONNECT)
        job->status = umi_data_server_create_sqlite(job->path, &job->opened);
    else if (job->operation == FORM_RECORD_OPEN)
        job->status = umi_record_document_latest(job->server, job->entity, job->document);
    else
        job->status = umi_record_save_request_commit(job->server, job->request, &job->saved);
    g_task_return_boolean(task, TRUE);
}
#endif
static void worker(GTask *task, gpointer source, gpointer data, GCancellable *cancellable) {
  (void)source;
  (void)cancellable;
  FormRecordJob *job = data;
  if (job->operation == FORM_RECORD_CONNECT)
    job->status = umi_data_server_create_sqlite(job->path, &job->opened);
  else if (job->operation == FORM_RECORD_OPEN)
    job->status = umi_record_document_latest(job->server, job->entity, job->document);
  else if (job->operation == FORM_RECORD_SAVE)
    job->status = umi_record_save_request_commit(job->server, job->request, &job->saved);
  else if (job->operation == FORM_RECORD_SAVE_SEARCH_DRAFT)
    job->status = umi_record_save_request_commit(job->server, job->request, &job->saved);
  else if (job->operation == FORM_RECORD_EDIT_SEARCH_DRAFT)
    job->status = umi_record_document_latest(job->server, job->entity, job->document);
  else if (job->operation == FORM_RECORD_KEEP_SEARCH_DRAFT)
    job->status = umi_record_draft_snapshot_write_new(job->path, job->snapshot);
  else if (job->operation == FORM_RECORD_RECOVER_SEARCH_DRAFT) {
    char *directory = g_path_get_dirname(job->path), *file = g_path_get_basename(job->path);
    job->status = umi_record_draft_snapshot_read(directory, file, job->snapshot);
    g_free(directory); g_free(file);
    if (job->status == UMI_STATUS_OK) job->status = umi_record_draft_snapshot_check_store(job->server, job->snapshot);
  }
  else if (job->operation == FORM_RECORD_EXPORT)
    job->status = umi_record_bundle_export_reviewed(job->server, &job->selected, job->path);
  else if (job->operation == FORM_RECORD_KEEP)
    job->status = umi_record_draft_snapshot_write_new(job->path, job->snapshot);
  else if (job->operation == FORM_RECORD_REVIEW) {
    job->status = umi_record_draft_snapshot_check_store(job->server, job->snapshot);
    if (job->status == UMI_STATUS_OK)
      job->status = umi_record_document_latest(job->server, job->entity, job->document);
  } else if (job->operation == FORM_RECORD_FIND) {
    job->status = umi_record_catalogue_page(job->server, &job->query, job->catalogue);
  } else if (job->operation == FORM_RECORD_SAVE_SEARCH) {
    job->status = umi_structured_form_search_preset_save(job->server, &job->preset_metadata,
        job->schema, job->preset, job->preset_expected, &job->saved);
  } else if (job->operation == FORM_RECORD_OPEN_SEARCH) {
    /* Retain the direct latest-open path for review; reviewed list selections
     * must check their exact head rather than silently substituting a revision. */
#if 0
    job->status = umi_structured_form_search_preset_load(job->server, job->entity, 0,
        job->schema, job->preset_loaded);
#endif
    job->status = job->reviewed_search
        ? umi_structured_form_search_preset_open_reviewed(job->server, &job->selected, job->schema, job->preset_loaded)
        : umi_structured_form_search_preset_load(job->server, job->entity, 0, job->schema, job->preset_loaded);
  } else if (job->operation == FORM_RECORD_FIND_SEARCH) {
    job->status = umi_structured_form_search_catalogue_page(job->server, job->schemas, &job->search_query, job->search_page);
  } else if (job->operation == FORM_RECORD_FIND_TYPED) {
    /* Only copied query values and immutable registered schemas reach storage.
     * Reuse the metadata result view after the typed service verifies payloads. */
    job->status = umi_structured_form_query_page(job->server, job->schemas, job->typed_query, job->typed_page);
    if (job->status == UMI_STATUS_OK) {
      UmiRecordCataloguePage *page = job->catalogue;
      memcpy(page->entity_prefix, job->query.entity_prefix, sizeof(page->entity_prefix));
      memcpy(page->text, job->query.text, sizeof(page->text));
      memcpy(page->snapshot, job->typed_page->snapshot, sizeof(page->snapshot));
      memcpy(page->next_after_entity, job->typed_page->next_after_entity, sizeof(page->next_after_entity));
      page->count = job->typed_page->count;
      page->matching_count = job->typed_page->matching_count;
      page->namespace_count = job->typed_page->namespace_count;
      memcpy(page->rows, job->typed_page->rows, sizeof(page->rows));
    }
  } else if (job->operation == FORM_RECORD_OPEN_LISTED) {
    job->status = umi_record_document_open_reviewed(job->server, &job->selected, job->document);
  } else if (job->operation == FORM_RECORD_HISTORY) {
    job->status =
        umi_record_history_page(job->server, job->entity, job->history_head, job->history_before,
                                UMI_RECORD_HISTORY_PAGE_CAPACITY, job->history);
  } else if (job->operation == FORM_RECORD_INSPECT) {
    job->status = umi_record_document_load(job->server, job->selected.amendment.entity_id,
                                           job->selected.revision, job->document);
    if (job->status == UMI_STATUS_OK) {
      char actual[UMI_RECORD_REVISION_TEXT_CAPACITY], expected[UMI_RECORD_REVISION_TEXT_CAPACITY];
      job->status = umi_record_revision_format(&job->document->record, actual, sizeof(actual));
      if (job->status == UMI_STATUS_OK)
        job->status = umi_record_revision_format(&job->selected, expected, sizeof(expected));
      if (job->status == UMI_STATUS_OK && strcmp(actual, expected) != 0)
        job->status = UMI_STATUS_INVALID_STATE;
    }
  } else {
    char *directory = g_path_get_dirname(job->path), *file = g_path_get_basename(job->path);
    job->status = umi_record_draft_snapshot_read(directory, file, job->snapshot);
    g_free(directory);
    g_free(file);
    if (job->status == UMI_STATUS_OK &&
        !g_str_has_prefix(job->snapshot->baseline.record.amendment.entity_id, job->prefix))
      job->status = UMI_STATUS_PERMISSION_DENIED;
    if (job->status == UMI_STATUS_OK)
      job->status = umi_record_draft_snapshot_check_store(job->server, job->snapshot);
  }
  g_task_return_boolean(task, TRUE);
}
static UmiStatus accept_document(UmiFormRecordRoot *self, FormRecordJob *job) {
  /* A listed typed result belongs to its captured filter. Editing that filter
   * while Open runs must not replace the current form with an obsolete choice. */
  if (job->selected_typed &&
      (UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
       strcmp(job->query.text, gtk_editable_get_text(GTK_EDITABLE(self->search))) != 0))
    return UMI_STATUS_INVALID_STATE;
  if (!clean(self))
    return UMI_STATUS_INVALID_STATE;
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1);
  if (view == NULL || form == NULL) {
    g_free(view);
    g_free(form);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiStatus status = umi_structured_form_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK && view->generation != job->generation)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_snapshot_decode(self->schema, job->document->content,
                                                 job->document->length, form);
  GtkWidget *native = NULL;
  if (status == UMI_STATUS_OK) {
    form->saved_revision = form->revision;
    native = UmiStructuredFormGtk4New(self->schema, form);
    if (native == NULL)
      status = UMI_STATUS_INVALID_STATE;
    else
      g_object_ref_sink(native);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_open(self->session, job->document);
  if (status == UMI_STATUS_OK)
    install_form(self, native);
  if (native != NULL)
    g_object_unref(native);
  g_free(form);
  g_free(view);
  return status;
}
static UmiStatus accept_recovery(UmiFormRecordRoot *self, FormRecordJob *job) {
  if (!clean(self))
    return UMI_STATUS_INVALID_STATE;
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1),
                    *baseline = g_try_new0(UmiStructuredForm, 1);
  if (view == NULL || form == NULL || baseline == NULL) {
    g_free(view);
    g_free(form);
    g_free(baseline);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiStatus status = umi_structured_form_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK && view->generation != job->generation)
    status = UMI_STATUS_INVALID_STATE;
  char entity[64];
  if (status == UMI_STATUS_OK)
    status = entity_for(
        self, job->snapshot->baseline.record.amendment.entity_id + strlen(self->prefix), entity);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_snapshot_decode(self->schema, job->snapshot->content,
                                                 job->snapshot->length, form);
  if (status == UMI_STATUS_OK && job->snapshot->baseline.record.revision != 0)
    status = umi_structured_form_snapshot_decode(self->schema, job->snapshot->baseline.content,
                                                 job->snapshot->baseline.length, baseline);
  GtkWidget *native = NULL;
  if (status == UMI_STATUS_OK) {
    form->saved_revision = baseline->revision;
    native = UmiStructuredFormGtk4New(self->schema, form);
    if (native == NULL)
      status = UMI_STATUS_INVALID_STATE;
    else
      g_object_ref_sink(native);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_restore(self->session, job->snapshot);
  if (status == UMI_STATUS_OK)
    install_form(self, native);
  if (native != NULL)
    g_object_unref(native);
  g_free(view);
  g_free(form);
  g_free(baseline);
  return status;
}
typedef struct FormReviewChoice {
  uint64_t serial;
  char field_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
} FormReviewChoice;
static void review_choice_changed(GObject *object, GParamSpec *property, gpointer data) {
  (void)property;
  UmiFormRecordRoot *self = data;
  FormReviewChoice *choice = g_object_get_data(object, "form-review-choice");
  if (self->retired || self->busy || self->working || self->conflict == NULL || choice == NULL ||
      choice->serial != self->review_serial)
    return;
  guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
  if (selected > (guint)UMI_FORM_MERGE_TAKE_INCOMING)
    return;
  UmiFormMergeState state;
  UmiStatus status = umi_structured_form_conflict_state(self->conflict, &state);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_conflict_select(self->conflict, state.revision, choice->field_id,
                                                 (UmiFormMergeChoice)selected);
  g_object_ref(self);
  show_status(self, status, "Choice recorded for review. Apply the complete review before saving.");
  g_object_unref(self);
}
/* The earlier label conversion is retained for review. The replacement uses
 * the declared answer-conversion API and shows its unit, so numeric comparisons
 * do not silently lose their measurement context. Secrets are refused. */
#if 0
static UmiStatus review_label(const UmiStructuredFormSchema *schema, size_t index,
    const UmiFormAnswer *answer, const char *prefix, GtkWidget **out) {
    UmiFormTextAnswer text;
    UmiStatus status = umi_structured_form_text_format(schema, index, answer, &text);
    if (status != UMI_STATUS_OK) return status;
    if (!g_utf8_validate(text.value, -1, NULL) || !g_utf8_validate(text.reason, -1, NULL)) return UMI_STATUS_PARSE_ERROR;
    char *message = g_strdup_printf("%s: %s%s%s", prefix, umi_form_answer_state_text(answer->state),
        answer->state == UMI_FORM_KNOWN ? " — " : text.reason[0] != '\0' ? " — " : "",
        answer->state == UMI_FORM_KNOWN ? text.value : text.reason);
    GtkWidget *label = gtk_label_new(message); g_free(message);
    gtk_label_set_xalign(GTK_LABEL(label), 0); gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    *out = label; return UMI_STATUS_OK;
}
#endif
static UmiStatus review_label(const UmiStructuredFormSchema *schema, size_t index,
                              const UmiFormAnswer *answer, const char *prefix, GtkWidget **out) {
  if (schema->base->fields[index].kind == UMI_APPLICATION_EXPERIENCE_FIELD_SECRET)
    return UMI_STATUS_PERMISSION_DENIED;
  UmiFormTextAnswer text;
  UmiStatus status = umi_structured_form_answer_format(schema, index, answer, &text);
  if (status != UMI_STATUS_OK)
    return status;
  if (!g_utf8_validate(text.value, -1, NULL) || !g_utf8_validate(text.reason, -1, NULL) ||
      !g_utf8_validate(text.unit_id, -1, NULL))
    return UMI_STATUS_PARSE_ERROR;
  bool known = answer->state == UMI_FORM_KNOWN;
  char *message = g_strdup_printf(
      "%s: %s%s%s%s%s", prefix, umi_form_answer_state_text(answer->state),
      known || text.reason[0] != '\0' ? " — " : "", known ? text.value : text.reason,
      known && text.unit_id[0] != '\0' ? " " : "", known ? text.unit_id : "");
  GtkWidget *label = gtk_label_new(message);
  g_free(message);
  gtk_label_set_xalign(GTK_LABEL(label), 0);
  gtk_label_set_wrap(GTK_LABEL(label), TRUE);
  *out = label;
  return UMI_STATUS_OK;
}
static UmiStatus accept_review(UmiFormRecordRoot *self, FormRecordJob *job) {
  if (self->form == NULL ||
      !UmiStructuredFormGtk4MatchesRevision(self->form, job->native_revision) ||
      self->review_serial == UINT64_MAX)
    return UMI_STATUS_INVALID_STATE;
  UmiStructuredFormConflict *review = NULL;
  UmiStatus status =
      umi_structured_form_session_review(self->session, job->generation, job->document, &review);
  if (status != UMI_STATUS_OK)
    return status;
  UmiFormMergeState state;
  status = umi_structured_form_conflict_state(review, &state);
  UmiFormMergeField *field = g_try_new0(UmiFormMergeField, 1);
  if (field == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  GtkWidget *rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  g_object_ref_sink(rows);
  const char *choices[] = {"Automatic (conflicts remain unresolved)", "Keep baseline", "Keep local",
                           "Take incoming", NULL};
  for (size_t i = 0; status == UMI_STATUS_OK && i < state.field_count; ++i) {
    status = umi_structured_form_conflict_field(review, i, field);
    if (status != UMI_STATUS_OK || field->difference == UMI_FORM_MERGE_UNCHANGED)
      continue;
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(rows), row);
    gtk_box_append(GTK_BOX(row), gtk_label_new(self->schema->base->fields[i].label));
    const UmiFormAnswer *answers[] = {&field->baseline, &field->local, &field->incoming};
    const char *labels[] = {"Baseline", "Local", "Incoming"};
    for (size_t j = 0; j < 3 && status == UMI_STATUS_OK; ++j) {
      GtkWidget *label = NULL;
      status = review_label(self->schema, i, answers[j], labels[j], &label);
      if (status == UMI_STATUS_OK)
        gtk_box_append(GTK_BOX(row), label);
    }
    if (status != UMI_STATUS_OK)
      break;
    FormReviewChoice *binding = g_try_new0(FormReviewChoice, 1);
    if (binding == NULL) {
      status = UMI_STATUS_OUT_OF_MEMORY;
      break;
    }
    binding->serial = self->review_serial + 1;
    memcpy(binding->field_id, field->field_id, sizeof(binding->field_id));
    GtkWidget *control = gtk_drop_down_new_from_strings(choices);
    char *name = g_strdup_printf("form-review.%s.choice", field->field_id);
    gtk_widget_set_name(control, name);
    g_free(name);
    g_object_set_data_full(G_OBJECT(control), "form-review-choice", binding, g_free);
    gtk_box_append(GTK_BOX(row), control);
    g_signal_connect_object(control, "notify::selected", G_CALLBACK(review_choice_changed), self,
                            0);
  }
  if (status == UMI_STATUS_OK) {
    clear_review(self);
    if (self->retired)
      status = UMI_STATUS_CANCELLED;
    else {
      self->conflict = review;
      review = NULL;
      self->review_generation = job->generation;
      self->review_native_revision = job->native_revision;
      gtk_box_append(GTK_BOX(self->review_box), rows);
    }
  }
  g_free(field);
  g_object_unref(rows);
  umi_structured_form_conflict_destroy(review);
  return status;
}
static bool metadata_utf8(const UmiRecordRevision *record) {
  return g_utf8_validate(record->amendment.entity_id, -1, NULL) &&
         g_utf8_validate(record->amendment.actor, -1, NULL) &&
         g_utf8_validate(record->amendment.source, -1, NULL) &&
         g_utf8_validate(record->amendment.reason, -1, NULL);
}
static UmiStatus history_context(UmiFormRecordRoot *self, const FormRecordJob *job) {
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  if (view == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = umi_structured_form_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK && (!view->record.ready || view->generation != job->generation ||
                                  strcmp(view->record.entity_id, job->entity) != 0))
    status = UMI_STATUS_INVALID_STATE;
  g_free(view);
  return status;
}
static UmiStatus accept_catalogue(UmiFormRecordRoot *self, FormRecordJob *job) {
  bool typed = job->operation == FORM_RECORD_FIND_TYPED;
  if (typed && UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision)
    return UMI_STATUS_INVALID_STATE;
  if (strcmp(job->query.text, gtk_editable_get_text(GTK_EDITABLE(self->search))) != 0)
    return UMI_STATUS_INVALID_STATE;
  GtkStringList *list = gtk_string_list_new(NULL);
  UmiStatus status = UMI_STATUS_OK;
  for (size_t i = 0; i < job->catalogue->count; ++i) {
    const UmiRecordRevision *row = &job->catalogue->rows[i];
    if (!metadata_utf8(row)) {
      status = UMI_STATUS_PARSE_ERROR;
      break;
    }
    char *text = g_strdup_printf("%s | revision %" PRIu64 " | %s", row->amendment.entity_id,
                                 row->revision, row->amendment.reason);
    gtk_string_list_append(list, text);
    g_free(text);
  }
  if (status == UMI_STATUS_OK) {
    g_free(self->catalogue);
    self->catalogue = job->catalogue;
    self->catalogue_is_typed = typed;
    self->catalogue_filter_revision = typed ? job->filter_revision : 0;
    job->catalogue = NULL;
    GtkWidget *control = g_object_ref(self->results);
    gtk_drop_down_set_model(GTK_DROP_DOWN(control), G_LIST_MODEL(list));
    if (!self->retired)
      gtk_drop_down_set_selected(GTK_DROP_DOWN(control), GTK_INVALID_LIST_POSITION);
    g_object_unref(control);
  }
  g_object_unref(list);
  return status;
}
static UmiStatus accept_history(UmiFormRecordRoot *self, FormRecordJob *job) {
  UmiStatus status = history_context(self, job);
  if (status != UMI_STATUS_OK)
    return status;
  GtkStringList *list = gtk_string_list_new(NULL);
  for (size_t i = 0; i < job->history->count; ++i) {
    const UmiRecordRevision *row = &job->history->rows[i];
    if (!metadata_utf8(row)) {
      status = UMI_STATUS_PARSE_ERROR;
      break;
    }
    char *text = g_strdup_printf("Revision %" PRIu64 " | %s | %s", row->revision,
                                 row->amendment.actor, row->amendment.reason);
    gtk_string_list_append(list, text);
    g_free(text);
  }
  if (status == UMI_STATUS_OK) {
    g_free(self->history);
    self->history = job->history;
    job->history = NULL;
    self->history_generation = job->generation;
    GtkWidget *control = g_object_ref(self->history_results);
    gtk_drop_down_set_model(GTK_DROP_DOWN(control), G_LIST_MODEL(list));
    if (!self->retired)
      gtk_drop_down_set_selected(GTK_DROP_DOWN(control), GTK_INVALID_LIST_POSITION);
    g_object_unref(control);
  }
  g_object_unref(list);
  return status;
}
/* Earlier single-schema rendering is retained for comparison. Trusted catalogue
 * decoding now
 * supports immutable historical definitions without changing the
 * active editor or silently
 * migrating answers. */
#if 0
static UmiStatus accept_inspection(UmiFormRecordRoot *self, FormRecordJob *job) {
  UmiStatus status = history_context(self, job);
  if (status == UMI_STATUS_OK && !metadata_utf8(&job->document->record))
    status = UMI_STATUS_PARSE_ERROR;
  UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1);
  if (form == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_snapshot_decode(self->schema, job->document->content,
                                                 job->document->length, form);
  GtkWidget *inspect = NULL;
  if (status == UMI_STATUS_OK)
    status = UmiStructuredFormInspectGtk4New(self->schema, form, &inspect);
  g_free(form);
  if (status != UMI_STATUS_OK)
    return status;
  GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  g_object_ref_sink(page);
  char *heading =
      g_strdup_printf("%s | stored revision %" PRIu64 " | Source: %s | Actor label: %s | %s",
                      job->document->record.amendment.entity_id, job->document->record.revision,
                      job->document->record.amendment.source, job->document->record.amendment.actor,
                      job->document->record.amendment.reason);
  GtkWidget *label = gtk_label_new(heading);
  g_free(heading);
  gtk_widget_set_name(label, "form-record-inspection-metadata");
  gtk_label_set_wrap(GTK_LABEL(label), TRUE);
  gtk_box_append(GTK_BOX(page), label);
  gtk_box_append(GTK_BOX(page), inspect);
  GtkWidget *box = g_object_ref(self->history_preview), *child;
  while (!self->retired && (child = gtk_widget_get_first_child(box)) != NULL)
    gtk_box_remove(GTK_BOX(box), child);
  if (!self->retired)
    gtk_box_append(GTK_BOX(box), page);
  g_object_unref(box);
  g_object_unref(page);
  return status;
}
#endif
static UmiStatus accept_inspection(UmiFormRecordRoot *self, FormRecordJob *job) {
  UmiStatus status = history_context(self, job);
  if (status == UMI_STATUS_OK && !metadata_utf8(&job->document->record))
    status = UMI_STATUS_PARSE_ERROR;
  UmiStructuredFormHistorical *historical = g_try_new0(UmiStructuredFormHistorical, 1);
  if (historical == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) {
    if (self->history_schemas != NULL)
      status = umi_structured_form_reader_decode(self->history_schemas, job->document, historical);
    else {
      historical->resolved.schema = self->schema;
      status = umi_structured_form_snapshot_decode(
          self->schema, job->document->content, job->document->length, &historical->resolved.form);
    }
  }
  GtkWidget *inspect = NULL;
  if (status == UMI_STATUS_OK)
    status = UmiStructuredFormInspectGtk4New(historical->resolved.schema,
                                             &historical->resolved.form, &inspect);
  g_free(historical);
  if (status != UMI_STATUS_OK)
    return status;
  GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  g_object_ref_sink(page);
  char *heading =
      g_strdup_printf("%s | stored revision %" PRIu64 " | Source: %s | Actor label: %s | %s",
                      job->document->record.amendment.entity_id, job->document->record.revision,
                      job->document->record.amendment.source, job->document->record.amendment.actor,
                      job->document->record.amendment.reason);
  GtkWidget *label = gtk_label_new(heading);
  g_free(heading);
  gtk_widget_set_name(label, "form-record-inspection-metadata");
  gtk_label_set_wrap(GTK_LABEL(label), TRUE);
  gtk_box_append(GTK_BOX(page), label);
  gtk_box_append(GTK_BOX(page), inspect);
  GtkWidget *box = g_object_ref(self->history_preview), *child;
  while (!self->retired && (child = gtk_widget_get_first_child(box)) != NULL)
    gtk_box_remove(GTK_BOX(box), child);
  if (!self->retired)
    gtk_box_append(GTK_BOX(box), page);
  g_object_unref(box);
  g_object_unref(page);
  return status;
}

/* Preset input is independent of record-draft input. A local capture has no
 * database effects and retains the editor's exact authorized namespace. */
static UmiStatus capture_search(UmiFormRecordRoot *self, const char *title,
                                UmiStructuredFormSearchPreset *out, uint64_t *revision) {
  if (!self->filter || !title || !g_utf8_validate(title, -1, NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_text_replace(out->title, sizeof(out->title), title);
  if (status == UMI_STATUS_OK)
    status = umi_text_replace(out->metadata_text, sizeof(out->metadata_text),
                              gtk_editable_get_text(GTK_EDITABLE(self->search)));
  if (status == UMI_STATUS_OK) {
    memcpy(out->entity_prefix, self->prefix, strlen(self->prefix) + 1);
    status = UmiStructuredFormFilterGtk4Read(self->filter, &out->filter, revision);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_preset_validate(self->schema, out);
  return status;
}
static UmiStatus search_entity(const char *key, char out[64]) {
  if (!key || !key[0]) return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0;
  while (length <= 40 && key[length]) {
    if (!ascii_key_char(key[length])) return UMI_STATUS_INVALID_ARGUMENT;
    ++length;
  }
  if (length > 40) return UMI_STATUS_CAPACITY_EXCEEDED;
  const size_t prefix = sizeof(UMI_FORM_SEARCH_PRESET_PREFIX) - 1;
  memcpy(out, UMI_FORM_SEARCH_PRESET_PREFIX, prefix);
  memcpy(out + prefix, key, length + 1);
  return UMI_STATUS_OK;
}
/* The first preset-apply path checked the final query only. Retain it for
 * review; the replacement checks every observer boundary before overwriting
 * another control, preserving edits made during synchronous notifications. */
#if 0
static UmiStatus accept_search(UmiFormRecordRoot *self, FormRecordJob *job) {
  UmiStructuredFormSearchPreset *preset = &job->preset_loaded->preset;
  if (self->retired || UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
      strcmp(job->query.text, gtk_editable_get_text(GTK_EDITABLE(self->search))) ||
      strcmp(job->search_key, gtk_editable_get_text(GTK_EDITABLE(self->preset_key))) ||
      strcmp(job->search_title, gtk_editable_get_text(GTK_EDITABLE(self->preset_title))))
    return UMI_STATUS_INVALID_STATE;
  if (strcmp(preset->entity_prefix, self->prefix)) return UMI_STATUS_PERMISSION_DENIED;
  if (!g_utf8_validate(preset->title, -1, NULL) ||
      !g_utf8_validate(preset->metadata_text, -1, NULL)) return UMI_STATUS_PARSE_ERROR;
  /* Check all operand text before changing any visible control. */
  for (size_t i = 0; i < preset->filter.count; ++i) {
    UmiFormFilterTextTerm text;
    UmiStatus status = umi_structured_form_filter_term_format(self->schema, &preset->filter.terms[i], &text);
    if (status != UMI_STATUS_OK) return status;
    if (!g_utf8_validate(text.value, -1, NULL)) return UMI_STATUS_PARSE_ERROR;
  }
  GtkWidget *search = g_object_ref(self->search);
  gtk_editable_set_text(GTK_EDITABLE(search), preset->metadata_text);
  g_object_unref(search);
  if (self->retired || UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
      strcmp(preset->metadata_text, gtk_editable_get_text(GTK_EDITABLE(self->search))))
    return UMI_STATUS_CANCELLED;
  UmiStatus status = UmiStructuredFormFilterGtk4Set(self->filter, &preset->filter, job->filter_revision);
  if (self->retired) return UMI_STATUS_CANCELLED;
  if (status != UMI_STATUS_OK) return status;
  GtkWidget *title = g_object_ref(self->preset_title);
  gtk_editable_set_text(GTK_EDITABLE(title), preset->title);
  g_object_unref(title);
  if (self->retired) return UMI_STATUS_CANCELLED;
  UmiStructuredFormSearchPreset *actual = g_try_new0(UmiStructuredFormSearchPreset, 1);
  if (!actual) return UMI_STATUS_OUT_OF_MEMORY;
  uint64_t revision = 0; char expected[65], observed[65];
  status = capture_search(self, gtk_editable_get_text(GTK_EDITABLE(self->preset_title)), actual, &revision);
  if (status == UMI_STATUS_OK) status = umi_structured_form_filter_digest(self->schema, &preset->filter, expected);
  if (status == UMI_STATUS_OK) status = umi_structured_form_filter_digest(self->schema, &actual->filter, observed);
  if (status == UMI_STATUS_OK &&
      (strcmp(expected, observed) || strcmp(preset->title, actual->title) ||
       strcmp(preset->metadata_text, actual->metadata_text) ||
       strcmp(job->search_key, gtk_editable_get_text(GTK_EDITABLE(self->preset_key)))))
    status = UMI_STATUS_CANCELLED;
  g_free(actual);
  if (status == UMI_STATUS_OK) {
    self->saved_search = job->preset_loaded->record;
    self->has_saved_search = true;
  }
  return status;
}

#endif
/* Results are admitted only for the query still visible in the native list.
 * The editor owns a model reference so replacement models cannot reuse its
 * identity while a reviewed selection is pending. */
/* GtkStringList can be changed by native observers. Check visible labels as
 * well as object identity before treating a selected row as reviewed. */
static bool search_model_matches_page(GListModel *model, const UmiStructuredFormSearchPage *page) {
  if (!model || !page || !GTK_IS_STRING_LIST(model) ||
      g_list_model_get_n_items(model) != page->count) return false;
  for (size_t i = 0; i < page->count; ++i) {
    const UmiStructuredFormSearchRow *row = &page->rows[i];
    char *expected = g_strdup_printf("%s — %s (revision %" G_GUINT64_FORMAT ")", row->title,
        row->record.amendment.entity_id + strlen(UMI_FORM_SEARCH_PRESET_PREFIX),
        (guint64)row->record.revision);
    const char *actual = gtk_string_list_get_string(GTK_STRING_LIST(model), (guint)i);
    bool match = actual && !strcmp(actual, expected); g_free(expected);
    if (!match) return false;
  }
  return true;
}
static bool search_selection_matches(UmiFormRecordRoot *self, FormRecordJob *job) {
  if (!job->reviewed_search) return true;
  return !self->retired && self->preset_page &&
      search_model_matches_page(self->preset_model, self->preset_page) &&
      !strcmp(job->search_query.text, self->preset_query.text) &&
      !strcmp(job->search_query.text, gtk_editable_get_text(GTK_EDITABLE(self->preset_find))) &&
      gtk_drop_down_get_model(GTK_DROP_DOWN(self->preset_results)) == self->preset_model &&
      gtk_drop_down_get_selected(GTK_DROP_DOWN(self->preset_results)) == job->preset_index;
}
static UmiStatus accept_search_page(UmiFormRecordRoot *self, FormRecordJob *job) {
  if (self->retired ||
      strcmp(job->search_query.text, gtk_editable_get_text(GTK_EDITABLE(self->preset_find))))
    return UMI_STATUS_INVALID_STATE;
  GtkStringList *model = gtk_string_list_new(NULL);
  for (size_t i = 0; i < job->search_page->count; ++i) {
    UmiStructuredFormSearchRow *row = &job->search_page->rows[i];
    if (!g_utf8_validate(row->title, -1, NULL)) { g_object_unref(model); return UMI_STATUS_PARSE_ERROR; }
    char *label = g_strdup_printf("%s — %s (revision %" G_GUINT64_FORMAT ")", row->title,
        row->record.amendment.entity_id + strlen(UMI_FORM_SEARCH_PRESET_PREFIX),
        (guint64)row->record.revision);
    gtk_string_list_append(model, label); g_free(label);
  }
  /* Any observer replacement invalidates the old page too. Never bind old
   * metadata to a new visible model after a partially accepted presentation. */
  g_clear_pointer(&self->preset_page, g_free);
  g_clear_object(&self->preset_model);
  GtkWidget *results = g_object_ref(self->preset_results);
  gtk_drop_down_set_model(GTK_DROP_DOWN(results), G_LIST_MODEL(model));
  UmiStatus status = UMI_STATUS_OK;
  if (self->retired ||
      strcmp(job->search_query.text, gtk_editable_get_text(GTK_EDITABLE(self->preset_find))) ||
      gtk_drop_down_get_model(GTK_DROP_DOWN(results)) != G_LIST_MODEL(model) ||
      !search_model_matches_page(G_LIST_MODEL(model), job->search_page))
    status = UMI_STATUS_CANCELLED;
  if (status == UMI_STATUS_OK) {
    self->preset_page = job->search_page; job->search_page = NULL;
    self->preset_query = job->search_query;
    self->preset_model = g_object_ref(G_LIST_MODEL(model));
  }
  g_object_unref(results); g_object_unref(model);
  return status;
}
static UmiStatus accept_search(UmiFormRecordRoot *self, FormRecordJob *job) {
  UmiStructuredFormSearchPreset *preset = &job->preset_loaded->preset;
  if (self->retired || !search_selection_matches(self, job) || UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
      strcmp(job->query.text, gtk_editable_get_text(GTK_EDITABLE(self->search))) ||
      strcmp(job->search_key, gtk_editable_get_text(GTK_EDITABLE(self->preset_key))) ||
      strcmp(job->search_title, gtk_editable_get_text(GTK_EDITABLE(self->preset_title))))
    return UMI_STATUS_INVALID_STATE;
  if (strcmp(preset->entity_prefix, self->prefix)) return UMI_STATUS_PERMISSION_DENIED;
  if (!g_utf8_validate(preset->title, -1, NULL) ||
      !g_utf8_validate(preset->metadata_text, -1, NULL)) return UMI_STATUS_PARSE_ERROR;
  /* Check all operand text before changing any visible control. */
  for (size_t i = 0; i < preset->filter.count; ++i) {
    UmiFormFilterTextTerm text;
    UmiStatus status = umi_structured_form_filter_term_format(self->schema, &preset->filter.terms[i], &text);
    if (status != UMI_STATUS_OK) return status;
    if (!g_utf8_validate(text.value, -1, NULL)) return UMI_STATUS_PARSE_ERROR;
  }
  GtkWidget *search = g_object_ref(self->search);
  gtk_editable_set_text(GTK_EDITABLE(search), preset->metadata_text);
  g_object_unref(search);
  if (self->retired || !search_selection_matches(self, job) || UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
      strcmp(preset->metadata_text, gtk_editable_get_text(GTK_EDITABLE(self->search))) ||
      strcmp(job->search_key, gtk_editable_get_text(GTK_EDITABLE(self->preset_key))) ||
      strcmp(job->search_title, gtk_editable_get_text(GTK_EDITABLE(self->preset_title))))
    return UMI_STATUS_CANCELLED;
  UmiStatus status = UmiStructuredFormFilterGtk4Set(self->filter, &preset->filter, job->filter_revision);
  if (self->retired || !search_selection_matches(self, job)) return UMI_STATUS_CANCELLED;
  if (status != UMI_STATUS_OK) return status;
  /* Observers can edit other query controls while the filter is installed.
   * Check those controls before replacing the title, so their edits survive. */
  if (UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision + 1 ||
      strcmp(preset->metadata_text, gtk_editable_get_text(GTK_EDITABLE(self->search))) ||
      strcmp(job->search_key, gtk_editable_get_text(GTK_EDITABLE(self->preset_key))) ||
      strcmp(job->search_title, gtk_editable_get_text(GTK_EDITABLE(self->preset_title))))
    return UMI_STATUS_CANCELLED;
  GtkWidget *title = g_object_ref(self->preset_title);
  gtk_editable_set_text(GTK_EDITABLE(title), preset->title);
  g_object_unref(title);
  if (self->retired || !search_selection_matches(self, job)) return UMI_STATUS_CANCELLED;
  UmiStructuredFormSearchPreset *actual = g_try_new0(UmiStructuredFormSearchPreset, 1);
  if (!actual) return UMI_STATUS_OUT_OF_MEMORY;
  uint64_t revision = 0; char expected[65], observed[65];
  status = capture_search(self, gtk_editable_get_text(GTK_EDITABLE(self->preset_title)), actual, &revision);
  if (status == UMI_STATUS_OK) status = umi_structured_form_filter_digest(self->schema, &preset->filter, expected);
  if (status == UMI_STATUS_OK) status = umi_structured_form_filter_digest(self->schema, &actual->filter, observed);
  if (status == UMI_STATUS_OK &&
      (strcmp(expected, observed) || strcmp(preset->title, actual->title) ||
       strcmp(preset->metadata_text, actual->metadata_text) ||
       strcmp(job->search_key, gtk_editable_get_text(GTK_EDITABLE(self->preset_key)))))
    status = UMI_STATUS_CANCELLED;
  g_free(actual);
  if (status == UMI_STATUS_OK) {
    self->saved_search = job->preset_loaded->record;
    self->has_saved_search = true;
  }
  return status;
}

/* Read completion must still belong to the admitted draft. Native text can
 * change while storage runs, so accept/check it before comparing both tokens. */
static UmiStatus search_draft_read_ready(UmiFormRecordRoot *self, FormRecordJob *job) {
  if (!self->search_draft || !UmiStructuredFormSearchDraftGtk4CanClose(self->search_draft))
    return UMI_STATUS_INVALID_STATE;
  UmiStructuredFormSearchSessionView *view = g_try_new0(UmiStructuredFormSearchSessionView, 1);
  if (!view) return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = UmiStructuredFormSearchDraftGtk4State(self->search_draft, view);
  if (status == UMI_STATUS_OK && (view->generation != job->generation ||
      view->edit_revision != job->native_revision)) status = UMI_STATUS_INVALID_STATE;
  g_free(view); return status;
}

static void completed(GObject *source, GAsyncResult *result, gpointer data) {
  (void)data;
  UmiFormRecordRoot *self = (UmiFormRecordRoot *)source;
  GTask *task = G_TASK(result);
  FormRecordJob *job = g_task_get_task_data(task);
  (void)g_task_propagate_boolean(task, NULL);
  self->working = true;
  UmiStatus status = job->status;
  const char *message = "Database connected. Choose a record key and New or Open.";
  if (job->operation == FORM_RECORD_SAVE_SEARCH_DRAFT) {
    status = UmiStructuredFormSearchDraftGtk4FinishSave(job->draft_target, job->request->request_id,
        job->status, job->status == UMI_STATUS_OK ? &job->saved : NULL);
    message = "Saved the managed query snapshot. Later edits remain unsaved.";
  } else if (job->operation == FORM_RECORD_KEEP_SEARCH_DRAFT) {
    message = "Kept a new private query recovery copy; its database draft remains unsaved.";
  } else if (job->operation == FORM_RECORD_SAVE) {
    /* Finish storage ownership even after retirement; never claim a running
     * write was cancelled because its window disappeared. */
    status = umi_structured_form_session_finish_save(
        self->session, job->request->request_id, job->status,
        job->status == UMI_STATUS_OK ? &job->saved : NULL);
    if (status == UMI_STATUS_OK && !self->retired && self->form != NULL)
      status = UmiStructuredFormGtk4MarkSaved(self->form, job->native_revision);
    message = "Saved the captured answers. Later edits remain unsaved.";
  } else if (job->operation == FORM_RECORD_SAVE_SEARCH) {
    /* The committed definition is real even if its window closed or controls
     * changed. Never relabel an accepted write as cancelled or mark answers saved. */
    if (status == UMI_STATUS_OK) { self->saved_search = job->saved; self->has_saved_search = true; }
    message = "Saved the captured search definition. Later query edits remain visible.";
  } else if (job->operation == FORM_RECORD_EXPORT) {
    /* Completion reports the admitted file write even after the editor closes.
     * It never
     * touches the current form or marks later edits as saved. */
    message = "Exported the reviewed saved history to a new plaintext file.";
  } else if (job->operation == FORM_RECORD_KEEP) {
    /* A completed recovery write is real even when its host retired. */
    message = "Kept a recovery copy. This does not save or clear changes in the database.";
  } else if (!self->retired && status == UMI_STATUS_OK) {
    if (job->operation == FORM_RECORD_CONNECT) {
      self->server = job->opened;
      job->opened = NULL;
    } else if (job->operation == FORM_RECORD_OPEN || job->operation == FORM_RECORD_OPEN_LISTED) {
      status = accept_document(self, job);
      message = "Opened the requested record. Review its answers before editing.";
    } else if (job->operation == FORM_RECORD_EDIT_SEARCH_DRAFT || job->operation == FORM_RECORD_RECOVER_SEARCH_DRAFT) {
      status = search_draft_read_ready(self, job);
      if (status == UMI_STATUS_OK) status = job->operation == FORM_RECORD_EDIT_SEARCH_DRAFT
          ? UmiStructuredFormSearchDraftGtk4Open(self->search_draft, job->document)
          : UmiStructuredFormSearchDraftGtk4Restore(self->search_draft, job->snapshot);
      message = "Query draft opened for editing. Review it before saving; scratch conditions are unchanged.";
    } else if (job->operation == FORM_RECORD_FIND_SEARCH) {
      status = accept_search_page(self, job);
      message = "Saved definitions listed. Select one explicitly to open it.";
    } else if (job->operation == FORM_RECORD_OPEN_SEARCH) {
      status = accept_search(self, job);
      message = "Opened the saved search definition. Choose Search saved answers to run it.";
    } else if (job->operation == FORM_RECORD_FIND || job->operation == FORM_RECORD_FIND_TYPED) {
      status = accept_catalogue(self, job);
      if (status == UMI_STATUS_OK && job->operation == FORM_RECORD_FIND_TYPED &&
          (self->retired || UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
           strcmp(job->query.text, gtk_editable_get_text(GTK_EDITABLE(self->search))) != 0))
        status = UMI_STATUS_CANCELLED;
      message = self->catalogue != NULL && self->catalogue->count == 0
                    ? "No matching records."
                    : "Select a listed record explicitly before opening it.";
    } else if (job->operation == FORM_RECORD_HISTORY) {
      status = accept_history(self, job);
      message = "History metadata loaded. Select a revision to inspect its copied answers.";
    } else if (job->operation == FORM_RECORD_INSPECT) {
      status = accept_inspection(self, job);
      message = "Historical answers shown read-only. Your current draft is unchanged.";
    } else if (job->operation == FORM_RECORD_REVIEW) {
      status = accept_review(self, job);
      message = "Compare baseline, local and incoming answers. Resolve conflicts, then apply the "
                "review locally.";
    } else if (job->operation == FORM_RECORD_RECOVER) {
      status = accept_recovery(self, job);
      message = "Recovered the selected draft. Review it before saving; the stored head is checked "
                "again on save.";
    } else
      message = "Kept a recovery copy. This does not save or clear changes in the database.";
  } else if (self->retired && status == UMI_STATUS_OK)
    status = UMI_STATUS_CANCELLED;
  self->busy = false;
  show_status(self, status, message);
  self->working = false;
}
static void dispatch(UmiFormRecordRoot *self, FormRecordJob *job) {
  job->server = self->server;
  self->busy = true;
  GTask *task = g_task_new(self, NULL, completed, NULL);
  g_task_set_task_data(task, job, job_free);
  g_task_set_check_cancellable(task, FALSE);
  g_task_run_in_thread(task, worker);
  g_object_unref(task);
}
UmiStatus UmiFormRecordEditorGtk4SetHistoryCatalogue(GtkWidget *editor,
                                                     const UmiStructuredFormCatalogue *catalogue) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (catalogue == NULL)
    status = UMI_STATUS_INVALID_ARGUMENT;
  else if (self->server != NULL || self->history_schemas != NULL)
    status = UMI_STATUS_INVALID_STATE;
  const UmiStructuredFormSchema *active = NULL;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_catalogue_find(catalogue, self->schema->base->form_id,
                                                self->schema->version, &active);
  if (status == UMI_STATUS_OK && active != self->schema)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    self->history_schemas = catalogue;
  return leave(self, status, "Historical schemas registered. Active editing rules are unchanged.");
}
UmiStatus UmiFormRecordEditorGtk4Connect(GtkWidget *editor, const char *path) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server != NULL)
    return leave(self, UMI_STATUS_ALREADY_EXISTS, "");
  if (path == NULL || !g_path_is_absolute(path) || !g_utf8_validate(path, -1, NULL))
    return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (job == NULL)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  status = umi_text_replace(job->path, sizeof(job->path), path);
  if (status == UMI_STATUS_OK) {
    job->operation = FORM_RECORD_CONNECT;
    dispatch(self, job);
  } else
    job_free(job);
  return leave(self, status, "Opening the selected local database.");
}
/* Query values and selected metadata are copied before dispatch. The portable
 * catalogue binds continuations to a snapshot and exact selected opens recheck
 * the current head; the host never silently substitutes a different revision. */
UmiStatus UmiFormRecordEditorGtk4Find(GtkWidget *editor, const char *query, int next_page) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || query == NULL || (next_page != 0 && next_page != 1) ||
      !g_utf8_validate(query, -1, NULL))
    return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (job == NULL)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->catalogue = g_try_new0(UmiRecordCataloguePage, 1);
  status = job->catalogue == NULL
               ? UMI_STATUS_OUT_OF_MEMORY
               : umi_text_replace(job->query.text, sizeof(job->query.text), query);
  memcpy(job->query.entity_prefix, self->prefix, strlen(self->prefix) + 1);
  job->query.limit = UMI_RECORD_CATALOGUE_PAGE_CAPACITY;
  if (status == UMI_STATUS_OK && next_page) {
    if (self->catalogue == NULL || self->catalogue_is_typed || strcmp(self->catalogue->text, job->query.text) != 0 ||
        self->catalogue->next_after_entity[0] == '\0')
      status = UMI_STATUS_INVALID_STATE;
    else {
      memcpy(job->query.after_entity, self->catalogue->next_after_entity,
             sizeof(job->query.after_entity));
      memcpy(job->query.expected_snapshot, self->catalogue->snapshot,
             sizeof(job->query.expected_snapshot));
    }
  }
  if (status == UMI_STATUS_OK) {
    GtkWidget *search = g_object_ref(self->search);
    gtk_editable_set_text(GTK_EDITABLE(search), job->query.text);
    g_object_unref(search);
    if (self->retired)
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) {
    job->operation = FORM_RECORD_FIND;
    dispatch(self, job);
  } else
    job_free(job);
  return leave(self, status,
               "Finding records in the configured namespace. Results describe metadata only.");
}
/* Filters are query state, separate from unsaved record answers. This method
 * changes only the filter and never saves or replaces the current form. */
UmiStatus UmiFormRecordEditorGtk4SetFilter(GtkWidget *editor, const UmiStructuredFormFilter *filter) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  status = self->filter == NULL ? UMI_STATUS_NOT_IMPLEMENTED :
      UmiStructuredFormFilterGtk4Set(self->filter, filter, UmiStructuredFormFilterGtk4Revision(self->filter));
  return leave(self, status, "Conditions updated. Search again to refresh matching records.");
}
UmiStatus UmiFormRecordEditorGtk4FindTyped(GtkWidget *editor, const char *metadata, int next_page) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->filter) return leave(self, UMI_STATUS_NOT_IMPLEMENTED, "");
  if (!self->server || !metadata || (next_page != 0 && next_page != 1) ||
      !g_utf8_validate(metadata, -1, NULL)) return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (!job) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->typed_query = g_try_new0(UmiStructuredFormQuery, 1);
  job->typed_page = g_try_new0(UmiStructuredFormQueryPage, 1);
  job->catalogue = g_try_new0(UmiRecordCataloguePage, 1);
  if (!job->typed_query || !job->typed_page || !job->catalogue) status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) status = umi_text_replace(job->query.text, sizeof(job->query.text), metadata);
  if (status == UMI_STATUS_OK) status = UmiStructuredFormFilterGtk4Read(self->filter,
      &job->typed_query->filter, &job->filter_revision);
  if (status == UMI_STATUS_OK) {
    UmiStructuredFormQuery *query = job->typed_query;
    memcpy(job->query.entity_prefix, self->prefix, strlen(self->prefix) + 1);
    memcpy(query->entity_prefix, job->query.entity_prefix, sizeof(query->entity_prefix));
    memcpy(query->metadata_text, job->query.text, sizeof(query->metadata_text));
    memcpy(query->form_id, self->schema->base->form_id, strlen(self->schema->base->form_id) + 1);
    query->schema_version = self->schema->version;
    query->limit = UMI_RECORD_CATALOGUE_PAGE_CAPACITY;
    if (next_page) {
      if (!self->catalogue || !self->catalogue_is_typed ||
          self->catalogue_filter_revision != job->filter_revision ||
          strcmp(self->catalogue->text, query->metadata_text) != 0 ||
          !self->catalogue->next_after_entity[0]) status = UMI_STATUS_INVALID_STATE;
      else {
        memcpy(query->after_entity, self->catalogue->next_after_entity, sizeof(query->after_entity));
        memcpy(query->expected_snapshot, self->catalogue->snapshot, sizeof(query->expected_snapshot));
      }
    }
  }
  if (status == UMI_STATUS_OK) {
    job->schemas = self->history_schemas;
    if (!job->schemas) {
      const UmiStructuredFormSchema *schemas[] = {self->schema};
      status = umi_structured_form_catalogue_create(schemas, 1, &job->owned_schemas);
      job->schemas = job->owned_schemas;
    }
  }
  if (status == UMI_STATUS_OK) {
    GtkWidget *search = g_object_ref(self->search);
    gtk_editable_set_text(GTK_EDITABLE(search), job->query.text);
    g_object_unref(search);
    if (self->retired || UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
        strcmp(job->query.text, gtk_editable_get_text(GTK_EDITABLE(self->search))) != 0)
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) { job->operation = FORM_RECORD_FIND_TYPED; dispatch(self, job); }
  else job_free(job);
  return leave(self, status, "Searching verified saved answers. Current draft changes are not searched.");
}

/* Managed saved-query editing is composed from the reusable native draft.
 * The application host keeps one worker queue and never gives GTK to storage. */
UmiStatus UmiFormRecordEditorGtk4BeginSearchDraft(GtkWidget *editor, const char *key,
                                                const char *title) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->server || !self->search_draft) return leave(self, UMI_STATUS_INVALID_STATE, "");
  UmiStructuredFormSearchPreset *preset = g_try_new0(UmiStructuredFormSearchPreset, 1);
  char entity[64]; uint64_t revision = 0;
  if (!preset) status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) status = search_entity(key, entity);
  if (status == UMI_STATUS_OK) status = capture_search(self, title, preset, &revision);
  if (status == UMI_STATUS_OK) status = UmiStructuredFormSearchDraftGtk4Begin(self->search_draft, entity, preset);
  g_free(preset);
  return leave(self, status, "New query draft created from the current conditions. Save or discard it before closing.");
}
UmiStatus UmiFormRecordEditorGtk4SearchDraftState(GtkWidget *editor,
                                                UmiStructuredFormSearchSessionView *out) {
  UmiFormRecordRoot *self = root_from(editor);
  if (!self || !out) return UMI_STATUS_INVALID_ARGUMENT;
  if (!self->search_draft) return UMI_STATUS_UNAVAILABLE;
  return UmiStructuredFormSearchDraftGtk4State(self->search_draft, out);
}
static UmiStatus capture_search_draft_context(UmiFormRecordRoot *self, FormRecordJob *job) {
  if (!UmiStructuredFormSearchDraftGtk4CanClose(self->search_draft)) return UMI_STATUS_INVALID_STATE;
  UmiStructuredFormSearchSessionView *view = g_try_new0(UmiStructuredFormSearchSessionView, 1);
  if (!view) return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = UmiStructuredFormSearchDraftGtk4State(self->search_draft, view);
  if (status == UMI_STATUS_OK) { job->generation = view->generation; job->native_revision = view->edit_revision; }
  g_free(view); return status;
}
UmiStatus UmiFormRecordEditorGtk4EditSearchDraft(GtkWidget *editor, const char *key) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->server || !self->search_draft) return leave(self, UMI_STATUS_INVALID_STATE, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (!job) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->document = g_try_new0(UmiRecordDocument, 1);
  if (!job->document) status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) status = search_entity(key, job->entity);
  if (status == UMI_STATUS_OK) status = capture_search_draft_context(self, job);
  if (status == UMI_STATUS_OK) { job->operation = FORM_RECORD_EDIT_SEARCH_DRAFT; dispatch(self, job); }
  else job_free(job);
  return leave(self, status, "Reading a saved query for editing. Current record answers stay unchanged.");
}
UmiStatus UmiFormRecordEditorGtk4SaveSearchDraft(GtkWidget *editor,
                                               const UmiRecordDraftAttribution *attribution) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->server || !self->search_draft) return leave(self, UMI_STATUS_INVALID_STATE, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (!job) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->request = g_try_new0(UmiRecordSaveRequest, 1);
  if (!job->request) status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) status = UmiStructuredFormSearchDraftGtk4PrepareSave(self->search_draft, attribution, job->request);
  if (status == UMI_STATUS_OK) {
    /* Retain completion authority after parent disposal. Preparation succeeded;
     * no fallible admission step follows before the worker owns the request. */
    job->draft_target = g_object_ref(self->search_draft);
    job->operation = FORM_RECORD_SAVE_SEARCH_DRAFT; dispatch(self, job);
  } else job_free(job);
  return leave(self, status, "Saving the query draft snapshot. You can continue editing its controls.");
}
UmiStatus UmiFormRecordEditorGtk4DiscardSearchDraft(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  status = self->search_draft ? UmiStructuredFormSearchDraftGtk4Discard(self->search_draft) : UMI_STATUS_UNAVAILABLE;
  return leave(self, status, "Discarded managed query edits. Record answers and scratch query conditions are unchanged.");
}
static UmiStatus search_recovery_operation(GtkWidget *editor, const char *path, bool writing) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->server || !self->search_draft) return leave(self, UMI_STATUS_INVALID_STATE, "");
  if (!path || !g_path_is_absolute(path) || !g_utf8_validate(path, -1, NULL))
    return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (!job) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->snapshot = g_try_new0(UmiRecordDraftSnapshot, 1);
  status = job->snapshot ? umi_text_replace(job->path, sizeof job->path, path) : UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) status = writing
      ? UmiStructuredFormSearchDraftGtk4Capture(self->search_draft, job->snapshot)
      : capture_search_draft_context(self, job);
  if (status == UMI_STATUS_OK) {
    job->operation = writing ? FORM_RECORD_KEEP_SEARCH_DRAFT : FORM_RECORD_RECOVER_SEARCH_DRAFT;
    dispatch(self, job);
  } else job_free(job);
  return leave(self, status, writing ? "Writing a new private query recovery file. The draft stays unsaved."
      : "Reading query recovery and checking its baseline against the selected database.");
}
UmiStatus UmiFormRecordEditorGtk4KeepSearchRecovery(GtkWidget *editor, const char *path) {
  return search_recovery_operation(editor, path, true);
}
UmiStatus UmiFormRecordEditorGtk4RecoverSearch(GtkWidget *editor, const char *path) {
  return search_recovery_operation(editor, path, false);
}

UmiStatus UmiFormRecordEditorGtk4SavedSearch(GtkWidget *editor, UmiRecordRevision *out) {
  UmiFormRecordRoot *self = root_from(editor);
  if (!self || !out) return UMI_STATUS_INVALID_ARGUMENT;
  if (!self->has_saved_search) return UMI_STATUS_NOT_FOUND;
  *out = self->saved_search; return UMI_STATUS_OK;
}
UmiStatus UmiFormRecordEditorGtk4SaveSearch(GtkWidget *editor, const char *key, const char *title,
    uint64_t expected_revision, const UmiRecordDraftAttribution *attribution) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->server || !attribution || !self->filter)
    return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  if (expected_revision == UINT64_MAX)
    return leave(self, UMI_STATUS_CAPACITY_EXCEEDED, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (!job) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->preset = g_try_new0(UmiStructuredFormSearchPreset, 1);
  if (!job->preset) status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) status = search_entity(key, job->preset_metadata.entity_id);
  if (status == UMI_STATUS_OK) status = capture_search(self, title, job->preset, &job->filter_revision);
  if (status == UMI_STATUS_OK) {
    /* Fixed attribution arrays are copied before formatting validates their
     * termination and content. No borrowed native string reaches the worker. */
    memcpy(job->preset_metadata.source, attribution->source, sizeof(attribution->source));
    memcpy(job->preset_metadata.actor, attribution->actor, sizeof(attribution->actor));
    memcpy(job->preset_metadata.reason, attribution->reason, sizeof(attribution->reason));
    job->preset_metadata.recorded_at_nanoseconds = attribution->recorded_at_nanoseconds;
    job->preset_metadata.observed_at_nanoseconds = attribution->observed_at_nanoseconds;
    memset(job->preset_metadata.content_sha256, '0', 64);
    job->preset_metadata.content_sha256[64] = 0;
    UmiRecordRevision proposed = {.amendment = job->preset_metadata,
        .revision = expected_revision + 1, .previous_revision = expected_revision};
    char encoded[UMI_RECORD_REVISION_TEXT_CAPACITY];
    status = umi_record_revision_format(&proposed, encoded, sizeof(encoded));
  }
  if (status == UMI_STATUS_OK) {
    job->schema = self->schema; job->preset_expected = expected_revision;
    job->operation = FORM_RECORD_SAVE_SEARCH; dispatch(self, job);
  } else job_free(job);
  return leave(self, status, "Saving the captured search definition. Later edits remain visible.");
}
/* The manual-key implementation remains for review. Both manual opens and
 * listed selections now share query-control lifetime checks; selections also
 * carry the exact reviewed storage head through the worker. */
#if 0
UmiStatus UmiFormRecordEditorGtk4OpenSearch(GtkWidget *editor, const char *key) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->server || !self->filter) return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (!job) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->preset_loaded = g_try_new0(UmiStructuredFormSearchPresetRecord, 1);
  status = job->preset_loaded ? search_entity(key, job->entity) : UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) {
    status = umi_text_replace(job->search_key, sizeof(job->search_key), key);
    job->filter_revision = UmiStructuredFormFilterGtk4Revision(self->filter);
  }
  if (status == UMI_STATUS_OK) status = umi_text_replace(job->query.text, sizeof(job->query.text),
      gtk_editable_get_text(GTK_EDITABLE(self->search)));
  if (status == UMI_STATUS_OK) status = umi_text_replace(job->search_title, sizeof(job->search_title),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_title)));
  if (status == UMI_STATUS_OK) {
    GtkWidget *entry = g_object_ref(self->preset_key);
    gtk_editable_set_text(GTK_EDITABLE(entry), job->search_key); g_object_unref(entry);
    if (self->retired || UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
        strcmp(job->query.text, gtk_editable_get_text(GTK_EDITABLE(self->search))) ||
        strcmp(job->search_title, gtk_editable_get_text(GTK_EDITABLE(self->preset_title))) ||
        strcmp(job->search_key, gtk_editable_get_text(GTK_EDITABLE(self->preset_key))))
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) {
    job->schema = self->schema; job->operation = FORM_RECORD_OPEN_SEARCH; dispatch(self, job);
  } else job_free(job);
  return leave(self, status, "Opening a saved search. Changes made during the read are retained.");
}

#endif
UmiStatus UmiFormRecordEditorGtk4FindSavedSearches(GtkWidget *editor, const char *text, int next_page) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->server || !self->preset_find || !text || (next_page != 0 && next_page != 1) ||
      !g_utf8_validate(text, -1, NULL)) return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (!job) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->search_page = g_try_new0(UmiStructuredFormSearchPage, 1);
  status = job->search_page ? umi_text_replace(job->search_query.text, sizeof(job->search_query.text), text) : UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) {
    strcpy(job->search_query.form_id, self->schema->base->form_id);
    strcpy(job->search_query.entity_prefix, self->prefix);
    job->search_query.schema_version = self->schema->version;
    job->search_query.limit = UMI_RECORD_CATALOGUE_PAGE_CAPACITY;
    if (next_page) {
      if (!self->preset_page || strcmp(self->preset_query.text, text) ||
          strcmp(self->preset_query.text, gtk_editable_get_text(GTK_EDITABLE(self->preset_find))) ||
          !self->preset_page->next_after_entity[0]) status = UMI_STATUS_INVALID_STATE;
      else {
        strcpy(job->search_query.after_entity, self->preset_page->next_after_entity);
        strcpy(job->search_query.expected_snapshot, self->preset_page->snapshot);
      }
    }
  }
  if (status == UMI_STATUS_OK) {
    job->schemas = self->history_schemas;
    if (!job->schemas) {
      const UmiStructuredFormSchema *schemas[] = {self->schema};
      status = umi_structured_form_catalogue_create(schemas, 1, &job->owned_schemas);
      job->schemas = job->owned_schemas;
    }
  }
  if (status == UMI_STATUS_OK) {
    GtkWidget *entry = g_object_ref(self->preset_find);
    gtk_editable_set_text(GTK_EDITABLE(entry), job->search_query.text); g_object_unref(entry);
    if (self->retired || strcmp(job->search_query.text, gtk_editable_get_text(GTK_EDITABLE(self->preset_find))))
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) { job->operation = FORM_RECORD_FIND_SEARCH; dispatch(self, job); }
  else job_free(job);
  return leave(self, status, "Finding saved search definitions. Record drafts are unchanged.");
}
UmiStatus UmiFormRecordEditorGtk4SavedSearchPage(GtkWidget *editor, UmiStructuredFormSearchPage *out) {
  UmiFormRecordRoot *self = root_from(editor);
  if (!self || !out) return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired) return UMI_STATUS_INVALID_STATE;
  if (self->busy || self->working) return UMI_STATUS_BUSY;
  if (!self->preset_page) return UMI_STATUS_NOT_FOUND;
  if (strcmp(self->preset_query.text, gtk_editable_get_text(GTK_EDITABLE(self->preset_find))) ||
      gtk_drop_down_get_model(GTK_DROP_DOWN(self->preset_results)) != self->preset_model ||
      !search_model_matches_page(self->preset_model, self->preset_page))
    return UMI_STATUS_INVALID_STATE;
  *out = *self->preset_page; return UMI_STATUS_OK;
}
static UmiStatus open_search(GtkWidget *editor, const char *key, const UmiRecordRevision *reviewed, guint index) {
  UmiFormRecordRoot *self = root_from(editor); UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK) return status;
  if (!self->server || !self->filter) return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (!job) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->preset_loaded = g_try_new0(UmiStructuredFormSearchPresetRecord, 1);
  status = job->preset_loaded ? search_entity(key, job->entity) : UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) {
    status = umi_text_replace(job->search_key, sizeof(job->search_key), key);
    job->filter_revision = UmiStructuredFormFilterGtk4Revision(self->filter);
  }
  if (status == UMI_STATUS_OK) status = umi_text_replace(job->query.text, sizeof(job->query.text),
      gtk_editable_get_text(GTK_EDITABLE(self->search)));
  if (status == UMI_STATUS_OK) status = umi_text_replace(job->search_title, sizeof(job->search_title),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_title)));
  if (status == UMI_STATUS_OK && reviewed) {
    job->reviewed_search = true; job->selected = *reviewed; job->preset_index = index;
    job->search_query = self->preset_query;
    GtkWidget *results = g_object_ref(self->preset_results);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(results), index); g_object_unref(results);
    if (!search_selection_matches(self, job)) status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) {
    GtkWidget *entry = g_object_ref(self->preset_key);
    gtk_editable_set_text(GTK_EDITABLE(entry), job->search_key); g_object_unref(entry);
    if (self->retired || !search_selection_matches(self, job) || UmiStructuredFormFilterGtk4Revision(self->filter) != job->filter_revision ||
        strcmp(job->query.text, gtk_editable_get_text(GTK_EDITABLE(self->search))) ||
        strcmp(job->search_title, gtk_editable_get_text(GTK_EDITABLE(self->preset_title))) ||
        strcmp(job->search_key, gtk_editable_get_text(GTK_EDITABLE(self->preset_key))))
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) {
    job->schema = self->schema; job->operation = FORM_RECORD_OPEN_SEARCH; dispatch(self, job);
  } else job_free(job);
  return leave(self, status, "Opening a saved search. Changes made during the read are retained.");
}

UmiStatus UmiFormRecordEditorGtk4OpenSearch(GtkWidget *editor, const char *key) {
  return open_search(editor, key, NULL, 0);
}
UmiStatus UmiFormRecordEditorGtk4OpenSavedSearchResult(GtkWidget *editor, size_t index) {
  UmiFormRecordRoot *self = root_from(editor);
  if (!self) return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired) return UMI_STATUS_INVALID_STATE;
  if (self->busy || self->working) return UMI_STATUS_BUSY;
  if (!self->preset_page || index >= self->preset_page->count ||
      strcmp(self->preset_query.text, gtk_editable_get_text(GTK_EDITABLE(self->preset_find))) ||
      gtk_drop_down_get_model(GTK_DROP_DOWN(self->preset_results)) != self->preset_model ||
      !search_model_matches_page(self->preset_model, self->preset_page))
    return UMI_STATUS_INVALID_STATE;
  UmiRecordRevision selected = self->preset_page->rows[index].record;
  return open_search(editor, selected.amendment.entity_id + strlen(UMI_FORM_SEARCH_PRESET_PREFIX), &selected, (guint)index);
}

UmiStatus UmiFormRecordEditorGtk4OpenListed(GtkWidget *editor, size_t index) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || self->catalogue == NULL || index >= self->catalogue->count ||
      (self->catalogue_is_typed && self->catalogue_filter_revision != UmiStructuredFormFilterGtk4Revision(self->filter)) ||
      strcmp(self->catalogue->text, gtk_editable_get_text(GTK_EDITABLE(self->search))) != 0 ||
      !clean(self))
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  if (job == NULL || view == NULL) {
    job_free(job);
    g_free(view);
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  }
  job->selected = self->catalogue->rows[index];
  job->selected_typed = self->catalogue_is_typed;
  job->filter_revision = self->catalogue_filter_revision;
  memcpy(job->query.text, self->catalogue->text, sizeof(job->query.text));
  char entity[64];
  status = entity_for(self, job->selected.amendment.entity_id + strlen(self->prefix), entity);
  if (status == UMI_STATUS_OK && strcmp(entity, job->selected.amendment.entity_id) != 0)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_snapshot(self->session, view);
  job->generation = view->generation;
  g_free(view);
  job->document = g_try_new0(UmiRecordDocument, 1);
  if (job->document == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) {
    job->operation = FORM_RECORD_OPEN_LISTED;
    dispatch(self, job);
  } else
    job_free(job);
  return leave(self, status,
               "Opening exactly the listed record revision; later edits prevent replacement.");
}
UmiStatus UmiFormRecordEditorGtk4History(GtkWidget *editor, int older_page) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || (older_page != 0 && older_page != 1))
    return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  if (job == NULL || view == NULL) {
    job_free(job);
    g_free(view);
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  }
  status = umi_structured_form_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK && (!view->record.ready || view->record.is_new))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    memcpy(job->entity, view->record.entity_id, sizeof(job->entity));
    job->generation = view->generation;
    if (older_page) {
      if (self->history == NULL || self->history_generation != job->generation ||
          strcmp(self->history->entity_id, job->entity) != 0 ||
          self->history->next_before_revision == 0)
        status = UMI_STATUS_INVALID_STATE;
      else {
        job->history_head = self->history->head_revision;
        job->history_before = self->history->next_before_revision;
      }
    }
  }
  g_free(view);
  job->history = g_try_new0(UmiRecordHistoryPage, 1);
  if (job->history == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) {
    job->operation = FORM_RECORD_HISTORY;
    dispatch(self, job);
  } else
    job_free(job);
  return leave(self, status, "Reading stored amendment history; current answers remain editable.");
}
UmiStatus UmiFormRecordEditorGtk4InspectRevision(GtkWidget *editor, size_t index) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || self->history == NULL || index >= self->history->count)
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (job == NULL)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->selected = self->history->rows[index];
  job->generation = self->history_generation;
  memcpy(job->entity, self->history->entity_id, sizeof(job->entity));
  status = history_context(self, job);
  job->document = g_try_new0(UmiRecordDocument, 1);
  if (job->document == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK) {
    job->operation = FORM_RECORD_INSPECT;
    dispatch(self, job);
  } else
    job_free(job);
  return leave(self, status,
               "Reading the selected historical payload without replacing the active draft.");
}
UmiStatus UmiFormRecordEditorGtk4Begin(GtkWidget *editor, const char *key) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  char entity[64];
  status = self->server == NULL ? UMI_STATUS_INVALID_STATE : entity_for(self, key, entity);
  if (status == UMI_STATUS_OK && !clean(self))
    status = UMI_STATUS_INVALID_STATE;
  GtkWidget *native = NULL;
  if (status == UMI_STATUS_OK) {
    native = UmiStructuredFormGtk4New(self->schema, NULL);
    if (native == NULL)
      status = UMI_STATUS_OUT_OF_MEMORY;
    else
      g_object_ref_sink(native);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_begin(self->session, entity);
  if (status == UMI_STATUS_OK)
    install_form(self, native);
  if (native != NULL)
    g_object_unref(native);
  return leave(self, status,
               "New unsaved record. This key is checked for existing data when saved.");
}
UmiStatus UmiFormRecordEditorGtk4Open(GtkWidget *editor, const char *key) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || !clean(self))
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  if (job == NULL || view == NULL) {
    g_free(job);
    g_free(view);
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  }
  job->document = g_try_new0(UmiRecordDocument, 1);
  status = job->document == NULL ? UMI_STATUS_OUT_OF_MEMORY : entity_for(self, key, job->entity);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK) {
    job->generation = view->generation;
    job->operation = FORM_RECORD_OPEN;
    dispatch(self, job);
  } else
    job_free(job);
  g_free(view);
  return leave(self, status, "Reading the requested record; typing will prevent replacement.");
}
/* Save and recovery now capture native input through one shared path. The
 * previous save implementation is retained to review its lifetime checks. */
#if 0
UmiStatus UmiFormRecordEditorGtk4Save(GtkWidget *editor,
    const UmiRecordDraftAttribution *attribution, int submitting) {
    UmiFormRecordRoot *self = root_from(editor);
    UmiStatus status = enter(self);
    if (status != UMI_STATUS_OK) return status;
    if (self->server == NULL || self->form == NULL || attribution == NULL || (submitting != 0 && submitting != 1))
        return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
    /* Allocate all worker-owned storage before applying visible values. */
    FormRecordJob *job = g_try_new0(FormRecordJob, 1);
    UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1);
    UmiStructuredFormEdit *edit = g_try_new0(UmiStructuredFormEdit, 1);
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    if (job != NULL) job->request = g_try_new0(UmiRecordSaveRequest, 1);
    if (job == NULL || job->request == NULL || form == NULL || edit == NULL || view == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    GtkWidget *native = g_object_ref(self->form);
    if (status == UMI_STATUS_OK) status = UmiStructuredFormGtk4Apply(native, submitting);
    if (status == UMI_STATUS_OK && self->retired) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiStructuredFormGtk4Snapshot(native, form);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK) status = umi_structured_form_edit_capture(self->schema, &view->form, form, edit);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_apply(self->session, view->generation, edit);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_prepare_save(self->session,
        view->generation, attribution, submitting, job->request);
    if (status == UMI_STATUS_OK) {
        job->operation = FORM_RECORD_SAVE; job->native_revision = form->revision; dispatch(self, job);
    } else if (job != NULL) job_free(job);
    g_object_unref(native); g_free(view); g_free(edit); g_free(form);
    return leave(self, status, "Saving the captured answers; you may continue editing.");
}
#endif
static UmiStatus accept_visible(UmiFormRecordRoot *self, int submitting, uint64_t *generation,
                                uint64_t *native_revision) {
  UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1);
  UmiStructuredFormEdit *edit = g_try_new0(UmiStructuredFormEdit, 1);
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  UmiStatus status =
      form == NULL || edit == NULL || view == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
  GtkWidget *native = g_object_ref(self->form);
  if (status == UMI_STATUS_OK)
    status = UmiStructuredFormGtk4Apply(native, submitting);
  if (status == UMI_STATUS_OK && self->retired)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = UmiStructuredFormGtk4Snapshot(native, form);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_edit_capture(self->schema, &view->form, form, edit);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_apply(self->session, view->generation, edit);
  if (status == UMI_STATUS_OK) {
    *generation = view->generation;
    *native_revision = form->revision;
  }
  g_object_unref(native);
  g_free(view);
  g_free(edit);
  g_free(form);
  return status;
}
UmiStatus UmiFormRecordEditorGtk4Save(GtkWidget *editor,
                                      const UmiRecordDraftAttribution *attribution,
                                      int submitting) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || self->form == NULL || attribution == NULL ||
      (submitting != 0 && submitting != 1))
    return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  /* Capture command metadata before native Apply can notify application code. */
  const UmiRecordDraftAttribution captured = *attribution;
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (job == NULL)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->request = g_try_new0(UmiRecordSaveRequest, 1);
  status = job->request == NULL
               ? UMI_STATUS_OUT_OF_MEMORY
               : accept_visible(self, submitting, &job->generation, &job->native_revision);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_prepare_save(self->session, job->generation, &captured,
                                                      submitting, job->request);
  if (status == UMI_STATUS_OK) {
    job->operation = FORM_RECORD_SAVE;
    dispatch(self, job);
  } else
    job_free(job);
  return leave(self, status, "Saving the captured answers; you may continue editing.");
}
static UmiStatus recovery_operation(GtkWidget *editor, const char *path, bool writing) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || (writing && self->form == NULL) || (!writing && !clean(self)))
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  if (path == NULL || !g_path_is_absolute(path) || !g_utf8_validate(path, -1, NULL))
    return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (job == NULL)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->snapshot = g_try_new0(UmiRecordDraftSnapshot, 1);
  status = job->snapshot == NULL ? UMI_STATUS_OUT_OF_MEMORY
                                 : umi_text_replace(job->path, sizeof(job->path), path);
  memcpy(job->prefix, self->prefix, sizeof(job->prefix));
  if (status == UMI_STATUS_OK && writing) {
    status = accept_visible(self, 0, &job->generation, &job->native_revision);
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_session_capture(self->session, job->snapshot);
  } else if (status == UMI_STATUS_OK) {
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    status = view == NULL ? UMI_STATUS_OUT_OF_MEMORY
                          : umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK)
      job->generation = view->generation;
    g_free(view);
  }
  if (status == UMI_STATUS_OK) {
    job->operation = writing ? FORM_RECORD_KEEP : FORM_RECORD_RECOVER;
    dispatch(self, job);
  } else
    job_free(job);
  return leave(self, status,
               writing ? "Writing a new recovery file; the database remains unchanged."
                       : "Reading a recovery file and checking its stored baseline.");
}
UmiStatus UmiFormRecordEditorGtk4ExportHistory(GtkWidget *editor, const char *path) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || self->form == NULL || !clean(self))
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  if (path == NULL || !g_path_is_absolute(path) || !g_utf8_validate(path, -1, NULL))
    return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  UmiRecordDraftSnapshot *snapshot = g_try_new0(UmiRecordDraftSnapshot, 1);
  status = job == NULL || snapshot == NULL ? UMI_STATUS_OUT_OF_MEMORY
                                           : umi_text_replace(job->path, sizeof(job->path), path);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_capture(self->session, snapshot);
  if (status == UMI_STATUS_OK && snapshot->baseline.record.revision == 0)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    /* The worker receives only the reviewed saved identity, never GTK pointers
     * or a later
     * mutable draft. Capture completes before status notifications. */
    job->selected = snapshot->baseline.record;
    job->operation = FORM_RECORD_EXPORT;
    dispatch(self, job);
  } else if (job != NULL)
    job_free(job);
  g_free(snapshot);
  return leave(self, status, "Exporting the reviewed saved history; drafts are not exported.");
}
UmiStatus UmiFormRecordEditorGtk4KeepRecovery(GtkWidget *editor, const char *path) {
  return recovery_operation(editor, path, true);
}
UmiStatus UmiFormRecordEditorGtk4Recover(GtkWidget *editor, const char *path) {
  return recovery_operation(editor, path, false);
}
UmiStatus UmiFormRecordEditorGtk4ReviewStored(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->server == NULL || self->form == NULL)
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  FormRecordJob *job = g_try_new0(FormRecordJob, 1);
  if (job == NULL)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->document = g_try_new0(UmiRecordDocument, 1);
  job->snapshot = g_try_new0(UmiRecordDraftSnapshot, 1);
  status = job->document == NULL || job->snapshot == NULL
               ? UMI_STATUS_OUT_OF_MEMORY
               : accept_visible(self, 0, &job->generation, &job->native_revision);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_capture(self->session, job->snapshot);
  if (status == UMI_STATUS_OK && job->snapshot->baseline.record.revision == 0)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    memcpy(job->entity, job->snapshot->baseline.record.amendment.entity_id, sizeof(job->entity));
    job->operation = FORM_RECORD_REVIEW;
    dispatch(self, job);
  } else
    job_free(job);
  return leave(
      self, status,
      "Reading a checked stored version for comparison. Later typing will invalidate this review.");
}
UmiStatus UmiFormRecordEditorGtk4ApplyReview(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (self->conflict == NULL || self->form == NULL ||
      !UmiStructuredFormGtk4MatchesRevision(self->form, self->review_native_revision))
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  UmiFormMergeState state;
  status = umi_structured_form_conflict_state(self->conflict, &state);
  UmiStructuredForm *candidate = g_try_new0(UmiStructuredForm, 1);
  if (candidate == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_conflict_preview(self->conflict, state.revision, 0, candidate);
  GtkWidget *native = NULL;
  if (status == UMI_STATUS_OK) {
    native = UmiStructuredFormGtk4New(self->schema, candidate);
    if (native == NULL)
      status = UMI_STATUS_INVALID_STATE;
    else
      g_object_ref_sink(native);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_accept_review(self->session, self->review_generation,
                                                       self->conflict, state.revision, 0);
  if (status == UMI_STATUS_OK)
    install_form(self, native);
  if (native != NULL)
    g_object_unref(native);
  g_free(candidate);
  return leave(
      self, status,
      "Applied the reviewed answers locally. Save if you want to publish a new stored revision.");
}
UmiStatus UmiFormRecordEditorGtk4Discard(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  UmiRecordDraftSnapshot *snapshot = g_try_new0(UmiRecordDraftSnapshot, 1);
  UmiStructuredForm *baseline = g_try_new0(UmiStructuredForm, 1);
  GtkWidget *native = NULL;
  if (view == NULL || snapshot == NULL || baseline == NULL)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_snapshot(self->session, view);
  if (status == UMI_STATUS_OK && !view->record.ready)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK && !view->record.is_new) {
    status = umi_structured_form_session_capture(self->session, snapshot);
    if (status == UMI_STATUS_OK)
      status = umi_structured_form_snapshot_decode(self->schema, snapshot->baseline.content,
                                                   snapshot->baseline.length, baseline);
    if (status == UMI_STATUS_OK) {
      baseline->saved_revision = baseline->revision;
      native = UmiStructuredFormGtk4New(self->schema, baseline);
      if (native == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
      else
        g_object_ref_sink(native);
    }
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_discard(self->session, view->generation);
  if (status == UMI_STATUS_OK)
    install_form(self, native);
  if (native != NULL)
    g_object_unref(native);
  g_free(baseline);
  g_free(snapshot);
  g_free(view);
  return leave(self, status, "Discarded unsaved answers after the explicit discard action.");
}
UmiStatus UmiFormRecordEditorGtk4State(GtkWidget *editor, UmiStructuredFormSessionView *out) {
  UmiFormRecordRoot *self = root_from(editor);
  if (self == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  return umi_structured_form_session_snapshot(self->session, out);
}
UmiStatus UmiFormRecordEditorGtk4LastStatus(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  return self != NULL ? self->last : UMI_STATUS_INVALID_ARGUMENT;
}
bool UmiFormRecordEditorGtk4Busy(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  return self != NULL && self->busy;
}
/* The original record-only close guard is retained for review. The replacement
 * also protects the independent query draft and holds the owner during capture. */
#if 0
bool UmiFormRecordEditorGtk4CanClose(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  return self != NULL && !self->busy && !self->working && !self->retired && clean(self);
}
#endif
bool UmiFormRecordEditorGtk4CanClose(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  if (!self || self->busy || self->working || self->retired) return false;
  g_object_ref(self); self->working = true;
  bool allowed = clean(self) && (!self->search_draft || UmiStructuredFormSearchDraftGtk4CanClose(self->search_draft)) && !self->retired;
  self->working = false; g_object_unref(self); return allowed;
}
bool UmiFormRecordEditorGtk4RequestClose(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  if (self == NULL || self->retired)
    return false;
  bool allowed = UmiFormRecordEditorGtk4CanClose(editor);
  if (!allowed) {
    g_object_ref(self);
    GtkWidget *label = g_object_ref(self->feedback);
    gtk_label_set_text(GTK_LABEL(label),
                       self->busy ? "Wait for the current storage operation before closing."
                                  : "Save or explicitly discard both record and managed query drafts before closing.");
    g_object_unref(label);
    g_object_unref(self);
  }
  return allowed;
}
void UmiFormRecordEditorGtk4Retire(GtkWidget *editor) {
  UmiFormRecordRoot *self = root_from(editor);
  if (self != NULL)
    retire(self);
}
static void connect_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4Connect(GTK_WIDGET(self),
                                         gtk_editable_get_text(GTK_EDITABLE(self->path)));
}
static void begin_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4Begin(GTK_WIDGET(self),
                                       gtk_editable_get_text(GTK_EDITABLE(self->key)));
}
static void open_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4Open(GTK_WIDGET(self),
                                      gtk_editable_get_text(GTK_EDITABLE(self->key)));
}
/* The earlier button adapter is retained for review. It omitted the required
 * recorded timestamp,
 * so otherwise valid saves were rejected by Data Server.
 * The replacement captures checked UTC
 * time while keeping labels user-owned. */
#if 0
static void save_clicked(GtkButton *button, gpointer data) {
  UmiFormRecordRoot *self = data;
  if (self->retired)
    return;
  UmiRecordDraftAttribution attribution = {0};
  UmiStatus status = umi_text_replace(attribution.source, sizeof(attribution.source), self->source);
  if (status == UMI_STATUS_OK)
    status = umi_text_replace(attribution.actor, sizeof(attribution.actor),
                              gtk_editable_get_text(GTK_EDITABLE(self->actor)));
  if (status == UMI_STATUS_OK)
    status = umi_text_replace(attribution.reason, sizeof(attribution.reason),
                              gtk_editable_get_text(GTK_EDITABLE(self->reason)));
  if (status == UMI_STATUS_OK)
    (void)UmiFormRecordEditorGtk4Save(
        GTK_WIDGET(self), &attribution,
        GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "submit")));
  else {
    g_object_ref(self);
    show_status(self, status, "");
    g_object_unref(self);
  }
}
#endif
static void save_clicked(GtkButton *button, gpointer data) {
  UmiFormRecordRoot *self = data;
  if (self->retired)
    return;
  UmiRecordDraftAttribution attribution = {0};
  /* GLib reports UTC microseconds; storage requires a positive nanosecond
   * timestamp. Capture
   * once and reject overflow before admitting the save. */
  gint64 now = g_get_real_time();
  if (now <= 0 || (uint64_t)now > UINT64_MAX / 1000U) {
    g_object_ref(self);
    show_status(self, UMI_STATUS_INVALID_STATE, "");
    g_object_unref(self);
    return;
  }
  attribution.recorded_at_nanoseconds = (uint64_t)now * 1000U;
  UmiStatus status = umi_text_replace(attribution.source, sizeof(attribution.source), self->source);
  if (status == UMI_STATUS_OK)
    status = umi_text_replace(attribution.actor, sizeof(attribution.actor),
                              gtk_editable_get_text(GTK_EDITABLE(self->actor)));
  if (status == UMI_STATUS_OK)
    status = umi_text_replace(attribution.reason, sizeof(attribution.reason),
                              gtk_editable_get_text(GTK_EDITABLE(self->reason)));
  if (status == UMI_STATUS_OK)
    (void)UmiFormRecordEditorGtk4Save(
        GTK_WIDGET(self), &attribution,
        GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "submit")));
  else {
    g_object_ref(self);
    show_status(self, status, "");
    g_object_unref(self);
  }
}
/* Retain the original dialog composition for review. The context-bound GTK4
 * window below replaces it and avoids deprecated message-dialog interfaces. */
#if 0
static void discard_response(GtkDialog *dialog, int response, gpointer data) {
    UmiFormRecordRoot *self = data;
    if (response == GTK_RESPONSE_ACCEPT && !self->retired) (void)UmiFormRecordEditorGtk4Discard(GTK_WIDGET(self));
    gtk_window_destroy(GTK_WINDOW(dialog));
}
static void discard_clicked(GtkButton *button, gpointer data) {
    (void)button; UmiFormRecordRoot *self = data;
    if (self->retired || self->busy || self->working) return;
    GtkRoot *parent = gtk_widget_get_root(GTK_WIDGET(self));
    if (parent == NULL || !GTK_IS_WINDOW(parent)) return;
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "%s", "Discard unsaved answers?");
    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Keep editing", GTK_RESPONSE_CANCEL, "Discard", GTK_RESPONSE_ACCEPT, NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    g_signal_connect_object(dialog, "response", G_CALLBACK(discard_response), self, 0);
    gtk_window_present(GTK_WINDOW(dialog));
}
#endif
/* The modal decision carries the selected session generation. A stale dialog
 * cannot discard a record opened elsewhere while the prompt was visible. */
typedef struct FormDiscardPrompt {
  GWeakRef editor;
  uint64_t generation;
} FormDiscardPrompt;
static void prompt_free(gpointer data) {
  FormDiscardPrompt *prompt = data;
  g_weak_ref_clear(&prompt->editor);
  g_free(prompt);
}
static void prompt_cancel(GtkButton *button, gpointer data) {
  (void)button;
  gtk_window_destroy(GTK_WINDOW(data));
}
static void prompt_accept(GtkButton *button, gpointer data) {
  (void)button;
  GtkWindow *dialog = GTK_WINDOW(data);
  FormDiscardPrompt *prompt = g_object_get_data(G_OBJECT(dialog), "form-record-discard-context");
  UmiFormRecordRoot *self = g_weak_ref_get(&prompt->editor);
  if (self != NULL) {
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    UmiStatus status = view == NULL ? UMI_STATUS_OUT_OF_MEMORY
                                    : umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK && view->generation != prompt->generation)
      status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && !self->retired)
      status = UmiFormRecordEditorGtk4Discard(GTK_WIDGET(self));
    else if (!self->retired)
      show_status(self, status, "");
    g_free(view);
    g_object_unref(self);
  }
  gtk_window_destroy(dialog);
}
static void discard_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (self->retired || self->busy || self->working)
    return;
  GtkRoot *parent = gtk_widget_get_root(GTK_WIDGET(self));
  if (parent == NULL || !GTK_IS_WINDOW(parent))
    return;
  UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
  FormDiscardPrompt *prompt = g_try_new0(FormDiscardPrompt, 1);
  if (view == NULL || prompt == NULL) {
    g_free(view);
    g_free(prompt);
    return;
  }
  if (umi_structured_form_session_snapshot(self->session, view) != UMI_STATUS_OK ||
      !view->record.ready) {
    g_free(view);
    g_free(prompt);
    return;
  }
  prompt->generation = view->generation;
  g_free(view);
  g_weak_ref_init(&prompt->editor, self);
  GtkWidget *dialog = gtk_window_new();
  gtk_window_set_title(GTK_WINDOW(dialog), "Discard unsaved answers?");
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  g_object_set_data_full(G_OBJECT(dialog), "form-record-discard-context", prompt, prompt_free);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_widget_set_margin_start(box, 20);
  gtk_widget_set_margin_end(box, 20);
  gtk_widget_set_margin_top(box, 20);
  gtk_widget_set_margin_bottom(box, 20);
  gtk_box_append(GTK_BOX(box), gtk_label_new("Discard unsaved answers for the selected record?"));
  GtkWidget *cancel = gtk_button_new_with_label("Keep editing");
  GtkWidget *accept = gtk_button_new_with_label("Discard");
  gtk_widget_set_name(cancel, "form-record-discard-cancel");
  gtk_widget_set_name(accept, "form-record-discard-confirm");
  gtk_box_append(GTK_BOX(box), cancel);
  gtk_box_append(GTK_BOX(box), accept);
  gtk_window_set_child(GTK_WINDOW(dialog), box);
  gtk_window_set_default_widget(GTK_WINDOW(dialog), cancel);
  g_signal_connect_object(cancel, "clicked", G_CALLBACK(prompt_cancel), dialog, 0);
  g_signal_connect_object(accept, "clicked", G_CALLBACK(prompt_accept), dialog, 0);
  gtk_window_present(GTK_WINDOW(dialog));
}
static void export_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4ExportHistory(
        GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->export_path)));
}
static void keep_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4KeepRecovery(GTK_WIDGET(self),
                                              gtk_editable_get_text(GTK_EDITABLE(self->recovery)));
}
static void recover_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4Recover(GTK_WIDGET(self),
                                         gtk_editable_get_text(GTK_EDITABLE(self->recovery)));
}
static void review_clicked(GtkButton *button, gpointer data) {
  (void)button;
  (void)UmiFormRecordEditorGtk4ReviewStored(GTK_WIDGET(data));
}
static void apply_review_clicked(GtkButton *button, gpointer data) {
  (void)button;
  (void)UmiFormRecordEditorGtk4ApplyReview(GTK_WIDGET(data));
}
static void find_clicked(GtkButton *button, gpointer data) {
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4Find(GTK_WIDGET(self),
                                      gtk_editable_get_text(GTK_EDITABLE(self->search)),
                                      GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "next")));
}
static void typed_find_clicked(GtkButton *button, gpointer data) {
  UmiFormRecordRoot *self = data;
  if (!self->retired) (void)UmiFormRecordEditorGtk4FindTyped(GTK_WIDGET(self),
      gtk_editable_get_text(GTK_EDITABLE(self->search)),
      GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "next")));
}

static void find_searches_clicked(GtkButton *button_widget, gpointer data) {
  UmiFormRecordRoot *self = data;
  if (!self->retired) (void)UmiFormRecordEditorGtk4FindSavedSearches(GTK_WIDGET(self),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_find)),
      GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button_widget), "next")));
}
static void open_search_result_clicked(GtkButton *button_widget, gpointer data) {
  (void)button_widget; UmiFormRecordRoot *self = data;
  if (!self->retired) (void)UmiFormRecordEditorGtk4OpenSavedSearchResult(GTK_WIDGET(self),
      gtk_drop_down_get_selected(GTK_DROP_DOWN(self->preset_results)));
}
static void open_search_clicked(GtkButton *button, gpointer context) {
  (void)button; UmiFormRecordRoot *self = context;
  if (!self->retired) (void)UmiFormRecordEditorGtk4OpenSearch(GTK_WIDGET(self),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_key)));
}
static void save_search_clicked(GtkButton *button, gpointer context) {
  UmiFormRecordRoot *self = context;
  if (self->retired) return;
  UmiRecordDraftAttribution attribution = {0};
  UmiStatus status = umi_text_replace(attribution.source, sizeof attribution.source, self->source);
  if (status == UMI_STATUS_OK) status = umi_text_replace(attribution.actor, sizeof attribution.actor,
      gtk_editable_get_text(GTK_EDITABLE(self->actor)));
  if (status == UMI_STATUS_OK) status = umi_text_replace(attribution.reason, sizeof attribution.reason,
      gtk_editable_get_text(GTK_EDITABLE(self->reason)));
  gint64 now = g_get_real_time();
  if (now <= 0 || (uint64_t)now > UINT64_MAX / 1000U) status = UMI_STATUS_INVALID_STATE;
  else attribution.recorded_at_nanoseconds = (uint64_t)now * 1000U;
  uint64_t expected = 0; char entity[64];
  if (status == UMI_STATUS_OK && g_object_get_data(G_OBJECT(button), "update")) {
    status = search_entity(gtk_editable_get_text(GTK_EDITABLE(self->preset_key)), entity);
    if (status == UMI_STATUS_OK && (!self->has_saved_search || strcmp(entity, self->saved_search.amendment.entity_id)))
      status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) expected = self->saved_search.revision;
  }
  if (status == UMI_STATUS_OK) (void)UmiFormRecordEditorGtk4SaveSearch(GTK_WIDGET(self),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_key)),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_title)), expected, &attribution);
  else { g_object_ref(self); show_status(self, status, ""); g_object_unref(self); }
}

static void begin_search_draft_clicked(GtkButton *button_widget, gpointer context) {
  (void)button_widget; UmiFormRecordRoot *self = context;
  if (!self->retired) (void)UmiFormRecordEditorGtk4BeginSearchDraft(GTK_WIDGET(self),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_key)),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_title)));
}
static void edit_search_draft_clicked(GtkButton *button_widget, gpointer context) {
  (void)button_widget; UmiFormRecordRoot *self = context;
  if (!self->retired) (void)UmiFormRecordEditorGtk4EditSearchDraft(GTK_WIDGET(self),
      gtk_editable_get_text(GTK_EDITABLE(self->preset_key)));
}
static void save_search_draft_clicked(GtkButton *button_widget, gpointer context) {
  (void)button_widget; UmiFormRecordRoot *self = context;
  if (self->retired) return;
  UmiRecordDraftAttribution attribution = {0};
  UmiStatus status = umi_text_replace(attribution.source, sizeof attribution.source, self->source);
  if (status == UMI_STATUS_OK) status = umi_text_replace(attribution.actor, sizeof attribution.actor,
      gtk_editable_get_text(GTK_EDITABLE(self->actor)));
  if (status == UMI_STATUS_OK) status = umi_text_replace(attribution.reason, sizeof attribution.reason,
      gtk_editable_get_text(GTK_EDITABLE(self->reason)));
  gint64 now = g_get_real_time();
  if (now <= 0 || (uint64_t)now > UINT64_MAX / 1000U) status = UMI_STATUS_INVALID_STATE;
  else attribution.recorded_at_nanoseconds = (uint64_t)now * 1000U;
  if (status == UMI_STATUS_OK) (void)UmiFormRecordEditorGtk4SaveSearchDraft(GTK_WIDGET(self), &attribution);
  else { g_object_ref(self); show_status(self, status, ""); g_object_unref(self); }
}
static void search_recovery_clicked(GtkButton *button_widget, gpointer context) {
  UmiFormRecordRoot *self = context;
  if (self->retired) return;
  const char *path = gtk_editable_get_text(GTK_EDITABLE(self->search_recovery));
  if (g_object_get_data(G_OBJECT(button_widget), "reading"))
    (void)UmiFormRecordEditorGtk4RecoverSearch(GTK_WIDGET(self), path);
  else (void)UmiFormRecordEditorGtk4KeepSearchRecovery(GTK_WIDGET(self), path);
}

static void listed_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4OpenListed(
        GTK_WIDGET(self), gtk_drop_down_get_selected(GTK_DROP_DOWN(self->results)));
}
static void history_clicked(GtkButton *button, gpointer data) {
  (void)UmiFormRecordEditorGtk4History(
      GTK_WIDGET(data), GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "older")));
}
static void inspect_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiFormRecordRoot *self = data;
  if (!self->retired)
    (void)UmiFormRecordEditorGtk4InspectRevision(
        GTK_WIDGET(self), gtk_drop_down_get_selected(GTK_DROP_DOWN(self->history_results)));
}
static GtkWidget *entry(GtkWidget *parent, const char *name, const char *placeholder) {
  GtkWidget *result = gtk_entry_new();
  gtk_widget_set_name(result, name);
  gtk_entry_set_placeholder_text(GTK_ENTRY(result), placeholder);
  gtk_box_append(GTK_BOX(parent), result);
  return result;
}
static GtkWidget *button(GtkWidget *parent, UmiFormRecordRoot *self, const char *name,
                         const char *label, GCallback callback) {
  GtkWidget *result = gtk_button_new_with_label(label);
  gtk_widget_set_name(result, name);
  gtk_box_append(GTK_BOX(parent), result);
  g_signal_connect_object(result, "clicked", callback, self, 0);
  return result;
}
UmiStatus UmiFormRecordEditorGtk4New(const UmiFormRecordEditorGtk4Options *options,
                                     GtkWidget **out) {
  if (options == NULL || out == NULL || options->entity_prefix == NULL || options->source == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t prefix = strlen(options->entity_prefix), source = strlen(options->source);
  if (prefix == 0 || prefix > 48 || options->entity_prefix[prefix - 1] != '.' || source == 0 ||
      source >= 160 || !g_utf8_validate(options->source, -1, NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < prefix; ++i)
    if (!ascii_key_char(options->entity_prefix[i]) && options->entity_prefix[i] != '.')
      return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormSession *session = NULL;
  UmiStatus status = umi_structured_form_session_create(options->schema, &session);
  if (status != UMI_STATUS_OK)
    return status;
  /* Validate native rendering now, before accepting a schema that cannot be
   * displayed. No answers are kept or silently selected by this probe. */
  GtkWidget *probe = UmiStructuredFormGtk4New(options->schema, NULL);
  if (probe == NULL) {
    umi_structured_form_session_destroy(session);
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  g_object_ref_sink(probe);
  g_object_unref(probe);
  UmiFormRecordRoot *self = g_object_new(umi_form_record_root_get_type(), NULL);
  self->schema = options->schema;
  self->session = session;
  self->key_validator = options->key_validator;
  memcpy(self->prefix, options->entity_prefix, prefix + 1);
  memcpy(self->source, options->source, source + 1);
  gtk_box_append(GTK_BOX(self), gtk_label_new(options->schema->base->title));
  self->path =
      entry(GTK_WIDGET(self), "form-record-path", "Absolute path to a local SQLite database");
  (void)button(GTK_WIDGET(self), self, "form-record-connect", "Open or create database",
               G_CALLBACK(connect_clicked));
  self->key = entry(GTK_WIDGET(self), "form-record-key",
                    "Record key: letters, digits, underscore or hyphen");
  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_box_append(GTK_BOX(self), actions);
  (void)button(actions, self, "form-record-new", "New record", G_CALLBACK(begin_clicked));
  (void)button(actions, self, "form-record-open", "Open record", G_CALLBACK(open_clicked));
  self->search = entry(GTK_WIDGET(self), "form-record-search",
                       "Find record key, source, actor label or revision reason");
  GtkWidget *search_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_box_append(GTK_BOX(self), search_actions);
  (void)button(search_actions, self, "form-record-find", "Find records", G_CALLBACK(find_clicked));
  GtkWidget *next = button(search_actions, self, "form-record-find-next", "Next matching records",
                           G_CALLBACK(find_clicked));
  g_object_set_data(G_OBJECT(next), "next", GINT_TO_POINTER(1));
  /* Native filter editing belongs to Framework. Products supply only their
   * schema and namespace instead of duplicating query controls. */
  UmiStatus filter_status = UmiStructuredFormFilterGtk4New(self->schema, &self->filter);
  if (filter_status == UMI_STATUS_OK) {
    gtk_widget_set_name(self->filter, "form-record-filter");
    GtkWidget *expander = gtk_expander_new("Saved answer conditions");
    gtk_expander_set_child(GTK_EXPANDER(expander), self->filter);
    gtk_box_append(GTK_BOX(self), expander);
    GtkWidget *typed_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(self), typed_actions);
    (void)button(typed_actions, self, "form-record-find-typed", "Search saved answers", G_CALLBACK(typed_find_clicked));
    GtkWidget *typed_next = button(typed_actions, self, "form-record-find-typed-next", "Next answer matches", G_CALLBACK(typed_find_clicked));
    g_object_set_data(G_OBJECT(typed_next), "next", GINT_TO_POINTER(1));
    self->preset_key = entry(GTK_WIDGET(self), "form-record-search-key", "Saved search key: letters, digits, underscore or hyphen");
    self->preset_title = entry(GTK_WIDGET(self), "form-record-search-title", "Name this search");
    GtkWidget *preset_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(self), preset_actions);
    (void)button(preset_actions, self, "form-record-search-create", "Save new search", G_CALLBACK(save_search_clicked));
    GtkWidget *update_search = button(preset_actions, self, "form-record-search-update", "Save changes to opened search", G_CALLBACK(save_search_clicked));
    g_object_set_data(G_OBJECT(update_search), "update", GINT_TO_POINTER(1));
    (void)button(preset_actions, self, "form-record-search-open", "Open saved search", G_CALLBACK(open_search_clicked));
    /* Discovery uses the same worker and close protection as record actions. */
    self->preset_find = entry(GTK_WIDGET(self), "form-record-search-find-text", "Find saved search title or key");
    GtkWidget *browse_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(self), browse_actions);
    (void)button(browse_actions, self, "form-record-search-find", "Find saved searches", G_CALLBACK(find_searches_clicked));
    GtkWidget *browse_next = button(browse_actions, self, "form-record-search-find-next", "Next saved searches", G_CALLBACK(find_searches_clicked));
    g_object_set_data(G_OBJECT(browse_next), "next", GINT_TO_POINTER(1));
    self->preset_results = gtk_drop_down_new(NULL, NULL);
    gtk_widget_set_name(self->preset_results, "form-record-search-results");
    gtk_box_append(GTK_BOX(self), self->preset_results);
    (void)button(GTK_WIDGET(self), self, "form-record-search-open-result", "Open selected search", G_CALLBACK(open_search_result_clicked));
    /* A managed draft is distinct from temporary query controls. Framework owns
     * its lifetime and dirty-close guard; products inherit the same workflow. */
    UmiStatus draft_status = UmiStructuredFormSearchDraftGtk4New(self->schema, self->prefix, &self->search_draft);
    if (draft_status == UMI_STATUS_OK) {
      g_object_ref_sink(self->search_draft);
      gtk_widget_set_name(self->search_draft, "form-record-search-draft");
      GtkWidget *managed = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
      GtkWidget *draft_expander = gtk_expander_new("Edit a saved search with recovery");
      gtk_expander_set_child(GTK_EXPANDER(draft_expander), managed);
      gtk_box_append(GTK_BOX(self), draft_expander);
      gtk_box_append(GTK_BOX(managed), gtk_label_new("Managed query drafts are saved separately from record answers."));
      (void)button(managed, self, "form-record-search-draft-new", "Start draft from current query", G_CALLBACK(begin_search_draft_clicked));
      (void)button(managed, self, "form-record-search-draft-edit", "Edit saved search by key", G_CALLBACK(edit_search_draft_clicked));
      gtk_box_append(GTK_BOX(managed), self->search_draft);
      (void)button(managed, self, "form-record-search-draft-save", "Save managed query draft", G_CALLBACK(save_search_draft_clicked));
      self->search_recovery = entry(managed, "form-record-search-recovery-path", "Absolute path for a private query recovery file");
      (void)button(managed, self, "form-record-search-recovery-keep", "Keep a new query recovery copy", G_CALLBACK(search_recovery_clicked));
      GtkWidget *restore_search = button(managed, self, "form-record-search-recovery-open", "Recover query draft", G_CALLBACK(search_recovery_clicked));
      g_object_set_data(G_OBJECT(restore_search), "reading", GINT_TO_POINTER(1));
    } else gtk_box_append(GTK_BOX(self), gtk_label_new("Managed search editing is unavailable for this schema."));


  } else {
    /* Ordinary editing and metadata discovery remain available for schemas
     * whose answers cannot be exposed as filters. */
    gtk_box_append(GTK_BOX(self), gtk_label_new("Answer filters are unavailable for this schema."));
  }
  self->results = gtk_drop_down_new(NULL, NULL);
  gtk_widget_set_name(self->results, "form-record-results");
  gtk_box_append(GTK_BOX(self), self->results);
  (void)button(GTK_WIDGET(self), self, "form-record-open-listed", "Open selected listed record",
               G_CALLBACK(listed_clicked));
  self->body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_box_append(GTK_BOX(self), self->body);
  self->actor =
      entry(GTK_WIDGET(self), "form-record-actor", "Attribution label (not authentication)");
  self->reason = entry(GTK_WIDGET(self), "form-record-reason", "Reason for this revision");
  GtkWidget *saves = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_box_append(GTK_BOX(self), saves);
  (void)button(saves, self, "form-record-save-draft", "Save draft", G_CALLBACK(save_clicked));
  GtkWidget *submit = button(saves, self, "form-record-save-complete",
                             "Check required answers and save", G_CALLBACK(save_clicked));
  g_object_set_data(G_OBJECT(submit), "submit", GINT_TO_POINTER(1));
  (void)button(saves, self, "form-record-discard", "Discard unsaved answers...",
               G_CALLBACK(discard_clicked));
  (void)button(GTK_WIDGET(self), self, "form-record-review", "Review stored changes",
               G_CALLBACK(review_clicked));
  self->review_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_box_append(GTK_BOX(self), self->review_box);
  (void)button(GTK_WIDGET(self), self, "form-record-apply-review", "Apply reviewed answers locally",
               G_CALLBACK(apply_review_clicked));
  self->recovery = entry(GTK_WIDGET(self), "form-record-recovery-path",
                         "Absolute path for a private recovery file");
  (void)button(GTK_WIDGET(self), self, "form-record-keep-recovery", "Keep a new recovery copy",
               G_CALLBACK(keep_clicked));
  (void)button(GTK_WIDGET(self), self, "form-record-recover", "Recover a checked draft",
               G_CALLBACK(recover_clicked));
  /* File transfer is a Framework capability shared by every record host.
   * Choosing a filename
   * is separate from the explicit disclosure action. */
  self->export_path = entry(GTK_WIDGET(self), "form-record-export-path",
                            "Absolute path for a new saved-history file");
  GtkWidget *export_browse =
      UmiPathPickerGtk4Create(GTK_ENTRY(self->export_path), UMI_PATH_PICKER_NEW_FILE,
                              "Choose a new saved-history file", "record.umibundle");
  if (export_browse != NULL)
    gtk_box_append(GTK_BOX(self), export_browse);
  (void)button(GTK_WIDGET(self), self, "form-record-export-history", "Export saved history",
               G_CALLBACK(export_clicked));
  GtkWidget *history_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_box_append(GTK_BOX(self), history_actions);
  (void)button(history_actions, self, "form-record-history", "Refresh history",
               G_CALLBACK(history_clicked));
  GtkWidget *older = button(history_actions, self, "form-record-history-older", "Older history",
                            G_CALLBACK(history_clicked));
  g_object_set_data(G_OBJECT(older), "older", GINT_TO_POINTER(1));
  self->history_results = gtk_drop_down_new(NULL, NULL);
  gtk_widget_set_name(self->history_results, "form-record-history-results");
  gtk_box_append(GTK_BOX(self), self->history_results);
  (void)button(GTK_WIDGET(self), self, "form-record-inspect", "Inspect selected revision",
               G_CALLBACK(inspect_clicked));
  self->history_preview = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_widget_set_name(self->history_preview, "form-record-history-preview");
  gtk_box_append(GTK_BOX(self), self->history_preview);
  self->feedback = gtk_label_new("Choose a local database. Stored records are not encrypted.");
  gtk_widget_set_name(self->feedback, "form-record-feedback");
  gtk_label_set_wrap(GTK_LABEL(self->feedback), TRUE);
  gtk_box_append(GTK_BOX(self), self->feedback);
  *out = GTK_WIDGET(self);
  return UMI_STATUS_OK;
}
