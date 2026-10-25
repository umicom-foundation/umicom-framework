/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_search_range_bounds.c
 *
 * PURPOSE:
 *   Verify failure atomicity, overlap rejection, complete match counts and byte-buffer range
 * contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/search_range.h"
#include <stdint.h>
#include <stdio.h>
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
  const char *name = argv[1];
  char output[128], before[128];
  memset(output, '!', sizeof output);
  memcpy(before, output, sizeof output);
  size_t bytes = 123U, count = 456U;
  UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SENSITIVE, 0, 1, 1};
  if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorSearchReplaceRangeSize(NULL, 1U, 0U, 0U, "a", 1U, 1U, NULL, &bytes, &count) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorSearchReplaceRangeSize("a", 1U, 0U, 1U, NULL, 1U, 1U, NULL, &bytes, &count) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorSearchReplaceRangeSize("a", 1U, 0U, 1U, "", 0U, 1U, NULL, &bytes, &count) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorSearchReplaceRange("a", 1U, 0U, 1U, "a", 1U, NULL, 1U, NULL, output,
                                      sizeof output, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(bytes == 123U && count == 456U && !memcmp(output, before, sizeof output));
  } else if (!strcmp(name, "bad-start") || !strcmp(name, "bad-span")) {
    CHECK(UmiEditorSearchReplaceRangeSize("a", 1U, !strcmp(name, "bad-start") ? 2U : 0U,
                                          !strcmp(name, "bad-span") ? SIZE_MAX : 0U, "a", 1U, 1U,
                                          NULL, &bytes, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(bytes == 123U && count == 456U);
  } else if (!strcmp(name, "invalid-case") || !strcmp(name, "invalid-word")) {
    if (!strcmp(name, "invalid-case"))
      options.case_mode = (UmiEditorSearchCaseMode)99;
    else
      options.whole_word = 2;
    CHECK(UmiEditorSearchReplaceRange("a", 1U, 0U, 1U, "a", 1U, "x", 1U, &options, output,
                                      sizeof output, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!memcmp(output, before, sizeof output) && count == 456U);
  } else if (!strcmp(name, "capacity")) {
    CHECK(UmiEditorSearchReplaceRange("a", 1U, 0U, 1U, "a", 1U, "xx", 2U, NULL, output, 2U,
                                      &count) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!memcmp(output, before, sizeof output) && count == 456U);
  } else if (!strncmp(name, "overlap-", 8U)) {
    memcpy(output, "aaaa", 5U);
    memcpy(before, output, sizeof output);
    const char *text = !strcmp(name, "overlap-text") ? output : "aaaa";
    const char *needle = !strcmp(name, "overlap-needle") ? output : "a";
    const char *replacement = !strcmp(name, "overlap-replacement") ? output : "x";
    CHECK(UmiEditorSearchReplaceRange(text, 4U, 0U, 4U, needle, 1U, replacement, 1U, NULL, output,
                                      sizeof output, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!memcmp(output, before, sizeof output) && count == 456U);
  } else if (!strcmp(name, "overflow")) {
    CHECK(UmiEditorSearchReplaceRangeSize("aa", 2U, 0U, 2U, "a", 1U, SIZE_MAX, NULL, &bytes,
                                          &count) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(bytes == 123U && count == 456U);
  } else if (!strcmp(name, "all-matches")) {
    char text[8192];
    memset(text, 'a', sizeof text);
    CHECK(UmiEditorSearchReplaceRangeSize(text, sizeof text, 1U, sizeof text - 2U, "a", 1U, 0U,
                                          &options, &bytes, &count) == UMI_STATUS_OK);
    CHECK(count == 8190U && bytes == 2U);
  } else if (!strcmp(name, "binary")) {
    const char text[] = {'a', 0, 'a'}, replacement[] = {'x', 0};
    CHECK(UmiEditorSearchReplaceRange(text, sizeof text, 1U, 2U, "a", 1U, replacement,
                                      sizeof replacement, NULL, output, sizeof output,
                                      &count) == UMI_STATUS_OK);
    CHECK(count == 1U && output[0] == 'a' && output[1] == 0 && output[2] == 'x' && output[3] == 0 &&
          output[4] == 0);
  } else if (!strcmp(name, "null-empty")) {
    CHECK(UmiEditorSearchReplaceRange(NULL, 0U, 0U, 0U, "a", 1U, NULL, 0U, NULL, output,
                                      sizeof output, &count) == UMI_STATUS_OK &&
          output[0] == 0 && count == 0U);
  } else if (!strcmp(name, "optional-count")) {
    CHECK(UmiEditorSearchReplaceRange("a", 1U, 0U, 1U, "a", 1U, "x", 1U, NULL, output,
                                      sizeof output, NULL) == UMI_STATUS_OK &&
          !strcmp(output, "x"));
  } else if (!strcmp(name, "size-only")) {
    CHECK(UmiEditorSearchReplaceRangeSize("abc", 3U, 1U, 1U, "b", 1U, 4U, NULL, &bytes, NULL) ==
              UMI_STATUS_OK &&
          bytes == 6U);
  } else if (!strcmp(name, "utf8-word")) {
    const char text[] = "\xc3\xa9"
                        "cat cat";
    options.whole_word = 1;
    CHECK(UmiEditorSearchReplaceRange(text, sizeof text - 1U, 2U, 3U, "cat", 3U, "x", 1U, &options,
                                      output, sizeof output, &count) == UMI_STATUS_OK &&
          !strcmp(text, output) && count == 0U);
  } else
    return 2;
  return 0;
}
