/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_apply.c
 *
 * PURPOSE:
 *   Require exact reviewed content and retain all existing board policies when completing cards.
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
    const char *cases[] = {"normal","batch","stale","changed-card","different-content","different-board",
        "dependency-batch","exception","exception-blocked","replay","null","activity"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    UmiKanbanBoard *board = Ready(), *other = NULL;
    UmiKanbanCompletionRecord records[2]; Record(&records[0], "alpha"); Record(&records[1], "beta");
    size_t count = !strcmp(name, "batch") || !strcmp(name, "dependency-batch") ? 2U : 1U;
    UmiKanbanEdit *edit = Edits(1);
    if (!strcmp(name, "dependency-batch")) {
        UmiKanbanCardSnapshot card; REQUIRE(umi_kanban_board_find(board, "alpha", &card) == UMI_STATUS_OK);
        edit->kind = UMI_KANBAN_UPDATE_CARD; edit->card = card.card;
        edit->card.dependency_count = 1; strcpy(edit->card.dependencies[0], "beta"); Apply(board, edit, 1);
    }
    if (!strncmp(name, "exception", 9)) {
        records[0].decision = UMI_KANBAN_COMPLETION_EXCEPTION; records[0].evidence_sha256[0] = '\0';
    }
    if (!strcmp(name, "different-content")) {
        other = Ready(); Update(board, "alpha", "alpha"); Update(other, "alpha", "Other acceptance");
        REQUIRE(umi_kanban_board_revision(board) == umi_kanban_board_revision(other));
    }
    if (!strcmp(name, "different-board")) {
        UmiKanbanConfig config; REQUIRE(umi_kanban_board_config(board, &config) == UMI_STATUS_OK);
        strcpy(config.id, "another");
        REQUIRE(umi_kanban_board_create(&config, &other) == UMI_STATUS_OK);
    }
    UmiKanbanCompletionPlan *plan = NULL;
    REQUIRE(Capture(board, records, count, &plan) == UMI_STATUS_OK);
    uint64_t reviewed = umi_kanban_board_revision(board);
    REQUIRE(UmiKanbanCompletionPlanCheck(plan, board, reviewed) == UMI_STATUS_OK);
    if (!strcmp(name, "changed-card")) Update(board, "alpha", "Changed criteria");
    if (!strcmp(name, "exception-blocked")) {
        UmiKanbanCardSnapshot card; REQUIRE(umi_kanban_board_find(board, "alpha", &card) == UMI_STATUS_OK);
        edit->kind = UMI_KANBAN_UPDATE_CARD; edit->card = card.card;
        strcpy(edit->card.blocked_reason, "Still blocked"); Apply(board, edit, 1);
        UmiKanbanCompletionPlan *refused = NULL;
        REQUIRE(Capture(board, records, 1, &refused) == UMI_STATUS_INVALID_STATE && !refused);
    }
    UmiKanbanBoard *target = other ? other : board;
    size_t size = 0, activity = umi_kanban_board_activity_count(target);
    void *before = Snapshot(target, &size);
    uint64_t revision = umi_kanban_board_revision(target);
    int refused = !strcmp(name, "stale") || !strcmp(name, "changed-card") || other ||
        !strcmp(name, "exception-blocked");
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanCompletionPlanApply(NULL, board, reviewed) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCompletionPlanApply(plan, NULL, reviewed) == UMI_STATUS_INVALID_ARGUMENT);
        Unchanged(target, before, size, activity);
    } else {
        UmiStatus status = UmiKanbanCompletionPlanApply(plan, target,
            !strcmp(name, "stale") ? reviewed - 1U : revision);
        if (refused) { REQUIRE(status != UMI_STATUS_OK); Unchanged(target, before, size, activity); }
        else {
            REQUIRE(status == UMI_STATUS_OK);
            REQUIRE(umi_kanban_board_revision(target) == revision + 1U);
            for (size_t i = 0; i < count; ++i) {
                UmiKanbanCardSnapshot card;
                REQUIRE(umi_kanban_board_find(target, records[i].card_id, &card) == UMI_STATUS_OK);
                REQUIRE(!strcmp(card.column_id, "done"));
            }
            if (count == 1U) {
                UmiKanbanCardSnapshot card; REQUIRE(umi_kanban_board_find(target, "beta", &card) == UMI_STATUS_OK);
                REQUIRE(!strcmp(card.column_id, "testing"));
            }
            if (!strcmp(name, "activity")) {
                UmiKanbanActivity event;
                REQUIRE(umi_kanban_board_activity_at(board, revision + 1U,
                    umi_kanban_board_activity_count(board) - 1U, &event) == UMI_STATUS_OK);
                REQUIRE(!strcmp(event.actor, "reviewer") && event.kind == UMI_KANBAN_MOVE_CARD);
            }
            if (!strcmp(name, "replay")) {
                free(before); before = Snapshot(target, &size); activity = umi_kanban_board_activity_count(target);
                REQUIRE(UmiKanbanCompletionPlanApply(plan, target, revision + 1U) == UMI_STATUS_INVALID_STATE);
                Unchanged(target, before, size, activity);
            }
        }
    }
    free(before); free(edit); UmiKanbanCompletionPlanDestroy(plan);
    umi_kanban_board_destroy(board); umi_kanban_board_destroy(other); return 0;
}
