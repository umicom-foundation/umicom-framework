/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_source_selection.c
 *
 * PURPOSE:
 *   Check exact source-word and line ranges across Unicode and physical line endings.
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
typedef struct Example {
  const char *name, *text;
  size_t caret;
  UmiEditorSourceSelectionKind kind;
  size_t offset, bytes;
} Example;
static const Example Examples[] = {{"word-start", "alpha beta", 0U, 0U, 0U, 5U},
                                   {"word-middle", "alpha beta", 2U, 0U, 0U, 5U},
                                   {"word-end", "alpha beta", 5U, 0U, 0U, 5U},
                                   {"next-word", "alpha beta", 6U, 0U, 6U, 4U},
                                   {"word-eof", "alpha", 5U, 0U, 0U, 5U},
                                   {"underscore", "foo_bar2()", 4U, 0U, 0U, 8U},
                                   {"digits", "123 + 7", 1U, 0U, 0U, 3U},
                                   {"before-dot", "name.value", 4U, 0U, 0U, 4U},
                                   {"after-dot", "name.value", 5U, 0U, 5U, 5U},
                                   {"unicode-word", "éclair beta", 2U, 0U, 0U, 7U},
                                   {"nonbmp-word", "x😀y.", 1U, 0U, 0U, 6U},
                                   {"combining", "éclair", 1U, 0U, 0U, 8U},
                                   {"line-start", "one\ntwo\n", 4U, 1U, 4U, 3U},
                                   {"line-middle", "one\ntwo\n", 5U, 1U, 4U, 3U},
                                   {"line-ending", "one\ntwo", 3U, 1U, 0U, 3U},
                                   {"line-lf", "one\ntwo", 2U, 2U, 0U, 4U},
                                   {"line-crlf", "one\r\ntwo", 3U, 2U, 0U, 5U},
                                   {"line-cr", "one\rtwo", 2U, 2U, 0U, 4U},
                                   {"after-crlf", "one\r\ntwo", 5U, 1U, 5U, 3U},
                                   {"final-empty", "one\n", 4U, 1U, 4U, 0U},
                                   {"empty", "", 0U, 1U, 0U, 0U},
                                   {"empty-ending", "", 0U, 2U, 0U, 0U},
                                   {"no-final", "one", 3U, 2U, 0U, 3U},
                                   {"blank-line", "one\n\nx", 4U, 2U, 4U, 1U},
                                   {"whitespace-line", "  \t\nx", 2U, 1U, 0U, 3U},
                                   {"unicode-line", "é\r\n中", 4U, 1U, 4U, 3U}};
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  for (size_t i = 0U; i < sizeof Examples / sizeof Examples[0]; ++i) {
    const Example *e = &Examples[i];
    if (strcmp(argv[1], e->name))
      continue;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof view;
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = e->text;
    view.byte_count = strlen(e->text);
    view.capacity = view.byte_count;
    UmiEditorSourceSelection range = {99U, 99U};
    CHECK(UmiEditorSourceSelectionAt(&view, e->caret, e->kind, &range) == UMI_STATUS_OK);
    CHECK(range.offset == e->offset && range.byte_count == e->bytes);
    UmiEditorTextPosition start, end;
    CHECK(UmiEditorTextViewPositionAt(&view, range.offset, &start) == UMI_STATUS_OK);
    CHECK(UmiEditorTextViewPositionAt(&view, range.offset + range.byte_count, &end) ==
          UMI_STATUS_OK);
    return 0;
  }
  return 2;
}
