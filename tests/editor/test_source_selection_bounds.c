/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_source_selection_bounds.c
 *
 * PURPOSE:
 *   Reject invalid source coordinates and preserve selection outputs on failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/source_selection.h"
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
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *names[] = {"arguments",    "missing-word", "empty-word",   "punctuation",
                         "bad-caret",    "split-utf8",   "split-crlf",   "overlong",
                         "truncated",    "nul",          "invalid-kind", "oversized",
                         "view-version", "view-capacity"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  UmiEditorTextBufferView view = {0};
  view.struct_size = (uint32_t)sizeof view;
  view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
  const char *text = "alpha";
  size_t bytes = 5U, caret = 1U;
  UmiEditorSourceSelectionKind kind = UMI_EDITOR_SELECT_SOURCE_WORD;
  if (!strcmp(name, "missing-word")) {
    text = "  ";
    bytes = 2U;
    caret = 1U;
  }
  if (!strcmp(name, "empty-word")) {
    text = "";
    bytes = caret = 0U;
  }
  if (!strcmp(name, "punctuation")) {
    text = "()";
    bytes = 2U;
    caret = 1U;
  }
  if (!strcmp(name, "bad-caret"))
    caret = 6U;
  if (!strcmp(name, "split-utf8")) {
    text = "\xc3\xa9";
    bytes = 2U;
    caret = 1U;
  }
  if (!strcmp(name, "split-crlf")) {
    text = "\r\n";
    bytes = 2U;
    caret = 1U;
  }
  if (!strcmp(name, "overlong")) {
    text = "\xc0\xaf";
    bytes = 2U;
    caret = 0U;
  }
  if (!strcmp(name, "truncated")) {
    text = "\xe2\x82";
    bytes = 2U;
    caret = 0U;
  }
  if (!strcmp(name, "nul")) {
    text = "a\0b";
    bytes = 3U;
    caret = 0U;
  }
  if (!strcmp(name, "invalid-kind"))
    kind = (UmiEditorSourceSelectionKind)99;
  view.bytes = text;
  view.byte_count = bytes;
  view.capacity = bytes;
  if (!strcmp(name, "view-version"))
    view.api_version = 99U;
  if (!strcmp(name, "view-capacity"))
    view.capacity = 0U;
  if (!strcmp(name, "oversized"))
    view.byte_count = view.capacity = 8U * 1024U * 1024U + 1U;
  UmiEditorSourceSelection range = {99U, 99U};
  if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorSourceSelectionAt(NULL, 0U, kind, &range) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorSourceSelectionAt(&view, 0U, kind, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else {
    UmiStatus status = UmiEditorSourceSelectionAt(&view, caret, kind, &range);
    CHECK(status != UMI_STATUS_OK);
    if (!strcmp(name, "missing-word") || !strcmp(name, "empty-word") ||
        !strcmp(name, "punctuation"))
      CHECK(status == UMI_STATUS_NOT_FOUND);
    if (!strcmp(name, "oversized"))
      CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
  }
  CHECK(range.offset == 99U && range.byte_count == 99U);
  return 0;
}
