/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_newline_gtk4.c
 *
 * PURPOSE:
 *   Exercise real Enter controllers, named draft ownership, native presentation and Undo.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "native_edit_fixture.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  const char *cases[] = {"enter",     "keypad",    "plain",
                         "crlf",      "selection", "unicode",
                         "readonly",  "retired",   "completion-close",
                         "other-tab", "ctrl",      "meta",
                         "caps-lock", "canonical", "plain-canonical"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0, readonly = !strcmp(name, "readonly");
  NativeEditFixture f = {0};
  const char *source = "  ab", *expected = "  ab\n  ";
  size_t cursor = 4U, selection = 0U;
  guint key = GDK_KEY_Return;
  GdkModifierType modifiers = 0;
  if (!strcmp(name, "keypad"))
    key = GDK_KEY_KP_Enter;
  if (!strcmp(name, "plain") || !strcmp(name, "plain-canonical")) {
    modifiers = GDK_SHIFT_MASK;
    expected = "  ab\n";
  }
  if (!strcmp(name, "crlf")) {
    source = "  ab\r\nx";
    expected = "  ab\r\n  \r\nx";
  }
  if (!strcmp(name, "selection")) {
    source = "  abcdef";
    cursor = 3U;
    selection = 3U;
    expected = "  a\n  f";
  }
  if (!strcmp(name, "unicode")) {
    source = "  \xc3\xa9";
    expected = "  \xc3\xa9\n  ";
  }
  if (!strcmp(name, "ctrl"))
    modifiers = GDK_CONTROL_MASK;
  if (!strcmp(name, "meta"))
    modifiers = GDK_META_MASK;
  if (!strcmp(name, "caps-lock"))
    modifiers = GDK_LOCK_MASK;
  CHECK(NativeEditStart(&f, source, cursor, selection, readonly));
  const int ignored = !strcmp(name, "ctrl") || !strcmp(name, "meta") || !strcmp(name, "retired");
  UmiDocumentId other = 0U;
  if (!strcmp(name, "other-tab")) {
    char view[UMI_UI_ID_CAPACITY];
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_new(f.documents, "other.c", view, sizeof view) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK);
    other = active.document_id;
  }
  if (!strcmp(name, "retired")) {
    umi_gtk4_adapter_destroy(f.adapter);
    f.adapter = NULL;
  }
  f.retire_on_completion = !strcmp(name, "completion-close");
  if (!strcmp(name, "canonical") || !strcmp(name, "plain-canonical")) {
    const char *id = !strcmp(name, "canonical") ? UMI_DOCUMENT_COMMAND_INSERT_NEWLINE
                                                : UMI_DOCUMENT_COMMAND_INSERT_PLAIN_NEWLINE;
    CHECK(UmiGtk4AdapterDocumentCommandEnabled(f.adapter, id));
    CHECK(UmiGtk4AdapterDocumentCommand(f.adapter, id) == UMI_STATUS_OK);
  } else {
    if (readonly)
      CHECK(!UmiGtk4AdapterDocumentCommandEnabled(f.adapter, UMI_DOCUMENT_COMMAND_INSERT_NEWLINE));
    gboolean handled = FALSE;
    g_signal_emit_by_name(f.keys, "key-pressed", key, 0U, modifiers, &handled);
    CHECK(handled == !ignored);
  }
  CHECK(f.calls == (ignored ? 0U : 1U));
  if (!ignored)
    CHECK(f.status == (readonly ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_OK));
  int changed = !ignored && !readonly;
  CHECK(NativeEditText(&f, changed ? expected : source));
  if (f.adapter != NULL && !other) {
    GtkWidget *visible = NativeEditFind(umi_gtk4_adapter_native_window(f.adapter));
    CHECK(visible != NULL);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(visible));
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    char *text = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
    int same = !strcmp(text, changed ? expected : source);
    g_free(text);
    CHECK(same);
  }
  if (other) {
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &active) == UMI_STATUS_OK &&
          active.document_id == other);
  }
  if (changed) {
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
          NativeEditText(&f, source));
    CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK &&
          NativeEditText(&f, expected));
  }
cleanup:
  NativeEditStop(&f);
  return failed;
}
