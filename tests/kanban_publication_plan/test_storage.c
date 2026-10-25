/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_publication_plan/test_storage.c
 *
 * PURPOSE:
 *   Verify private plan replacement and refusal without damaging stored selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "fixture.h"
#include "umicom/kanban/publication_plan_storage.h"
#include "umicom/platform/rooted_files.h"
int main(int argc, char **argv) {
  REQUIRE(argc == 2);
  const char *name = argv[1],
             *cases[] = {"roundtrip",     "replace", "missing",          "malformed",
                         "wrong-suffix",  "nested",  "relative",         "missing-directory",
                         "short-archive", "null",    "preserve-invalid", "unicode"};
  CaseKnown(name, cases, sizeof cases / sizeof cases[0]);
  char directory[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  UmiKanbanBoard *board = PlanBoard();
  UmiKanbanPublicationPlan *plan = Plan(board), *loaded = NULL;
  const char *leaf = !strcmp(name, "unicode") ? "caf\xc3\xa9.umipublic" : "review.umipublic";
  UmiStatus status;
  if (!strcmp(name, "missing"))
    REQUIRE(UmiKanbanPublicationPlanLoad(directory, leaf, &loaded) != UMI_STATUS_OK &&
            loaded == NULL);
  else if (!strcmp(name, "wrong-suffix") || !strcmp(name, "nested") || !strcmp(name, "relative") ||
           !strcmp(name, "missing-directory")) {
    const char *filename = !strcmp(name, "nested")         ? "child/review.umipublic"
                           : !strcmp(name, "wrong-suffix") ? "index.html"
                                                           : leaf;
    char absent[UMI_PATH_CAPACITY];
    FixturePath(absent, directory, "absent");
    const char *root = !strcmp(name, "relative")            ? "."
                       : !strcmp(name, "missing-directory") ? absent
                                                            : directory;
    REQUIRE(UmiKanbanPublicationPlanSave(root, filename, plan) != UMI_STATUS_OK);
  } else if (!strcmp(name, "malformed") || !strcmp(name, "short-archive")) {
    size_t n;
    unsigned char *bytes = PlanBytes(plan, &n);
    if (!strcmp(name, "malformed"))
      bytes[n - 1U] ^= 1U;
    else
      --n;
    REQUIRE(UmiRootedFileWrite(directory, leaf, bytes, n) == UMI_STATUS_OK);
    free(bytes);
    REQUIRE(UmiKanbanPublicationPlanLoad(directory, leaf, &loaded) != UMI_STATUS_OK &&
            loaded == NULL);
  } else if (!strcmp(name, "null")) {
    REQUIRE(UmiKanbanPublicationPlanSave(directory, leaf, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiKanbanPublicationPlanLoad(directory, leaf, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else {
    REQUIRE(UmiKanbanPublicationPlanSave(directory, leaf, plan) == UMI_STATUS_OK);
    if (!strcmp(name, "replace")) {
      UmiKanbanPublicationPlanDestroy(plan);
      REQUIRE(UmiKanbanPublicationPlanCreate(board, umi_kanban_board_revision(board),
                                             "New public title", NULL, 0U, &plan) == UMI_STATUS_OK);
      REQUIRE(UmiKanbanPublicationPlanSave(directory, leaf, plan) == UMI_STATUS_OK);
    }
    if (!strcmp(name, "preserve-invalid"))
      REQUIRE(UmiKanbanPublicationPlanSave(directory, leaf, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    status = UmiKanbanPublicationPlanLoad(directory, leaf, &loaded);
    REQUIRE(status == UMI_STATUS_OK);
    UmiKanbanPublicationPlanInfo info;
    REQUIRE(UmiKanbanPublicationPlanRead(loaded, &info) == UMI_STATUS_OK);
    REQUIRE(info.card_count == (!strcmp(name, "replace") ? 0U : 2U));
    REQUIRE(UmiKanbanPublicationPlanCheck(loaded, board, umi_kanban_board_revision(board)) ==
            UMI_STATUS_OK);
  }
  UmiKanbanPublicationPlanDestroy(loaded);
  UmiKanbanPublicationPlanDestroy(plan);
  umi_kanban_board_destroy(board);
  return 0;
}
