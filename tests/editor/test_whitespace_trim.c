/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_whitespace_trim.c
 *
 * PURPOSE:
 *   Verify exact trailing whitespace removal, selected-line boundaries and owned output.
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
  UmiEditorWhitespaceOptions options = {UMI_EDITOR_WHITESPACE_TRIM_TRAILING, 4U, 0};
  const char *source = NULL, *expected = NULL;
  size_t cursor = 0U, selected = 0U, mapped = 0U, mapped_selected = 0U;
  if (!strcmp(name, "mixed-endings")) {
    source = "a \r\nb\t\rc  \n";
    expected = "a\r\nb\rc\n";
  } else if (!strcmp(name, "no-final")) {
    source = "a \t";
    expected = "a";
  } else if (!strcmp(name, "only-whitespace")) {
    source = " \t\n \t";
    expected = "\n";
  } else if (!strcmp(name, "empty")) {
    source = "";
    expected = "";
  } else if (!strcmp(name, "unicode")) {
    source = "\xc3\xa9 \t\n";
    expected = "\xc3\xa9\n";
    cursor = 2U;
    mapped = 2U;
  } else if (!strcmp(name, "non-ascii-space")) {
    source = "a\xc2\xa0 \n";
    expected = "a\xc2\xa0\n";
  } else if (!strcmp(name, "interior-space")) {
    source = "a \t b\n";
    expected = source;
  } else if (!strcmp(name, "caret-trimmed")) {
    source = "a \t\n";
    expected = "a\n";
    cursor = 2U;
    mapped = 1U;
  } else if (!strcmp(name, "selection-trimmed")) {
    source = "a \t\nb \n";
    expected = "a\nb\n";
    cursor = 1U;
    selected = 5U;
    mapped = 1U;
    mapped_selected = 2U;
  } else if (!strcmp(name, "selected-lines")) {
    source = "a \nb \nc \n";
    expected = "a \nb\nc \n";
    cursor = 3U;
    selected = 1U;
    mapped = 3U;
    mapped_selected = 1U;
    options.selected_lines = 1;
  } else if (!strcmp(name, "end-at-start")) {
    source = "a \nb \n";
    expected = "a\nb \n";
    selected = 3U;
    mapped_selected = 2U;
    options.selected_lines = 1;
  } else if (!strcmp(name, "caret-line")) {
    source = "a \nb \n";
    expected = "a \nb\n";
    cursor = 4U;
    mapped = 4U;
    options.selected_lines = 1;
  } else if (!strcmp(name, "eof-line")) {
    source = "a \nb \t";
    expected = "a \nb";
    cursor = 6U;
    mapped = 4U;
    options.selected_lines = 1;
  } else if (!strcmp(name, "terminal-empty-line")) {
    source = "a \n";
    expected = source;
    cursor = 3U;
    mapped = 3U;
    options.selected_lines = 1;
  } else if (!strcmp(name, "unchanged")) {
    source = "a\nb";
    expected = source;
    cursor = 1U;
    mapped = 1U;
  } else if (!strcmp(name, "owned-output")) {
    char input[] = "a \n";
    UmiEditorWhitespacePlan *p = NULL;
    CHECK(UmiEditorWhitespacePlanCreate(input, 3U, 0U, 0U, &options, NULL, &p) == UMI_STATUS_OK);
    input[0] = 'z';
    const char *text;
    size_t n;
    CHECK(UmiEditorWhitespacePlanRead(p, &text, &n) == UMI_STATUS_OK && n == 2U &&
          !strcmp(text, "a\n"));
    UmiEditorWhitespacePlanDestroy(p);
    return 0;
  } else
    return 2;
  return CheckPlan(source, cursor, selected, options, expected, mapped, mapped_selected);
}
