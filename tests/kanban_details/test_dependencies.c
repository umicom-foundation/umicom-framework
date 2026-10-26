/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_dependencies.c
 *
 * PURPOSE:
 *   Check prerequisite edits, cycle prevention and atomic refusal.
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
static unsigned char *Archive(UmiKanbanBoard *board, size_t *size)
{
    uint64_t revision = umi_kanban_board_revision(board);
    REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, size) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(*size); REQUIRE(bytes);
    REQUIRE(umi_kanban_board_encode(board, revision, bytes, *size, size) == UMI_STATUS_OK);
    return bytes;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    UmiKanbanBoard *initial = Board(), *board = NULL; UmiKanbanConfig config;
    REQUIRE(umi_kanban_board_config(initial, &config) == UMI_STATUS_OK); config.card_capacity = 32U;
    REQUIRE(umi_kanban_board_create(&config, &board) == UMI_STATUS_OK); umi_kanban_board_destroy(initial);
    UmiKanbanEdit *edit = Edits(1U); Create(edit, "card");
    strcpy(edit->card.description, "Private card details"); strcpy(edit->card.assignee, "developer");
    edit->card.priority = UMI_KANBAN_URGENT; edit->card.due_unix_seconds = 123U;
    Apply(board, edit, 1U); Create(edit, "required"); Apply(board, edit, 1U); Create(edit, "other"); Apply(board, edit, 1U);
    bool remove = !strcmp(name, "remove") || !strcmp(name, "remove-missing") || !strcmp(name, "remove-order");
    if (!strcmp(name, "remove") || !strcmp(name, "remove-order") || !strcmp(name, "duplicate")) {
        REQUIRE(UmiKanbanCardDependencyAdd(board, umi_kanban_board_revision(board), "card", "required", "owner") == UMI_STATUS_OK);
        if (!strcmp(name, "remove-order"))
            REQUIRE(UmiKanbanCardDependencyAdd(board, umi_kanban_board_revision(board), "card", "other", "owner") == UMI_STATUS_OK);
    }
    if (!strcmp(name, "cycle")) REQUIRE(UmiKanbanCardDependencyAdd(board, umi_kanban_board_revision(board),
        "required", "card", "owner") == UMI_STATUS_OK);
    if (!strcmp(name, "done")) Done(board, edit, "card");
    if (!strcmp(name, "archived") || !strcmp(name, "required-archived")) {
        Action(edit, UMI_KANBAN_ARCHIVE_CARD, !strcmp(name, "archived") ? "card" : "required", NULL);
        Apply(board, edit, 1U);
    }
    if (!strcmp(name, "capacity")) {
        for (size_t i = 0U; i < UMI_KANBAN_MAX_DEPENDENCIES; ++i) {
            char id[64]; REQUIRE(snprintf(id, sizeof id, "filler-%u", (unsigned)i) > 0);
            Create(edit, id); Apply(board, edit, 1U);
            REQUIRE(UmiKanbanCardDependencyAdd(board, umi_kanban_board_revision(board), "card", id, "owner") == UMI_STATUS_OK);
        }
    }
    uint64_t revision = umi_kanban_board_revision(board), requested = !strcmp(name, "stale") ? revision - 1U : revision;
    const char *required = !strcmp(name, "self") ? "card" : !strcmp(name, "missing-required") ? "absent" :
        !strcmp(name, "invalid-id") ? "bad id" : "required";
    size_t before_size = 0U; unsigned char *before = Archive(board, &before_size);
    UmiStatus status = remove ? UmiKanbanCardDependencyRemove(board, requested, "card", required, "reviewer") :
        UmiKanbanCardDependencyAdd(!strcmp(name, "null") ? NULL : board, requested,
            !strcmp(name, "missing-card") ? "absent" : "card", required, !strcmp(name, "actor") ? "" : "reviewer");
    bool accepted = !strcmp(name, "normal") || !strcmp(name, "remove") || !strcmp(name, "remove-order");
    if (accepted) {
        REQUIRE(status == UMI_STATUS_OK && umi_kanban_board_revision(board) == revision + 1U);
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
        REQUIRE(!strcmp(row.card.description, "Private card details") && !strcmp(row.card.assignee, "developer"));
        REQUIRE(row.card.priority == UMI_KANBAN_URGENT && row.card.due_unix_seconds == 123U && !strcmp(row.column_id, "todo"));
        REQUIRE(row.card.dependency_count == (!strcmp(name, "remove") ? 0U : 1U));
        if (row.card.dependency_count)
            REQUIRE(!strcmp(row.card.dependencies[0], !strcmp(name, "remove-order") ? "other" : "required"));
    } else {
        REQUIRE(status != UMI_STATUS_OK);
        if (!strcmp(name, "duplicate")) REQUIRE(status == UMI_STATUS_ALREADY_EXISTS);
        if (!strcmp(name, "remove-missing")) REQUIRE(status == UMI_STATUS_NOT_FOUND);
        if (!strcmp(name, "capacity")) REQUIRE(status == UMI_STATUS_CAPACITY_EXCEEDED);
        size_t after_size = 0U; unsigned char *after = Archive(board, &after_size);
        REQUIRE(before_size == after_size && !memcmp(before, after, after_size)); free(after);
    }
    free(before); free(edit); umi_kanban_board_destroy(board); return 0;
}
