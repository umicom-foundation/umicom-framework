/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/card_details.h
 *
 * PURPOSE:
 *   Edit a bounded subset of card fields without replacing unrelated planning state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_KANBAN_CARD_DETAILS_H
#define UMICOM_KANBAN_CARD_DETAILS_H
#include "umicom/kanban/board.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiKanbanCardDetails {
    char title[256];
    char description[2048];
    char blocked_reason[256];
    UmiKanbanPriority priority;
} UmiKanbanCardDetails;
typedef struct UmiKanbanCardDetailSnapshot {
    char card_id[UMI_KANBAN_ID_SIZE];
    uint64_t board_revision;
    UmiKanbanCardDetails details;
} UmiKanbanCardDetailSnapshot;
/* Validate bounded NUL-terminated byte strings and priority. Title is required;
 * description and blocker may be empty. Native hosts separately require UTF-8.
 * No I/O, callbacks, allocation or board changes occur. */
UmiStatus UmiKanbanCardDetailsCheck(const UmiKanbanCardDetails *details);
/* Copy the current active card at exactly revision. Archived and stale cards
 * are refused. Failure leaves out unchanged. Serialize access to the board. */
UmiStatus UmiKanbanCardDetailsRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardDetailSnapshot *out);
/* Change only title, description, priority and blocker through the normal
 * atomic board transaction. Preserve assignee, checks, dependencies, labels,
 * due date, column and row order. A stale form cannot silently replace newer
 * details. Existing Done/checklist/dependency rules remain authoritative.
 * actor is local attribution, not authentication. A cleared blocker does not
 * move a card out of the Blocked column. The caller explicitly saves its board.
 * Inputs/outputs must be separate from one another and the live board. */
UmiStatus UmiKanbanCardDetailsApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, const UmiKanbanCardDetails *details);

/* A checklist is an owner's acceptance statement, never automatic proof that
 * a test ran. Replacing its bounded rows is an explicit, revision-checked edit. */
typedef struct UmiKanbanCardChecklist {
    size_t count;
    UmiKanbanCheck checks[UMI_KANBAN_MAX_CHECKS];
} UmiKanbanCardChecklist;
typedef struct UmiKanbanCardChecklistSnapshot {
    char card_id[UMI_KANBAN_ID_SIZE];
    uint64_t board_revision;
    UmiKanbanCardChecklist checklist;
} UmiKanbanCardChecklistSnapshot;
UmiStatus UmiKanbanCardChecklistRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardChecklistSnapshot *out);
/* Replace only checklist rows; count zero explicitly clears them. Rows beyond
 * count are ignored and are not stored. Each retained text is nonempty and
 * NUL-terminated, and complete is exactly 0 or 1. Unchecking an item on a Done
 * card is refused by normal board validation; move the card back first.
 * Refusal preserves the entire board. No callbacks or I/O take place. */
UmiStatus UmiKanbanCardChecklistApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, const UmiKanbanCardChecklist *checklist);

#ifdef __cplusplus
}
#endif
#endif
