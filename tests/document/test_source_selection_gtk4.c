/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_selection_gtk4.c
 *
 * PURPOSE:
 *   Exercise source selection through native editing completion and retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "native_edit_fixture.h"
#include "umicom/ui/gtk4/source_selection.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                              \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1], *cases[] = {"word",     "line",    "line-ending",       "readonly",
                                          "pending",  "no-word", "completion-retire", "unbound",
                                          "arguments"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    known |= !strcmp(name, cases[i]);
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  NativeEditFixture f = {0};
  const char *source = !strcmp(name, "no-word") ? "  " : "alpha beta\r\nnext";
  CHECK(NativeEditStart(&f, source, 1U, 0U, !strcmp(name, "readonly")));
  UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
  UmiUiDocumentViewSnapshot view;
  CHECK(umi_ui_document_view_model_find(views, f.view, &view) == UMI_STATUS_OK);
  view.cursor_offset = 1U;
  view.selection_length = 0U;
  if (!strcmp(name, "pending")) {
    source = "typed draft";
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, source, strlen(source)) == UMI_STATUS_OK);
  } else
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
  UmiEditorSourceSelectionKind kind = !strcmp(name, "line") ? UMI_EDITOR_SELECT_LINE_CONTENT
                                      : !strcmp(name, "line-ending")
                                          ? UMI_EDITOR_SELECT_LINE_WITH_ENDING
                                          : UMI_EDITOR_SELECT_SOURCE_WORD;
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4AdapterSelectSourceUnit(NULL, kind) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4AdapterSelectSourceUnit(f.adapter, (UmiEditorSourceSelectionKind)99) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(f.calls == 0U);
    goto cleanup;
  }
  if (!strcmp(name, "unbound")) {
    CHECK(UmiGtk4AdapterBindDocumentEditing(f.adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterSelectSourceUnit(f.adapter, kind) == UMI_STATUS_UNAVAILABLE &&
          f.calls == 0U);
    goto cleanup;
  }
  f.retire_on_completion = !strcmp(name, "completion-retire");
  UmiStatus status = UmiGtk4AdapterSelectSourceUnit(f.adapter, kind);
  if (!strcmp(name, "no-word"))
    CHECK(status == UMI_STATUS_NOT_FOUND);
  else
    CHECK(status == UMI_STATUS_OK);
  CHECK(f.calls == 1U && f.status == status && NativeEditText(&f, source));
  CHECK(umi_ui_document_view_model_find(views, f.view, &view) == UMI_STATUS_OK);
  CHECK(view.cursor_offset == (!strcmp(name, "no-word") ? 1U : 0U));
  CHECK(view.selection_length == (!strcmp(name, "no-word")       ? 0U
                                  : !strcmp(name, "line")        ? 10U
                                  : !strcmp(name, "line-ending") ? 12U
                                                                 : 5U));
  if (!strcmp(name, "completion-retire"))
    CHECK(f.adapter == NULL);
cleanup:
  NativeEditStop(&f);
  return failed;
}
