/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_column_policy.c
 *
 * PURPOSE:
 *   Check occupancy limits, stable cards and archive retention under list policy edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../kanban/fixture.h"
#include "umicom/kanban/column_policy.h"

int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    const char *known[] = {"normal", "archived", "done", "unlimited", "stale", "missing", "invalid-id", "below", "capacity", "over-capacity", "overflow", "null", "actor", "roundtrip", "admission"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized);
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "one"); Apply(board, edit, 1U); Create(edit, "two"); Apply(board, edit, 1U);
    if (!strcmp(name, "archived")) { Action(edit, UMI_KANBAN_ARCHIVE_CARD, "two", NULL); Apply(board, edit, 1U); }
    if (!strcmp(name, "done")) { Done(board, edit, "one"); Done(board, edit, "two"); }
    if (!strcmp(name, "unlimited")) REQUIRE(UmiKanbanColumnPolicyApply(board,
        umi_kanban_board_revision(board), "todo", "owner", 2U) == UMI_STATUS_OK);
    uint64_t revision = umi_kanban_board_revision(board);
    uint64_t expected = !strcmp(name, "stale") ? revision - 1U : revision;
    const char *column = !strcmp(name, "missing") ? "missing" :
        !strcmp(name, "invalid-id") ? "bad id" : !strcmp(name, "done") ? "done" : "todo";
    UmiKanbanColumnPolicySnapshot snapshot;
    memset(&snapshot, 0x6b, sizeof snapshot); unsigned char sentinel[sizeof snapshot];
    memcpy(sentinel, &snapshot, sizeof snapshot);
    UmiStatus status = UmiKanbanColumnPolicyRead(board, expected, column, &snapshot);
    bool unreadable = !strcmp(name, "missing") || !strcmp(name, "stale") || !strcmp(name, "invalid-id");
    if (unreadable) { REQUIRE(status != UMI_STATUS_OK); REQUIRE(!memcmp(&snapshot, sentinel, sizeof snapshot)); }
    else {
        REQUIRE(status == UMI_STATUS_OK && snapshot.board_revision == revision && snapshot.board_capacity == 16U);
        REQUIRE(snapshot.active_cards == (!strcmp(name, "archived") ? 1U : 2U));
    }
    size_t size = 0U, after_size = 0U;
    REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *before = malloc(size), *after = malloc(size); REQUIRE(before && after);
    REQUIRE(umi_kanban_board_encode(board, revision, before, size, &size) == UMI_STATUS_OK);
    size_t activity = umi_kanban_board_activity_count(board);
    size_t limit = !strcmp(name, "below") || !strcmp(name, "archived") ? 1U :
        !strcmp(name, "unlimited") ? 0U : !strcmp(name, "capacity") ? 16U :
        !strcmp(name, "over-capacity") ? 17U : !strcmp(name, "overflow") ? SIZE_MAX : 2U;
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanColumnPolicyRead(NULL, revision, "todo", &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanColumnPolicyRead(board, revision, "todo", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        status = UmiKanbanColumnPolicyApply(NULL, revision, "todo", "owner", 2U);
    } else status = UmiKanbanColumnPolicyApply(board, expected, column,
        !strcmp(name, "actor") ? "" : "owner", limit);
    bool refused = unreadable || !strcmp(name, "below") || !strcmp(name, "over-capacity") ||
        !strcmp(name, "overflow") || !strcmp(name, "null") || !strcmp(name, "actor");
    if (refused) {
        REQUIRE(status != UMI_STATUS_OK);
        if (!strcmp(name, "below")) REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_kanban_board_revision(board) == revision && umi_kanban_board_activity_count(board) == activity);
        REQUIRE(umi_kanban_board_encode(board, revision, after, size, &after_size) == UMI_STATUS_OK);
        REQUIRE(after_size == size && !memcmp(before, after, size));
    } else {
        REQUIRE(status == UMI_STATUS_OK && umi_kanban_board_revision(board) == revision + 1U);
        REQUIRE(UmiKanbanColumnPolicyRead(board, revision + 1U, column, &snapshot) == UMI_STATUS_OK);
        REQUIRE(snapshot.column.wip_limit == limit);
        UmiKanbanCardSnapshot row;
        REQUIRE(umi_kanban_board_find(board, "one", &row) == UMI_STATUS_OK && !row.archived);
        REQUIRE(!strcmp(row.column_id, column) && !strcmp(row.card.title, "one"));
        REQUIRE(umi_kanban_board_find(board, "two", &row) == UMI_STATUS_OK);
        REQUIRE(row.archived == (!strcmp(name, "archived") ? 1 : 0));
        UmiKanbanActivity event;
        REQUIRE(umi_kanban_board_activity_at(board, revision + 1U,
            umi_kanban_board_activity_count(board) - 1U, &event) == UMI_STATUS_OK);
        REQUIRE(event.kind == UMI_KANBAN_SET_WIP && !strcmp(event.to_column, column));
        REQUIRE(UmiKanbanColumnPolicyApply(board, revision, column, "owner", limit) == UMI_STATUS_INVALID_STATE);
        if (!strcmp(name, "roundtrip")) {
            REQUIRE(umi_kanban_board_encode(board, revision + 1U, after, size, &after_size) == UMI_STATUS_OK);
            UmiKanbanBoard *copy = NULL;
            /* The earlier draft omitted the required saved-revision output.
             * Keep it for review; the public decoder returns history separately
             * from the reopened board revision, which starts at one. */
#if 0
            REQUIRE(umi_kanban_board_decode(after, after_size, &copy) == UMI_STATUS_OK);
#endif
            uint64_t saved_revision = 0U;
            REQUIRE(umi_kanban_board_decode(after, after_size, &copy, &saved_revision) == UMI_STATUS_OK);
            REQUIRE(saved_revision == revision + 1U);
            REQUIRE(UmiKanbanColumnPolicyRead(copy, umi_kanban_board_revision(copy), column, &snapshot) == UMI_STATUS_OK);
            REQUIRE(snapshot.column.wip_limit == limit && snapshot.active_cards == 2U); umi_kanban_board_destroy(copy);
        } else if (!strcmp(name, "admission")) {
            Create(edit, "three"); Refused(board, edit, 1U, UMI_KANBAN_PROBLEM_WIP);
        }
    }
    free(before); free(after); free(edit); umi_kanban_board_destroy(board); return 0;
}
