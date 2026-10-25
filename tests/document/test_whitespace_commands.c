/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_whitespace_commands.c
 *
 * PURPOSE:
 *   Qualify the shared whitespace catalogue and per-document action policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/whitespace_commands.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  const char *names[] = {"catalogue", "defaults", "override", "unknown", "closed", "invalid"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  CHECK(known);
  CHECK(UmiDocumentWhitespaceCommandCount() == 6U && UmiDocumentWhitespaceCommandAt(6U) == NULL);
  for (size_t i = 0U; i < 6U; ++i) {
    const UmiDocumentWhitespaceCommand *c = UmiDocumentWhitespaceCommandAt(i);
    CHECK(c && c->id[0] && c->label[0] && c->keywords[0]);
    for (size_t j = 0U; j < i; ++j)
      CHECK(strcmp(c->id, UmiDocumentWhitespaceCommandAt(j)->id));
  }
  ReplacementFixture f = {0};
  CHECK(Start(&f, "\tx") == 0);
  UmiEditorIndentation policy = {8U, 0};
  CHECK(UmiDocumentCoordinatorSetIndentation(f.documents, &policy) == UMI_STATUS_OK);
  if (!strcmp(name, "override")) {
    UmiDocumentIndentationReview r;
    policy.tab_width = 2U;
    CHECK(UmiDocumentCoordinatorInspectIndentation(f.documents, f.id, &r) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorApplyIndentation(f.documents, &r, &policy, NULL) == UMI_STATUS_OK);
  }
  if (!strcmp(name, "closed"))
    CHECK(UmiDocumentCoordinatorClose(f.documents, f.id, 1) == UMI_STATUS_OK);
  for (size_t i = 0U; i < 6U; ++i) {
    UmiEditorWhitespaceOptions options = {0};
    options.tab_width = 99U;
    const UmiDocumentWhitespaceCommand *c = UmiDocumentWhitespaceCommandAt(i);
    UmiStatus status = UmiDocumentWhitespaceCommandOptions(
        f.documents, f.id, !strcmp(name, "unknown") ? "unknown" : c->id, &options);
    if (!strcmp(name, "closed"))
      CHECK(status == UMI_STATUS_NOT_FOUND && options.tab_width == 99U);
    else if (!strcmp(name, "unknown"))
      CHECK(status == UMI_STATUS_NOT_FOUND && options.tab_width == 99U);
    else
      CHECK(status == UMI_STATUS_OK && options.tab_width == policy.tab_width &&
            options.operation == c->operation && options.selected_lines == c->selected_lines);
  }
  if (!strcmp(name, "invalid"))
    CHECK(UmiDocumentWhitespaceCommandOptions(NULL, f.id, "editor.trim-trailing-lines", NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
  Stop(&f);
  return 0;
}
