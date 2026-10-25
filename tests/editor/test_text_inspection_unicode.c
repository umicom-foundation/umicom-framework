/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_text_inspection_unicode.c
 *
 * PURPOSE:
 *   Reject noncanonical Unicode encodings and count valid scalars independently of byte length.
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
  const char *names[] = {"two-byte",       "three-byte",       "four-byte",     "largest",
                         "overlong-two",   "overlong-three",   "overlong-four", "surrogate",
                         "above-unicode",  "continuation",     "truncated-two", "truncated-three",
                         "truncated-four", "bad-continuation", "nul",           "utf8-bom"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  int failed = 0;
  UmiEditorTextInspection *value = NULL;
  UmiEditorTextInspectionSummary s = {0};

  const char *text = "\xc2\xa2";
  size_t bytes = 2U;
  int valid = 1;
  if (!strcmp(name, "three-byte")) {
    text = "\xe2\x82\xac";
    bytes = 3U;
  } else if (!strcmp(name, "four-byte")) {
    text = "\xf0\x9f\x98\x80";
    bytes = 4U;
  } else if (!strcmp(name, "largest")) {
    text = "\xf4\x8f\xbf\xbf";
    bytes = 4U;
  } else if (!strcmp(name, "utf8-bom")) {
    text = "\xef\xbb\xbf";
    bytes = 3U;
  } else if (strcmp(name, "two-byte")) {
    valid = 0;
    if (!strcmp(name, "overlong-two")) {
      text = "\xc0\x80";
      bytes = 2U;
    } else if (!strcmp(name, "overlong-three")) {
      text = "\xe0\x80\xaf";
      bytes = 3U;
    } else if (!strcmp(name, "overlong-four")) {
      text = "\xf0\x80\x80\xaf";
      bytes = 4U;
    } else if (!strcmp(name, "surrogate")) {
      text = "\xed\xa0\x80";
      bytes = 3U;
    } else if (!strcmp(name, "above-unicode")) {
      text = "\xf4\x90\x80\x80";
      bytes = 4U;
    } else if (!strcmp(name, "continuation")) {
      text = "\x80";
      bytes = 1U;
    } else if (!strcmp(name, "truncated-two")) {
      text = "\xc2";
      bytes = 1U;
    } else if (!strcmp(name, "truncated-three")) {
      text = "\xe2\x82";
      bytes = 2U;
    } else if (!strcmp(name, "truncated-four")) {
      text = "\xf0\x9f\x98";
      bytes = 3U;
    } else if (!strcmp(name, "bad-continuation")) {
      text = "\xe2x\xac";
      bytes = 3U;
    } else {
      text = "a\0b";
      bytes = 3U;
    }
  }
  UmiStatus status = UmiEditorTextInspectionCreate(text, bytes, 0U, 0U, NULL, &value);
  if (valid) {
    CHECK(status == UMI_STATUS_OK);
    OK(UmiEditorTextInspectionSummaryRead(value, &s));
    CHECK(s.document.scalars == 1U && s.document.utf16_units == (bytes == 4U ? 2U : 1U));
  } else
    CHECK(status == UMI_STATUS_PARSE_ERROR && value == NULL);
cleanup:
  UmiEditorTextInspectionDestroy(value);
  return failed;
}
