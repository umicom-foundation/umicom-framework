/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_line_sort.c
 *
 * PURPOSE:
 *   Exercise ordinal selected-line ordering, exact separators, bounds and immutable source
 * ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/line_sort.h"
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
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *cases[] = {
                 "ascending",  "descending", "partial",   "end-at-start", "reverse-selection",
                 "no-final",   "crlf",       "mixed",     "blank",        "unicode",
                 "case",       "duplicate",  "unchanged", "one-line",     "empty-selection",
                 "bare-cr",    "utf8",       "boundary",  "nul",          "limit",
                 "line-limit", "cancel",     "owned",     "arguments"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = 1;
  CHECK(known);
  const char *source = "b\na\n", *expected = "a\nb\n";
  size_t first = 0U, last = 4U, selected = 4U, start = 0U;
  int descending = 0;
  UmiStatus wanted = UMI_STATUS_OK;
  char *large = NULL;
  if (strcmp(name, "descending") == 0) {
    source = "a\nb\n";
    expected = "b\na\n";
    descending = 1;
  }
  if (strcmp(name, "partial") == 0) {
    source = "z\nbee\nant\ny\n";
    expected = "z\nant\nbee\ny\n";
    first = 3U;
    last = 8U;
    start = 2U;
    selected = 8U;
  }
  if (strcmp(name, "end-at-start") == 0) {
    source = "z\nb\na\n";
    expected = "b\nz\na\n";
  }
  if (strcmp(name, "reverse-selection") == 0) {
    first = 4U;
    last = 0U;
  }
  if (strcmp(name, "no-final") == 0) {
    source = "b\na";
    expected = "a\nb";
    last = selected = 3U;
  }
  if (strcmp(name, "crlf") == 0) {
    source = "b\r\na\r\n";
    expected = "a\r\nb\r\n";
    last = selected = 6U;
  }
  if (strcmp(name, "mixed") == 0) {
    source = "b\r\na\n";
    expected = "a\r\nb\n";
    last = selected = 5U;
  }
  if (strcmp(name, "blank") == 0) {
    source = "b\n\nA\n";
    expected = "\nA\nb\n";
    last = selected = 5U;
  }
  if (strcmp(name, "unicode") == 0) {
    source = "\xc3\xa9\n\xe9\x9b\xaa\na\n";
    expected = "a\n\xc3\xa9\n\xe9\x9b\xaa\n";
    last = selected = strlen(source);
  }
  if (strcmp(name, "case") == 0) {
    source = "a\nA\n";
    expected = "A\na\n";
  }
  if (strcmp(name, "duplicate") == 0) {
    source = "b\na\na\n";
    expected = "a\na\nb\n";
    last = selected = 6U;
  }
  if (strcmp(name, "unchanged") == 0) {
    source = expected = "a\nb\n";
    first = 1U;
    last = 3U;
    start = 1U;
    selected = 2U;
  }
  if (strcmp(name, "one-line") == 0) {
    source = expected = "alpha";
    first = 1U;
    last = 3U;
    start = 1U;
    selected = 2U;
  }
  if (strcmp(name, "empty-selection") == 0) {
    last = 0U;
    wanted = UMI_STATUS_NOT_FOUND;
  }
  if (strcmp(name, "bare-cr") == 0) {
    source = "b\ra\r";
    wanted = UMI_STATUS_NOT_IMPLEMENTED;
  }
  /* Malformed Unicode is a parse failure; a split scalar selection is an invalid argument. */
#if 0
    if(strcmp(name,"utf8")==0){source="\xc0\xaf\nx";wanted=UMI_STATUS_INVALID_ARGUMENT;}
#endif
  if (strcmp(name, "utf8") == 0) {
    source = "\xc0\xaf\nx";
    wanted = UMI_STATUS_PARSE_ERROR;
  }
  if (strcmp(name, "boundary") == 0) {
    source = "\xc3\xa9\na\n";
    first = 1U;
    last = 5U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  size_t bytes = strlen(source);
  if (strcmp(name, "nul") == 0) {
    source = "b\0a\n";
    bytes = 4U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "limit") == 0) {
    bytes = UMI_EDITOR_LINE_SORT_TEXT_LIMIT + 1U;
    wanted = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  if (strcmp(name, "line-limit") == 0) {
    bytes = 2U * (UMI_EDITOR_LINE_SORT_LINE_LIMIT + 1U);
    large = malloc(bytes + 1U);
    CHECK(large != NULL);
    for (size_t i = 0U; i < bytes; i += 2U) {
      large[i] = 'x';
      large[i + 1U] = '\n';
    }
    large[bytes] = '\0';
    source = large;
    last = bytes;
    wanted = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  char owned[] = "b\na\n";
  if (strcmp(name, "owned") == 0)
    source = owned;
  UmiCancellationToken *cancel = NULL;
  CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
  if (strcmp(name, "cancel") == 0) {
    umi_cancellation_token_request(cancel);
    wanted = UMI_STATUS_CANCELLED;
  }
  UmiEditorLineSortPlan *plan = NULL;
  if (strcmp(name, "arguments") == 0) {
    CHECK(UmiEditorLineSortPlanCreate(NULL, 0U, 0U, 0U, 0, NULL, &plan) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorLineSortPlanCreate(source, bytes, 0U, bytes, 2, NULL, &plan) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorLineSortPlanCreate(source, bytes, 0U, bytes + 1U, 0, NULL, &plan) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorLineSortPlanCreate(source, bytes, 0U, bytes, 0, NULL, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    umi_cancellation_token_destroy(cancel);
    return 0;
  }
  CHECK(UmiEditorLineSortPlanCreate(source, bytes, first, last, descending, cancel, &plan) ==
        wanted);
  if (wanted == UMI_STATUS_OK) {
    UmiEditorLineSortSummary summary;
    const char *text = NULL;
    size_t length = 0U;
    CHECK(UmiEditorLineSortPlanInspect(plan, &summary) == UMI_STATUS_OK);
    CHECK(UmiEditorLineSortPlanRead(plan, &text, &length) == UMI_STATUS_OK);
    if (strcmp(name, "owned") == 0)
      memset(owned, 'x', sizeof owned - 1U);
    CHECK(length == strlen(expected) && memcmp(text, expected, length) == 0);
    CHECK(summary.selection_start == start && summary.selection_bytes == selected);
    CHECK(summary.changed == (strcmp(name, "unchanged") != 0 && strcmp(name, "one-line") != 0));
  } else
    CHECK(plan == NULL);
  UmiEditorLineSortPlanDestroy(plan);
  umi_cancellation_token_destroy(cancel);
  free(large);
  return 0;
}
