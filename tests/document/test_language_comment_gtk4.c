/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_language_comment_gtk4.c
 *
 * PURPOSE:
 *   Check language-specific native comment dispatch, refusals, Undo and completion retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "native_edit_fixture.h"
#include "umicom/document/language_comment.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *names[] = {"c",        "python",   "sql",           "unknown",
                         "readonly", "selected", "inactive-view", "completion-retire",
                         "legacy",   "unbound"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  NativeEditFixture f = {0};
  CHECK(NativeEditStart(&f, "a\nb", 0U, !strcmp(name, "selected") ? 3U : 0U,
                        !strcmp(name, "readonly")));
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.view, &view) == UMI_STATUS_OK);
  const char *language = !strcmp(name, "python") || !strcmp(name, "legacy") ? "python3"
                         : !strcmp(name, "sql")                             ? "sql"
                         : !strcmp(name, "unknown")                         ? "json"
                                                                            : "c";
  (void)snprintf(view.language_id, sizeof view.language_id, "%s", language);
  CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  const char *command = !strcmp(name, "legacy") ? UMI_DOCUMENT_COMMAND_TOGGLE_LINE_COMMENT
                                                : UMI_DOCUMENT_COMMAND_TOGGLE_LANGUAGE_COMMENT;
  const char *expected = !strcmp(name, "python")     ? "# a\nb"
                         : !strcmp(name, "sql")      ? "-- a\nb"
                         : !strcmp(name, "selected") ? "// a\n// b"
                                                     : "// a\nb";
  UmiStatus wanted = UMI_STATUS_OK;
  if (!strcmp(name, "readonly"))
    wanted = UMI_STATUS_PERMISSION_DENIED;
  if (!strcmp(name, "unknown"))
    wanted = UMI_STATUS_NOT_FOUND;
  if (!strcmp(name, "unbound")) {
    CHECK(UmiGtk4AdapterBindDocumentEditing(f.adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    wanted = UMI_STATUS_UNAVAILABLE;
  }
  if (!strcmp(name, "inactive-view")) {
    char other[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(f.documents, "other.py", other, sizeof other) ==
          UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, other, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "x", 1U) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterDocumentCommand(f.adapter, command) == UMI_STATUS_OK);
    CHECK(NativeEditText(&f, "a\nb"));
    char *text = NULL;
    size_t bytes = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(views, other, &text, &bytes) == UMI_STATUS_OK);
    int same = bytes == 3U && !strcmp(text, "# x");
    UmiUiDocumentViewModelFreeText(text);
    CHECK(same && f.calls == 1U);
    goto cleanup;
  }
  CHECK(UmiGtk4AdapterDocumentCommandEnabled(f.adapter, command) == (wanted == UMI_STATUS_OK));
  f.retire_on_completion = !strcmp(name, "completion-retire");
  CHECK(UmiGtk4AdapterDocumentCommand(f.adapter, command) == wanted);
  CHECK(f.calls == (!strcmp(name, "unbound") ? 0U : 1U));
  if (f.calls)
    CHECK(f.status == wanted);
  CHECK(NativeEditText(&f, wanted == UMI_STATUS_OK ? expected : "a\nb"));
  if (f.retire_on_completion)
    CHECK(f.adapter == NULL);
  if (wanted == UMI_STATUS_OK) {
    CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK);
    CHECK(NativeEditText(&f, "a\nb"));
  }
cleanup:
  NativeEditStop(&f);
  return failed;
}
