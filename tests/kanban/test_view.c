/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban/test_view.c
 * PURPOSE: Check stable pages, local filters and visible planning counts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    UmiKanbanBoard *board = Board();
    UmiKanbanEdit *edits = Edits(3U);
    Create(&edits[0], "alpha"); Create(&edits[1], "beta"); Create(&edits[2], "gamma");
    strcpy(edits[1].card.assignee, "reviewer");
    strcpy(edits[1].card.description, "Check portable paths");
    edits[1].card.priority = UMI_KANBAN_HIGH;
    edits[1].card.label_count = 1U; strcpy(edits[1].card.labels[0], "build");
    edits[1].card.due_unix_seconds = 100U;
    strcpy(edits[1].card.blocked_reason, "Needs input");
    Apply(board, edits, 3U);
    UmiKanbanCardSnapshot *rows = calloc(3U, sizeof(*rows));
    REQUIRE(rows != NULL);
    size_t count = 0U, total = 0U;
    uint64_t revision = umi_kanban_board_revision(board);
    UmiKanbanFilter filter = {0};
    if (strcmp(argv[1], "filters") == 0) {
        strcpy(filter.assignee, "reviewer"); strcpy(filter.label, "build");
        strcpy(filter.text, "portable"); strcpy(filter.column_id, "todo");
        filter.minimum_priority = UMI_KANBAN_HIGH; filter.blocked_only = 1;
        filter.overdue_only = 1; filter.now_unix_seconds = 101U;
        REQUIRE(umi_kanban_board_page(board, revision, &filter, 0U, rows, 3U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 1U && total == 1U && strcmp(rows[0].card.id, "beta") == 0);
        filter.now_unix_seconds = 100U;
        REQUIRE(umi_kanban_board_page(board, revision, &filter, 0U, rows, 3U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 0U && total == 0U);
    } else if (strcmp(argv[1], "pages") == 0) {
        REQUIRE(umi_kanban_board_page(board, revision, NULL, 0U, NULL, 0U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 0U && total == 3U);
        REQUIRE(umi_kanban_board_page(board, revision, NULL, 1U, rows, 1U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 1U && total == 3U && strcmp(rows[0].card.id, "beta") == 0);
        Action(edits, UMI_KANBAN_MOVE_CARD, "gamma", "todo"); strcpy(edits->before_card_id, "alpha");
        Apply(board, edits, 1U);
        count = 99U; total = 98U; strcpy(rows[0].card.id, "sentinel");
        REQUIRE(umi_kanban_board_page(board, revision, NULL, 0U, rows, 3U, &count, &total) == UMI_STATUS_INVALID_STATE);
        REQUIRE(count == 99U && total == 98U && strcmp(rows[0].card.id, "sentinel") == 0);
        revision = umi_kanban_board_revision(board);
        REQUIRE(umi_kanban_board_page(board, revision, NULL, 0U, rows, 3U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(strcmp(rows[0].card.id, "gamma") == 0 && strcmp(rows[1].card.id, "alpha") == 0);
    } else if (strcmp(argv[1], "summary") == 0) {
        Done(board, edits, "alpha");
        Action(edits, UMI_KANBAN_ARCHIVE_CARD, "gamma", NULL); Apply(board, edits, 1U);
        UmiKanbanSummary summary;
        revision = umi_kanban_board_revision(board);
        REQUIRE(umi_kanban_board_summary(board, revision, 101U, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.active == 2U && summary.archived == 1U && summary.done == 1U);
        REQUIRE(summary.blocked == 1U && summary.overdue == 1U && summary.columns[0] == 1U);
        filter.archived = 2;
        REQUIRE(umi_kanban_board_page(board, revision, &filter, 0U, rows, 3U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 1U && strcmp(rows[0].card.id, "gamma") == 0 && rows[0].archived);
    } else if (strcmp(argv[1], "invalid-filter") == 0) {
        filter.archived = 3;
        count = 99U; total = 98U; strcpy(rows[0].card.id, "sentinel");
        REQUIRE(umi_kanban_board_page(board, revision, &filter, 0U, rows, 3U, &count, &total) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(count == 99U && total == 98U && strcmp(rows[0].card.id, "sentinel") == 0);
    } else REQUIRE(0);
    free(rows); free(edits); umi_kanban_board_destroy(board);
    return 0;
}
