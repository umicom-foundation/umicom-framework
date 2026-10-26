/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_labels.c
 *
 * PURPOSE:
 *   Check label-only edits, archives, exact filtering and atomic refusal.
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

static unsigned char *Encoded(UmiKanbanBoard *board, size_t *size)
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
    const char *known[] = {"normal", "clear", "maximum", "length", "unicode", "case", "count", "empty", "unterminated", "duplicate", "ignored-tail", "stale", "missing", "archived", "null", "actor", "done", "roundtrip", "filter", "public-privacy"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized);
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "card"); strcpy(edit->card.assignee, "owner");
    strcpy(edit->card.description, "Private note"); edit->card.due_unix_seconds = 123U;
    edit->card.check_count = 1U; strcpy(edit->card.checks[0].text, "Review"); edit->card.checks[0].complete = 1;
    edit->card.label_count = 1U; strcpy(edit->card.labels[0], "old"); Apply(board, edit, 1U);
    if (!strcmp(name, "done")) Done(board, edit, "card");
    if (!strcmp(name, "archived")) { Action(edit, UMI_KANBAN_ARCHIVE_CARD, "card", NULL); Apply(board, edit, 1U); }
    uint64_t revision = umi_kanban_board_revision(board);
    uint64_t requested = !strcmp(name, "stale") ? revision - 1U : revision;
    const char *id = !strcmp(name, "missing") ? "missing" : "card";
    UmiKanbanCardLabelsSnapshot snapshot;
    memset(&snapshot, 0x5a, sizeof snapshot);
    unsigned char unchanged[sizeof snapshot]; memcpy(unchanged, &snapshot, sizeof snapshot);
    UmiStatus read = UmiKanbanCardLabelsRead(board, requested, id, &snapshot);
    bool unreadable = !strcmp(name, "stale") || !strcmp(name, "missing") || !strcmp(name, "archived");
    if (unreadable) {
        REQUIRE(read == (!strcmp(name, "missing") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_STATE));
        REQUIRE(memcmp(unchanged, &snapshot, sizeof snapshot) == 0);
    } else REQUIRE(read == UMI_STATUS_OK && snapshot.labels.count == 1U && !strcmp(snapshot.labels.labels[0], "old"));
    UmiKanbanCardLabels labels = {0}; labels.count = 2U;
    strcpy(labels.labels[0], "Private-release"); strcpy(labels.labels[1], "review needed");
    bool bad = false;
    if (!strcmp(name, "clear")) labels.count = 0U;
    else if (!strcmp(name, "maximum")) {
        labels.count = UMI_KANBAN_MAX_LABELS;
        for (size_t i = 0U; i < labels.count; ++i) snprintf(labels.labels[i], sizeof labels.labels[i], "label-%zu", i);
    } else if (!strcmp(name, "length")) memset(labels.labels[0], 'a', UMI_KANBAN_ID_SIZE - 1U);
    else if (!strcmp(name, "unicode")) strcpy(labels.labels[0], "\xe7\xa7\x81\xe4\xba\xba");
    else if (!strcmp(name, "case")) strcpy(labels.labels[1], "private-release");
    else if (!strcmp(name, "count")) { labels.count = UMI_KANBAN_MAX_LABELS + 1U; bad = true; }
    else if (!strcmp(name, "empty")) { labels.labels[0][0] = '\0'; bad = true; }
    else if (!strcmp(name, "unterminated")) { memset(labels.labels[0], 'a', UMI_KANBAN_ID_SIZE); bad = true; }
    else if (!strcmp(name, "duplicate")) { strcpy(labels.labels[1], labels.labels[0]); bad = true; }
    else if (!strcmp(name, "ignored-tail")) memset(labels.labels[7], 'z', UMI_KANBAN_ID_SIZE);
    size_t old_size = 0U; unsigned char *old = Encoded(board, &old_size);
    size_t old_activity = umi_kanban_board_activity_count(board);
    UmiStatus status;
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanCardLabelsRead(NULL, revision, "card", &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCardLabelsRead(board, revision, "card", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        status = UmiKanbanCardLabelsApply(board, revision, "card", "owner", NULL);
    } else status = UmiKanbanCardLabelsApply(board, requested, id, !strcmp(name, "actor") ? "" : "owner", &labels);
    bool refused = unreadable || bad || !strcmp(name, "null") || !strcmp(name, "actor");
    if (refused) {
        REQUIRE(status != UMI_STATUS_OK);
        if (!strcmp(name, "duplicate")) REQUIRE(status == UMI_STATUS_ALREADY_EXISTS);
        size_t new_size = 0U; unsigned char *next = Encoded(board, &new_size);
        REQUIRE(new_size == old_size && memcmp(old, next, old_size) == 0);
        REQUIRE(umi_kanban_board_activity_count(board) == old_activity); free(next);
    } else {
        REQUIRE(status == UMI_STATUS_OK && umi_kanban_board_revision(board) == revision + 1U);
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
        REQUIRE(row.card.label_count == labels.count && !strcmp(row.card.assignee, "owner"));
        REQUIRE(!strcmp(row.card.description, "Private note") && row.card.due_unix_seconds == 123U);
        REQUIRE(row.card.check_count == 1U && row.card.checks[0].complete == 1);
        REQUIRE(!strcmp(row.column_id, !strcmp(name, "done") ? "done" : "todo"));
        for (size_t i = 0U; i < labels.count; ++i) REQUIRE(!strcmp(row.card.labels[i], labels.labels[i]));
        for (size_t i = labels.count; i < UMI_KANBAN_MAX_LABELS; ++i) REQUIRE(row.card.labels[i][0] == '\0');
        if (!strcmp(name, "roundtrip")) {
            size_t size = 0U; unsigned char *bytes = Encoded(board, &size); UmiKanbanBoard *copy = NULL;
            /* The earlier draft omitted the required saved-revision output.
             * Keep it for review; the public decoder returns history separately
             * from the reopened board revision, which starts at one. */
#if 0
            REQUIRE(umi_kanban_board_decode(bytes, size, &copy) == UMI_STATUS_OK);
#endif
            uint64_t saved_revision = 0U;
            REQUIRE(umi_kanban_board_decode(bytes, size, &copy, &saved_revision) == UMI_STATUS_OK);
            REQUIRE(saved_revision == revision + 1U);
            UmiKanbanCardLabelsSnapshot restored;
            REQUIRE(UmiKanbanCardLabelsRead(copy, umi_kanban_board_revision(copy), "card", &restored) == UMI_STATUS_OK);
            REQUIRE(restored.labels.count == labels.count && !strcmp(restored.labels.labels[1], labels.labels[1]));
            free(bytes); umi_kanban_board_destroy(copy);
        } else if (!strcmp(name, "filter")) {
            UmiKanbanFilter filter = {0}; strcpy(filter.label, "Private-release");
            size_t count = 0U, total = 0U;
            REQUIRE(umi_kanban_board_page(board, revision + 1U, &filter, 0U, NULL, 0U, &count, &total) == UMI_STATUS_OK);
            REQUIRE(total == 1U && count == 0U);
            strcpy(filter.label, "private-release");
            REQUIRE(umi_kanban_board_page(board, revision + 1U, &filter, 0U, NULL, 0U, &count, &total) == UMI_STATUS_OK);
            REQUIRE(total == 0U);
        } else if (!strcmp(name, "public-privacy")) {
            UmiKanbanPublicCard public_card = {0}; strcpy(public_card.card_id, "card"); strcpy(public_card.title, "Public feature");
            size_t size = 0U;
            REQUIRE(umi_kanban_publication_html(board, revision + 1U, "Public", &public_card, 1U, NULL, 0U, &size) == UMI_STATUS_OK);
            char *html = calloc(size + 1U, 1U); REQUIRE(html);
            REQUIRE(umi_kanban_publication_html(board, revision + 1U, "Public", &public_card, 1U, html, size + 1U, &size) == UMI_STATUS_OK);
            REQUIRE(strstr(html, "Public feature") && !strstr(html, "Private-release") && !strstr(html, "review needed")); free(html);
        }
        REQUIRE(UmiKanbanCardLabelsApply(board, revision, "card", "owner", &labels) == UMI_STATUS_INVALID_STATE);
        REQUIRE(!strcmp(snapshot.labels.labels[0], "old"));
    }
    free(old); free(edit); umi_kanban_board_destroy(board); return 0;
}
