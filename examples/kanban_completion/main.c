/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/kanban_completion/main.c
 *
 * PURPOSE:
 *   Demonstrate private review retention and checked completion through installed public APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/kanban/completion_storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus Edit(UmiKanbanBoard *board, UmiKanbanEdit *edit)
{
    return umi_kanban_board_apply(board, umi_kanban_board_revision(board),
        "example-owner", edit, 1U, NULL);
}
static UmiStatus Move(UmiKanbanBoard *board, UmiKanbanEdit *edit, const char *column)
{
    memset(edit, 0, sizeof *edit);
    edit->kind = UMI_KANBAN_MOVE_CARD;
    strcpy(edit->card.id, "example");
    strcpy(edit->column_id, column);
    return Edit(board, edit);
}
#define REQUIRE_OK(call) do { status = (call); if (status != UMI_STATUS_OK) goto failed; } while (0)
int main(int argc, char **argv)
{
    if (argc != 3 || (strcmp(argv[1], "normal") && strcmp(argv[1], "blocked") &&
                     strcmp(argv[1], "stale"))) {
        fprintf(stderr, "Usage: kanban-completion-consumer normal|blocked|stale ABSOLUTE_NEW.umicompletion\n");
        return 2;
    }
    UmiKanbanBoard *board = NULL;
    UmiKanbanCompletionPlan *plan = NULL, *loaded = NULL;
    UmiKanbanEdit *edit = calloc(1U, sizeof *edit);
    UmiStatus status = edit ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
    int result = 2;
    if (!edit) goto failed;
    REQUIRE_OK(umi_kanban_board_create_development("example-board", "Synthetic SDK example", &board));
    edit->kind = UMI_KANBAN_CREATE_CARD;
    strcpy(edit->card.id, "example");
    strcpy(edit->card.title, "Review a synthetic feature");
    strcpy(edit->column_id, "todo");
    if (!strcmp(argv[1], "blocked")) {
        edit->card.check_count = 1U;
        strcpy(edit->card.checks[0].text, "Acceptance is still incomplete");
    }
    REQUIRE_OK(Edit(board, edit));
    REQUIRE_OK(Move(board, edit, "doing"));
    REQUIRE_OK(Move(board, edit, "testing"));
    uint64_t revision = umi_kanban_board_revision(board);
    UmiKanbanCompletionRecord record = {0};
    strcpy(record.card_id, "example");
    strcpy(record.rationale, "Demonstration decision only; this is not product qualification.");
    const char evidence[] = "Synthetic SDK review fixture";
    REQUIRE_OK(UmiSha256Buffer(evidence, sizeof evidence - 1U, record.evidence_sha256));
    status = UmiKanbanCompletionPlanCreate(board, revision, "done", "example-reviewer",
        "synthetic-sdk-source", "example-only", &record, 1U, &plan);
    if (!strcmp(argv[1], "blocked")) {
        if (status != UMI_STATUS_INVALID_STATE || plan || umi_kanban_board_revision(board) != revision)
            goto failed;
        puts("Incomplete checklist refused; no review file or completion was created.");
        result = 0; goto cleanup;
    }
    if (status != UMI_STATUS_OK) goto failed;
    REQUIRE_OK(UmiKanbanCompletionPlanWriteNew(argv[2], plan));
    REQUIRE_OK(UmiKanbanCompletionPlanLoadFile(argv[2], &loaded));
    /* Loading an archive is not permission to mutate a live board. Recheck its
     * reviewed state immediately before the explicit owner action. */
    if (!strcmp(argv[1], "stale")) {
        UmiKanbanCardSnapshot row;
        REQUIRE_OK(umi_kanban_board_find(board, "example", &row));
        memset(edit, 0, sizeof *edit); edit->kind = UMI_KANBAN_UPDATE_CARD;
        edit->card = row.card; strcpy(edit->card.title, "Changed acceptance");
        REQUIRE_OK(Edit(board, edit));
        revision = umi_kanban_board_revision(board);
        status = UmiKanbanCompletionPlanApply(loaded, board, revision);
        if (status != UMI_STATUS_INVALID_STATE || umi_kanban_board_revision(board) != revision)
            goto failed;
        REQUIRE_OK(umi_kanban_board_find(board, "example", &row));
        if (strcmp(row.column_id, "testing")) goto failed;
        puts("Stale completion refused; the card remains Testing and the historical review is retained.");
        result = 0; goto cleanup;
    }
    REQUIRE_OK(UmiKanbanCompletionPlanCheck(loaded, board, revision));
    REQUIRE_OK(UmiKanbanCompletionPlanApply(loaded, board, revision));
    UmiKanbanCardSnapshot row;
    REQUIRE_OK(umi_kanban_board_find(board, "example", &row));
    if (strcmp(row.column_id, "done") || umi_kanban_board_revision(board) != revision + 1U)
        goto failed;
    puts("Synthetic card completed; private review retained. No product tests or public publication occurred.");
    result = 0; goto cleanup;
failed:
    if (status == UMI_STATUS_OK) status = UMI_STATUS_INVALID_STATE;
    fprintf(stderr, "Completion example refused: %s. Existing evidence is retained.\n", umi_status_text(status));
cleanup:
    UmiKanbanCompletionPlanDestroy(loaded); UmiKanbanCompletionPlanDestroy(plan);
    umi_kanban_board_destroy(board); free(edit); return result;
}
