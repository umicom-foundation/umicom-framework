/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban/fixture.h
 * PURPOSE: Keep planning tests independent of display servers, repositories and process working directories.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_KANBAN_FIXTURE_H
#define UMICOM_TEST_KANBAN_FIXTURE_H
#include "umicom/kanban/board.h"
#include "umicom/kanban/view.h"
#include "umicom/kanban/archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Unlike assert(), these checks and their setup calls remain active in release builds. */
#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); exit(1); \
} } while (0)
static inline UmiKanbanBoard *Board(void)
{
    UmiKanbanBoard *initial = NULL, *board = NULL;
    UmiKanbanConfig config;
    REQUIRE(umi_kanban_board_create_development("demo", "Development", &initial) == UMI_STATUS_OK);
    REQUIRE(umi_kanban_board_config(initial, &config) == UMI_STATUS_OK);
    config.card_capacity = 16U;
    REQUIRE(umi_kanban_board_create(&config, &board) == UMI_STATUS_OK);
    umi_kanban_board_destroy(initial);
    return board;
}
static inline UmiKanbanEdit *Edits(size_t count)
{
    UmiKanbanEdit *edits = calloc(count, sizeof(*edits));
    REQUIRE(edits != NULL);
    return edits;
}
static inline void Create(UmiKanbanEdit *edit, const char *id)
{
    memset(edit, 0, sizeof(*edit));
    edit->kind = UMI_KANBAN_CREATE_CARD;
    strcpy(edit->card.id, id);
    strcpy(edit->card.title, id);
    edit->card.priority = UMI_KANBAN_NORMAL;
    strcpy(edit->column_id, "todo");
}
static inline void Action(UmiKanbanEdit *edit, UmiKanbanEditKind kind,
    const char *id, const char *column)
{
    memset(edit, 0, sizeof(*edit));
    edit->kind = kind;
    if (id != NULL) strcpy(edit->card.id, id);
    if (column != NULL) strcpy(edit->column_id, column);
}
static inline void Apply(UmiKanbanBoard *board, UmiKanbanEdit *edits, size_t count)
{
    REQUIRE(umi_kanban_board_apply(board, umi_kanban_board_revision(board),
        "developer", edits, count, NULL) == UMI_STATUS_OK);
}
static inline void Move(UmiKanbanBoard *board, UmiKanbanEdit *edit,
    const char *id, const char *column)
{
    Action(edit, UMI_KANBAN_MOVE_CARD, id, column);
    Apply(board, edit, 1U);
}
static inline void Done(UmiKanbanBoard *board, UmiKanbanEdit *edit, const char *id)
{
    Move(board, edit, id, "doing");
    Move(board, edit, id, "testing");
    Move(board, edit, id, "done");
}
static inline void Refused(UmiKanbanBoard *board, UmiKanbanEdit *edits,
    size_t count, UmiKanbanProblemKind expected)
{
    /* Compare portable state as well as revision and activity: a failed batch
     * must not leak an earlier edit into the live board. */
    uint64_t revision = umi_kanban_board_revision(board);
    size_t size = 0U, after_size = 0U;
    size_t activity = umi_kanban_board_activity_count(board);
    REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *before = malloc(size), *after = malloc(size);
    REQUIRE(before != NULL && after != NULL);
    REQUIRE(umi_kanban_board_encode(board, revision, before, size, &size) == UMI_STATUS_OK);
    UmiKanbanProblem problem;
    REQUIRE(umi_kanban_board_apply(board, revision, "developer", edits, count, &problem) != UMI_STATUS_OK);
    REQUIRE(problem.kind == expected);
    REQUIRE(umi_kanban_board_revision(board) == revision);
    REQUIRE(umi_kanban_board_activity_count(board) == activity);
    REQUIRE(umi_kanban_board_encode(board, revision, after, size, &after_size) == UMI_STATUS_OK);
    REQUIRE(after_size == size && memcmp(before, after, size) == 0);
    free(before); free(after);
}
#endif
