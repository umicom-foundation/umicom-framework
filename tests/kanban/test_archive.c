/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban/test_archive.c
 * PURPOSE: Verify portable recovery and refusal of damaged or semantically invalid planning data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/base/value_archive_internal.h"
static void Rejected(const void *bytes, size_t size, UmiKanbanBoard *sentinel)
{
    UmiKanbanBoard *output = sentinel;
    uint64_t revision = 999U;
    REQUIRE(umi_kanban_board_decode(bytes, size, &output, &revision) != UMI_STATUS_OK);
    REQUIRE(output == sentinel && revision == 999U);
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    UmiKanbanBoard *board = Board();
    UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "portable");
    strcpy(edit->card.title, "Caf\xc3\xa9 planning");
    strcpy(edit->card.description, "Plain text <not markup>");
    strcpy(edit->card.assignee, "developer");
    edit->card.priority = UMI_KANBAN_URGENT;
    edit->card.due_unix_seconds = UINT64_C(2000000000);
    edit->card.label_count = 1U; strcpy(edit->card.labels[0], "SDK");
    edit->card.check_count = 1U; strcpy(edit->card.checks[0].text, "Review"); edit->card.checks[0].complete = 1;
    Apply(board, edit, 1U); Done(board, edit, "portable");
    uint64_t revision = umi_kanban_board_revision(board);
    size_t size = 0U;
    REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(size + 1U), *copy = malloc(size);
    REQUIRE(bytes != NULL && copy != NULL);
    REQUIRE(umi_kanban_board_encode(board, revision, bytes, size, &size) == UMI_STATUS_OK);
    memcpy(copy, bytes, size);
    if (strcmp(argv[1], "roundtrip") == 0) {
        UmiKanbanBoard *restored = NULL;
        uint64_t saved_revision = 0U;
        REQUIRE(umi_kanban_board_decode(bytes, size, &restored, &saved_revision) == UMI_STATUS_OK);
        REQUIRE(saved_revision == revision && umi_kanban_board_revision(restored) == 1U);
        REQUIRE(umi_kanban_board_activity_count(restored) == 0U);
        UmiKanbanCardSnapshot a, b;
        REQUIRE(umi_kanban_board_find(board, "portable", &a) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_board_find(restored, "portable", &b) == UMI_STATUS_OK);
        REQUIRE(strcmp(a.card.title, b.card.title) == 0 && strcmp(a.card.description, b.card.description) == 0);
        REQUIRE(strcmp(b.card.assignee, "developer") == 0 && b.card.priority == UMI_KANBAN_URGENT);
        REQUIRE(b.card.due_unix_seconds == UINT64_C(2000000000) && b.card.checks[0].complete == 1);
        REQUIRE(b.card.check_count == 1U && strcmp(b.card.checks[0].text, "Review") == 0);
        REQUIRE(b.card.label_count == 1U && strcmp(b.card.labels[0], "SDK") == 0);
        REQUIRE(strcmp(b.column_id, "done") == 0 && !b.archived);
        Move(restored, edit, "portable", "todo");
        REQUIRE(umi_kanban_board_find(board, "portable", &a) == UMI_STATUS_OK);
        REQUIRE(strcmp(a.column_id, "done") == 0);
        umi_kanban_board_destroy(restored);
    } else if (strcmp(argv[1], "short-buffer") == 0) {
        memset(bytes, 0xa5, size); size_t required = 0U;
        REQUIRE(umi_kanban_board_encode(board, revision, bytes, size - 1U, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(required == size);
        for (size_t i = 0U; i < size; ++i) REQUIRE(bytes[i] == 0xa5);
    } else if (strcmp(argv[1], "truncation") == 0) {
        for (size_t length = 0U; length < size; ++length) Rejected(bytes, length, board);
    } else if (strcmp(argv[1], "checksum") == 0) {
        bytes[size - 1U] ^= 1U; Rejected(bytes, size, board);
    } else if (strcmp(argv[1], "schema") == 0) {
        UmiArchiveFinish(bytes, size, UINT64_C(42)); Rejected(bytes, size, board);
    } else if (strcmp(argv[1], "trailing") == 0) {
        bytes[size] = 0U;
        UmiArchiveFinish(bytes, size + 1U, UINT64_C(0x554d494b414e4201));
        Rejected(bytes, size + 1U, board);
    } else if (strcmp(argv[1], "invalid-domain") == 0) {
        /* Keep a valid checksum while corrupting a domain count. Parsing must
         * not treat an intact envelope as proof of a valid board. */
        UmiArchiveReader reader = {bytes, size, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK};
        char id[UMI_KANBAN_ID_SIZE], title[256];
        UmiArchiveReadText(&reader, id, sizeof(id));
        UmiArchiveReadText(&reader, title, sizeof(title));
        (void)UmiArchiveReadUnsigned(&reader, UINT64_MAX);
        UmiArchiveStoreInteger(bytes + reader.offset, UMI_KANBAN_MAX_CARDS + 1U, 8U);
        UmiArchiveFinish(bytes, size, UINT64_C(0x554d494b414e4201));
        Rejected(bytes, size, board);
    } else REQUIRE(0);
    free(copy); free(bytes); free(edit); umi_kanban_board_destroy(board);
    return 0;
}
