/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban/test_board.c
 * PURPOSE: Exercise planning transitions, atomic refusal and dependency ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    UmiKanbanBoard *board = Board();
    UmiKanbanEdit *edits = Edits(4U);
    UmiKanbanCardSnapshot card;
    Create(&edits[0], "first");
    Create(&edits[1], "second");
    Apply(board, edits, 2U);
    if (strcmp(argv[1], "lifecycle") == 0) {
        Done(board, edits, "first");
        REQUIRE(umi_kanban_board_find(board, "first", &card) == UMI_STATUS_OK);
        REQUIRE(strcmp(card.column_id, "done") == 0);
        Move(board, edits, "first", "todo");
        Action(edits, UMI_KANBAN_ARCHIVE_CARD, "first", NULL); Apply(board, edits, 1U);
        REQUIRE(umi_kanban_board_find(board, "first", &card) == UMI_STATUS_OK && card.archived);
        Action(edits, UMI_KANBAN_RESTORE_CARD, "first", NULL); Apply(board, edits, 1U);
        REQUIRE(umi_kanban_board_find(board, "first", &card) == UMI_STATUS_OK && !card.archived);
    } else if (strcmp(argv[1], "atomic") == 0) {
        Action(&edits[0], UMI_KANBAN_MOVE_CARD, "first", "doing");
        Create(&edits[1], "second");
        Refused(board, edits, 2U, UMI_KANBAN_PROBLEM_DUPLICATE);
    } else if (strcmp(argv[1], "stale") == 0) {
        uint64_t old = umi_kanban_board_revision(board);
        Move(board, edits, "first", "doing");
        UmiKanbanProblem problem;
        REQUIRE(umi_kanban_board_apply(board, old, NULL, edits, 1U, &problem) == UMI_STATUS_INVALID_STATE);
        REQUIRE(problem.kind == UMI_KANBAN_PROBLEM_STALE);
        REQUIRE(umi_kanban_board_apply(board, old, NULL, NULL, 0U, &problem) == UMI_STATUS_INVALID_STATE);
        uint64_t current = umi_kanban_board_revision(board);
        REQUIRE(umi_kanban_board_apply(board, current, NULL, NULL, 0U, &problem) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_board_revision(board) == current);
    } else if (strcmp(argv[1], "transition") == 0) {
        Action(edits, UMI_KANBAN_MOVE_CARD, "first", "done");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_TRANSITION);
    } else if (strcmp(argv[1], "wip") == 0) {
        Action(edits, UMI_KANBAN_SET_WIP, NULL, "doing"); edits->wip_limit = 1U;
        Apply(board, edits, 1U);
        Move(board, edits, "first", "doing");
        Action(edits, UMI_KANBAN_MOVE_CARD, "second", "doing");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_WIP);
        /* The complete transaction, rather than its intermediate counts,
         * determines whether the replacement work fits the column. */
        Action(&edits[0], UMI_KANBAN_MOVE_CARD, "second", "doing");
        Action(&edits[1], UMI_KANBAN_MOVE_CARD, "first", "testing");
        Apply(board, edits, 2U);
    } else if (strcmp(argv[1], "checklist") == 0 || strcmp(argv[1], "blocked") == 0) {
        REQUIRE(umi_kanban_board_find(board, "first", &card) == UMI_STATUS_OK);
        Action(edits, UMI_KANBAN_UPDATE_CARD, NULL, NULL);
        edits->card = card.card;
        if (strcmp(argv[1], "blocked") == 0) {
            strcpy(edits->card.blocked_reason, "Awaiting review"); Apply(board, edits, 1U);
            Action(edits, UMI_KANBAN_MOVE_CARD, "first", "doing");
            Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_BLOCKED);
        } else {
            edits->card.check_count = 1U; strcpy(edits->card.checks[0].text, "Review tests");
            Apply(board, edits, 1U);
            Move(board, edits, "first", "doing"); Move(board, edits, "first", "testing");
            Action(edits, UMI_KANBAN_MOVE_CARD, "first", "done");
            Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_CHECKLIST);
            REQUIRE(umi_kanban_board_find(board, "first", &card) == UMI_STATUS_OK);
            Action(&edits[0], UMI_KANBAN_UPDATE_CARD, NULL, NULL);
            edits[0].card = card.card; edits[0].card.checks[0].complete = 1;
            Action(&edits[1], UMI_KANBAN_MOVE_CARD, "first", "done");
            Apply(board, edits, 2U);
        }
    } else if (strcmp(argv[1], "dependencies") == 0 || strcmp(argv[1], "cycle") == 0) {
        REQUIRE(umi_kanban_board_find(board, "second", &card) == UMI_STATUS_OK);
        Action(edits, UMI_KANBAN_UPDATE_CARD, NULL, NULL); edits->card = card.card;
        edits->card.dependency_count = 1U; strcpy(edits->card.dependencies[0], "first");
        Apply(board, edits, 1U);
        if (strcmp(argv[1], "cycle") == 0) {
            REQUIRE(umi_kanban_board_find(board, "first", &card) == UMI_STATUS_OK);
            Action(edits, UMI_KANBAN_UPDATE_CARD, NULL, NULL); edits->card = card.card;
            edits->card.dependency_count = 1U; strcpy(edits->card.dependencies[0], "second");
            Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_CYCLE);
        } else {
            Action(edits, UMI_KANBAN_MOVE_CARD, "second", "doing");
            Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_DEPENDENCY);
            Done(board, edits, "first"); Done(board, edits, "second");
            Action(edits, UMI_KANBAN_ARCHIVE_CARD, "first", NULL);
            Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_DEPENDENCY);
            Action(edits, UMI_KANBAN_MOVE_CARD, "first", "todo");
            Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_DEPENDENCY);
            Action(&edits[1], UMI_KANBAN_MOVE_CARD, "second", "todo");
            Apply(board, edits, 2U);
        }
    } else if (strcmp(argv[1], "restore-wip") == 0) {
        Move(board, edits, "first", "doing");
        Action(edits, UMI_KANBAN_ARCHIVE_CARD, "first", NULL); Apply(board, edits, 1U);
        Action(edits, UMI_KANBAN_SET_WIP, NULL, "doing"); edits->wip_limit = 1U; Apply(board, edits, 1U);
        Move(board, edits, "second", "doing");
        Action(edits, UMI_KANBAN_RESTORE_CARD, "first", NULL);
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_WIP);
    } else if (strcmp(argv[1], "activity") == 0) {
        for (size_t i = 0U; i < 260U; ++i) {
            Action(edits, UMI_KANBAN_SET_WIP, NULL, "doing");
            edits->wip_limit = i % 2U; Apply(board, edits, 1U);
        }
        REQUIRE(umi_kanban_board_activity_count(board) == UMI_KANBAN_ACTIVITY_CAPACITY);
        UmiKanbanActivity event;
        REQUIRE(umi_kanban_board_activity_at(board, umi_kanban_board_revision(board), 0U, &event) == UMI_STATUS_OK);
        REQUIRE(event.sequence == 7U && strcmp(event.actor, "developer") == 0);
        REQUIRE(umi_kanban_board_activity_at(board, umi_kanban_board_revision(board), 255U, &event) == UMI_STATUS_OK);
        REQUIRE(event.sequence == 262U && event.board_revision == umi_kanban_board_revision(board));
    } else if (strcmp(argv[1], "invalid") == 0) {
        Create(edits, "invalid");
        memset(edits->card.title, 'x', sizeof(edits->card.title));
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_ARGUMENT);
        Create(edits, "third"); edits->card.dependency_count = 1U;
        strcpy(edits->card.dependencies[0], "missing");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_DEPENDENCY);
    } else REQUIRE(0);
    free(edits); umi_kanban_board_destroy(board);
    return 0;
}
