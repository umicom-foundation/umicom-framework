/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_search_range.c
 *
 * PURPOSE:
 *   Check range-limited replacement, complete-source word boundaries and literal matching policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/search_range.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
typedef struct Case {
  const char *id, *source;
  size_t start, span;
  const char *needle, *replacement;
  int mode, word;
  const char *expected;
  size_t count;
} Case;
static const Case cases[] = {
    {"middle", "cat cat cat", 4U, 3U, "cat", "dog", 0U, 0U, "cat dog cat", 1U},
    {"prefix", "cat cat", 0U, 3U, "cat", "x", 0U, 0U, "x cat", 1U},
    {"suffix", "cat cat", 4U, 3U, "cat", "x", 0U, 0U, "cat x", 1U},
    {"full", "cat cat", 0U, 7U, "cat", "x", 0U, 0U, "x x", 2U},
    {"zero", "cat", 1U, 0U, "a", "X", 0U, 0U, "cat", 0U},
    {"empty", "", 0U, 0U, "a", "X", 0U, 0U, "", 0U},
    {"nomatch", "cat", 0U, 3U, "z", "X", 0U, 0U, "cat", 0U},
    {"straddle", "cat cat", 0U, 5U, "cat", "x", 0U, 0U, "x cat", 1U},
    {"left-boundary", "scat cat", 1U, 3U, "cat", "x", 0U, 1U, "scat cat", 0U},
    {"right-boundary", "cats cat", 0U, 3U, "cat", "x", 0U, 1U, "cats cat", 0U},
    {"whole-word", "cat cats cat", 0U, 12U, "cat", "x", 0U, 1U, "x cats x", 2U},
    {"inside-other-match", "aaa", 1U, 2U, "aa", "X", 0U, 0U, "aX", 1U},
    {"overlap", "aaaa", 0U, 4U, "aa", "x", 0U, 0U, "xx", 2U},
    {"grow", "a a", 0U, 3U, "a", "long", 0U, 0U, "long long", 2U},
    {"shrink", "long long", 0U, 4U, "long", "x", 0U, 0U, "x long", 1U},
    {"delete", "a a", 0U, 3U, "a", "", 0U, 0U, " ", 2U},
    {"same", "cat", 0U, 3U, "cat", "cat", 0U, 0U, "cat", 1U},
    {"case-exact", "Cat cat", 0U, 7U, "cat", "x", 0U, 0U, "Cat x", 1U},
    {"case-insensitive", "Cat CAT", 0U, 7U, "cat", "x", 1U, 0U, "x x", 2U},
    {"smart-lower", "Cat CAT", 0U, 7U, "cat", "x", 2U, 0U, "x x", 2U},
    {"smart-upper", "Cat CAT", 0U, 7U, "Cat", "x", 2U, 0U, "x CAT", 1U},
    /* Preserve the malformed multi-line literals for review. Explicit escapes
     * below keep LF and CRLF test data independent of source-file line endings.
     * They still test replacement across a line and replacement of CRLF itself.
    {"newline", "a
     b a ",0U,5U," a "," x ",0U,0U," x b x ",2U},
     {"crlf",
      "a
      b ",1U,2U,"
      ","
      ",0U,0U," a b ",1U},
     */
    {"newline", "a\nb a", 0U, 5U, "a", "x", 0U, 0U, "x\nb x", 2U},
    {"crlf", "a\r\nb", 1U, 2U, "\r\n", "\n", 0U, 0U, "a\nb", 1U},
      {"not-search-output", "a", 0U, 1U, "a", "aa", 0U, 0U, "aa", 1U},
      {"range-end", "cat", 3U, 0U, "cat", "x", 0U, 0U, "cat", 0U}};
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const Case *c = NULL;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i].id))
      c = &cases[i];
  CHECK(c);
  UmiEditorSearchOptions options = {(UmiEditorSearchCaseMode)c->mode, c->word, 1, 1};
  size_t size = 999U, count = 999U;
  char output[128];
  memset(output, '!', sizeof output);
  CHECK(UmiEditorSearchReplaceRangeSize(c->source, strlen(c->source), c->start, c->span, c->needle,
                                        strlen(c->needle), strlen(c->replacement), &options, &size,
                                        &count) == UMI_STATUS_OK);
  CHECK(size == strlen(c->expected) && count == c->count);
  count = 999U;
  CHECK(UmiEditorSearchReplaceRange(c->source, strlen(c->source), c->start, c->span, c->needle,
                                    strlen(c->needle), c->replacement, strlen(c->replacement),
                                    &options, output, sizeof output, &count) == UMI_STATUS_OK);
  CHECK(!strcmp(output, c->expected) && count == c->count && output[size + 1U] == '!');
  return 0;
}
