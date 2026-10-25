/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_whitespace_indentation.c
 *
 * PURPOSE:
 *   Verify leading indentation column preservation and exact caret mapping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/whitespace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static int CheckPlan(const char *source, size_t cursor, size_t selected,
                     UmiEditorWhitespaceOptions options, const char *expected, size_t mapped_cursor,
                     size_t mapped_selected) {
  UmiEditorWhitespacePlan *plan = NULL;
  CHECK(UmiEditorWhitespacePlanCreate(source, strlen(source), cursor, selected, &options, NULL,
                                      &plan) == UMI_STATUS_OK);
  const char *text = NULL;
  size_t bytes = 0U;
  UmiEditorWhitespaceSummary summary;
  CHECK(UmiEditorWhitespacePlanRead(plan, &text, &bytes) == UMI_STATUS_OK);
  CHECK(UmiEditorWhitespacePlanInspect(plan, &summary) == UMI_STATUS_OK);
  CHECK(bytes == strlen(expected) && !strcmp(text, expected));
  CHECK(summary.cursor_offset == mapped_cursor && summary.selection_bytes == mapped_selected);
  CHECK(summary.changed == (strcmp(source, expected) != 0));
  CHECK(summary.source_bytes == strlen(source) && summary.proposed_bytes == bytes);
  UmiEditorWhitespacePlanDestroy(plan);
  return 0;
}

int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  UmiEditorWhitespaceOptions options = {UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS, 4U, 0};
  const char *source = NULL, *expected = NULL;
  size_t cursor = 0U, selected = 0U, mapped = 0U, mapped_selected = 0U;
  if (!strcmp(name, "expand")) {
    source = "\tx\n";
    expected = "    x\n";
  } else if (!strcmp(name, "compact")) {
    source = "      x\n";
    expected = "\t  x\n";
    options.operation = UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES;
  } else if (!strcmp(name, "mixed-prefix")) {
    source = " \t \tx";
    expected = "        x";
  } else if (!strcmp(name, "interior-tabs")) {
    source = "x\ty";
    expected = source;
  } else if (!strcmp(name, "blank-prefix")) {
    source = "\t \n";
    expected = "     \n";
  } else if (!strcmp(name, "unicode")) {
    source = "\t\xc3\xa9";
    expected = "    \xc3\xa9";
    cursor = 3U;
    mapped = 6U;
  } else if (!strcmp(name, "mixed-endings")) {
    source = "\ta\r\n\tb\r\tc\n";
    expected = "    a\r\n    b\r    c\n";
  } else if (!strcmp(name, "selected-lines")) {
    source = "\ta\n\tb\n\tc";
    expected = "\ta\n    b\n\tc";
    cursor = 3U;
    selected = 1U;
    mapped = 3U;
    mapped_selected = 4U;
    options.selected_lines = 1;
  } else if (!strcmp(name, "end-at-start")) {
    source = "\ta\n\tb";
    expected = "    a\n\tb";
    selected = 3U;
    mapped_selected = 6U;
    options.selected_lines = 1;
  } else if (!strcmp(name, "eof")) {
    source = "\tx";
    expected = "    x";
    cursor = 2U;
    mapped = 5U;
    options.selected_lines = 1;
  } else if (!strcmp(name, "expand-caret")) {
    source = " \t x";
    expected = "     x";
    cursor = 2U;
    mapped = 4U;
  } else if (!strcmp(name, "compact-caret")) {
    source = "      x";
    expected = "\t  x";
    cursor = 3U;
    mapped = 0U;
    options.operation = UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES;
  } else if (!strcmp(name, "selection")) {
    source = "      x";
    expected = "\t  x";
    cursor = 3U;
    selected = 2U;
    mapped = 0U;
    mapped_selected = 2U;
    options.operation = UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES;
  } else if (!strcmp(name, "width-one")) {
    source = "  x";
    expected = "\t\tx";
    options.tab_width = 1U;
    options.operation = UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES;
  } else if (!strcmp(name, "width-sixteen")) {
    source = "\tx";
    expected = "                x";
    options.tab_width = 16U;
  } else if (!strcmp(name, "unchanged")) {
    source = "\t  x";
    expected = source;
    options.operation = UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES;
  } else
    return 2;
  return CheckPlan(source, cursor, selected, options, expected, mapped, mapped_selected);
}
