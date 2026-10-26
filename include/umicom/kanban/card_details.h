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
/* Copy active checklist rows at exactly revision. Archived, missing and stale
 * cards are refused; failure leaves out unchanged. Outputs do not alias inputs. */
UmiStatus UmiKanbanCardChecklistRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardChecklistSnapshot *out);
/* Replace only checklist rows; count zero explicitly clears them. Rows beyond
 * count are ignored and are not stored. Each retained text is nonempty and
 * NUL-terminated, and complete is exactly 0 or 1. Unchecking an item on a Done
 * card is refused by normal board validation; move the card back first.
 * Refusal preserves the entire board. No callbacks or I/O take place. */
UmiStatus UmiKanbanCardChecklistApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, const UmiKanbanCardChecklist *checklist);


/* Add/remove one prerequisite without replacing any other card data. Both
 * identifiers must be valid, active existing cards where required by normal
 * board rules. Adding a duplicate returns ALREADY_EXISTS; removing an absent
 * link returns NOT_FOUND. Self-links, cycles and invalid Done dependencies
 * remain rejected by the existing atomic transaction. Serialize board access. */
UmiStatus UmiKanbanCardDependencyAdd(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *required_card_id, const char *actor);
UmiStatus UmiKanbanCardDependencyRemove(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *required_card_id, const char *actor);


/* Labels are private case-sensitive text, not developer IDs. The active rows
 * must be nonempty and distinct. Unused slots are ignored and cleared on edit. */
typedef struct UmiKanbanCardLabels {
    size_t count;
    char labels[UMI_KANBAN_MAX_LABELS][UMI_KANBAN_ID_SIZE];
} UmiKanbanCardLabels;
typedef struct UmiKanbanCardLabelsSnapshot {
    char card_id[UMI_KANBAN_ID_SIZE];
    uint64_t board_revision;
    UmiKanbanCardLabels labels;
} UmiKanbanCardLabelsSnapshot;
/* Copy labels of an active card at exactly revision; refusal leaves out alone. */
UmiStatus UmiKanbanCardLabelsRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardLabelsSnapshot *out);
/* Replace only labels through the existing atomic transaction. Count zero
 * explicitly clears them. No date, assignment, checklist, dependency, column
 * or ordering changes are implied. Native callers validate UTF-8 separately.
 * Requires nonempty actor attribution; this does not grant authorization. */
UmiStatus UmiKanbanCardLabelsApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, const UmiKanbanCardLabels *labels);


/* Deadlines remain absolute UTC seconds in the portable model. Zero is the
 * established no-deadline sentinel, so Unix epoch zero cannot represent a due
 * instant. No current clock, local timezone or business-calendar rule is read. */
typedef struct UmiKanbanCardDeadlineSnapshot {
    char card_id[UMI_KANBAN_ID_SIZE];
    uint64_t board_revision;
    uint64_t due_unix_seconds;
} UmiKanbanCardDeadlineSnapshot;
UmiStatus UmiKanbanCardDeadlineRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardDeadlineSnapshot *out);
/* Change only the deadline of a current active card; zero explicitly clears it.
 * Full uint64_t storage is retained for compatibility. A frontend may support
 * a narrower civil-date display range but must never silently clamp a value.
 * Actor is nonempty attribution. Refusal preserves the entire board. */
UmiStatus UmiKanbanCardDeadlineApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, uint64_t due_unix_seconds);

#ifdef __cplusplus
}
#endif
#endif
