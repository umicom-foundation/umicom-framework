/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_search_draft_gtk4.c
 *
 * PURPOSE:
 *   Native text and portable drafts share explicit save and recovery boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native text and portable drafts share explicit save and recovery boundaries. */
#include "search_session_fixture.h"
#undef CHECK
#include "umicom/base/sha256.h"
#include "umicom/ui/gtk4/structured_form_search_draft.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static GtkWidget *find(GtkWidget *root, const char *name) {
  if (!root)
    return NULL;
  if (!strcmp(gtk_widget_get_name(root), name))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *match = find(child, name);
    if (match)
      return match;
  }
  return NULL;
}
static void retire_parent(GObject *object, GParamSpec *spec, gpointer context) {
  (void)object;
  (void)spec;
  UmiStructuredFormSearchDraftGtk4Retire(context);
}
int main(int argc, char **argv) {
  const char *cases[] = {"empty",
                         "new",
                         "open",
                         "title",
                         "metadata",
                         "filter",
                         "invalid-filter",
                         "invalid-title",
                         "long-title",
                         "revert",
                         "dirty-open",
                         "dirty-begin",
                         "discard-new",
                         "discard-saved",
                         "discard-button",
                         "retained-control",
                         "retained-discard",
                         "save",
                         "later-edit",
                         "retire-save",
                         "dispose-save",
                         "failed-save",
                         "wrong-token",
                         "capture",
                         "invalid-capture",
                         "pending-capture",
                         "restore",
                         "dirty-restore",
                         "wrong-scope",
                         "utf8-title",
                         "retired",
                         "disposed",
                         "null",
                         "discard-observer-retire",
                         "unclosed-entity"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  SearchFixture *f = NULL;
  GtkWidget *root = NULL, *second = NULL, *retained = NULL, *observed = NULL;
  gulong observer = 0;
  OK(search_fixture_new(2, false, &f));
  OK(UmiStructuredFormSearchDraftGtk4New(&f->form->schema, "parcel.record.", &root));
  g_object_ref_sink(root);
  if (!strcmp(name, "empty")) {
    CHECK(UmiStructuredFormSearchDraftGtk4CanClose(root));
    OK(UmiStructuredFormSearchDraftGtk4State(root, f->view));
    CHECK(!f->view->record.ready);
    goto cleanup;
  }
  if (!strcmp(name, "unclosed-entity")) {
    char entity[64];
    memset(entity, 'x', sizeof entity);
    CHECK(UmiStructuredFormSearchDraftGtk4Begin(root, entity, f->preset) != UMI_STATUS_OK);
    CHECK(UmiStructuredFormSearchDraftGtk4CanClose(root));
    goto cleanup;
  }
  bool is_new = !strcmp(name, "new") || !strcmp(name, "discard-new");
  if (is_new)
    OK(UmiStructuredFormSearchDraftGtk4Begin(root, "form.search.new", f->preset));
  else
    OK(UmiStructuredFormSearchDraftGtk4Open(root, f->document));
  OK(UmiStructuredFormSearchDraftGtk4State(root, f->view));
  *f->before = *f->view;
  if (!strcmp(name, "new")) {
    CHECK(!UmiStructuredFormSearchDraftGtk4CanClose(root) && f->view->record.is_new);
    goto cleanup;
  }
  if (!strcmp(name, "open")) {
    CHECK(UmiStructuredFormSearchDraftGtk4CanClose(root));
    goto cleanup;
  }
  if (!strcmp(name, "null")) {
    CHECK(UmiStructuredFormSearchDraftGtk4Begin(root, NULL, f->preset) != UMI_STATUS_OK);
    CHECK(UmiStructuredFormSearchDraftGtk4Open(root, NULL) != UMI_STATUS_OK);
    CHECK(UmiStructuredFormSearchDraftGtk4Accept(NULL) != UMI_STATUS_OK);
    CHECK(UmiStructuredFormSearchDraftGtk4State(root, NULL) != UMI_STATUS_OK);
    CHECK(UmiStructuredFormSearchDraftGtk4PrepareSave(root, NULL, f->request) != UMI_STATUS_OK);
    CHECK(UmiStructuredFormSearchDraftGtk4Capture(root, NULL) != UMI_STATUS_OK);
    CHECK(UmiStructuredFormSearchDraftGtk4Restore(root, NULL) != UMI_STATUS_OK);
    CHECK(UmiStructuredFormSearchDraftGtk4CanClose(root));
    goto cleanup;
  }
  if (!strcmp(name, "retired") || !strcmp(name, "disposed")) {
    if (!strcmp(name, "disposed"))
      g_object_run_dispose(G_OBJECT(root));
    else
      UmiStructuredFormSearchDraftGtk4Retire(root);
    CHECK(UmiStructuredFormSearchDraftGtk4Accept(root) == UMI_STATUS_INVALID_STATE);
    CHECK(!UmiStructuredFormSearchDraftGtk4CanClose(root));
    OK(UmiStructuredFormSearchDraftGtk4State(root, f->view));
    CHECK(!f->view->record.dirty);
    goto cleanup;
  }
  if (!strcmp(name, "wrong-scope") || !strcmp(name, "utf8-title")) {
    if (!strcmp(name, "wrong-scope"))
      strcpy(f->preset->entity_prefix, "other.record.");
    else
      strcpy(f->preset->title, "\xff");
    OK(umi_structured_form_search_preset_encode(&f->form->schema, f->preset, f->document->content,
                                                sizeof f->document->content, &f->document->length));
    OK(UmiSha256Buffer(f->document->content, f->document->length,
                       f->document->record.amendment.content_sha256));
    CHECK(UmiStructuredFormSearchDraftGtk4Open(root, f->document) != UMI_STATUS_OK);
    OK(UmiStructuredFormSearchDraftGtk4State(root, f->view));
    CHECK(!memcmp(f->view, f->before, sizeof *f->view) &&
          UmiStructuredFormSearchDraftGtk4CanClose(root));
    goto cleanup;
  }
  GtkWidget *title = find(root, "search-draft-title");
  if (!strcmp(name, "metadata"))
    gtk_editable_set_text(GTK_EDITABLE(find(root, "search-draft-metadata")), "New metadata");
  else if (!strcmp(name, "filter") || !strcmp(name, "invalid-filter") ||
           !strcmp(name, "invalid-capture"))
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-filter-value")),
                          !strcmp(name, "filter") ? "8" : "-");
  else if (!strcmp(name, "invalid-title"))
    gtk_editable_set_text(GTK_EDITABLE(title), "");
  else if (!strcmp(name, "long-title")) {
    char long_title[200];
    memset(long_title, 'x', sizeof long_title - 1);
    long_title[sizeof long_title - 1] = 0;
    gtk_editable_set_text(GTK_EDITABLE(title), long_title);
  } else
    gtk_editable_set_text(GTK_EDITABLE(title), "Changed query");
  if (!strcmp(name, "invalid-filter") || !strcmp(name, "invalid-title") ||
      !strcmp(name, "long-title") || !strcmp(name, "invalid-capture")) {
    CHECK(!UmiStructuredFormSearchDraftGtk4CanClose(root));
    CHECK(UmiStructuredFormSearchDraftGtk4Accept(root) != UMI_STATUS_OK);
    OK(UmiStructuredFormSearchDraftGtk4State(root, f->view));
    CHECK(!memcmp(f->view, f->before, sizeof *f->view));
    if (!strcmp(name, "invalid-capture")) {
      memset(f->snapshot, 0x5f, sizeof *f->snapshot);
      UmiRecordDraftSnapshot *before = malloc(sizeof *before);
      CHECK(before);
      *before = *f->snapshot;
      UmiStatus status = UmiStructuredFormSearchDraftGtk4Capture(root, f->snapshot);
      bool same = !memcmp(before, f->snapshot, sizeof *before);
      free(before);
      CHECK(status != UMI_STATUS_OK && same);
    }
    goto cleanup;
  }
  CHECK(!UmiStructuredFormSearchDraftGtk4CanClose(root));
  if (!strcmp(name, "title") || !strcmp(name, "metadata") || !strcmp(name, "filter")) {
    OK(UmiStructuredFormSearchDraftGtk4State(root, f->view));
    CHECK(f->view->record.dirty);
    goto cleanup;
  }
  if (!strcmp(name, "revert")) {
    gtk_editable_set_text(GTK_EDITABLE(title), "Baseline search");
    CHECK(UmiStructuredFormSearchDraftGtk4CanClose(root));
    goto cleanup;
  }
  if (!strcmp(name, "dirty-open")) {
    CHECK(UmiStructuredFormSearchDraftGtk4Open(root, f->document) == UMI_STATUS_INVALID_STATE);
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(title)), "Changed query"));
    goto cleanup;
  }
  if (!strcmp(name, "dirty-begin")) {
    CHECK(UmiStructuredFormSearchDraftGtk4Begin(root, "form.search.other", f->preset) ==
          UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (!strcmp(name, "discard-new") || !strcmp(name, "discard-saved") ||
      !strcmp(name, "discard-button") || !strcmp(name, "retained-control") ||
      !strcmp(name, "retained-discard") || !strcmp(name, "discard-observer-retire")) {
    if (!strcmp(name, "retained-control"))
      retained = g_object_ref(title);
    if (!strcmp(name, "retained-discard"))
      retained = g_object_ref(find(root, "search-draft-discard"));
    if (!strcmp(name, "discard-observer-retire")) {
      observed = g_object_ref(gtk_widget_get_parent(title));
      observer = g_signal_connect(observed, "notify::parent", G_CALLBACK(retire_parent), root);
    }
    if (!strcmp(name, "discard-button"))
      g_signal_emit_by_name(find(root, "search-draft-discard"), "clicked");
    else if (!strcmp(name, "discard-observer-retire")) {
      CHECK(UmiStructuredFormSearchDraftGtk4Discard(root) == UMI_STATUS_CANCELLED);
      OK(UmiStructuredFormSearchDraftGtk4State(root, f->view));
      CHECK(!f->view->record.dirty);
      goto cleanup;
    } else
      OK(UmiStructuredFormSearchDraftGtk4Discard(root));
    CHECK(UmiStructuredFormSearchDraftGtk4CanClose(root));
    if (!strcmp(name, "retained-control")) {
      gtk_editable_set_text(GTK_EDITABLE(retained), "Stale control");
      CHECK(UmiStructuredFormSearchDraftGtk4CanClose(root));
    }
    if (!strcmp(name, "retained-discard")) {
      gtk_editable_set_text(GTK_EDITABLE(find(root, "search-draft-title")), "Keep this edit");
      g_signal_emit_by_name(retained, "clicked");
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "search-draft-title"))),
                    "Keep this edit"));
    }
    goto cleanup;
  }
  if (!strcmp(name, "capture") || !strcmp(name, "restore") || !strcmp(name, "dirty-restore")) {
    OK(UmiStructuredFormSearchDraftGtk4Capture(root, f->snapshot));
    OK(umi_structured_form_search_preset_decode(&f->form->schema, f->snapshot->content,
                                                f->snapshot->length, f->preset));
    CHECK(!strcmp(f->preset->title, "Changed query"));
    if (!strcmp(name, "dirty-restore"))
      CHECK(UmiStructuredFormSearchDraftGtk4Restore(root, f->snapshot) == UMI_STATUS_INVALID_STATE);
    if (!strcmp(name, "restore")) {
      OK(UmiStructuredFormSearchDraftGtk4New(&f->form->schema, "parcel.record.", &second));
      g_object_ref_sink(second);
      OK(umi_record_draft_snapshot_check_store(f->server, f->snapshot));
      OK(UmiStructuredFormSearchDraftGtk4Restore(second, f->snapshot));
      CHECK(!UmiStructuredFormSearchDraftGtk4CanClose(second));
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(second, "search-draft-title"))),
                    "Changed query"));
    }
    goto cleanup;
  }
  OK(UmiStructuredFormSearchDraftGtk4PrepareSave(root, &f->attribution, f->request));
  CHECK(!UmiStructuredFormSearchDraftGtk4CanClose(root));
  if (!strcmp(name, "pending-capture")) {
    CHECK(UmiStructuredFormSearchDraftGtk4Capture(root, f->snapshot) == UMI_STATUS_BUSY);
    CHECK(UmiStructuredFormSearchDraftGtk4FinishSave(
              root, f->request->request_id, UMI_STATUS_IO_ERROR, NULL) == UMI_STATUS_IO_ERROR);
    goto cleanup;
  }
  if (!strcmp(name, "failed-save")) {
    CHECK(UmiStructuredFormSearchDraftGtk4FinishSave(
              root, f->request->request_id, UMI_STATUS_IO_ERROR, NULL) == UMI_STATUS_IO_ERROR);
    CHECK(!UmiStructuredFormSearchDraftGtk4CanClose(root));
    goto cleanup;
  }
  if (!strcmp(name, "later-edit"))
    gtk_editable_set_text(GTK_EDITABLE(title), "Typed while saving");
  if (!strcmp(name, "retire-save"))
    UmiStructuredFormSearchDraftGtk4Retire(root);
  if (!strcmp(name, "dispose-save"))
    g_object_run_dispose(G_OBJECT(root));
  OK(umi_record_save_request_commit(f->server, f->request, &f->saved));
  if (!strcmp(name, "wrong-token"))
    CHECK(UmiStructuredFormSearchDraftGtk4FinishSave(root, f->request->request_id + 1,
                                                     UMI_STATUS_OK, &f->saved) != UMI_STATUS_OK);
  OK(UmiStructuredFormSearchDraftGtk4FinishSave(root, f->request->request_id, UMI_STATUS_OK,
                                                &f->saved));
  OK(UmiStructuredFormSearchDraftGtk4State(root, f->view));
  CHECK(!f->view->record.saving && f->view->record.expected_revision == 2);
  if (!strcmp(name, "later-edit")) {
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(title)), "Typed while saving"));
    CHECK(!UmiStructuredFormSearchDraftGtk4CanClose(root));
  } else if (strcmp(name, "retire-save") && strcmp(name, "dispose-save"))
    CHECK(UmiStructuredFormSearchDraftGtk4CanClose(root));
cleanup:
  if (observer)
    g_signal_handler_disconnect(observed, observer);
  if (observed)
    g_object_unref(observed);
  if (retained)
    g_object_unref(retained);
  if (root) {
    UmiStructuredFormSearchDraftGtk4Retire(root);
    g_object_unref(root);
  }
  if (second) {
    UmiStructuredFormSearchDraftGtk4Retire(second);
    g_object_unref(second);
  }
  search_fixture_free(f);
  return result;
}
