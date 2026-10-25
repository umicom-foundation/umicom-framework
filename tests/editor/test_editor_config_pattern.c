/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_config_pattern.c
 *
 * PURPOSE:
 *   Exercise configuration text and pattern boundaries with explicit memory-only expectations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "editor_config_fixture.h"
#include "umicom/editor/editor_config_pattern.h"
typedef struct Example {
  const char *name, *pattern, *file;
  int matches;
  UmiStatus status;
} Example;
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const Example cases[] = {{"basename", "*.c", "source/main.c", 1, UMI_STATUS_OK},
                           {"case", "*.c", "source/main.C", 0, UMI_STATUS_OK},
                           {"single-star", "src/*.c", "src/deep/a.c", 0, UMI_STATUS_OK},
                           {"double-star", "src/**.c", "src/deep/a.c", 1, UMI_STATUS_OK},
                           {"zero-directories", "src/**/a.c", "src/a.c", 1, UMI_STATUS_OK},
                           {"deep-directories", "src/**/a.c", "src/deep/a.c", 1, UMI_STATUS_OK},
                           {"leading-slash", "/src/*.c", "src/a.c", 1, UMI_STATUS_OK},
                           {"root-only", "/main.c", "src/main.c", 0, UMI_STATUS_OK},
                           {"unicode-question", "?.c", "é.c", 1, UMI_STATUS_OK},
                           {"unicode-class", "[éx].c", "é.c", 1, UMI_STATUS_OK},
                           {"negative-class", "[!ab].c", "c.c", 1, UMI_STATUS_OK},
                           {"negative-refusal", "[!ab].c", "a.c", 0, UMI_STATUS_OK},
                           {"literal-class", "[a-c].c", "b.c", 0, UMI_STATUS_OK},
                           {"literal-hyphen", "[a-c].c", "-.c", 1, UMI_STATUS_OK},
                           {"alternatives", "*.{c,h}", "a.h", 1, UMI_STATUS_OK},
                           {"nested-alternatives", "*.{c,{hpp,h}}", "a.hpp", 1, UMI_STATUS_OK},
                           {"numeric", "file{1..3}.c", "file2.c", 1, UMI_STATUS_OK},
                           {"negative-number", "file{-2..2}.c", "file-1.c", 1, UMI_STATUS_OK},
                           {"numeric-miss", "file{1..3}.c", "file4.c", 0, UMI_STATUS_OK},
                           {"literal-brace", "{literal}.c", "{literal}.c", 1, UMI_STATUS_OK},
                           {"escaped-star", "file\\*.c", "file*.c", 1, UMI_STATUS_OK},
                           {"escaped-question", "file\\?.c", "file?.c", 1, UMI_STATUS_OK},
                           {"class-metacharacters", "[ab*c{1..2}]", "*", 1, UMI_STATUS_OK},
                           {"recursive-root", "**/main.c", "main.c", 1, UMI_STATUS_OK},
                           {"recursive-many", "**/main.c", "one/two/main.c", 1, UMI_STATUS_OK},
                           {"directory-pattern", "src/", "src/main.c", 0, UMI_STATUS_OK},
                           {"traversal", "*.c", "a/../b.c", 0, UMI_STATUS_INVALID_ARGUMENT},
                           {"empty-component", "*.c", "a//b.c", 0, UMI_STATUS_INVALID_ARGUMENT},
                           {"backslash-path", "*.c", "a\\b.c", 0, UMI_STATUS_INVALID_ARGUMENT},
                           {"absolute-path", "*.c", "/a.c", 0, UMI_STATUS_INVALID_ARGUMENT},
                           {"malformed-class", "[abc", "a", 0, UMI_STATUS_PARSE_ERROR},
                           {"dangling-escape", "x\\", "x", 0, UMI_STATUS_PARSE_ERROR},
                           {"empty-pattern", "", "a", 0, UMI_STATUS_PARSE_ERROR},
                           {"empty-path", "*", "", 0, UMI_STATUS_INVALID_ARGUMENT},
                           {"wide-numeric-range", "{1..300}.c", "1.c", 1, UMI_STATUS_OK},
                           {"leading-zero-number", "file{1..3}.c", "file02.c", 1, UMI_STATUS_OK},
                           {"positive-number", "file{1..3}.c", "file+2.c", 1, UMI_STATUS_OK},
                           {"drive-path", "*", "C:/a.c", 0, UMI_STATUS_INVALID_ARGUMENT}};
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i].name)) {
      int match = 73;
      CHECK(UmiEditorConfigPatternMatch(cases[i].pattern, cases[i].file, &match) ==
            cases[i].status);
      CHECK(match == (cases[i].status == UMI_STATUS_OK ? cases[i].matches : 73));
      return 0;
    }
  if (!strcmp(argv[1], "arguments")) {
    int m = 42;
    CHECK(UmiEditorConfigPatternMatch(NULL, "a", &m) == UMI_STATUS_INVALID_ARGUMENT && m == 42);
    CHECK(UmiEditorConfigPatternMatch("*", NULL, &m) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorConfigPatternMatch("*", "a", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  if (!strcmp(argv[1], "pattern-limit")) {
    char p[1026];
    memset(p, 'a', sizeof p);
    p[1025] = 0;
    int m = 42;
    CHECK(UmiEditorConfigPatternMatch(p, "a", &m) == UMI_STATUS_CAPACITY_EXCEEDED && m == 42);
    return 0;
  }
  if (!strcmp(argv[1], "path-limit")) {
    char p[2050];
    memset(p, 'a', sizeof p);
    p[2049] = 0;
    int m = 42;
    CHECK(UmiEditorConfigPatternMatch("*", p, &m) == UMI_STATUS_CAPACITY_EXCEEDED && m == 42);
    return 0;
  }
  if (!strcmp(argv[1], "expansion-limit")) {
    int m = 42;
    CHECK(UmiEditorConfigPatternMatch("{a,b}{a,b}{a,b}{a,b}{a,b}{a,b}{a,b}{a,b}{a,b}", "a", &m) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          m == 42);
    return 0;
  }
  if (!strcmp(argv[1], "malformed-utf8")) {
    int m = 42;
    CHECK(UmiEditorConfigPatternMatch("?", "\xc0\xaf", &m) != UMI_STATUS_OK && m == 42);
    return 0;
  }
  if (!strcmp(argv[1], "work-budget")) {
    char pattern[1025] = "", file[2049];
    for (unsigned i = 0; i < 8U; ++i)
      strcat(pattern, "{a,b}");
    for (unsigned i = 0; i < 450U; ++i)
      strcat(pattern, "?x");
    memset(file, 'a', sizeof file - 1U);
    file[sizeof file - 1U] = 0;
    int matches = 42;
    CHECK(UmiEditorConfigPatternMatch(pattern, file, &matches) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(matches == 42);
    return 0;
  }
  return 2;
}
