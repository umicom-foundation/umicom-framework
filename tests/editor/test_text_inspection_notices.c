/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_text_inspection_notices.c
 *
 * PURPOSE:
 *   Check explicit invisible-character categories, exact locations and bounded retained notice
 * lists.
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
  const char *names[] = {"bidi", "isolate",   "marks",    "zero-width", "nbsp",       "control",
                         "tabs", "locations", "selected", "limit",      "over-limit", "plain"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  int failed = 0;
  UmiEditorTextInspection *value = NULL;
  UmiEditorTextInspectionSummary s = {0};

  const char *text = "plain";
  char *many = NULL;
  size_t expected = 0U;
  UmiEditorTextNoticeKind kind = UMI_EDITOR_TEXT_CONTROL;
  if (!strcmp(name, "bidi")) {
    text = "\xe2\x80\xaa\xe2\x80\xae";
    expected = 2U;
    kind = UMI_EDITOR_TEXT_BIDI_CONTROL;
  } else if (!strcmp(name, "isolate")) {
    text = "\xe2\x81\xa6\xe2\x81\xa9";
    expected = 2U;
    kind = UMI_EDITOR_TEXT_BIDI_CONTROL;
  } else if (!strcmp(name, "marks")) {
    text = "\xd8\x9c\xe2\x80\x8e\xe2\x80\x8f";
    expected = 3U;
    kind = UMI_EDITOR_TEXT_BIDI_CONTROL;
  } else if (!strcmp(name, "zero-width")) {
    text = "\xe2\x80\x8b\xe2\x80\x8c\xe2\x80\x8d\xe2\x81\xa0\xef\xbb\xbf";
    expected = 5U;
    kind = UMI_EDITOR_TEXT_ZERO_WIDTH;
  } else if (!strcmp(name, "nbsp")) {
    text = "\xc2\xa0\xe2\x80\xaf";
    expected = 2U;
    kind = UMI_EDITOR_TEXT_NONBREAKING_SPACE;
  } else if (!strcmp(name, "control")) {
    text = "\x01\x7f\xc2\x85";
    expected = 3U;
  } else if (!strcmp(name, "tabs"))
    text = " \t\r\n";
  else if (!strcmp(name, "locations")) {
    text = "\xc3\xa9\r\nX\xe2\x80\xae";
    expected = 1U;
    kind = UMI_EDITOR_TEXT_BIDI_CONTROL;
  } else if (!strcmp(name, "selected")) {
    text = "x\xe2\x80\xaey";
    expected = 1U;
    kind = UMI_EDITOR_TEXT_BIDI_CONTROL;
  } else if (!strcmp(name, "limit") || !strcmp(name, "over-limit")) {
    expected = UMI_EDITOR_INSPECTION_NOTICE_LIMIT + (!strcmp(name, "over-limit") ? 1U : 0U);
    many = malloc(expected + 1U);
    CHECK(many != NULL);
    memset(many, 1, expected);
    many[expected] = '\0';
    text = many;
  }
  OK(UmiEditorTextInspectionCreate(text, strlen(text), 0U, !strcmp(name, "selected") ? 1U : 0U,
                                   NULL, &value));
  OK(UmiEditorTextInspectionSummaryRead(value, &s));
  CHECK(s.notice_count == expected &&
        s.retained_notices == (expected > UMI_EDITOR_INSPECTION_NOTICE_LIMIT
                                   ? UMI_EDITOR_INSPECTION_NOTICE_LIMIT
                                   : expected));
  CHECK(s.notices_truncated == (expected > UMI_EDITOR_INSPECTION_NOTICE_LIMIT));
  for (size_t i = 0U; i < s.retained_notices; ++i) {
    UmiEditorTextNotice n;
    OK(UmiEditorTextInspectionNoticeAt(value, i, &n));
    CHECK(n.kind == kind);
    if (!strcmp(name, "locations"))
      CHECK(n.line == 2U && n.scalar_column == 2U && n.byte_offset == 5U && n.byte_length == 3U &&
            n.codepoint == 0x202eU);
  }
  UmiEditorTextNotice guard = {.codepoint = 42U};
  CHECK(UmiEditorTextInspectionNoticeAt(value, s.retained_notices, &guard) ==
            UMI_STATUS_NOT_FOUND &&
        guard.codepoint == 42U);
cleanup:
  free(many);
  UmiEditorTextInspectionDestroy(value);
  return failed;
}
