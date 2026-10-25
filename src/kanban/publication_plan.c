/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/publication_plan.c
 *
 * PURPOSE:
 *   Own reviewed publication fields and require explicit revision review before rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "board_internal.h"
#include "publication_plan_internal.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiKanbanPublicationPlanValidate(const UmiKanbanPublicationPlan *plan) {
  if (plan == NULL || !UmiKanbanId(plan->info.board_id, false) ||
      !UmiKanbanPublicText(plan->info.title, sizeof plan->info.title, false) ||
      plan->info.reviewed_revision == 0U || plan->info.card_count > UMI_KANBAN_MAX_CARDS ||
      (plan->info.card_count != 0U && plan->cards == NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0U; i < plan->info.card_count; ++i) {
    const UmiKanbanPublicCard *c = &plan->cards[i];
    if (!UmiKanbanId(c->card_id, false) || !UmiKanbanPublicText(c->title, sizeof c->title, false) ||
        !UmiKanbanPublicText(c->summary, sizeof c->summary, true) ||
        !UmiKanbanPublicText(c->developer, sizeof c->developer, true))
      return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t j = 0U; j < i; ++j)
      if (!strcmp(c->card_id, plan->cards[j].card_id))
        return UMI_STATUS_ALREADY_EXISTS;
  }
  return UMI_STATUS_OK;
}
void UmiKanbanPublicationPlanDestroy(UmiKanbanPublicationPlan *plan) {
  if (plan != NULL) {
    free(plan->cards);
    free(plan);
  }
}
UmiStatus UmiKanbanPublicationPlanCreate(const UmiKanbanBoard *board, uint64_t revision,
                                         const char *title, const UmiKanbanPublicCard *cards,
                                         size_t count, UmiKanbanPublicationPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  size_t size = 0U;
  UmiStatus status =
      umi_kanban_publication_html(board, revision, title, cards, count, NULL, 0U, &size);
  if (status != UMI_STATUS_OK)
    return status;
  UmiKanbanConfig config;
  status = umi_kanban_board_config(board, &config);
  if (status != UMI_STATUS_OK)
    return status;
  UmiKanbanPublicationPlan *plan = calloc(1U, sizeof *plan);
  if (plan == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  if (count != 0U) {
    plan->cards = calloc(count, sizeof *plan->cards);
    if (plan->cards == NULL) {
      free(plan);
      return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(plan->cards, cards, count * sizeof *cards);
  }
  memcpy(plan->info.board_id, config.id, strlen(config.id) + 1U);
  memcpy(plan->info.title, title, strlen(title) + 1U);
  plan->info.reviewed_revision = revision;
  plan->info.card_count = count;
  *out = plan;
  return UMI_STATUS_OK;
}
UmiStatus UmiKanbanPublicationPlanRead(const UmiKanbanPublicationPlan *plan,
                                       UmiKanbanPublicationPlanInfo *out) {
  if (plan == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = plan->info;
  return UMI_STATUS_OK;
}
const UmiKanbanPublicCard *UmiKanbanPublicationPlanAt(const UmiKanbanPublicationPlan *plan,
                                                      size_t index) {
  return plan != NULL && index < plan->info.card_count ? &plan->cards[index] : NULL;
}
static UmiStatus Identity(const UmiKanbanPublicationPlan *plan, const UmiKanbanBoard *board,
                          uint64_t revision) {
  UmiStatus status = UmiKanbanPublicationPlanValidate(plan);
  if (status != UMI_STATUS_OK || board == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiKanbanConfig config;
  status = umi_kanban_board_config(board, &config);
  if (status != UMI_STATUS_OK)
    return status;
  if (strcmp(config.id, plan->info.board_id) || umi_kanban_board_revision(board) != revision)
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}
UmiStatus UmiKanbanPublicationPlanCheck(const UmiKanbanPublicationPlan *plan,
                                        const UmiKanbanBoard *board, uint64_t revision) {
  UmiStatus status = Identity(plan, board, revision);
  if (status != UMI_STATUS_OK)
    return status;
  if (plan->info.reviewed_revision != revision)
    return UMI_STATUS_INVALID_STATE;
  size_t size = 0U;
  return umi_kanban_publication_html(board, revision, plan->info.title, plan->cards,
                                     plan->info.card_count, NULL, 0U, &size);
}
UmiStatus UmiKanbanPublicationPlanReview(const UmiKanbanPublicationPlan *plan,
                                         const UmiKanbanBoard *board, uint64_t revision,
                                         UmiKanbanPublicationPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  UmiStatus status = Identity(plan, board, revision);
  return status == UMI_STATUS_OK
             ? UmiKanbanPublicationPlanCreate(board, revision, plan->info.title, plan->cards,
                                              plan->info.card_count, out)
             : status;
}
UmiStatus UmiKanbanPublicationPlanHtml(const UmiKanbanPublicationPlan *plan,
                                       const UmiKanbanBoard *board, uint64_t revision, char *bytes,
                                       size_t capacity, size_t *out_size) {
  UmiStatus status = UmiKanbanPublicationPlanCheck(plan, board, revision);
  return status == UMI_STATUS_OK
             ? umi_kanban_publication_html(board, revision, plan->info.title, plan->cards,
                                           plan->info.card_count, bytes, capacity, out_size)
             : status;
}
