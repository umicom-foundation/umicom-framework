/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_newline.c
 *
 * PURPOSE:
 *   Check newline tokens against explicit source bytes, boundaries and size limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/newline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
typedef struct Example {
  const char *name, *source, *expected;
  size_t cursor, selection;
  int inherit;
} Example;
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const Example examples[] = {{"empty", "", "\n", 0U, 0U, 1},
                              {"lf", "  code\nnext", "\n  ", 6U, 0U, 1},
                              {"crlf", " \tcode\r\nnext", "\r\n \t", 6U, 0U, 1},
                              {"last-crlf", "a\r\n  b", "\r\n  ", 6U, 0U, 1},
                              {"last-lf", "a\n\tb", "\n\t", 4U, 0U, 1},
                              {"mixed-current", "a\r\n  b\nc", "\n  ", 6U, 0U, 1},
                              {"mixed-last", "a\nb\r\n c", "\r\n ", 7U, 0U, 1},
                              {"mid-indent", "    code", "\n  ", 2U, 0U, 1},
                              {"start-indent", "    code", "\n", 0U, 0U, 1},
                              {"blank", " \t  ", "\n \t  ", 4U, 0U, 1},
                              {"plain", "  code", "\n", 6U, 0U, 0},
                              {"plain-crlf", "  code\r\nnext", "\r\n", 6U, 0U, 0},
                              {"unicode", "  \xc3\xa9\xf0\x9f\x98\x80", "\n  ", 8U, 0U, 1},
                              {"selection", "  one\ntwo", "\n  ", 2U, 7U, 1},
                              {"whole", "abc", "\n", 0U, 3U, 1},
                              {"middle", "  abc", "\n  ", 3U, 0U, 1},
                              {"after-ending", "a\r\n", "\r\n", 3U, 0U, 1},
                              {"whitespace-selection", "    abc", "\n ", 1U, 3U, 1}};
  for (size_t i = 0U; i < sizeof examples / sizeof examples[0]; ++i)
    if (!strcmp(argv[1], examples[i].name)) {
      const Example *e = &examples[i];
      char *text = NULL;
      size_t bytes = 0U;
      CHECK(UmiEditorNewlineCreate(e->source, strlen(e->source), e->cursor, e->selection,
                                   e->inherit, &text, &bytes) == UMI_STATUS_OK);
      CHECK(bytes == strlen(e->expected) && !strcmp(text, e->expected));
      UmiEditorNewlineFree(text);
      return 0;
    }
  char sentinel = 'x', *out = &sentinel;
  size_t bytes = 111U;
  UmiStatus status = UMI_STATUS_OK;
  const char *name = argv[1];
  if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorNewlineCreate(NULL, 0, 0, 0, 1, &out, &bytes) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorNewlineCreate("", 0, 0, 0, 1, NULL, &bytes) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorNewlineCreate("", 0, 0, 0, 1, &out, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorNewlineCreate("", 0, 0, 0, 2, &out, &bytes) == UMI_STATUS_INVALID_ARGUMENT);
    UmiEditorNewlineFree(NULL);
    status = UMI_STATUS_INVALID_ARGUMENT;
  } else if (!strcmp(name, "range"))
    status = UmiEditorNewlineCreate("ab", 2, 3, 0, 1, &out, &bytes);
  else if (!strcmp(name, "selection-range"))
    status = UmiEditorNewlineCreate("ab", 2, 1, (size_t)-1, 1, &out, &bytes);
  else if (!strcmp(name, "utf8-boundary"))
    status = UmiEditorNewlineCreate("\xc3\xa9", 2, 1, 0, 1, &out, &bytes);
  else if (!strcmp(name, "selection-boundary"))
    status = UmiEditorNewlineCreate("\xc3\xa9", 2, 0, 1, 1, &out, &bytes);
  else if (!strcmp(name, "crlf-boundary"))
    status = UmiEditorNewlineCreate("a\r\nb", 4, 2, 0, 1, &out, &bytes);
  else if (!strcmp(name, "utf8-tail"))
    status = UmiEditorNewlineCreate("a\xff", 2, 0, 0, 1, &out, &bytes);
  else if (!strcmp(name, "nul"))
    status = UmiEditorNewlineCreate("a\0b", 3, 0, 0, 1, &out, &bytes);
  else if (!strcmp(name, "bare-cr")) {
    status = UmiEditorNewlineCreate("a\rb", 3, 1, 0, 1, &out, &bytes);
    CHECK(status == UMI_STATUS_NOT_IMPLEMENTED);
  } else if (!strcmp(name, "limit") || !strcmp(name, "limit-replace") ||
             !strcmp(name, "too-large")) {
    size_t length = UMI_EDITOR_NEWLINE_MAXIMUM_SOURCE;
    if (!strcmp(name, "too-large"))
      ++length;
    char *source = malloc(length);
    CHECK(source != NULL);
    memset(source, 'a', length);
    status = UmiEditorNewlineCreate(source, length, 0U, !strcmp(name, "limit-replace") ? 1U : 0U, 1,
                                    &out, &bytes);
    free(source);
    if (!strcmp(name, "limit-replace")) {
      CHECK(status == UMI_STATUS_OK && bytes == 1U && !strcmp(out, "\n"));
      UmiEditorNewlineFree(out);
      return 0;
    }
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
  } else
    return 2;
  CHECK(status != UMI_STATUS_OK && out == &sentinel && bytes == 111U && sentinel == 'x');
  return 0;
}
