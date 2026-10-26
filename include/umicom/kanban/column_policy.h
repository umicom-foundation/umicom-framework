/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/column_policy.h
 *
 * PURPOSE:
 *   Review list occupancy and change its work limit at an observed board revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_COLUMN_POLICY_H
#define UMICOM_KANBAN_COLUMN_POLICY_H
#include "umicom/kanban/board.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiKanbanColumnPolicySnapshot {
    uint64_t board_revision;
    UmiKanbanColumn column;
    size_t active_cards;
    size_t board_capacity;
} UmiKanbanColumnPolicySnapshot;
/* Copy the named column and active occupancy at exactly revision. Archived
 * cards are excluded. Failure leaves out unchanged. Serialize board access. */
UmiStatus UmiKanbanColumnPolicyRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *column_id, UmiKanbanColumnPolicySnapshot *out);
/* Zero removes the limit; a nonzero limit bounds all active cards in that list,
 * including Done or Blocked lists. A limit below current occupancy is refused,
 * never evicts/moves cards, and does not erase archived cards. The existing
 * SET_WIP transaction owns final validation and activity. A stale review is
 * refused. No I/O/callbacks; explicit document save retains the accepted edit.
 * Actor is nonempty local attribution, not an authentication credential. */
UmiStatus UmiKanbanColumnPolicyApply(UmiKanbanBoard *board, uint64_t revision,
    const char *column_id, const char *actor, size_t limit);
#ifdef __cplusplus
}
#endif
#endif
