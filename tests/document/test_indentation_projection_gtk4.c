/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_indentation_projection_gtk4.c
 *
 * PURPOSE:
 *   Verify per-document native tab display, key input and callback-driven retirement or policy
 * changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "native_edit_fixture.h"
#include "umicom/document/indentation_override.h"
#include "umicom/ui/gtk4/editor_indentation.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
typedef struct Observer {
  NativeEditFixture *fixture;
  const char *mode;
  UmiDocumentId other;
  UmiStatus status;
  int called;
} Observer;
static void Changed(GObject *object, GParamSpec *spec, gpointer data) {
  (void)object;
  (void)spec;
  Observer *o = data;
  if (o->called++)
    return;
  if (!strcmp(o->mode, "retire")) {
    umi_gtk4_adapter_destroy(o->fixture->adapter);
    o->fixture->adapter = NULL;
  } else if (!strcmp(o->mode, "default-observer")) {
    UmiEditorIndentation next = {8U, 0};
    o->status = UmiDocumentCoordinatorSetIndentation(o->fixture->documents, &next);
  } else if (!strcmp(o->mode, "other-document-observer")) {
    UmiDocumentIndentationReview review;
    UmiEditorIndentation next = {8U, 0};
    o->status = UmiDocumentCoordinatorInspectIndentation(o->fixture->documents, o->other, &review);
    if (o->status == UMI_STATUS_OK)
      o->status =
          UmiDocumentCoordinatorApplyIndentation(o->fixture->documents, &review, &next, NULL);
  } else if (!strcmp(o->mode, "close-observer"))
    o->status = UmiDocumentCoordinatorClose(o->fixture->documents, o->fixture->id, 1);
  else if (!strcmp(o->mode, "rebind-observer"))
    o->status = UmiGtk4AdapterBindDocumentEditing(o->fixture->adapter, NULL, NULL, NULL);
}
static int Width(GtkWidget *view, unsigned expected, int spaces) {
  if (g_object_class_find_property(G_OBJECT_GET_CLASS(view), "tab-width")) {
    guint width = 0U;
    gboolean insert = FALSE;
    g_object_get(view, "tab-width", &width, "insert-spaces-instead-of-tabs", &insert, NULL);
    return width == expected && !!insert == !!spaces;
  }
  char blank[17];
  memset(blank, ' ', expected);
  blank[expected] = 0;
  PangoLayout *layout = gtk_widget_create_pango_layout(view, blank);
  int expectedPixels = 0;
  pango_layout_get_pixel_size(layout, &expectedPixels, NULL);
  g_object_unref(layout);
  if (expectedPixels < 1)
    expectedPixels = 1;
  PangoTabArray *tabs = gtk_text_view_get_tabs(GTK_TEXT_VIEW(view));
  int pixels = 0, ok = tabs != NULL && pango_tab_array_get_size(tabs) == 1 &&
                       pango_tab_array_get_positions_in_pixels(tabs);
  PangoTabAlign align = PANGO_TAB_LEFT;
  if (ok)
    pango_tab_array_get_tab(tabs, 0, &align, &pixels);
  if (tabs)
    pango_tab_array_free(tabs);
  return ok && align == PANGO_TAB_LEFT && pixels == expectedPixels;
}
static GtkWidget *FindPeer(GtkWidget *root, GtkWidget *first) {
  if (GTK_IS_TEXT_VIEW(root) && root != first && gtk_widget_has_css_class(root, "umicom-editor"))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = FindPeer(child, first);
    if (found)
      return found;
  }
  return NULL;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *names[] = {
                 "override",       "inherit",         "two-documents",    "tab",
                 "selection",      "retire",          "default-observer", "other-document-observer",
                 "close-observer", "rebind-observer", "arguments"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  NativeEditFixture f = {0};
  int failed = 0;
  GtkWidget *peer = NULL;
  gulong signal = 0;
  Observer observer = {.fixture = &f, .mode = name};
  CHECK(NativeEditStart(&f, "ab\nc", 2U, !strcmp(name, "selection") ? 2U : 0U, 0));
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4AdapterRefreshIndentation(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (!strcmp(name, "two-documents") || !strcmp(name, "other-document-observer")) {
    char other[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_new(f.documents, "peer.c", other, sizeof other) ==
          UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &snapshot) == UMI_STATUS_OK);
    observer.other = snapshot.document_id;
    CHECK(umi_gtk4_adapter_refresh(f.adapter) == UMI_STATUS_OK);
    peer = FindPeer(umi_gtk4_adapter_native_window(f.adapter), f.editor);
    CHECK(peer != NULL);
    g_object_ref(peer);
  }
  UmiDocumentIndentationReview review;
  UmiEditorIndentation policy = {2U, 0};
  CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &review) == UMI_STATUS_OK);
  CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &review, &policy, &review) ==
        UMI_STATUS_OK);
  if (strstr(name, "observer") || !strcmp(name, "retire")) {
    const char *property = g_object_class_find_property(G_OBJECT_GET_CLASS(f.editor), "tab-width")
                               ? "notify::tab-width"
                               : "notify::tabs";
    signal = g_signal_connect(f.editor, property, G_CALLBACK(Changed), &observer);
  }
  UmiStatus status = UmiGtk4AdapterRefreshIndentation(f.adapter);
  if (!strcmp(name, "retire") || !strcmp(name, "rebind-observer"))
    CHECK(status == UMI_STATUS_CANCELLED && observer.called > 0);
  else if (!strcmp(name, "close-observer"))
    CHECK(status == UMI_STATUS_NOT_FOUND && observer.called > 0 &&
          observer.status == UMI_STATUS_OK);
  else if (strstr(name, "observer"))
    CHECK(status == UMI_STATUS_INVALID_STATE && observer.called > 0 &&
          observer.status == UMI_STATUS_OK);
  else {
    CHECK(status == UMI_STATUS_OK && Width(f.editor, 2U, 0));
    if (peer)
      CHECK(Width(peer, 4U, 1));
    if (!strcmp(name, "inherit")) {
      CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &review, NULL, NULL) ==
            UMI_STATUS_OK);
      CHECK(UmiGtk4AdapterRefreshIndentation(f.adapter) == UMI_STATUS_OK && Width(f.editor, 4U, 1));
    }
    if (!strcmp(name, "tab") || !strcmp(name, "selection")) {
      gboolean handled = FALSE;
      g_signal_emit_by_name(f.keys, "key-pressed", GDK_KEY_Tab, 0U, 0, &handled);
      CHECK(handled && f.status == UMI_STATUS_OK);
      CHECK(NativeEditText(&f, !strcmp(name, "selection") ? "\tab\n\tc" : "ab\t\nc"));
    } else
      CHECK(NativeEditText(&f, "ab\nc"));
  }
cleanup:
  if (signal)
    g_signal_handler_disconnect(f.editor, signal);
  g_clear_object(&peer);
  NativeEditStop(&f);
  return failed;
}
