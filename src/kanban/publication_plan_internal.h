/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/publication_plan_internal.h
 *
 * PURPOSE:
 *   Keep immutable publication-plan ownership and shared text validation private.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_PUBLICATION_PLAN_INTERNAL_H
#define UMICOM_KANBAN_PUBLICATION_PLAN_INTERNAL_H
#include "umicom/kanban/publication_plan.h"
#include <stdbool.h>
struct UmiKanbanPublicationPlan {
  UmiKanbanPublicationPlanInfo info;
  UmiKanbanPublicCard *cards;
};
bool UmiKanbanPublicText(const char *text, size_t capacity, bool optional);
UmiStatus UmiKanbanPublicationPlanValidate(const UmiKanbanPublicationPlan *plan);
#endif
