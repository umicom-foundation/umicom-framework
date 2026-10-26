/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_deadline.c
 *
 * PURPOSE:
 *   Check deadline-only edits, overdue boundaries and atomic archive behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../kanban/fixture.h"
#include "umicom/kanban/card_details.h"

int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    const char *known[] = {"normal", "clear", "maximum", "first-second", "stale", "missing", "archived",
        "null", "actor", "done", "roundtrip", "overdue", "due-now", "future", "done-not-overdue"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized);
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "card"); strcpy(edit->card.description, "Preserved private details");
    strcpy(edit->card.assignee, "owner"); edit->card.label_count = 1U; strcpy(edit->card.labels[0], "private-label");
    edit->card.due_unix_seconds = 123U; Apply(board, edit, 1U);
    if (!strcmp(name, "done") || !strcmp(name, "done-not-overdue")) Done(board, edit, "card");
    if (!strcmp(name, "archived")) { Action(edit, UMI_KANBAN_ARCHIVE_CARD, "card", NULL); Apply(board, edit, 1U); }
    uint64_t revision = umi_kanban_board_revision(board);
    uint64_t reviewed = !strcmp(name, "stale") ? revision - 1U : revision;
    const char *id = !strcmp(name, "missing") ? "missing" : "card";
    UmiKanbanCardDeadlineSnapshot snapshot;
    memset(&snapshot, 0x6c, sizeof snapshot); unsigned char sentinel[sizeof snapshot];
    memcpy(sentinel, &snapshot, sizeof snapshot);
    UmiStatus status = UmiKanbanCardDeadlineRead(board, reviewed, id, &snapshot);
    bool refused = !strcmp(name, "stale") || !strcmp(name, "missing") || !strcmp(name, "archived") ||
        !strcmp(name, "null") || !strcmp(name, "actor");
    if (!strcmp(name, "stale") || !strcmp(name, "missing") || !strcmp(name, "archived")) {
        REQUIRE(status != UMI_STATUS_OK && !memcmp(&snapshot, sentinel, sizeof snapshot));
    } else REQUIRE(status == UMI_STATUS_OK && snapshot.due_unix_seconds == 123U);
    size_t size = 0U, after_size = 0U;
    REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *before = malloc(size), *after = malloc(size); REQUIRE(before && after);
    REQUIRE(umi_kanban_board_encode(board, revision, before, size, &size) == UMI_STATUS_OK);
    size_t activity = umi_kanban_board_activity_count(board);
    uint64_t seconds = !strcmp(name, "clear") ? 0U : !strcmp(name, "maximum") ? UINT64_MAX :
        !strcmp(name, "first-second") ? 1U : 946684800U;
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanCardDeadlineRead(NULL, revision, "card", &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCardDeadlineRead(board, revision, "card", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        status = UmiKanbanCardDeadlineApply(NULL, revision, "card", "owner", seconds);
    } else status = UmiKanbanCardDeadlineApply(board, reviewed, id, !strcmp(name, "actor") ? "" : "owner", seconds);
    if (refused) {
        REQUIRE(status != UMI_STATUS_OK && umi_kanban_board_revision(board) == revision);
        REQUIRE(umi_kanban_board_activity_count(board) == activity);
        REQUIRE(umi_kanban_board_encode(board, revision, after, size, &after_size) == UMI_STATUS_OK);
        REQUIRE(after_size == size && !memcmp(before, after, size));
    } else {
        REQUIRE(status == UMI_STATUS_OK && umi_kanban_board_revision(board) == revision + 1U);
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
        REQUIRE(row.card.due_unix_seconds == seconds && !strcmp(row.card.description, "Preserved private details"));
        REQUIRE(!strcmp(row.card.assignee, "owner") && row.card.label_count == 1U && !strcmp(row.card.labels[0], "private-label"));
        REQUIRE(snapshot.due_unix_seconds == 123U);
        REQUIRE(UmiKanbanCardDeadlineApply(board, revision, "card", "owner", 0U) == UMI_STATUS_INVALID_STATE);
        if (!strcmp(name, "roundtrip")) {
            REQUIRE(umi_kanban_board_encode(board, revision + 1U, after, size, &after_size) == UMI_STATUS_OK);
            UmiKanbanBoard *copy = NULL; uint64_t historical = 0U;
            REQUIRE(umi_kanban_board_decode(after, after_size, &copy, &historical) == UMI_STATUS_OK);
            REQUIRE(historical == revision + 1U);
            REQUIRE(UmiKanbanCardDeadlineRead(copy, umi_kanban_board_revision(copy), "card", &snapshot) == UMI_STATUS_OK);
            REQUIRE(snapshot.due_unix_seconds == seconds); umi_kanban_board_destroy(copy);
        }
        uint64_t now = !strcmp(name, "due-now") ? seconds : !strcmp(name, "future") ? seconds - 1U :
            seconds == UINT64_MAX ? UINT64_MAX : seconds + 1U;
        UmiKanbanSummary summary;
        REQUIRE(umi_kanban_board_summary(board, revision + 1U, now, &summary) == UMI_STATUS_OK);
        bool overdue = seconds != 0U && seconds < now && strcmp(name, "done") && strcmp(name, "done-not-overdue");
        REQUIRE(summary.overdue == (overdue ? 1U : 0U));
        UmiKanbanFilter filter = {0}; filter.overdue_only = 1; filter.now_unix_seconds = now;
        size_t count = 0U, total = 0U;
        REQUIRE(umi_kanban_board_page(board, revision + 1U, &filter, 0U, NULL, 0U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(total == summary.overdue && count == 0U);
    }
    free(before); free(after); free(edit); umi_kanban_board_destroy(board); return 0;
}
