/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_capture.c
 *
 * PURPOSE:
 *   Verify explicit evidence admission and unchanged live boards during completion preview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc, char **argv)
{
    const char *cases[] = {"normal","owned","after-destroy","missing","archived","duplicate","stale",
        "invalid-hash","missing-hash","exception","exception-no-reason","invalid-decision","null",
        "oversized","empty","invalid-reviewer","source-empty","config-empty","invalid-utf8",
        "unterminated","wrong-column","complete-checklist","pending-checklist","blocked",
        "dependency","transition","already-done"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    UmiKanbanBoard *board = Ready();
    UmiKanbanCompletionRecord records[2]; Record(&records[0], "alpha"); Record(&records[1], "beta");
    UmiKanbanEdit *edit = Edits(1);
    UmiKanbanCardSnapshot card;
    REQUIRE(umi_kanban_board_find(board, "alpha", &card) == UMI_STATUS_OK);
    size_t count = 1;
    UmiStatus wanted = UMI_STATUS_OK;
    if (!strcmp(name, "missing")) { strcpy(records[0].card_id, "absent"); wanted = UMI_STATUS_NOT_FOUND; }
    if (!strcmp(name, "duplicate")) { records[1] = records[0]; count = 2; wanted = UMI_STATUS_ALREADY_EXISTS; }
    if (!strcmp(name, "invalid-hash")) { records[0].evidence_sha256[9] = 'g'; wanted = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "missing-hash")) { records[0].evidence_sha256[0] = '\0'; wanted = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strncmp(name, "exception", 9)) {
        records[0].decision = UMI_KANBAN_COMPLETION_EXCEPTION; records[0].evidence_sha256[0] = '\0';
        if (!strcmp(name, "exception-no-reason")) { records[0].rationale[0] = '\0'; wanted = UMI_STATUS_INVALID_ARGUMENT; }
    }
    if (!strcmp(name, "invalid-decision")) { records[0].decision = (UmiKanbanCompletionDecision)77; wanted = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "invalid-utf8")) { records[0].rationale[0] = (char)0xff; wanted = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "unterminated")) { memset(records[0].rationale, 'x', sizeof records[0].rationale); wanted = UMI_STATUS_INVALID_ARGUMENT; }
    if (!strcmp(name, "archived")) {
        Action(edit, UMI_KANBAN_ARCHIVE_CARD, "alpha", NULL); Apply(board, edit, 1); wanted = UMI_STATUS_INVALID_STATE;
    }
    if (!strcmp(name, "already-done")) { Move(board, edit, "alpha", "done"); wanted = UMI_STATUS_INVALID_STATE; }
    if (!strcmp(name, "transition")) { Move(board, edit, "alpha", "doing"); Move(board, edit, "alpha", "todo"); wanted = UMI_STATUS_INVALID_STATE; }
    if (strstr(name, "checklist") || !strcmp(name, "blocked") || !strcmp(name, "dependency")) {
        edit->kind = UMI_KANBAN_UPDATE_CARD; edit->card = card.card;
        if (strstr(name, "checklist")) {
            edit->card.check_count = 1; strcpy(edit->card.checks[0].text, "Review acceptance");
            edit->card.checks[0].complete = !strcmp(name, "complete-checklist");
            if (!edit->card.checks[0].complete) wanted = UMI_STATUS_INVALID_STATE;
        } else if (!strcmp(name, "blocked")) {
            strcpy(edit->card.blocked_reason, "Review is blocked"); wanted = UMI_STATUS_INVALID_STATE;
        } else {
            edit->card.dependency_count = 1; strcpy(edit->card.dependencies[0], "beta"); wanted = UMI_STATUS_INVALID_STATE;
        }
        Apply(board, edit, 1);
    }
    size_t size = 0, activity = umi_kanban_board_activity_count(board); void *before = Snapshot(board, &size);
    UmiKanbanCompletionPlan *plan = NULL;
    uint64_t revision = umi_kanban_board_revision(board);
    if (!strcmp(name, "null")) {
        REQUIRE(Capture(board, records, 1, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(Capture(NULL, records, 1, &plan) == UMI_STATUS_INVALID_ARGUMENT && !plan);
        REQUIRE(Capture(board, NULL, 1, &plan) == UMI_STATUS_INVALID_ARGUMENT && !plan);
    } else {
        if (!strcmp(name, "stale")) { --revision; wanted = UMI_STATUS_INVALID_STATE; }
        if (!strcmp(name, "oversized")) { count = UMI_KANBAN_MAX_EDITS + 1U; wanted = UMI_STATUS_CAPACITY_EXCEEDED; }
        if (!strcmp(name, "empty")) { count = 0; wanted = UMI_STATUS_INVALID_ARGUMENT; }
        if (!strcmp(name, "invalid-reviewer") || !strcmp(name, "source-empty") ||
            !strcmp(name, "config-empty") || !strcmp(name, "wrong-column")) wanted = UMI_STATUS_INVALID_ARGUMENT;
        UmiStatus status = UmiKanbanCompletionPlanCreate(board, revision,
            !strcmp(name, "wrong-column") ? "testing" : "done",
            !strcmp(name, "invalid-reviewer") ? "bad name" : "reviewer",
            !strcmp(name, "source-empty") ? "" : "synthetic-source",
            !strcmp(name, "config-empty") ? "" : "Debug/native", records, count, &plan);
        REQUIRE(status == wanted);
        if (status != UMI_STATUS_OK) REQUIRE(!plan);
        else {
            Unchanged(board, before, size, activity);
            if (!strcmp(name, "owned")) memset(records, 'x', sizeof records);
            if (!strcmp(name, "after-destroy")) { umi_kanban_board_destroy(board); board = NULL; }
            UmiKanbanCompletionPlanInfo info;
            REQUIRE(UmiKanbanCompletionPlanRead(plan, &info) == UMI_STATUS_OK);
            REQUIRE(info.card_count == 1 && info.reviewed_revision == revision && strlen(info.board_sha256) == 64);
            REQUIRE(!strcmp(UmiKanbanCompletionPlanAt(plan, 0)->card_id, "alpha"));
            REQUIRE(UmiKanbanCompletionPlanAt(plan, 1) == NULL);
        }
    }
    if (board) Unchanged(board, before, size, activity);
    free(before); free(edit); UmiKanbanCompletionPlanDestroy(plan); umi_kanban_board_destroy(board);
    return 0;
}
