/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_text_inspection_limits.c
 *
 * PURPOSE:
 *   Keep outputs stable on cancellation, oversized spans and ambiguous source endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/text_inspection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  const char *names[] = {"arguments",      "existing-output", "cursor-range", "selection-range",
                         "split-scalar",   "split-crlf",      "end",          "zero-span",
                         "cancel",         "reset",           "oversized",    "exact-limit",
                         "summary-output", "notice-output",   "text-output",  "after-input"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  int failed = 0;
  UmiEditorTextInspection *value = NULL;
  UmiEditorTextInspectionSummary s = {0};

  char *large = NULL;
  UmiCancellationToken *cancel = NULL;
  if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorTextInspectionCreate(NULL, 1U, 0U, 0U, NULL, &value) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          value == NULL);
    CHECK(UmiEditorTextInspectionCreate("x", 1U, 0U, 0U, NULL, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (!strcmp(name, "cancel") || !strcmp(name, "reset")) {
    OK(umi_cancellation_token_create(&cancel));
    umi_cancellation_token_request(cancel);
    CHECK(UmiEditorTextInspectionCreate("x", 1U, 0U, 0U, cancel, &value) == UMI_STATUS_CANCELLED &&
          value == NULL);
    if (!strcmp(name, "reset")) {
      umi_cancellation_token_reset(cancel);
      OK(UmiEditorTextInspectionCreate("x", 1U, 0U, 0U, cancel, &value));
    }
    goto cleanup;
  }
  if (!strcmp(name, "oversized")) {
    CHECK(UmiEditorTextInspectionCreate("x", UMI_EDITOR_INSPECTION_TEXT_LIMIT + 1U, 0U, 0U, NULL,
                                        &value) == UMI_STATUS_CAPACITY_EXCEEDED &&
          value == NULL);
    goto cleanup;
  }
  if (!strcmp(name, "exact-limit")) {
    large = malloc(UMI_EDITOR_INSPECTION_TEXT_LIMIT);
    CHECK(large);
    memset(large, 'x', UMI_EDITOR_INSPECTION_TEXT_LIMIT);
    OK(UmiEditorTextInspectionCreate(large, UMI_EDITOR_INSPECTION_TEXT_LIMIT, 0U, 0U, NULL,
                                     &value));
    OK(UmiEditorTextInspectionSummaryRead(value, &s));
    CHECK(s.document.bytes == UMI_EDITOR_INSPECTION_TEXT_LIMIT && s.document.words == 1U);
    goto cleanup;
  }
  if (!strcmp(name, "cursor-range")) {
    CHECK(UmiEditorTextInspectionCreate("x", 1U, 2U, 0U, NULL, &value) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          value == NULL);
    goto cleanup;
  }
  if (!strcmp(name, "selection-range")) {
    CHECK(UmiEditorTextInspectionCreate("x", 1U, 1U, SIZE_MAX, NULL, &value) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          value == NULL);
    goto cleanup;
  }
  if (!strcmp(name, "split-scalar")) {
    CHECK(UmiEditorTextInspectionCreate("\xc3\xa9", 2U, 1U, 0U, NULL, &value) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorTextInspectionCreate("\xc3\xa9", 2U, 0U, 1U, NULL, &value) ==
          UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (!strcmp(name, "split-crlf")) {
    CHECK(UmiEditorTextInspectionCreate("\r\n", 2U, 1U, 0U, NULL, &value) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorTextInspectionCreate("\r\n", 2U, 0U, 1U, NULL, &value) ==
          UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (!strcmp(name, "zero-span")) {
    OK(UmiEditorTextInspectionCreate(NULL, 0U, 0U, 0U, NULL, &value));
    goto cleanup;
  }
  OK(UmiEditorTextInspectionCreate("x", 1U, 1U, 0U, NULL, &value));
  if (!strcmp(name, "existing-output")) {
    UmiEditorTextInspection *guard = value;
    CHECK(UmiEditorTextInspectionCreate("y", 1U, 0U, 0U, NULL, &value) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          value == guard);
  } else if (!strcmp(name, "end")) {
    OK(UmiEditorTextInspectionSummaryRead(value, &s));
    CHECK(s.cursor_offset == 1U && s.selection.bytes == 0U);
  } else if (!strcmp(name, "summary-output")) {
    s.document.bytes = 42U;
    CHECK(UmiEditorTextInspectionSummaryRead(NULL, &s) == UMI_STATUS_INVALID_ARGUMENT &&
          s.document.bytes == 42U);
    CHECK(UmiEditorTextInspectionSummaryRead(value, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "notice-output")) {
    UmiEditorTextNotice n = {.codepoint = 42U};
    CHECK(UmiEditorTextInspectionNoticeAt(NULL, 0U, &n) == UMI_STATUS_INVALID_ARGUMENT &&
          n.codepoint == 42U);
    CHECK(UmiEditorTextInspectionNoticeAt(value, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "text-output")) {
    const char *text = "guard";
    size_t bytes = 42U;
    CHECK(UmiEditorTextInspectionRead(NULL, &text, &bytes) == UMI_STATUS_INVALID_ARGUMENT &&
          bytes == 42U && !strcmp(text, "guard"));
    CHECK(UmiEditorTextInspectionRead(value, NULL, &bytes) == UMI_STATUS_INVALID_ARGUMENT &&
          bytes == 42U);
  } else if (!strcmp(name, "after-input")) {
    UmiEditorTextInspectionDestroy(value);
    value = NULL;
    large = malloc(2U);
    CHECK(large);
    memcpy(large, "z", 2U);
    OK(UmiEditorTextInspectionCreate(large, 1U, 0U, 0U, NULL, &value));
    free(large);
    large = NULL;
    const char *text = NULL;
    size_t bytes = 0U;
    OK(UmiEditorTextInspectionRead(value, &text, &bytes));
    CHECK(bytes == 1U && !strcmp(text, "z"));
  }
cleanup:
  free(large);
  umi_cancellation_token_destroy(cancel);
  UmiEditorTextInspectionDestroy(value);
  return failed;
}
