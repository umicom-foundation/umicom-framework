/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_publication_plan/test_review.c
 *
 * PURPOSE:
 *   Verify exact revision checks and explicit adoption without private text refresh.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  REQUIRE(argc == 2);
  const char *name = argv[1],
             *cases[] = {"current",         "moved",         "changed-title", "archived",
                         "different-board", "stale-request", "empty",         "old-unchanged",
                         "render-stale",    "render-short"};
  CaseKnown(name, cases, sizeof cases / sizeof cases[0]);
  UmiKanbanBoard *board = PlanBoard();
  UmiKanbanPublicationPlan *plan = Plan(board), *adopted = NULL;
  uint64_t old = umi_kanban_board_revision(board);
  if (!strcmp(name, "empty")) {
    UmiKanbanPublicationPlanDestroy(plan);
    REQUIRE(UmiKanbanPublicationPlanCreate(board, old, "Empty", NULL, 0U, &plan) == UMI_STATUS_OK);
  }
  UmiKanbanEdit edit;
  if (!strcmp(name, "moved") || !strcmp(name, "old-unchanged") || !strcmp(name, "render-stale"))
    Move(board, &edit, "card.first", "doing");
  if (!strcmp(name, "changed-title")) {
    UmiKanbanCardSnapshot snapshot;
    REQUIRE(umi_kanban_board_find(board, "card.first", &snapshot) == UMI_STATUS_OK);
    memset(&edit, 0, sizeof edit);
    edit.kind = UMI_KANBAN_UPDATE_CARD;
    edit.card = snapshot.card;
    strcpy(edit.card.title, "PRIVATE_CHANGED");
    Apply(board, &edit, 1U);
  }
  if (!strcmp(name, "archived")) {
    Action(&edit, UMI_KANBAN_ARCHIVE_CARD, "card.first", NULL);
    Apply(board, &edit, 1U);
  }
  if (!strcmp(name, "different-board")) {
    umi_kanban_board_destroy(board);
    REQUIRE(umi_kanban_board_create_workflow("other.board", "PRIVATE_BOARD_TITLE", &board) ==
            UMI_STATUS_OK);
  }
  uint64_t current = umi_kanban_board_revision(board);
  if (!strcmp(name, "render-stale")) {
    char out[5] = "keep";
    size_t n = 99U;
    REQUIRE(UmiKanbanPublicationPlanHtml(plan, board, current, out, sizeof out, &n) ==
            UMI_STATUS_INVALID_STATE);
    REQUIRE(!strcmp(out, "keep") && n == 99U);
  } else {
    UmiStatus status = UmiKanbanPublicationPlanReview(
        plan, board, !strcmp(name, "stale-request") ? current - 1U : current, &adopted);
    if (!strcmp(name, "archived") || !strcmp(name, "different-board") ||
        !strcmp(name, "stale-request"))
      REQUIRE(status == UMI_STATUS_INVALID_STATE && adopted == NULL);
    else {
      REQUIRE(status == UMI_STATUS_OK &&
              UmiKanbanPublicationPlanCheck(adopted, board, current) == UMI_STATUS_OK);
      UmiKanbanPublicationPlanInfo original;
      REQUIRE(UmiKanbanPublicationPlanRead(plan, &original) == UMI_STATUS_OK &&
              original.reviewed_revision == old);
      size_t n;
      REQUIRE(UmiKanbanPublicationPlanHtml(adopted, board, current, NULL, 0U, &n) == UMI_STATUS_OK);
      char *html = malloc(n);
      REQUIRE(html != NULL);
      memset(html, 'x', n);
      if (!strcmp(name, "render-short")) {
        REQUIRE(UmiKanbanPublicationPlanHtml(adopted, board, current, html, n - 1U, &n) ==
                UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(html[0] == 'x');
      } else {
        REQUIRE(UmiKanbanPublicationPlanHtml(adopted, board, current, html, n, &n) ==
                UMI_STATUS_OK);
        REQUIRE(strstr(html, "PRIVATE_") == NULL && strstr(html, "private.board") == NULL);
      }
      free(html);
    }
  }
  UmiKanbanPublicationPlanDestroy(adopted);
  UmiKanbanPublicationPlanDestroy(plan);
  umi_kanban_board_destroy(board);
  return 0;
}
