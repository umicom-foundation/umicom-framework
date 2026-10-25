/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_case_engine.c
 *
 * PURPOSE:
 *   Check the existing engine entry points preserve high bytes and reversible history independently
 * of the document facade.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/edit_engine.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                   \
  do {                                                                                             \
    if (!(c)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  int lower = strcmp(name, "lowercase") == 0, high = strcmp(name, "high-bytes") == 0,
      reverse = strcmp(name, "reverse") == 0;
  CHECK(lower || high || reverse || strcmp(name, "uppercase") == 0);
  char source[256], expected[256];
  size_t bytes = 5U;
  memcpy(source, "aB\xc3\xa9!", bytes);
  memcpy(expected, lower ? "ab\xc3\xa9!" : "AB\xc3\xa9!", bytes);
  if (high) {
    bytes = 128U;
    for (size_t i = 0U; i < bytes; ++i)
      source[i] = expected[i] = (char)(i + 128U);
  }
  UmiEditorEditEngine *engine = NULL;
  UmiEditorTextBufferView view;
  UmiEditorEditCommandRequest request;
  UmiEditorEditCommandResult result;
  CHECK(umi_editor_edit_engine_create(NULL, &engine) == UMI_STATUS_OK);
  CHECK(umi_editor_edit_engine_load(engine, source, bytes) == UMI_STATUS_OK);
  CHECK(umi_editor_edit_command_request_initialize(
            &request,
            lower ? UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE
                  : UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE,
            0U) == UMI_STATUS_OK);
  request.selection_start = reverse ? bytes : 0U;
  request.selection_end = reverse ? 0U : bytes;
  CHECK(umi_editor_edit_engine_execute(engine, &request, &result) == UMI_STATUS_OK);
  CHECK(umi_editor_edit_engine_view(engine, &view) == UMI_STATUS_OK && view.byte_count == bytes &&
        memcmp(view.bytes, expected, bytes) == 0);
  CHECK(result.changed == !high);
  if (!high) {
    CHECK(umi_editor_edit_engine_undo(engine, &result) == UMI_STATUS_OK);
    CHECK(umi_editor_edit_engine_view(engine, &view) == UMI_STATUS_OK &&
          memcmp(view.bytes, source, bytes) == 0);
    CHECK(umi_editor_edit_engine_redo(engine, &result) == UMI_STATUS_OK);
    CHECK(umi_editor_edit_engine_view(engine, &view) == UMI_STATUS_OK &&
          memcmp(view.bytes, expected, bytes) == 0);
  }
  umi_editor_edit_engine_destroy(engine);
  return 0;
}
