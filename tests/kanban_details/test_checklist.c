/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_checklist.c
 *
 * PURPOSE:
 *   Check checklist replacement, preserved card fields and Done-stage rules.
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
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "card"); strcpy(edit->card.description, "Retain private description");
    strcpy(edit->card.assignee, "developer"); edit->card.priority = UMI_KANBAN_HIGH;
    edit->card.check_count = 1U; strcpy(edit->card.checks[0].text, "Original acceptance");
    edit->card.checks[0].complete = 1; Apply(board, edit, 1U);
    if (!strcmp(name, "done-refusal")) Done(board, edit, "card");
    if (!strcmp(name, "archived")) { Action(edit, UMI_KANBAN_ARCHIVE_CARD, "card", NULL); Apply(board, edit, 1U); }
    uint64_t revision = umi_kanban_board_revision(board), requested = !strcmp(name, "stale") ? revision - 1U : revision;
    const char *id = !strcmp(name, "missing") ? "missing" : "card";
    UmiKanbanCardChecklistSnapshot snapshot;
    memset(&snapshot, 0x5a, sizeof snapshot);
    unsigned char sentinel[sizeof snapshot]; memcpy(sentinel, &snapshot, sizeof snapshot);
    UmiStatus status = UmiKanbanCardChecklistRead(board, requested, id, &snapshot);
    if (!strcmp(name, "stale") || !strcmp(name, "missing") || !strcmp(name, "archived")) {
        REQUIRE(status != UMI_STATUS_OK && memcmp(sentinel, &snapshot, sizeof snapshot) == 0);
    } else REQUIRE(status == UMI_STATUS_OK && snapshot.checklist.count == 1U);
    UmiKanbanCardChecklist checklist = {0}; checklist.count = 2U;
    strcpy(checklist.checks[0].text, "First new acceptance"); checklist.checks[0].complete = 1;
    strcpy(checklist.checks[1].text, "Second pending acceptance");
    if (!strcmp(name, "empty")) checklist.count = 0U;
    else if (!strcmp(name, "maximum")) {
        checklist.count = UMI_KANBAN_MAX_CHECKS;
        for (size_t i = 0U; i < checklist.count; ++i)
            REQUIRE(snprintf(checklist.checks[i].text, sizeof checklist.checks[i].text, "Item %u", (unsigned)i) > 0);
    } else if (!strcmp(name, "count")) checklist.count = UMI_KANBAN_MAX_CHECKS + 1U;
    else if (!strcmp(name, "empty-text")) checklist.checks[0].text[0] = '\0';
    else if (!strcmp(name, "unterminated")) memset(checklist.checks[0].text, 'x', sizeof checklist.checks[0].text);
    else if (!strcmp(name, "invalid-complete")) checklist.checks[0].complete = 2;
    else if (!strcmp(name, "ignored-tail")) memset(&checklist.checks[2], 0xff, sizeof checklist.checks[2]);
    bool refused = !strcmp(name, "count") || !strcmp(name, "empty-text") || !strcmp(name, "unterminated") ||
        !strcmp(name, "invalid-complete") || !strcmp(name, "stale") || !strcmp(name, "missing") ||
        !strcmp(name, "archived") || !strcmp(name, "done-refusal") || !strcmp(name, "null");
    size_t before_size = 0U; unsigned char *before = Archive(board, &before_size);
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanCardChecklistRead(board, revision, "card", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCardChecklistRead(NULL, revision, "card", &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        status = UmiKanbanCardChecklistApply(board, revision, "card", "reviewer", NULL);
    } else status = UmiKanbanCardChecklistApply(board, requested, id, "reviewer", &checklist);
    if (refused) {
        REQUIRE(status != UMI_STATUS_OK);
        size_t after_size = 0U; unsigned char *after = Archive(board, &after_size);
        REQUIRE(before_size == after_size && !memcmp(before, after, after_size)); free(after);
    } else {
        REQUIRE(status == UMI_STATUS_OK && umi_kanban_board_revision(board) == revision + 1U);
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
        REQUIRE(row.card.check_count == checklist.count && !strcmp(row.card.description, "Retain private description"));
        REQUIRE(!strcmp(row.card.assignee, "developer") && row.card.priority == UMI_KANBAN_HIGH);
        REQUIRE(!strcmp(row.column_id, "todo"));
        for (size_t i = 0U; i < checklist.count; ++i) {
            REQUIRE(!strcmp(row.card.checks[i].text, checklist.checks[i].text));
            REQUIRE(row.card.checks[i].complete == checklist.checks[i].complete);
        }
        REQUIRE(!strcmp(snapshot.checklist.checks[0].text, "Original acceptance"));
        REQUIRE(UmiKanbanCardChecklistApply(board, revision, "card", "reviewer", &checklist) == UMI_STATUS_INVALID_STATE);
    }
    free(before); free(edit); umi_kanban_board_destroy(board); return 0;
}
