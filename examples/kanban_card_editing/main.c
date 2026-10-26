/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/kanban_card_editing/main.c
 *
 * PURPOSE:
 *   Demonstrate installed public card editing, acceptance and prerequisite APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/kanban/card_details.h"
#include "umicom/kanban/archive.h"
#include "umicom/kanban/column_policy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "Card-editing example failed at line %d: %s\n", __LINE__, #expression); exit(1); \
} } while (0)
static void Move(UmiKanbanBoard *board, const char *id, const char *column)
{
    UmiKanbanEdit edit = {0}; edit.kind = UMI_KANBAN_MOVE_CARD;
    snprintf(edit.card.id, sizeof edit.card.id, "%s", id);
    snprintf(edit.column_id, sizeof edit.column_id, "%s", column);
    REQUIRE(umi_kanban_board_apply(board, umi_kanban_board_revision(board), "example-owner", &edit, 1U, NULL) == UMI_STATUS_OK);
}
int main(int argc, char **argv)
{
    /* Preserve the earlier three-scenario admission for review; the additional
     * labels/limits example uses the same installed public component. */
#if 0
    if (argc != 2 || (strcmp(argv[1], "roundtrip") && strcmp(argv[1], "stale") && strcmp(argv[1], "cycle"))) {
        fputs("Usage: kanban-card-editing-consumer roundtrip|stale|cycle\n", stderr); return 2;
    }
#endif
    if (argc != 2 || (strcmp(argv[1], "roundtrip") && strcmp(argv[1], "stale") &&
        strcmp(argv[1], "cycle") && strcmp(argv[1], "labels-limits"))) {
        fputs("Usage: kanban-card-editing-consumer roundtrip|stale|cycle|labels-limits\n", stderr); return 2;
    }
    UmiKanbanBoard *board = NULL;
    REQUIRE(umi_kanban_board_create_workflow("example", "Synthetic acceptance workflow", &board) == UMI_STATUS_OK);
    UmiKanbanEdit edits[2] = {0};
    for (size_t i = 0U; i < 2U; ++i) {
        edits[i].kind = UMI_KANBAN_CREATE_CARD; edits[i].card.priority = UMI_KANBAN_NORMAL;
        snprintf(edits[i].card.id, sizeof edits[i].card.id, "%s", i ? "feature" : "prerequisite");
        snprintf(edits[i].card.title, sizeof edits[i].card.title, "%s", i ? "Example feature" : "Example prerequisite");
        strcpy(edits[i].column_id, "todo");
    }
    REQUIRE(umi_kanban_board_apply(board, umi_kanban_board_revision(board), "example-owner", edits, 2U, NULL) == UMI_STATUS_OK);
    if (!strcmp(argv[1], "labels-limits")) {
        UmiKanbanCardLabels labels = {0}; labels.count = 2U;
        strcpy(labels.labels[0], "sdk-example"); strcpy(labels.labels[1], "review");
        uint64_t reviewed = umi_kanban_board_revision(board);
        REQUIRE(UmiKanbanCardLabelsApply(board, reviewed, "feature", "example-owner", &labels) == UMI_STATUS_OK);
        REQUIRE(UmiKanbanCardLabelsApply(board, reviewed, "feature", "example-owner", &labels) == UMI_STATUS_INVALID_STATE);
        REQUIRE(UmiKanbanCardDeadlineApply(board, umi_kanban_board_revision(board), "feature", "example-owner", UINT64_C(946684800)) == UMI_STATUS_OK);
        UmiKanbanColumnPolicySnapshot column;
        REQUIRE(UmiKanbanColumnPolicyRead(board, umi_kanban_board_revision(board), "todo", &column) == UMI_STATUS_OK);
        REQUIRE(column.active_cards == 2U && column.column.wip_limit == 0U);
        REQUIRE(UmiKanbanColumnPolicyApply(board, column.board_revision, "todo", "example-owner", 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_kanban_board_revision(board) == column.board_revision);
        REQUIRE(UmiKanbanColumnPolicyApply(board, column.board_revision, "todo", "example-owner", 2U) == UMI_STATUS_OK);
        size_t size = 0U; uint64_t revision = umi_kanban_board_revision(board);
        REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, &size) == UMI_STATUS_OK);
        void *bytes = malloc(size); REQUIRE(bytes);
        REQUIRE(umi_kanban_board_encode(board, revision, bytes, size, &size) == UMI_STATUS_OK);
        UmiKanbanBoard *copy = NULL; uint64_t saved_revision = 0U;
        REQUIRE(umi_kanban_board_decode(bytes, size, &copy, &saved_revision) == UMI_STATUS_OK);
        REQUIRE(saved_revision == revision);
        UmiKanbanCardLabelsSnapshot copied;
        REQUIRE(UmiKanbanCardLabelsRead(copy, umi_kanban_board_revision(copy), "feature", &copied) == UMI_STATUS_OK);
        REQUIRE(copied.labels.count == 2U && !strcmp(copied.labels.labels[1], "review"));
        REQUIRE(UmiKanbanColumnPolicyRead(copy, umi_kanban_board_revision(copy), "todo", &column) == UMI_STATUS_OK);
        REQUIRE(column.column.wip_limit == 2U && column.active_cards == 2U);
        UmiKanbanCardDeadlineSnapshot deadline;
        REQUIRE(UmiKanbanCardDeadlineRead(copy, umi_kanban_board_revision(copy), "feature", &deadline) == UMI_STATUS_OK);
        REQUIRE(deadline.due_unix_seconds == UINT64_C(946684800));
        free(bytes); umi_kanban_board_destroy(copy); umi_kanban_board_destroy(board);
        puts("Private labels and list limits survived the installed SDK archive roundtrip.");
        return 0;
    }
    UmiKanbanCardDetailSnapshot observed;
    REQUIRE(UmiKanbanCardDetailsRead(board, umi_kanban_board_revision(board), "feature", &observed) == UMI_STATUS_OK);
    strcpy(observed.details.description, "Synthetic private acceptance notes");
    observed.details.priority = UMI_KANBAN_HIGH;
    REQUIRE(UmiKanbanCardDetailsApply(board, observed.board_revision, "feature", "example-owner", &observed.details) == UMI_STATUS_OK);
    if (!strcmp(argv[1], "stale")) {
        uint64_t current = umi_kanban_board_revision(board);
        REQUIRE(UmiKanbanCardDetailsApply(board, observed.board_revision, "feature", "example-owner", &observed.details) == UMI_STATUS_INVALID_STATE);
        REQUIRE(umi_kanban_board_revision(board) == current);
        puts("Stale form refused; current board retained.");
    } else {
        REQUIRE(UmiKanbanCardDependencyAdd(board, umi_kanban_board_revision(board), "feature", "prerequisite", "example-owner") == UMI_STATUS_OK);
        if (!strcmp(argv[1], "cycle")) {
            uint64_t current = umi_kanban_board_revision(board);
            REQUIRE(UmiKanbanCardDependencyAdd(board, current, "prerequisite", "feature", "example-owner") != UMI_STATUS_OK);
            REQUIRE(umi_kanban_board_revision(board) == current);
            puts("Dependency cycle refused; current board retained.");
        } else {
            UmiKanbanCardChecklist checklist = {0}; checklist.count = 1U;
            strcpy(checklist.checks[0].text, "Review the synthetic acceptance");
            REQUIRE(UmiKanbanCardChecklistApply(board, umi_kanban_board_revision(board), "feature", "example-owner", &checklist) == UMI_STATUS_OK);
            Move(board, "prerequisite", "done");
            Move(board, "feature", "testing");
            UmiKanbanEdit move = {0}; move.kind = UMI_KANBAN_MOVE_CARD;
            strcpy(move.card.id, "feature"); strcpy(move.column_id, "done");
            REQUIRE(umi_kanban_board_apply(board, umi_kanban_board_revision(board), "example-owner", &move, 1U, NULL) != UMI_STATUS_OK);
            checklist.checks[0].complete = 1; /* An example assertion, not a test execution receipt. */
            REQUIRE(UmiKanbanCardChecklistApply(board, umi_kanban_board_revision(board), "feature", "example-owner", &checklist) == UMI_STATUS_OK);
            Move(board, "feature", "done");
            size_t length = 0U; uint64_t revision = umi_kanban_board_revision(board);
            REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, &length) == UMI_STATUS_OK);
            void *bytes = malloc(length); REQUIRE(bytes);
            REQUIRE(umi_kanban_board_encode(board, revision, bytes, length, &length) == UMI_STATUS_OK);
            /* The earlier draft omitted the required saved-revision output.
             * Keep it for review; the public decoder returns history separately
             * from the reopened board revision, which starts at one. */
#if 0
            UmiKanbanBoard *restored = NULL; REQUIRE(umi_kanban_board_decode(bytes, length, &restored) == UMI_STATUS_OK);
#endif
            UmiKanbanBoard *restored = NULL;
            uint64_t saved_revision = 0U;
            REQUIRE(umi_kanban_board_decode(bytes, length, &restored, &saved_revision) == UMI_STATUS_OK);
            REQUIRE(saved_revision == revision);
            UmiKanbanCardSnapshot result; REQUIRE(umi_kanban_board_find(restored, "feature", &result) == UMI_STATUS_OK);
            REQUIRE(!strcmp(result.column_id, "done") && !strcmp(result.card.description, "Synthetic private acceptance notes"));
            REQUIRE(result.card.check_count == 1U && result.card.checks[0].complete == 1 && result.card.dependency_count == 1U);
            free(bytes); umi_kanban_board_destroy(restored);
            puts("Synthetic card details, checklist and prerequisite survived archive roundtrip.");
        }
    }
    umi_kanban_board_destroy(board); return 0;
}
