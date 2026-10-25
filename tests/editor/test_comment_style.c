/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_comment_style.c
 *
 * PURPOSE:
 *   Check conservative language style discovery, bounded identifiers and stable static catalogue
 * ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/comment_style.h"
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
  const char *name = argv[1], *names[] = {"catalogue", "lookup",  "unknown",    "arguments",
                                          "bounded",   "literal", "unsupported"};
  int known = 0;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known |= !strcmp(name, names[i]);
  if (!known)
    return 2;
  const UmiEditorLineCommentStyle *style = UmiEditorLineCommentStyleAt(0U), *guard = style;
  CHECK(style != NULL);
  if (!strcmp(name, "catalogue")) {
    for (size_t i = 0U; i < UmiEditorLineCommentStyleCount(); ++i) {
      const UmiEditorLineCommentStyle *p = UmiEditorLineCommentStyleAt(i);
      CHECK(p && p->language_id[0] && p->display_name[0]);
      CHECK(UmiEditorLineCommentStyleFind(p->language_id, &style) == UMI_STATUS_OK && style == p);
      CHECK(strlen(p->prefix) >= 1U && strlen(p->prefix) <= 2U);
      for (size_t j = 0U; j < i; ++j)
        CHECK(strcmp(p->language_id, UmiEditorLineCommentStyleAt(j)->language_id));
    }
    CHECK(UmiEditorLineCommentStyleAt(UmiEditorLineCommentStyleCount()) == NULL);
  } else if (!strcmp(name, "lookup")) {
    CHECK(UmiEditorLineCommentStyleFind("python3", &style) == UMI_STATUS_OK &&
          !strcmp(style->prefix, "#"));
    CHECK(UmiEditorLineCommentStyleFind("sql", &style) == UMI_STATUS_OK &&
          !strcmp(style->prefix, "--"));
    CHECK(UmiEditorLineCommentStyleFind("c", &style) == UMI_STATUS_OK &&
          !strcmp(style->prefix, "//"));
  } else if (!strcmp(name, "unknown"))
    CHECK(UmiEditorLineCommentStyleFind("Python", &style) == UMI_STATUS_NOT_FOUND &&
          style == guard);
  else if (!strcmp(name, "arguments")) {
    CHECK(UmiEditorLineCommentStyleFind(NULL, &style) == UMI_STATUS_INVALID_ARGUMENT &&
          style == guard);
    CHECK(UmiEditorLineCommentStyleFind("", &style) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEditorLineCommentStyleFind("c", NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "bounded")) {
    char id[128];
    memset(id, 'x', sizeof id);
    CHECK(UmiEditorLineCommentStyleFind(id, &style) == UMI_STATUS_CAPACITY_EXCEEDED &&
          style == guard);
  } else if (!strcmp(name, "literal")) {
    CHECK(UmiEditorLineCommentStyleFind("ini", &style) == UMI_STATUS_OK &&
          !strcmp(style->prefix, ";"));
    CHECK(UmiEditorLineCommentStyleFind("lua", &style) == UMI_STATUS_OK &&
          !strcmp(style->prefix, "--"));
  } else {
    const char *unknown[] = {"json", "html", "css", "markdown", "asm", "plain"};
    for (size_t i = 0U; i < sizeof unknown / sizeof unknown[0]; ++i)
      CHECK(UmiEditorLineCommentStyleFind(unknown[i], &style) == UMI_STATUS_NOT_FOUND);
  }
  return 0;
}
