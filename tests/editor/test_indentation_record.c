/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_indentation_record.c
 *
 * PURPOSE:
 *   Verify bounded preference records reject malformed or future input without changing the
 * caller's policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/indentation_record.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *cases[] = {"spaces",         "tabs",   "one",   "sixteen",   "crlf",     "no-newline",
                         "version",        "order",  "extra", "duplicate", "zero",     "wide",
                         "boolean",        "nul",    "empty", "large",     "capacity", "arguments",
                         "invalid-encode", "bare-cr"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  UmiEditorIndentation input = {4, 1}, out = {8, 0};
  if (!strcmp(name, "tabs"))
    input.insert_spaces = 0;
  if (!strcmp(name, "one"))
    input.tab_width = 1;
  if (!strcmp(name, "sixteen"))
    input.tab_width = 16;
  char encoded[UMI_EDITOR_INDENTATION_RECORD_MAXIMUM_BYTES + 1U];
  size_t bytes = 0;
  CHECK(UmiEditorIndentationEncode(&input, encoded, sizeof encoded, &bytes) == UMI_STATUS_OK);
  if (!strcmp(name, "capacity") || !strcmp(name, "invalid-encode")) {
    strcpy(encoded, "retained");
    bytes = 99U;
    if (!strcmp(name, "invalid-encode"))
      input.tab_width = 0;
    CHECK(UmiEditorIndentationEncode(&input, encoded,
                                     !strcmp(name, "capacity") ? 2U : sizeof encoded,
                                     &bytes) != UMI_STATUS_OK);
    CHECK(!strcmp(encoded, "retained") && bytes == 99U);
    return 0;
  }
  if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorIndentationEncode(NULL, encoded, sizeof encoded, &bytes) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorIndentationEncode(&input, NULL, sizeof encoded, &bytes) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorIndentationEncode(&input, encoded, sizeof encoded, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorIndentationDecode(NULL, bytes, &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorIndentationDecode(encoded, bytes, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  const char *bad = NULL;
  UmiStatus expected = UMI_STATUS_OK;
  if (!strcmp(name, "crlf"))
    bad = "umicom.editor.indentation/1\r\nwidth=4\r\nspaces=1\r\n";
  if (!strcmp(name, "no-newline"))
    --bytes;
  if (!strcmp(name, "version")) {
    bad = "umicom.editor.indentation/2\nwidth=4\nspaces=1\n";
    expected = UMI_STATUS_NOT_IMPLEMENTED;
  }
  if (!strcmp(name, "order"))
    bad = "umicom.editor.indentation/1\nspaces=1\nwidth=4\n";
  if (!strcmp(name, "extra"))
    bad = "umicom.editor.indentation/1\nwidth=4\nspaces=1\nextra=1\n";
  if (!strcmp(name, "duplicate"))
    bad = "umicom.editor.indentation/1\nwidth=4\nwidth=4\n";
  if (!strcmp(name, "zero"))
    bad = "umicom.editor.indentation/1\nwidth=0\nspaces=1\n";
  if (!strcmp(name, "wide"))
    bad = "umicom.editor.indentation/1\nwidth=17\nspaces=1\n";
  if (!strcmp(name, "boolean"))
    bad = "umicom.editor.indentation/1\nwidth=4\nspaces=true\n";
  if (!strcmp(name, "bare-cr"))
    bad = "umicom.editor.indentation/1\rwidth=4\nspaces=1\n";
  if (bad != NULL) {
    strcpy(encoded, bad);
    bytes = strlen(bad);
    if (strcmp(name, "crlf") && strcmp(name, "version"))
      expected = UMI_STATUS_PARSE_ERROR;
  }
  if (!strcmp(name, "zero") || !strcmp(name, "wide"))
    expected = UMI_STATUS_INVALID_ARGUMENT;
  if (!strcmp(name, "nul")) {
    encoded[5] = '\0';
    expected = UMI_STATUS_PARSE_ERROR;
  }
  if (!strcmp(name, "empty")) {
    bytes = 0U;
    expected = UMI_STATUS_PARSE_ERROR;
  }
  if (!strcmp(name, "large")) {
    memset(encoded, 'x', sizeof encoded);
    bytes = sizeof encoded;
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  }
  CHECK(UmiEditorIndentationDecode(encoded, bytes, &out) == expected);
  if (expected == UMI_STATUS_OK)
    CHECK(out.tab_width == input.tab_width && out.insert_spaces == input.insert_spaces);
  else
    CHECK(out.tab_width == 8U && out.insert_spaces == 0);
  return 0;
}
