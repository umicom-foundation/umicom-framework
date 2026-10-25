/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_text_inspection_metrics.c
 *
 * PURPOSE:
 *   Check logical line, scalar, word and selected-range metrics without platform locale
 * assumptions.
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
  const char *names[] = {"empty",         "ascii",     "lf",       "crlf",      "cr",
                         "mixed",         "blank",     "trailing", "interior",  "unicode",
                         "nonbmp",        "combining", "words",    "selection", "empty-selection",
                         "selected-crlf", "owned",     "no-final"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  int failed = 0;
  UmiEditorTextInspection *value = NULL;
  UmiEditorTextInspectionSummary s = {0};

  const char *text = "one two";
  size_t cursor = 0U, selection = 0U;
  char owned[] = "first";
  if (!strcmp(name, "empty"))
    text = "";
  else if (!strcmp(name, "lf"))
    text = "a\nb\n";
  else if (!strcmp(name, "crlf"))
    text = "a\r\nb\r\n";
  else if (!strcmp(name, "cr"))
    text = "a\rb\r";
  else if (!strcmp(name, "mixed"))
    text = "a\rb\nc\r\n";
  else if (!strcmp(name, "blank"))
    text = " \t\n\nword";
  else if (!strcmp(name, "trailing"))
    text = "one \r\ntwo\t";
  else if (!strcmp(name, "interior"))
    text = "a b\tc";
  else if (!strcmp(name, "unicode"))
    text = "\xc3\xa9\xe4\xb8\xad";
  else if (!strcmp(name, "nonbmp"))
    text = "a\xf0\x9f\x98\x80";
  else if (!strcmp(name, "combining"))
    text = "e\xcc\x81";
  else if (!strcmp(name, "words"))
    text = " one\ttwo\vthree\ffour\r\nfive ";
  else if (!strcmp(name, "selection")) {
    text = "left\n\xc3\xa9\xf0\x9f\x98\x80 right";
    cursor = 5U;
    selection = 6U;
  } else if (!strcmp(name, "empty-selection")) {
    text = "ab\n";
    cursor = 2U;
  } else if (!strcmp(name, "selected-crlf")) {
    text = "a\r\nb";
    cursor = 1U;
    selection = 2U;
  } else if (!strcmp(name, "owned"))
    text = owned;
  else if (!strcmp(name, "no-final"))
    text = "a\nb";
  OK(UmiEditorTextInspectionCreate(text, strlen(text), cursor, selection, NULL, &value));
  OK(UmiEditorTextInspectionSummaryRead(value, &s));
  CHECK(s.document.bytes == strlen(text) && s.cursor_offset == cursor &&
        s.selection_bytes == selection);
  if (!strcmp(name, "empty"))
    CHECK(s.document.lines == 1U && s.document.scalars == 0U && s.document.words == 0U &&
          s.document.blank_lines == 1U);
  else if (!strcmp(name, "ascii"))
    CHECK(s.document.scalars == 7U && s.document.utf16_units == 7U && s.document.words == 2U &&
          s.document.spaces == 1U);
  else if (!strcmp(name, "lf"))
    CHECK(s.document.lines == 3U && s.document.lf == 2U && s.document.crlf == 0U &&
          s.document.scalars == 4U && s.document.ends_with_line_break);
  else if (!strcmp(name, "crlf"))
    CHECK(s.document.lines == 3U && s.document.crlf == 2U && s.document.lf == 0U &&
          s.document.cr == 0U && s.document.scalars == 6U);
  else if (!strcmp(name, "cr"))
    CHECK(s.document.lines == 3U && s.document.cr == 2U && s.document.crlf == 0U);
  else if (!strcmp(name, "mixed"))
    CHECK(s.document.lines == 4U && s.document.cr == 1U && s.document.lf == 1U &&
          s.document.crlf == 1U);
  else if (!strcmp(name, "blank"))
    CHECK(s.document.blank_lines == 2U && s.document.trailing_whitespace_lines == 1U &&
          s.document.longest_line_scalars == 4U);
  else if (!strcmp(name, "trailing"))
    CHECK(s.document.trailing_whitespace_lines == 2U && s.document.blank_lines == 0U &&
          s.document.longest_line_scalars == 4U);
  else if (!strcmp(name, "interior"))
    CHECK(s.document.spaces == 1U && s.document.tabs == 1U &&
          s.document.trailing_whitespace_lines == 0U);
  else if (!strcmp(name, "unicode"))
    CHECK(s.document.scalars == 2U && s.document.utf16_units == 2U && s.document.non_ascii == 2U);
  else if (!strcmp(name, "nonbmp"))
    CHECK(s.document.scalars == 2U && s.document.utf16_units == 3U && s.document.non_ascii == 1U);
  else if (!strcmp(name, "combining"))
    CHECK(s.document.scalars == 2U && s.document.utf16_units == 2U);
  else if (!strcmp(name, "words"))
    CHECK(s.document.words == 5U);
  else if (!strcmp(name, "selection"))
    CHECK(s.selection.bytes == 6U && s.selection.scalars == 2U && s.selection.utf16_units == 3U &&
          s.selection.lines == 1U);
  else if (!strcmp(name, "empty-selection"))
    CHECK(s.selection.bytes == 0U && s.selection.lines == 1U && s.selection.words == 0U);
  else if (!strcmp(name, "selected-crlf"))
    CHECK(s.selection.scalars == 2U && s.selection.lines == 2U && s.selection.crlf == 1U);
  else if (!strcmp(name, "owned")) {
    owned[0] = 'X';
    const char *retained = NULL;
    size_t bytes = 0U;
    OK(UmiEditorTextInspectionRead(value, &retained, &bytes));
    CHECK(bytes == 5U && !strcmp(retained, "first"));
  } else if (!strcmp(name, "no-final"))
    CHECK(s.document.lines == 2U && !s.document.ends_with_line_break &&
          s.document.blank_lines == 0U);
cleanup:
  UmiEditorTextInspectionDestroy(value);
  return failed;
}
