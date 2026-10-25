/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_line_sort_engine.c
 *
 * PURPOSE:
 *   Qualify sorting through the existing editing engine and immutable line-edit facade.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/edit_engine.h"
#include "umicom/editor/line_edit_plan.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  CHECK(strcmp(name, "engine") == 0 || strcmp(name, "plan") == 0 || strcmp(name, "empty") == 0 ||
        strcmp(name, "selection") == 0);
  const char *source = "z\nbee\nant\ny\n", *expected = "z\nant\nbee\ny\n";
  UmiEditorEditCommandRequest request;
  CHECK(umi_editor_edit_command_request_initialize(
            &request, UMI_EDITOR_EDIT_COMMAND_SORT_LINES_ASCENDING, 3U) == UMI_STATUS_OK);
  request.selection_start = 3U;
  request.selection_end = 8U;
  /* Reverse endpoints exercise normalization separately from the forward plan. */
  if (strcmp(name, "selection") == 0) {
    request.selection_start = 8U;
    request.selection_end = 3U;
  }
  if (strcmp(name, "empty") == 0)
    request.selection_end = 3U;
  UmiStatus wanted = strcmp(name, "empty") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK;
  if (strcmp(name, "engine") == 0) {
    UmiEditorEditEngine *engine = NULL;
    UmiEditorEditCommandResult result;
    UmiEditorTextBufferView view;
    CHECK(umi_editor_edit_engine_create(NULL, &engine) == UMI_STATUS_OK);
    CHECK(umi_editor_edit_engine_load(engine, source, strlen(source)) == UMI_STATUS_OK);
    CHECK(umi_editor_edit_engine_execute(engine, &request, &result) == UMI_STATUS_OK &&
          result.changed);
    CHECK(umi_editor_edit_engine_view(engine, &view) == UMI_STATUS_OK &&
          view.byte_count == strlen(expected) &&
          memcmp(view.bytes, expected, view.byte_count) == 0);
    CHECK(umi_editor_edit_engine_undo(engine, &result) == UMI_STATUS_OK);
    CHECK(umi_editor_edit_engine_view(engine, &view) == UMI_STATUS_OK &&
          memcmp(view.bytes, source, view.byte_count) == 0);
    umi_editor_edit_engine_destroy(engine);
  } else {
    UmiEditorLineEditPlan *plan = NULL;
    CHECK(UmiEditorLineEditPlanCreate(source, strlen(source), &request, NULL, &plan) == wanted);
    if (wanted == UMI_STATUS_OK) {
      const char *text = NULL;
      size_t bytes = 0U;
      UmiEditorLineEditSummary summary;
      CHECK(UmiEditorLineEditPlanRead(plan, &text, &bytes) == UMI_STATUS_OK &&
            bytes == strlen(expected) && memcmp(text, expected, bytes) == 0);
      CHECK(UmiEditorLineEditPlanInspect(plan, &summary) == UMI_STATUS_OK &&
            summary.cursor_offset == 2U && summary.selection_bytes == 8U &&
            summary.affected_line_count == 2U);
    } else
      CHECK(plan == NULL);
    UmiEditorLineEditPlanDestroy(plan);
  }
  return 0;
}
