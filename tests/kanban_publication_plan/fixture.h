/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_publication_plan/fixture.h
 *
 * PURPOSE:
 *   Prepare isolated boards and reviewed public fields without private data publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_PUBLIC_PLAN_FIXTURE_H
#define UMICOM_TEST_PUBLIC_PLAN_FIXTURE_H
#include "../kanban/fixture.h"
#include "umicom/kanban/publication_plan.h"
static inline UmiKanbanBoard *PlanBoard(void) {
  UmiKanbanBoard *board = NULL;
  REQUIRE(umi_kanban_board_create_workflow("private.board", "PRIVATE_BOARD_TITLE", &board) ==
          UMI_STATUS_OK);
  UmiKanbanEdit *e = Edits(2U);
  Create(&e[0], "card.first");
  Create(&e[1], "card.other");
  strcpy(e[0].card.description, "PRIVATE_DESCRIPTION");
  strcpy(e[0].card.assignee, "private.person");
  Apply(board, e, 2U);
  free(e);
  return board;
}
static inline UmiKanbanPublicCard PublicCard(const char *id) {
  UmiKanbanPublicCard c = {0};
  strcpy(c.card_id, id);
  strcpy(c.title, "Reviewed feature");
  strcpy(c.summary, "Reviewed summary");
  strcpy(c.developer, "Approved name");
  return c;
}
static inline UmiKanbanPublicationPlan *Plan(UmiKanbanBoard *board) {
  UmiKanbanPublicCard cards[2] = {PublicCard("card.first"), PublicCard("card.other")};
  UmiKanbanPublicationPlan *plan = NULL;
  REQUIRE(UmiKanbanPublicationPlanCreate(board, umi_kanban_board_revision(board), "Public roadmap",
                                         cards, 2U, &plan) == UMI_STATUS_OK);
  return plan;
}
static inline unsigned char *PlanBytes(const UmiKanbanPublicationPlan *plan, size_t *size) {
  REQUIRE(UmiKanbanPublicationPlanEncode(plan, NULL, 0U, size) == UMI_STATUS_OK);
  unsigned char *bytes = malloc(*size);
  REQUIRE(bytes != NULL);
  REQUIRE(UmiKanbanPublicationPlanEncode(plan, bytes, *size, size) == UMI_STATUS_OK);
  return bytes;
}
static inline size_t FindBytes(const unsigned char *bytes, size_t size, const char *text) {
  size_t n = strlen(text);
  for (size_t i = 0U; n <= size && i <= size - n; ++i)
    if (!memcmp(bytes + i, text, n))
      return i;
  REQUIRE(0);
  return 0U;
}
static inline void CaseKnown(const char *name, const char *const *cases, size_t count) {
  for (size_t i = 0U; i < count; ++i)
    if (!strcmp(name, cases[i]))
      return;
  REQUIRE(0);
}
#endif
