/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/completion_plan.h
 *
 * PURPOSE:
 *   Bind explicit completion review to private evidence and an unchanged planning board.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_KANBAN_COMPLETION_PLAN_H
#define UMICOM_KANBAN_COMPLETION_PLAN_H
#include "umicom/kanban/board.h"
#include "umicom/base/sha256.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_KANBAN_COMPLETION_MAXIMUM_BYTES 262144U
typedef struct UmiKanbanCompletionPlan UmiKanbanCompletionPlan;
typedef enum UmiKanbanCompletionDecision {
    UMI_KANBAN_COMPLETION_REVIEWED,
    UMI_KANBAN_COMPLETION_EXCEPTION
} UmiKanbanCompletionDecision;
typedef struct UmiKanbanCompletionRecord {
    char card_id[UMI_KANBAN_ID_SIZE];
    UmiKanbanCompletionDecision decision;
    /* Required for REVIEWED, optional for EXCEPTION: exact lowercase SHA-256
     * of the retained review evidence, such as a report or evidence manifest.
     * This is supplied by the reviewer; the plan does not open or verify files. */
    char evidence_sha256[UMI_SHA256_HEX_CAPACITY];
    char rationale[1024]; /* Required scope/acceptance explanation or exception reason. */
} UmiKanbanCompletionRecord;
typedef struct UmiKanbanCompletionPlanInfo {
    char board_id[UMI_KANBAN_ID_SIZE];
    char done_column[UMI_KANBAN_ID_SIZE];
    char reviewer[UMI_KANBAN_ID_SIZE]; /* Local attribution, not authentication. */
    char source_identity[256]; /* Reviewer-declared build/source context. */
    char configuration[128];
    char board_sha256[UMI_SHA256_HEX_CAPACITY];
    uint64_t reviewed_revision;
    size_t card_count;
} UmiKanbanCompletionPlanInfo;
/* Capture 1..MAX_EDITS distinct active, non-Done cards at one live revision.
 * REVIEWED records need evidence digest and rationale; EXCEPTION records need
 * an explicit rationale. Both need declared source/configuration and reviewer.
 * Simulate the complete Done transaction privately before returning an owned
 * plan. Existing transitions, checklist, blocker, dependency and WIP rules
 * still apply, including for exceptions. The live board is never changed.
 *
 * The board's complete portable archive digest binds the plan to its content,
 * not merely its title or numeric revision. Later unrelated board edits also
 * require fresh explicit review. No automatic rebase/review is performed.
 * No I/O, callbacks, clock access or execution takes place. Serialize access.
 * out becomes NULL on failure. All outputs must be separate from inputs. */
UmiStatus UmiKanbanCompletionPlanCreate(const UmiKanbanBoard *board, uint64_t revision,
    const char *done_column, const char *reviewer, const char *source_identity,
    const char *configuration, const UmiKanbanCompletionRecord *records, size_t count,
    UmiKanbanCompletionPlan **out);
void UmiKanbanCompletionPlanDestroy(UmiKanbanCompletionPlan *plan);
UmiStatus UmiKanbanCompletionPlanRead(const UmiKanbanCompletionPlan *plan,
    UmiKanbanCompletionPlanInfo *out);
const UmiKanbanCompletionRecord *UmiKanbanCompletionPlanAt(
    const UmiKanbanCompletionPlan *plan, size_t index);
/* Recheck exact board identity, revision, archive digest and completion policy.
 * A matching digest establishes content identity, not a trusted signer or
 * evidence quality. A same-ID/same-revision different board is refused. */
UmiStatus UmiKanbanCompletionPlanCheck(const UmiKanbanCompletionPlan *plan,
    const UmiKanbanBoard *board, uint64_t revision);
/* Explicitly apply only the reviewed moves, in recorded order, as one existing
 * board transaction. Failure leaves the board unchanged. Success advances its
 * revision once, making this plan stale; keep it as private historical evidence.
 * The board's normal apply API remains available for existing hosts. This is
 * an opt-in review workflow, not a security boundary or universal Done policy.
 * Saving the board and retaining the plan are separate caller-owned operations. */
UmiStatus UmiKanbanCompletionPlanApply(const UmiKanbanCompletionPlan *plan,
    UmiKanbanBoard *board, uint64_t revision);
/* Private bounded archive. It contains attribution, rationale, source context
 * and evidence identifiers; never include it in a public roadmap export.
 * The checksum detects corruption, not malicious alteration. Encode NULL/0
 * measures; short capacity leaves bytes unchanged and reports required size.
 * Decode creates an independent plan, or clears out on refusal. */
UmiStatus UmiKanbanCompletionPlanEncode(const UmiKanbanCompletionPlan *plan,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus UmiKanbanCompletionPlanDecode(const void *bytes, size_t size,
    UmiKanbanCompletionPlan **out);
#ifdef __cplusplus
}
#endif
#endif
