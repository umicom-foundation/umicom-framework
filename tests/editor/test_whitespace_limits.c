/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_whitespace_limits.c
 *
 * PURPOSE:
 *   Exercise malformed text, capacity limits and cancellation before whitespace publication.
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
(const char *source, size_t cursor, size_t selected, UmiEditorWhitespaceOptions options,
 const char *expected, size_t mapped_cursor, size_t mapped_selected) {
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
  const char *source = "x \n";
  size_t bytes = 3U, cursor = 0U, selected = 0U;
  UmiEditorWhitespaceOptions options = {UMI_EDITOR_WHITESPACE_TRIM_TRAILING, 4U, 0};
  UmiEditorWhitespacePlan *plan = (void *)1;
  UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
  UmiCancellationToken *cancel = NULL;
  char *allocated = NULL;
  if (!strcmp(name, "invalid-utf8")) {
    source = "\xc0\xaf";
    bytes = 2U;
  } else if (!strcmp(name, "embedded-nul")) {
    source = "x\0z";
    bytes = 3U;
  } else if (!strcmp(name, "split-scalar")) {
    source = "\xc3\xa9";
    bytes = 2U;
    cursor = 1U;
  } else if (!strcmp(name, "split-crlf")) {
    source = "x\r\n";
    cursor = 2U;
  } else if (!strcmp(name, "cursor-range"))
    cursor = 4U;
  else if (!strcmp(name, "selection-range"))
    selected = 4U;
  else if (!strcmp(name, "invalid-operation"))
    options.operation = (UmiEditorWhitespaceOperation)99;
  else if (!strcmp(name, "invalid-width"))
    options.tab_width = 0U;
  else if (!strcmp(name, "invalid-scope"))
    options.selected_lines = 2;
  else if (!strcmp(name, "cancel")) {
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    umi_cancellation_token_request(cancel);
    expected = UMI_STATUS_CANCELLED;
  } else if (!strcmp(name, "input-limit")) {
    bytes = UMI_EDITOR_WHITESPACE_TEXT_LIMIT + 1U;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(name, "output-limit")) {
    bytes = UMI_EDITOR_WHITESPACE_TEXT_LIMIT / 16U + 1U;
    allocated = malloc(bytes);
    CHECK(allocated);
    memset(allocated, '\t', bytes);
    source = allocated;
    options.operation = UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS;
    options.tab_width = 16U;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  } else if (!strcmp(name, "exact-limit")) {
    bytes = UMI_EDITOR_WHITESPACE_TEXT_LIMIT;
    allocated = malloc(bytes);
    CHECK(allocated);
    memset(allocated, 'x', bytes);
    source = allocated;
    expected = UMI_STATUS_OK;
  } else if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorWhitespacePlanCreate(NULL, 0U, 0U, 0U, &options, NULL, &plan) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          plan == NULL);
    CHECK(UmiEditorWhitespacePlanCreate("", 0U, 0U, 0U, NULL, NULL, &plan) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          plan == NULL);
    CHECK(UmiEditorWhitespacePlanCreate("", 0U, 0U, 0U, &options, NULL, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  } else if (!strcmp(name, "failed-output")) {
    const char *text = (void *)1;
    size_t n = 99U;
    UmiEditorWhitespaceSummary summary = {0};
    summary.changed = 99;
    CHECK(UmiEditorWhitespacePlanRead(NULL, &text, &n) == UMI_STATUS_INVALID_ARGUMENT &&
          text == NULL && n == 0U);
    CHECK(UmiEditorWhitespacePlanInspect(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT &&
          summary.changed == 99);
    return 0;
  } else
    return 2;
  UmiStatus status =
      UmiEditorWhitespacePlanCreate(source, bytes, cursor, selected, &options, cancel, &plan);
  if (!strcmp(name, "invalid-utf8") || !strcmp(name, "split-scalar") || !strcmp(name, "split-crlf"))
    CHECK(status != UMI_STATUS_OK);
  else
    CHECK(status == expected);
  CHECK((status == UMI_STATUS_OK) == (plan != NULL));
  UmiEditorWhitespacePlanDestroy(plan);
  umi_cancellation_token_destroy(cancel);
  free(allocated);
  return 0;
}
