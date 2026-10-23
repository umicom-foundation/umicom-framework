/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban/test_workflow.c
 * PURPOSE: Exercise five-list planning, isolated assignment and archive compatibility through public APIs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/base/value_archive_internal.h"

static UmiKanbanBoard *Workflow(void)
{
    UmiKanbanBoard *initial = NULL, *board = NULL;
    UmiKanbanConfig config;
    REQUIRE(umi_kanban_board_create_workflow("team", "Team development", &initial) == UMI_STATUS_OK);
    REQUIRE(umi_kanban_board_config(initial, &config) == UMI_STATUS_OK);
    config.card_capacity = 16U;
    REQUIRE(umi_kanban_board_create(&config, &board) == UMI_STATUS_OK);
    umi_kanban_board_destroy(initial);
    return board;
}
static UmiKanbanCardSnapshot Find(UmiKanbanBoard *board, const char *id)
{
    UmiKanbanCardSnapshot card;
    REQUIRE(umi_kanban_board_find(board, id, &card) == UMI_STATUS_OK);
    return card;
}
static void Assign(UmiKanbanEdit *edit, const char *id, const char *developer)
{
    Action(edit, UMI_KANBAN_ASSIGN_CARD, id, NULL);
    strcpy(edit->card.assignee, developer);
}
static void ChangeDetails(UmiKanbanBoard *board, UmiKanbanEdit *edit, const char *id)
{
    UmiKanbanCardSnapshot row = Find(board, id);
    Action(edit, UMI_KANBAN_UPDATE_CARD, id, NULL);
    edit->card = row.card;
}
static unsigned char *Archive(UmiKanbanBoard *board, size_t *size)
{
    uint64_t revision = umi_kanban_board_revision(board);
    REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, size) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(*size);
    REQUIRE(bytes != NULL);
    REQUIRE(umi_kanban_board_encode(board, revision, bytes, *size, size) == UMI_STATUS_OK);
    return bytes;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    UmiKanbanBoard *board = Workflow();
    UmiKanbanEdit *edits = Edits(3U);
    Create(edits, "task");
    strcpy(edits->card.title, "Implement file opening");
    strcpy(edits->card.description, "Retain pending editor changes");
    edits->card.priority = UMI_KANBAN_HIGH;
    edits->card.due_unix_seconds = 200U;
    edits->card.label_count = 1U;
    strcpy(edits->card.labels[0], "editor");
    edits->card.check_count = 1U;
    strcpy(edits->card.checks[0].text, "Verify cancellation");
    Apply(board, edits, 1U);
    if (strcmp(argv[1], "layout") == 0) {
        UmiKanbanConfig config;
        REQUIRE(umi_kanban_board_config(board, &config) == UMI_STATUS_OK);
        const char *ids[] = {"todo", "doing", "testing", "blocked", "done"};
        const UmiKanbanStage stages[] = {UMI_KANBAN_TODO, UMI_KANBAN_DOING,
            UMI_KANBAN_TESTING, UMI_KANBAN_BLOCKED, UMI_KANBAN_DONE};
        REQUIRE(config.column_count == 5U && UMI_KANBAN_DONE == 3);
        for (size_t i = 0U; i < config.column_count; ++i) {
            REQUIRE(strcmp(config.columns[i].id, ids[i]) == 0);
            REQUIRE(config.columns[i].stage == stages[i]);
            for (size_t j = 0U; j < config.column_count; ++j)
                REQUIRE(((config.transitions[i] & (UINT32_C(1) << j)) != 0U) == (i != j));
        }
        UmiKanbanBoard *sentinel = board;
        REQUIRE(umi_kanban_board_create_workflow("invalid id", "title", &sentinel) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(sentinel == board);
    } else if (strcmp(argv[1], "legacy") == 0) {
        UmiKanbanBoard *legacy = Board();
        UmiKanbanConfig config;
        REQUIRE(umi_kanban_board_config(legacy, &config) == UMI_STATUS_OK);
        REQUIRE(config.column_count == 4U && config.columns[3].stage == UMI_KANBAN_DONE);
        Create(edits, "old"); Apply(legacy, edits, 1U);
        Action(edits, UMI_KANBAN_MOVE_CARD, "old", "testing");
        Refused(legacy, edits, 1U, UMI_KANBAN_PROBLEM_TRANSITION);
        umi_kanban_board_destroy(legacy);
    } else if (strcmp(argv[1], "assign") == 0 || strcmp(argv[1], "unassign") == 0) {
        Assign(edits, "task", "dev.alex");
        /* Unused input fields deliberately contain stale values. Assignment
         * must not behave like a full-card update from a host snapshot. */
        strcpy(edits->card.title, "Stale title");
        edits->card.check_count = 0U;
        Apply(board, edits, 1U);
        if (strcmp(argv[1], "unassign") == 0) {
            Assign(edits, "task", ""); Apply(board, edits, 1U);
        }
        UmiKanbanCardSnapshot row = Find(board, "task");
        REQUIRE(strcmp(row.card.assignee, strcmp(argv[1], "assign") == 0 ? "dev.alex" : "") == 0);
        REQUIRE(strcmp(row.card.title, "Implement file opening") == 0);
        REQUIRE(strcmp(row.card.description, "Retain pending editor changes") == 0);
        REQUIRE(row.card.priority == UMI_KANBAN_HIGH && row.card.due_unix_seconds == 200U);
        REQUIRE(row.card.label_count == 1U && strcmp(row.card.labels[0], "editor") == 0);
        REQUIRE(row.card.check_count == 1U && row.card.checks[0].complete == 0);
        REQUIRE(strcmp(row.card.checks[0].text, "Verify cancellation") == 0);
        REQUIRE(strcmp(row.column_id, "todo") == 0);
        UmiKanbanActivity event;
        REQUIRE(umi_kanban_board_activity_at(board, umi_kanban_board_revision(board),
            umi_kanban_board_activity_count(board) - 1U, &event) == UMI_STATUS_OK);
        REQUIRE(event.kind == UMI_KANBAN_ASSIGN_CARD && strcmp(event.actor, "developer") == 0);
        REQUIRE(strcmp(event.from_column, "todo") == 0 && strcmp(event.to_column, "todo") == 0);
    } else if (strcmp(argv[1], "assign-invalid") == 0) {
        Assign(edits, "task", "bad name");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_ARGUMENT);
        Assign(edits, "missing", "dev.alex");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_MISSING);
        Assign(edits, "task", "");
        memset(edits->card.assignee, 'x', sizeof(edits->card.assignee));
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_ARGUMENT);
    } else if (strcmp(argv[1], "assign-stale") == 0) {
        uint64_t revision = umi_kanban_board_revision(board);
        Move(board, edits, "task", "doing");
        size_t activity = umi_kanban_board_activity_count(board);
        Assign(edits, "task", "dev.alex");
        UmiKanbanProblem problem;
        REQUIRE(umi_kanban_board_apply(board, revision, "developer", edits, 1U, &problem) == UMI_STATUS_INVALID_STATE);
        REQUIRE(problem.kind == UMI_KANBAN_PROBLEM_STALE);
        REQUIRE(umi_kanban_board_activity_count(board) == activity);
        REQUIRE(umi_kanban_board_revision(board) == revision + 1U);
        UmiKanbanCardSnapshot row = Find(board, "task");
        REQUIRE(row.card.assignee[0] == '\0' && strcmp(row.column_id, "doing") == 0);
    } else if (strcmp(argv[1], "assign-atomic") == 0) {
        Assign(&edits[0], "task", "dev.alex");
        Action(&edits[1], UMI_KANBAN_MOVE_CARD, "task", "done");
        Refused(board, edits, 2U, UMI_KANBAN_PROBLEM_CHECKLIST);
        Action(&edits[1], UMI_KANBAN_MOVE_CARD, "task", "doing");
        uint64_t revision = umi_kanban_board_revision(board);
        Apply(board, edits, 2U);
        UmiKanbanCardSnapshot row = Find(board, "task");
        REQUIRE(strcmp(row.card.assignee, "dev.alex") == 0 && strcmp(row.column_id, "doing") == 0);
        REQUIRE(umi_kanban_board_revision(board) == revision + 1U);
    } else if (strcmp(argv[1], "assign-archived") == 0) {
        Action(edits, UMI_KANBAN_ARCHIVE_CARD, "task", NULL); Apply(board, edits, 1U);
        Assign(edits, "task", "dev.alex");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_ARCHIVED);
    } else if (strcmp(argv[1], "assign-order") == 0) {
        Create(edits, "later"); Apply(board, edits, 1U);
        Assign(edits, "task", "dev.alex"); Apply(board, edits, 1U);
        UmiKanbanCardSnapshot *rows = calloc(2U, sizeof(*rows));
        REQUIRE(rows != NULL);
        size_t count = 0U, total = 0U;
        REQUIRE(umi_kanban_board_page(board, umi_kanban_board_revision(board), NULL,
            0U, rows, 2U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 2U && total == 2U);
        REQUIRE(strcmp(rows[0].card.id, "task") == 0 && strcmp(rows[1].card.id, "later") == 0);
        UmiKanbanFilter filter = {0}; strcpy(filter.assignee, "dev.alex");
        REQUIRE(umi_kanban_board_page(board, umi_kanban_board_revision(board), &filter,
            0U, rows, 2U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 1U && total == 1U && strcmp(rows[0].card.id, "task") == 0);
        free(rows);
    } else if (strcmp(argv[1], "blocked-view") == 0) {
        Move(board, edits, "task", "blocked");
        UmiKanbanSummary summary;
        REQUIRE(umi_kanban_board_summary(board, umi_kanban_board_revision(board), 201U, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.blocked == 1U && summary.overdue == 1U && summary.columns[3] == 1U);
        UmiKanbanFilter filter = {0}; filter.blocked_only = 1;
        UmiKanbanCardSnapshot row; size_t count = 0U, total = 0U;
        REQUIRE(umi_kanban_board_page(board, umi_kanban_board_revision(board), &filter,
            0U, &row, 1U, &count, &total) == UMI_STATUS_OK);
        REQUIRE(count == 1U && total == 1U && row.card.blocked_reason[0] == '\0');
        Move(board, edits, "task", "testing");
        REQUIRE(umi_kanban_board_summary(board, umi_kanban_board_revision(board), 201U, &summary) == UMI_STATUS_OK);
        REQUIRE(summary.blocked == 0U && summary.columns[2] == 1U);
    } else if (strcmp(argv[1], "blocked-reason") == 0) {
        ChangeDetails(board, edits, "task"); strcpy(edits->card.blocked_reason, "Waiting for review"); Apply(board, edits, 1U);
        Move(board, edits, "task", "blocked");
        Action(edits, UMI_KANBAN_MOVE_CARD, "task", "testing");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_BLOCKED);
        ChangeDetails(board, &edits[0], "task"); edits[0].card.blocked_reason[0] = '\0';
        Action(&edits[1], UMI_KANBAN_MOVE_CARD, "task", "testing"); Apply(board, edits, 2U);
        REQUIRE(strcmp(Find(board, "task").column_id, "testing") == 0);
    } else if (strcmp(argv[1], "done-checklist") == 0) {
        Move(board, edits, "task", "blocked");
        Action(edits, UMI_KANBAN_MOVE_CARD, "task", "done");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_CHECKLIST);
        ChangeDetails(board, &edits[0], "task"); edits[0].card.checks[0].complete = 1;
        Action(&edits[1], UMI_KANBAN_MOVE_CARD, "task", "done"); Apply(board, edits, 2U);
        REQUIRE(strcmp(Find(board, "task").column_id, "done") == 0);
    } else if (strcmp(argv[1], "done-dependency") == 0) {
        Create(edits, "prerequisite"); Apply(board, edits, 1U);
        ChangeDetails(board, edits, "task"); edits->card.checks[0].complete = 1;
        edits->card.dependency_count = 1U; strcpy(edits->card.dependencies[0], "prerequisite"); Apply(board, edits, 1U);
        Move(board, edits, "task", "blocked");
        Action(edits, UMI_KANBAN_MOVE_CARD, "task", "done");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_DEPENDENCY);
        Move(board, edits, "prerequisite", "done"); Move(board, edits, "task", "done");
        Action(edits, UMI_KANBAN_MOVE_CARD, "prerequisite", "blocked");
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_DEPENDENCY);
    } else if (strcmp(argv[1], "blocked-wip") == 0) {
        Create(edits, "later"); Apply(board, edits, 1U);
        Action(edits, UMI_KANBAN_SET_WIP, NULL, "blocked"); edits->wip_limit = 1U; Apply(board, edits, 1U);
        Move(board, edits, "task", "blocked");
        Assign(&edits[0], "later", "dev.alex"); Action(&edits[1], UMI_KANBAN_MOVE_CARD, "later", "blocked");
        Refused(board, edits, 2U, UMI_KANBAN_PROBLEM_WIP);
        Action(&edits[2], UMI_KANBAN_MOVE_CARD, "task", "todo"); Apply(board, edits, 3U);
        REQUIRE(strcmp(Find(board, "later").card.assignee, "dev.alex") == 0);
    } else if (strcmp(argv[1], "archive-roundtrip") == 0 || strcmp(argv[1], "archive-schema") == 0) {
        Assign(edits, "task", "dev.alex"); Apply(board, edits, 1U); Move(board, edits, "task", "blocked");
        size_t size = 0U; unsigned char *bytes = Archive(board, &size);
        UmiValueArchiveInfo info;
        REQUIRE(umi_value_archive_inspect(bytes, size, &info) == UMI_STATUS_OK);
        REQUIRE(info.schema == UINT64_C(0x554d494b414e4202));
        UmiKanbanBoard *restored = NULL; uint64_t revision = 0U;
        if (strcmp(argv[1], "archive-schema") == 0) {
            /* A valid checksum cannot make the newer stage legal in the old schema. */
            UmiArchiveFinish(bytes, size, UINT64_C(0x554d494b414e4201));
            restored = board; revision = 999U;
            REQUIRE(umi_kanban_board_decode(bytes, size, &restored, &revision) != UMI_STATUS_OK);
            REQUIRE(restored == board && revision == 999U);
        } else {
            REQUIRE(umi_kanban_board_decode(bytes, size, &restored, &revision) == UMI_STATUS_OK);
            REQUIRE(revision == umi_kanban_board_revision(board));
            UmiKanbanCardSnapshot row = Find(restored, "task");
            REQUIRE(strcmp(row.card.assignee, "dev.alex") == 0 && strcmp(row.column_id, "blocked") == 0);
            UmiKanbanSummary summary;
            REQUIRE(umi_kanban_board_summary(restored, umi_kanban_board_revision(restored), 201U, &summary) == UMI_STATUS_OK);
            REQUIRE(summary.blocked == 1U);
            umi_kanban_board_destroy(restored);
        }
        free(bytes);
    } else if (strcmp(argv[1], "archive-legacy") == 0) {
        UmiKanbanBoard *legacy = Board(), *restored = NULL;
        Create(edits, "old"); Apply(legacy, edits, 1U);
        Assign(edits, "old", "dev.alex"); Apply(legacy, edits, 1U);
        size_t size = 0U; unsigned char *bytes = Archive(legacy, &size);
        UmiValueArchiveInfo info; uint64_t revision = 0U;
        REQUIRE(umi_value_archive_inspect(bytes, size, &info) == UMI_STATUS_OK);
        REQUIRE(info.schema == UINT64_C(0x554d494b414e4201));
        REQUIRE(umi_kanban_board_decode(bytes, size, &restored, &revision) == UMI_STATUS_OK);
        REQUIRE(strcmp(Find(restored, "old").card.assignee, "dev.alex") == 0);
        free(bytes); umi_kanban_board_destroy(restored); umi_kanban_board_destroy(legacy);
    } else REQUIRE(0);
    free(edits); umi_kanban_board_destroy(board);
    return 0;
}
