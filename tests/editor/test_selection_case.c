/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_selection_case.c
 *
 * PURPOSE:
 *   Verify byte-preserving ASCII conversion, explicit selection intent and immutable output
 * ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/line_edit_plan.h"
#include <stdio.h>
#include <stdlib.h>
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
  const char *name = argv[1],
             *cases[] = {"uppercase",   "lowercase", "partial", "reverse",   "unicode", "emoji",
                         "punctuation", "unchanged", "crlf",    "multiline", "empty",   "utf8",
                         "boundary",    "range",     "nul",     "bare-cr",   "limit",   "cancel",
                         "owned",       "arguments", "version"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(name, cases[i]) == 0)
      known = 1;
  CHECK(known);
  const char *source = "Abc!", *expected = "ABC!";
  size_t first = 0U, last = 4U;
  UmiStatus wanted = UMI_STATUS_OK;
  UmiEditorEditCommandKind kind = UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE;
  if (strcmp(name, "lowercase") == 0) {
    source = "AbC!";
    expected = "abc!";
    kind = UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE;
  }
  if (strcmp(name, "partial") == 0) {
    source = "xAbcy";
    expected = "xABcy";
    first = 1U;
    last = 3U;
  }
  if (strcmp(name, "reverse") == 0) {
    source = "xAbcy";
    expected = "xABCy";
    first = 4U;
    last = 1U;
  }
  if (strcmp(name, "unicode") == 0) {
    source = "a\xc3\xa9\xe9\x9b\xaa"
             "z";
    expected = "A\xc3\xa9\xe9\x9b\xaa"
               "Z";
    last = strlen(source);
  }
  if (strcmp(name, "emoji") == 0) {
    source = "\xf0\x9f\x8c\x8d"
             "a";
    expected = "\xf0\x9f\x8c\x8d"
               "A";
    last = strlen(source);
  }
  if (strcmp(name, "punctuation") == 0) {
    source = expected = "123_[]{}@!";
    last = strlen(source);
  }
  if (strcmp(name, "unchanged") == 0) {
    source = expected = "ABC!";
  }
  if (strcmp(name, "crlf") == 0) {
    source = "a\r\nb\r\n";
    expected = "A\r\nB\r\n";
    last = strlen(source);
  }
  if (strcmp(name, "multiline") == 0) {
    source = "xa\nby";
    expected = "xA\nBy";
    first = 1U;
    last = 4U;
  }
  if (strcmp(name, "empty") == 0) {
    last = 0U;
    wanted = UMI_STATUS_NOT_FOUND;
  }
  if (strcmp(name, "utf8") == 0) {
    source = "\xc0\xaf"
             "ab";
    wanted = UMI_STATUS_PARSE_ERROR;
  }
  if (strcmp(name, "boundary") == 0) {
    source = "\xc3\xa9"
             "ab";
    first = 1U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "range") == 0) {
    last = 5U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "bare-cr") == 0) {
    source = "a\rb!";
    wanted = UMI_STATUS_NOT_IMPLEMENTED;
  }
  size_t bytes = strlen(source);
  if (strcmp(name, "nul") == 0) {
    source = "a\0b!";
    bytes = 4U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (strcmp(name, "limit") == 0) {
    bytes = 8U * 1024U * 1024U + 1U;
    wanted = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  char owned[] = "Abc!";
  if (strcmp(name, "owned") == 0)
    source = owned;
  UmiEditorEditCommandRequest request;
  CHECK(umi_editor_edit_command_request_initialize(&request, kind, first) == UMI_STATUS_OK);
  request.selection_start = first;
  request.selection_end = last;
  if (strcmp(name, "version") == 0) {
    request.api_version++;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  UmiCancellationToken *cancel = NULL;
  CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
  if (strcmp(name, "cancel") == 0) {
    umi_cancellation_token_request(cancel);
    wanted = UMI_STATUS_CANCELLED;
  }
  UmiEditorLineEditPlan *plan = NULL;
  if (strcmp(name, "arguments") == 0) {
    CHECK(UmiEditorLineEditPlanCreate(NULL, 0, &request, NULL, &plan) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorLineEditPlanCreate(source, bytes, NULL, NULL, &plan) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorLineEditPlanCreate(source, bytes, &request, NULL, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    umi_cancellation_token_destroy(cancel);
    return 0;
  }
  CHECK(UmiEditorLineEditPlanCreate(source, bytes, &request, cancel, &plan) == wanted);
  if (wanted == UMI_STATUS_OK) {
    UmiEditorLineEditSummary summary;
    const char *text = NULL;
    size_t count = 0U;
    CHECK(UmiEditorLineEditPlanInspect(plan, &summary) == UMI_STATUS_OK);
    CHECK(UmiEditorLineEditPlanRead(plan, &text, &count) == UMI_STATUS_OK);
    int changed = strcmp(source, expected) != 0;
    if (strcmp(name, "owned") == 0)
      memset(owned, 'x', sizeof owned - 1U);
    CHECK(count == strlen(expected) && memcmp(text, expected, count) == 0);
    CHECK(summary.changed == changed && summary.source_bytes == bytes &&
          summary.proposed_bytes == bytes);
    CHECK(summary.cursor_offset == (first < last ? first : last));
    CHECK(summary.selection_bytes == (first < last ? last - first : first - last));
    if (changed && strcmp(name, "multiline") == 0)
      CHECK(summary.affected_line_count == 2U);
  } else
    CHECK(plan == NULL);
  UmiEditorLineEditPlanDestroy(plan);
  umi_cancellation_token_destroy(cancel);
  return 0;
}
