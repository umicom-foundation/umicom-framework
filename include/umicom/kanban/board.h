/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/board.h
 * PURPOSE: Share reviewed planning workflows across native applications without executing work.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KANBAN_BOARD_H
#define UMICOM_KANBAN_BOARD_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_KANBAN_ID_SIZE 64U
#define UMI_KANBAN_MAX_COLUMNS 16U
#define UMI_KANBAN_MAX_CARDS 1024U
#define UMI_KANBAN_MAX_CHECKS 16U
#define UMI_KANBAN_MAX_DEPENDENCIES 16U
#define UMI_KANBAN_MAX_LABELS 8U
#define UMI_KANBAN_MAX_EDITS 128U
#define UMI_KANBAN_ACTIVITY_CAPACITY 256U

typedef struct UmiKanbanBoard UmiKanbanBoard;
/* Blocked is appended below so existing stage values, including Done, retain their archive and API identity. Retained for engineering review. */
#if 0
typedef enum UmiKanbanStage {
    UMI_KANBAN_TODO, UMI_KANBAN_DOING, UMI_KANBAN_TESTING, UMI_KANBAN_DONE
} UmiKanbanStage;
#endif

typedef enum UmiKanbanStage {
    UMI_KANBAN_TODO, UMI_KANBAN_DOING, UMI_KANBAN_TESTING, UMI_KANBAN_DONE,
    UMI_KANBAN_BLOCKED
} UmiKanbanStage;
typedef enum UmiKanbanPriority {
    UMI_KANBAN_LOW, UMI_KANBAN_NORMAL, UMI_KANBAN_HIGH, UMI_KANBAN_URGENT
} UmiKanbanPriority;

typedef struct UmiKanbanColumn {
    char id[UMI_KANBAN_ID_SIZE];
    char title[128];
    UmiKanbanStage stage;
    size_t wip_limit; /* Zero means unlimited. Archived cards do not count. */
} UmiKanbanColumn;

typedef struct UmiKanbanConfig {
    char id[UMI_KANBAN_ID_SIZE];
    char title[256];
    size_t card_capacity; /* 1..MAX_CARDS; storage is owned on the heap. */
    size_t column_count;
    UmiKanbanColumn columns[UMI_KANBAN_MAX_COLUMNS];
    /* Bit j permits a move from column i to j. Columns are fixed for this
     * board's lifetime. Same-column reordering needs no transition edge. */
    uint32_t transitions[UMI_KANBAN_MAX_COLUMNS];
} UmiKanbanConfig;

typedef struct UmiKanbanCheck {
    char text[256];
    int complete; /* Exactly 0 or 1. This is a user's assertion, not test evidence. */
} UmiKanbanCheck;

typedef struct UmiKanbanCard {
    char id[UMI_KANBAN_ID_SIZE];
    char title[256];
    char description[2048];
    char assignee[UMI_KANBAN_ID_SIZE]; /* Optional local identifier, not authorization. */
    UmiKanbanPriority priority;
    uint64_t due_unix_seconds; /* Zero means no deadline; caller supplies UTC time. */
    char blocked_reason[256];
    size_t label_count;
    char labels[UMI_KANBAN_MAX_LABELS][UMI_KANBAN_ID_SIZE];
    size_t check_count;
    UmiKanbanCheck checks[UMI_KANBAN_MAX_CHECKS];
    size_t dependency_count;
    char dependencies[UMI_KANBAN_MAX_DEPENDENCIES][UMI_KANBAN_ID_SIZE];
} UmiKanbanCard;

typedef struct UmiKanbanCardSnapshot {
    UmiKanbanCard card;
    char column_id[UMI_KANBAN_ID_SIZE];
    int archived;
    uint64_t board_revision;
} UmiKanbanCardSnapshot;

/* An assignment-only edit below preserves all other card fields and participates in the existing atomic transaction. Retained for engineering review. */
#if 0
typedef enum UmiKanbanEditKind {
    UMI_KANBAN_CREATE_CARD, UMI_KANBAN_UPDATE_CARD, UMI_KANBAN_MOVE_CARD,
    UMI_KANBAN_ARCHIVE_CARD, UMI_KANBAN_RESTORE_CARD, UMI_KANBAN_SET_WIP
} UmiKanbanEditKind;
#endif

/* The extended edit enum appends developer registration while retaining existing transaction identities. Retained for engineering review. */
#if 0
typedef enum UmiKanbanEditKind {
    UMI_KANBAN_CREATE_CARD, UMI_KANBAN_UPDATE_CARD, UMI_KANBAN_MOVE_CARD,
    UMI_KANBAN_ARCHIVE_CARD, UMI_KANBAN_RESTORE_CARD, UMI_KANBAN_SET_WIP,
    UMI_KANBAN_ASSIGN_CARD
} UmiKanbanEditKind;
#endif

typedef enum UmiKanbanEditKind {
    UMI_KANBAN_CREATE_CARD, UMI_KANBAN_UPDATE_CARD, UMI_KANBAN_MOVE_CARD,
    UMI_KANBAN_ARCHIVE_CARD, UMI_KANBAN_RESTORE_CARD, UMI_KANBAN_SET_WIP,
    UMI_KANBAN_ASSIGN_CARD, UMI_KANBAN_PUT_DEVELOPER
} UmiKanbanEditKind;

typedef struct UmiKanbanEdit {
    UmiKanbanEditKind kind;
    /* PUT_DEVELOPER uses card.id as developer ID and card.title as display
     * name (1..127 bytes); other fields are ignored. See developers.h. */
    /* ASSIGN uses card.id and card.assignee only; an empty assignee unassigns.
     * It preserves details, checklist, dependencies, column and row order. */
    /* CREATE/UPDATE use the complete card. Other card actions use card.id.
     * CREATE must name a TODO column. UPDATE never moves a card. MOVE may
     * place it before an active card in the destination column; empty means
     * append. SET_WIP uses column_id and wip_limit only. */
    UmiKanbanCard card;
    char column_id[UMI_KANBAN_ID_SIZE];
    char before_card_id[UMI_KANBAN_ID_SIZE];
    size_t wip_limit;
} UmiKanbanEdit;

typedef enum UmiKanbanProblemKind {
    UMI_KANBAN_PROBLEM_NONE, UMI_KANBAN_PROBLEM_ARGUMENT,
    UMI_KANBAN_PROBLEM_STALE, UMI_KANBAN_PROBLEM_DUPLICATE,
    UMI_KANBAN_PROBLEM_MISSING, UMI_KANBAN_PROBLEM_TRANSITION,
    UMI_KANBAN_PROBLEM_WIP, UMI_KANBAN_PROBLEM_BLOCKED,
    UMI_KANBAN_PROBLEM_CHECKLIST, UMI_KANBAN_PROBLEM_DEPENDENCY,
    UMI_KANBAN_PROBLEM_CYCLE, UMI_KANBAN_PROBLEM_ARCHIVED,
    UMI_KANBAN_PROBLEM_CAPACITY
} UmiKanbanProblemKind;

typedef struct UmiKanbanProblem {
    UmiKanbanProblemKind kind;
    size_t edit_index; /* SIZE_MAX denotes the complete transaction/domain check. */
    char card_id[UMI_KANBAN_ID_SIZE];
    char column_id[UMI_KANBAN_ID_SIZE];
} UmiKanbanProblem;

typedef struct UmiKanbanActivity {
    uint64_t sequence;
    uint64_t board_revision;
    UmiKanbanEditKind kind;
    char actor[UMI_KANBAN_ID_SIZE]; /* Caller-supplied attribution, not authenticated. */
    char card_id[UMI_KANBAN_ID_SIZE]; /* PUT_DEVELOPER records its developer ID here. */
    char from_column[UMI_KANBAN_ID_SIZE];
    char to_column[UMI_KANBAN_ID_SIZE];
} UmiKanbanActivity;

/* IDs use ASCII letters/digits plus '.', '_', '-', ':' and must be nonempty.
 * Text fields contain bounded NUL-terminated bytes. A UI must validate/convert
 * text for its encoding and display it as plain text, never markup or commands.
 * Serialize all access on one owner thread. These APIs perform no I/O, network,
 * tool execution or callbacks; a planning status cannot certify a passing build.
 * All outputs must be separate from inputs/each other and the live board.
 * Unless documented otherwise, refusals leave outputs and the board unchanged.
 */
UmiStatus umi_kanban_board_create(const UmiKanbanConfig *config, UmiKanbanBoard **out_board);
/* Default transitions: todo->doing; doing->todo/testing; testing->doing/done;
 * done->todo. No WIP limits. Capacity MAX_CARDS. */
UmiStatus umi_kanban_board_create_development(const char *id, const char *title,
    UmiKanbanBoard **out_board);
/* Five-list workflow: To do, Doing, Testing, Blocked, Done. Every list can
 * move to another; existing blocker, checklist, dependency and WIP rules still
 * apply. The four-column development constructor retains its original rules.
 * Blocked is a planning stage; no reason text is required to place a card there. */
UmiStatus umi_kanban_board_create_workflow(const char *id, const char *title,
    UmiKanbanBoard **out_board);
void umi_kanban_board_destroy(UmiKanbanBoard *board);
uint64_t umi_kanban_board_revision(const UmiKanbanBoard *board);
size_t umi_kanban_board_count(const UmiKanbanBoard *board); /* Includes archived cards. */
UmiStatus umi_kanban_board_config(const UmiKanbanBoard *board, UmiKanbanConfig *out_config);
UmiStatus umi_kanban_board_find(const UmiKanbanBoard *board, const char *id,
    UmiKanbanCardSnapshot *out_card);

/* Publish all edits together at one observed revision, or publish none.
 * Edits are ordered; they may refer to earlier creates in the same batch.
 * Final validation enforces WIP and dependency rules, permitting an atomic
 * column swap. No cycles, missing dependencies or archived prerequisites of
 * active cards are allowed. DONE requires completed checklist, no explicit
 * blocker and all prerequisites in DONE. Reopening a prerequisite therefore
 * needs reopening its completed dependents in the same reviewed transaction.
 * Blocking a card already in progress is allowed and remains visible.
 * Moving into DOING/TESTING requires no explicit blocker and completed
 * prerequisites; same-column reordering is always allowed.
 * An empty current batch is a no-op. Every nonempty successful batch advances
 * the revision once, even if details are identical. Allocation, validation,
 * stale revision and exhausted counters leave the original owner untouched.
 * out_problem is optional and is initialized on every return; it contains no
 * private card text. actor is optional (NULL/empty), otherwise a bounded ID.
 */
UmiStatus umi_kanban_board_apply(UmiKanbanBoard *board, uint64_t expected_revision,
    const char *actor, const UmiKanbanEdit *edits, size_t count,
    UmiKanbanProblem *out_problem);

/* Return retained activity in oldest-first order, at the observed revision.
 * The ring retains at most ACTIVITY_CAPACITY events; monotonically increasing
 * sequence values expose truncation. Events are local observations and are
 * intentionally not a durable audit trail or part of portable board archives.
 */
size_t umi_kanban_board_activity_count(const UmiKanbanBoard *board);
UmiStatus umi_kanban_board_activity_at(const UmiKanbanBoard *board,
    uint64_t expected_revision, size_t index, UmiKanbanActivity *out_activity);
#ifdef __cplusplus
}
#endif
#endif
