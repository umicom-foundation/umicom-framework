/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_details.c
 *
 * PURPOSE:
 *   Exercise narrow card updates, retained fields and stale-form refusal.
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
#include "umicom/kanban/publication.h"

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
    Create(edit, "dependency"); Apply(board, edit, 1U);
    Create(edit, "card"); strcpy(edit->card.assignee, "developer");
    strcpy(edit->card.description, "Private original");
    edit->card.due_unix_seconds = UINT64_C(123456789);
    edit->card.label_count = 1U; strcpy(edit->card.labels[0], "release");
    edit->card.check_count = 1U; strcpy(edit->card.checks[0].text, "Acceptance check");
    edit->card.checks[0].complete = 1;
    edit->card.dependency_count = 1U; strcpy(edit->card.dependencies[0], "dependency");
    Apply(board, edit, 1U);
    if (!strcmp(name, "done-blocker")) {
        Done(board, edit, "dependency"); Done(board, edit, "card");
    }
    if (!strcmp(name, "archived")) {
        Action(edit, UMI_KANBAN_ARCHIVE_CARD, "card", NULL); Apply(board, edit, 1U);
    }
    uint64_t revision = umi_kanban_board_revision(board);
    UmiKanbanCardDetailSnapshot snapshot;
    memset(&snapshot, 0x5a, sizeof snapshot);
    unsigned char sentinel[sizeof snapshot]; memcpy(sentinel, &snapshot, sizeof snapshot);
    const char *id = !strcmp(name, "missing") ? "missing" : "card";
    uint64_t requested = !strcmp(name, "stale") ? revision - 1U : revision;
    UmiStatus status = UmiKanbanCardDetailsRead(board, requested, id, &snapshot);
    if (!strcmp(name, "missing") || !strcmp(name, "stale") || !strcmp(name, "archived")) {
        REQUIRE(status == (!strcmp(name, "missing") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_STATE));
        REQUIRE(memcmp(sentinel, &snapshot, sizeof snapshot) == 0);
    } else {
        REQUIRE(status == UMI_STATUS_OK && !strcmp(snapshot.card_id, "card") && snapshot.board_revision == revision);
    }
    UmiKanbanCardDetails details = {0};
    strcpy(details.title, "Reviewed title"); strcpy(details.description, "Private updated description");
    strcpy(details.blocked_reason, "Waiting for evidence"); details.priority = UMI_KANBAN_HIGH;
    bool invalid = false;
    if (!strcmp(name, "empty-title")) { details.title[0] = '\0'; invalid = true; }
    else if (!strcmp(name, "title-limit")) memset(details.title, 't', sizeof details.title - 1U);
    else if (!strcmp(name, "description-limit")) memset(details.description, 'd', sizeof details.description - 1U);
    else if (!strcmp(name, "blocker-limit")) memset(details.blocked_reason, 'b', sizeof details.blocked_reason - 1U);
    else if (!strcmp(name, "title-unclosed")) { memset(details.title, 't', sizeof details.title); invalid = true; }
    else if (!strcmp(name, "description-unclosed")) { memset(details.description, 'd', sizeof details.description); invalid = true; }
    else if (!strcmp(name, "blocker-unclosed")) { memset(details.blocked_reason, 'b', sizeof details.blocked_reason); invalid = true; }
    else if (!strcmp(name, "priority")) { details.priority = (UmiKanbanPriority)99; invalid = true; }
    else if (!strcmp(name, "clear")) { details.description[0] = '\0'; details.blocked_reason[0] = '\0'; }
    size_t before_size = 0U; unsigned char *before = Archive(board, &before_size);
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanCardDetailsCheck(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCardDetailsRead(NULL, revision, "card", &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCardDetailsRead(board, revision, "card", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCardDetailsApply(board, revision, "card", "reviewer", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        status = UMI_STATUS_INVALID_ARGUMENT;
    } else {
        status = UmiKanbanCardDetailsApply(board, requested, id,
            !strcmp(name, "actor") ? "" : "reviewer", &details);
    }
    bool refused = invalid || !strcmp(name, "stale") || !strcmp(name, "missing") ||
        !strcmp(name, "archived") || !strcmp(name, "null") || !strcmp(name, "actor") || !strcmp(name, "done-blocker");
    if (refused) {
        REQUIRE(status != UMI_STATUS_OK);
        size_t after_size = 0U; unsigned char *after = Archive(board, &after_size);
        REQUIRE(after_size == before_size && memcmp(before, after, before_size) == 0);
        free(after);
    } else {
        REQUIRE(status == UMI_STATUS_OK && umi_kanban_board_revision(board) == revision + 1U);
        UmiKanbanCardSnapshot row;
        REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
        REQUIRE(!strcmp(row.card.title, details.title) && !strcmp(row.card.description, details.description));
        REQUIRE(!strcmp(row.card.blocked_reason, details.blocked_reason) && row.card.priority == details.priority);
        REQUIRE(!strcmp(row.card.assignee, "developer") && row.card.due_unix_seconds == UINT64_C(123456789));
        REQUIRE(row.card.label_count == 1U && !strcmp(row.card.labels[0], "release"));
        REQUIRE(row.card.check_count == 1U && row.card.checks[0].complete == 1);
        REQUIRE(!strcmp(row.card.checks[0].text, "Acceptance check"));
        REQUIRE(row.card.dependency_count == 1U && !strcmp(row.card.dependencies[0], "dependency"));
        REQUIRE(!strcmp(row.column_id, "todo") && !row.archived);
        REQUIRE(UmiKanbanCardDetailsApply(board, revision, "card", "reviewer", &details) == UMI_STATUS_INVALID_STATE);

        if (!strcmp(name, "roundtrip")) {
            size_t size = 0U; unsigned char *encoded = Archive(board, &size);
            UmiKanbanBoard *copy = NULL;
            /* The earlier draft omitted the required saved-revision output.
             * Keep it for review; the public decoder returns history separately
             * from the reopened board revision, which starts at one. */
#if 0
            REQUIRE(umi_kanban_board_decode(encoded, size, &copy) == UMI_STATUS_OK);
#endif
            uint64_t saved_revision = 0U;
            REQUIRE(umi_kanban_board_decode(encoded, size, &copy, &saved_revision) == UMI_STATUS_OK);
            REQUIRE(saved_revision == revision + 1U);
            UmiKanbanCardDetailSnapshot restored;
            REQUIRE(UmiKanbanCardDetailsRead(copy, umi_kanban_board_revision(copy), "card", &restored) == UMI_STATUS_OK);
            REQUIRE(!strcmp(restored.details.description, details.description) &&
                !strcmp(restored.details.blocked_reason, details.blocked_reason) &&
                restored.details.priority == details.priority);
            free(encoded); umi_kanban_board_destroy(copy);
        } else if (!strcmp(name, "public-privacy")) {
            UmiKanbanPublicCard public_card = {0};
            strcpy(public_card.card_id, "card"); strcpy(public_card.title, "Public feature");
            size_t size = 0U;
            REQUIRE(umi_kanban_publication_html(board, revision + 1U, "Public board",
                &public_card, 1U, NULL, 0U, &size) == UMI_STATUS_OK);
            char *html = calloc(size + 1U, 1U); REQUIRE(html);
            REQUIRE(umi_kanban_publication_html(board, revision + 1U, "Public board",
                &public_card, 1U, html, size + 1U, &size) == UMI_STATUS_OK);
            REQUIRE(strstr(html, "Public feature") && !strstr(html, details.description) &&
                !strstr(html, details.blocked_reason) && !strstr(html, "Reviewed title"));
            free(html);
        }

        /* The earlier copied snapshot remains independent of the live update. */
        REQUIRE(!strcmp(snapshot.details.description, "Private original"));
    }
    free(before); free(edit); umi_kanban_board_destroy(board); return 0;
}
