/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/completion_plan_internal.h
 *
 * PURPOSE:
 *   Keep completion review ownership and validation private to the Kanban component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_KANBAN_COMPLETION_PLAN_INTERNAL_H
#define UMICOM_KANBAN_COMPLETION_PLAN_INTERNAL_H
#include "umicom/kanban/completion_plan.h"
struct UmiKanbanCompletionPlan {
    UmiKanbanCompletionPlanInfo info;
    UmiKanbanCompletionRecord *records;
};
UmiStatus UmiKanbanCompletionValidate(const UmiKanbanCompletionPlan *plan);
UmiStatus UmiKanbanCompletionCaptureBoard(const UmiKanbanBoard *board, uint64_t revision,
    char digest[UMI_SHA256_HEX_CAPACITY], UmiKanbanBoard **copy);
UmiStatus UmiKanbanCompletionMove(const UmiKanbanCompletionPlan *plan, UmiKanbanBoard *board);
#endif
