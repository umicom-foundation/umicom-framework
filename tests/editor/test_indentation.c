/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_indentation.c
 *
 * PURPOSE:
 *   Verify validated indentation, tab-stop boundaries and unchanged failure outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/indentation.h"
#include <stdio.h>
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
             *cases[] = {"defaults",  "unit-spaces", "unit-tab",  "width-one", "width-sixteen",
                         "next-stop", "at-stop",     "after-tab", "unicode",   "emoji",
                         "newline",   "crlf",        "boundary",  "utf8",      "nul",
                         "range",     "short",       "invalid",   "arguments"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  CHECK(known);
  UmiEditorIndentation policy;
  CHECK(UmiEditorIndentationDefault(&policy) == UMI_STATUS_OK && policy.tab_width == 4U &&
        policy.insert_spaces == 1);
  char output[20];
  memset(output, '~', sizeof output);
  size_t bytes = 99U;
  const char *source = "ab";
  size_t length = 2U, offset = 2U;
  const char *expected = "  ";
  UmiStatus wanted = UMI_STATUS_OK;
  if (!strcmp(name, "defaults")) {
    CHECK(UmiEditorIndentationValidate(&policy) == UMI_STATUS_OK);
    return 0;
  }
  if (!strcmp(name, "unit-spaces")) {
    CHECK(UmiEditorIndentationUnit(&policy, output, sizeof output, &bytes) == UMI_STATUS_OK &&
          bytes == 4U && !strcmp(output, "    "));
    return 0;
  }
  if (!strcmp(name, "unit-tab")) {
    policy.insert_spaces = 0;
    CHECK(UmiEditorIndentationUnit(&policy, output, sizeof output, &bytes) == UMI_STATUS_OK &&
          bytes == 1U && !strcmp(output, "\t"));
    return 0;
  }
  if (!strcmp(name, "width-one")) {
    policy.tab_width = 1U;
    expected = " ";
  }
  if (!strcmp(name, "width-sixteen")) {
    policy.tab_width = 16U;
    source = "";
    length = offset = 0U;
    expected = "                ";
  }
  if (!strcmp(name, "at-stop")) {
    source = "abcd";
    length = offset = 4U;
    expected = "    ";
  }
  if (!strcmp(name, "after-tab")) {
    source = "\ta";
    length = offset = 2U;
    expected = "   ";
  }
  if (!strcmp(name, "unicode")) {
    source = "\xc3\xa9";
    length = offset = 2U;
    expected = "   ";
  }
  if (!strcmp(name, "emoji")) {
    source = "\xf0\x9f\x8c\x8d";
    length = offset = 4U;
    expected = "   ";
  }
  if (!strcmp(name, "newline")) {
    source = "abc\nx";
    length = offset = 5U;
    expected = "   ";
  }
  if (!strcmp(name, "crlf")) {
    source = "abc\r\nx";
    length = offset = 6U;
    expected = "   ";
  }
  if (!strcmp(name, "boundary")) {
    source = "\xc3\xa9";
    length = 2U;
    offset = 1U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "utf8")) {
    source = "\xc0\xaf";
    wanted = UMI_STATUS_PARSE_ERROR;
  }
  if (!strcmp(name, "nul")) {
    source = "a\0";
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "range")) {
    offset = 3U;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "invalid")) {
    policy.tab_width = 0U;
    CHECK(UmiEditorIndentationValidate(&policy) == UMI_STATUS_INVALID_ARGUMENT);
    policy.tab_width = 17U;
    CHECK(UmiEditorIndentationValidate(&policy) == UMI_STATUS_INVALID_ARGUMENT);
    policy.tab_width = 4U;
    policy.insert_spaces = 2;
    wanted = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorIndentationDefault(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorIndentationUnit(NULL, output, sizeof output, &bytes) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorIndentationAt(&policy, NULL, 0U, 0U, output, sizeof output, &bytes) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorIndentationAt(&policy, source, length, offset, NULL, 0U, &bytes) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorIndentationAt(&policy, source, length, offset, output, sizeof output, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(output[0] == '~' && bytes == 99U);
    return 0;
  }
  size_t capacity = !strcmp(name, "short") ? 2U : sizeof output;
  if (capacity == 2U)
    wanted = UMI_STATUS_CAPACITY_EXCEEDED;
  CHECK(UmiEditorIndentationAt(&policy, source, length, offset, output, capacity, &bytes) ==
        wanted);
  if (wanted == UMI_STATUS_OK)
    CHECK(bytes == strlen(expected) && !strcmp(output, expected) && output[bytes + 1U] == '~');
  else
    CHECK(output[0] == '~' && bytes == 99U);
  return 0;
}
