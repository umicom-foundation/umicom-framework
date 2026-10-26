/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/fixture.h
 *
 * PURPOSE:
 *   Arrange synthetic completion decisions and retain exact before/after board observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TEST_KANBAN_COMPLETION_FIXTURE_H
#define UMICOM_TEST_KANBAN_COMPLETION_FIXTURE_H
#include "../kanban/fixture.h"
#include "umicom/kanban/completion_plan.h"
static inline int Known(const char *name, const char *const *cases, size_t count)
{
    for (size_t i = 0; i < count; ++i) if (!strcmp(name, cases[i])) return 1;
    return 0;
}
static inline UmiKanbanBoard *Ready(void)
{
    UmiKanbanBoard *board = Board();
    UmiKanbanEdit *edit = Edits(2);
    Create(&edit[0], "alpha"); Create(&edit[1], "beta"); Apply(board, edit, 2);
    Move(board, edit, "alpha", "doing"); Move(board, edit, "alpha", "testing");
    Move(board, edit, "beta", "doing"); Move(board, edit, "beta", "testing");
    free(edit);
    return board;
}
static inline void Record(UmiKanbanCompletionRecord *record, const char *id)
{
    memset(record, 0, sizeof *record);
    strcpy(record->card_id, id);
    memset(record->evidence_sha256, 'a', 64);
    strcpy(record->rationale, "Synthetic owner review of the complete acceptance criteria.");
}
static inline void Update(UmiKanbanBoard *board, const char *id, const char *title)
{
    UmiKanbanCardSnapshot value;
    REQUIRE(umi_kanban_board_find(board, id, &value) == UMI_STATUS_OK);
    UmiKanbanEdit *edit = Edits(1);
    edit->kind = UMI_KANBAN_UPDATE_CARD; edit->card = value.card;
    strcpy(edit->card.title, title); Apply(board, edit, 1); free(edit);
}
static inline void *Snapshot(const UmiKanbanBoard *board, size_t *size)
{
    uint64_t revision = umi_kanban_board_revision(board);
    REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0, size) == UMI_STATUS_OK);
    void *bytes = malloc(*size); REQUIRE(bytes);
    REQUIRE(umi_kanban_board_encode(board, revision, bytes, *size, size) == UMI_STATUS_OK);
    return bytes;
}
static inline void Unchanged(const UmiKanbanBoard *board, const void *before, size_t size, size_t activity)
{
    size_t after_size = 0; void *after = Snapshot(board, &after_size);
    REQUIRE(size == after_size && !memcmp(before, after, size));
    REQUIRE(umi_kanban_board_activity_count(board) == activity);
    free(after);
}
static inline UmiStatus Capture(const UmiKanbanBoard *board, const UmiKanbanCompletionRecord *records,
    size_t count, UmiKanbanCompletionPlan **out)
{
    return UmiKanbanCompletionPlanCreate(board, umi_kanban_board_revision(board),
        "done", "reviewer", "synthetic-source", "Debug/native", records, count, out);
}
#endif
