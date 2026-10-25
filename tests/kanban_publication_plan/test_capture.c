/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_publication_plan/test_capture.c
 *
 * PURPOSE:
 *   Verify plan ownership, selection admission and isolation from private board text.
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
             *cases[] = {"normal",    "empty",          "owned",        "after-destroy",
                         "null",      "stale",          "missing",      "archived",
                         "duplicate", "title-empty",    "invalid-utf8", "unterminated",
                         "bounds",    "private-fields", "unicode",      "card-order"};
  CaseKnown(name, cases, sizeof cases / sizeof cases[0]);
  UmiKanbanBoard *board = PlanBoard();
  UmiKanbanPublicCard cards[2] = {PublicCard("card.first"), PublicCard("card.other")};
  const char *title = "Public roadmap";
  uint64_t revision = umi_kanban_board_revision(board);
  size_t count = 2U;
  UmiStatus expected = UMI_STATUS_OK;
  UmiKanbanPublicationPlan *plan = NULL;
  if (!strcmp(name, "empty"))
    count = 0U;
  if (!strcmp(name, "stale")) {
    --revision;
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "missing")) {
    strcpy(cards[0].card_id, "missing");
    expected = UMI_STATUS_NOT_FOUND;
  }
  if (!strcmp(name, "archived")) {
    UmiKanbanEdit e;
    Action(&e, UMI_KANBAN_ARCHIVE_CARD, "card.first", NULL);
    Apply(board, &e, 1U);
    revision = umi_kanban_board_revision(board);
    expected = UMI_STATUS_INVALID_STATE;
  }
  if (!strcmp(name, "duplicate")) {
    cards[1] = cards[0];
    expected = UMI_STATUS_ALREADY_EXISTS;
  }
  if (!strcmp(name, "title-empty")) {
    title = "";
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "invalid-utf8")) {
    strcpy(cards[0].summary, "\xc0\xaf");
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "unterminated")) {
    memset(cards[0].developer, 'x', sizeof cards[0].developer);
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "unicode"))
    strcpy(cards[0].title, "Caf\xc3\xa9");
  if (!strcmp(name, "bounds")) {
    count = UMI_KANBAN_MAX_CARDS + 1U;
    expected = UMI_STATUS_INVALID_ARGUMENT;
  }
  if (!strcmp(name, "null")) {
    REQUIRE(UmiKanbanPublicationPlanCreate(board, revision, title, cards, 2U, NULL) ==
            UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiKanbanPublicationPlanCreate(NULL, revision, title, cards, 2U, &plan) ==
                UMI_STATUS_INVALID_ARGUMENT &&
            plan == NULL);
  } else {
    REQUIRE(UmiKanbanPublicationPlanCreate(board, revision, title, cards, count, &plan) ==
            expected);
    if (expected == UMI_STATUS_OK) {
      UmiKanbanPublicationPlanInfo info;
      REQUIRE(UmiKanbanPublicationPlanRead(plan, &info) == UMI_STATUS_OK);
      REQUIRE(info.card_count == count && !strcmp(info.board_id, "private.board") &&
              !strcmp(info.title, title));
      if (!strcmp(name, "owned")) {
        strcpy(cards[0].title, "Changed");
        REQUIRE(!strcmp(UmiKanbanPublicationPlanAt(plan, 0U)->title, "Reviewed feature"));
      }
      if (!strcmp(name, "after-destroy")) {
        umi_kanban_board_destroy(board);
        board = NULL;
        REQUIRE(UmiKanbanPublicationPlanAt(plan, 1U) != NULL);
      }
      if (!strcmp(name, "private-fields")) {
        size_t n;
        unsigned char *bytes = PlanBytes(plan, &n);
        for (size_t i = 0U; i + 8U <= n; ++i)
          REQUIRE(memcmp(bytes + i, "PRIVATE_", 8U));
        free(bytes);
      }
      if (!strcmp(name, "card-order"))
        REQUIRE(!strcmp(UmiKanbanPublicationPlanAt(plan, 1U)->card_id, "card.other"));
      REQUIRE(UmiKanbanPublicationPlanAt(plan, count) == NULL);
    } else
      REQUIRE(plan == NULL);
  }
  UmiKanbanPublicationPlanDestroy(plan);
  umi_kanban_board_destroy(board);
  return 0;
}
