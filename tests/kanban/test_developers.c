/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban/test_developers.c
 * PURPOSE: Verify stable assignment identities, atomic directory edits and portable named archives.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/kanban/developers.h"
#include "../../src/base/value_archive_internal.h"
static void Person(UmiKanbanEdit *edit, const char *id, const char *name)
{
    Action(edit, UMI_KANBAN_PUT_DEVELOPER, id, NULL);
    strcpy(edit->card.title, name);
}
static unsigned char *Encode(UmiKanbanBoard *board, size_t *size)
{
    REQUIRE(umi_kanban_board_encode(board, umi_kanban_board_revision(board), NULL, 0U, size) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(*size); REQUIRE(bytes != NULL);
    REQUIRE(umi_kanban_board_encode(board, umi_kanban_board_revision(board), bytes, *size, size) == UMI_STATUS_OK);
    return bytes;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edits = Edits(3U);
    UmiKanbanDeveloper person;
    if (strcmp(argv[1], "register") == 0 || strcmp(argv[1], "rename") == 0) {
        Person(edits, "developer.one", "Alex Morgan");
        Create(&edits[1], "card");
        Action(&edits[2], UMI_KANBAN_ASSIGN_CARD, "card", NULL);
        strcpy(edits[2].card.assignee, "developer.one"); Apply(board, edits, 3U);
        if (strcmp(argv[1], "rename") == 0) { Person(edits, "developer.one", "Alex Chen"); Apply(board, edits, 1U); }
        REQUIRE(umi_kanban_board_developer_count(board) == 1U);
        REQUIRE(umi_kanban_board_developer_at(board, umi_kanban_board_revision(board), 0U, &person) == UMI_STATUS_OK);
        REQUIRE(strcmp(person.id, "developer.one") == 0);
        REQUIRE(strcmp(person.name, strcmp(argv[1], "rename") == 0 ? "Alex Chen" : "Alex Morgan") == 0);
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
        REQUIRE(strcmp(row.card.assignee, person.id) == 0);
        REQUIRE(strcmp(row.card.title, "card") == 0 && strcmp(row.column_id, "todo") == 0);
    } else if (strcmp(argv[1], "atomic") == 0) {
        Person(edits, "developer.one", "Alex Morgan");
        Action(&edits[1], UMI_KANBAN_ASSIGN_CARD, "missing", NULL);
        strcpy(edits[1].card.assignee, "developer.one");
        Refused(board, edits, 2U, UMI_KANBAN_PROBLEM_MISSING);
        REQUIRE(umi_kanban_board_developer_count(board) == 0U);
    } else if (strcmp(argv[1], "invalid") == 0) {
        Person(edits, "bad name", "Alex"); Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_ARGUMENT);
        Person(edits, "developer.one", ""); Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_ARGUMENT);
        Person(edits, "developer.one", "Alex");
        memset(edits->card.title, 'x', 128U); edits->card.title[128] = '\0';
        Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_ARGUMENT);
    } else if (strcmp(argv[1], "stale") == 0) {
        uint64_t revision = umi_kanban_board_revision(board);
        Person(edits, "developer.one", "Alex"); Apply(board, edits, 1U);
        UmiKanbanProblem problem;
        Person(edits, "developer.two", "Sam");
        REQUIRE(umi_kanban_board_apply(board, revision, "owner", edits, 1U, &problem) == UMI_STATUS_INVALID_STATE);
        REQUIRE(problem.kind == UMI_KANBAN_PROBLEM_STALE && umi_kanban_board_developer_count(board) == 1U);
        memset(&person, 0x5a, sizeof(person)); UmiKanbanDeveloper before = person;
        REQUIRE(umi_kanban_board_developer_at(board, revision, 0U, &person) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&person, &before, sizeof(person)) == 0);
        REQUIRE(umi_kanban_board_developer_find(board, revision, "developer.one", &person) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&person, &before, sizeof(person)) == 0);
    } else if (strcmp(argv[1], "capacity") == 0) {
        for (size_t i = 0U; i < UMI_KANBAN_MAX_DEVELOPERS; ++i) {
            char id[64]; (void)snprintf(id, sizeof(id), "developer.%zu", i);
            Person(edits, id, "Shared display name"); Apply(board, edits, 1U);
        }
        Person(edits, "overflow", "Another name"); Refused(board, edits, 1U, UMI_KANBAN_PROBLEM_CAPACITY);
        Person(edits, "developer.0", "Renamed at capacity"); Apply(board, edits, 1U);
        REQUIRE(umi_kanban_board_developer_count(board) == UMI_KANBAN_MAX_DEVELOPERS);
        REQUIRE(umi_kanban_board_developer_find(board, umi_kanban_board_revision(board), "developer.0", &person) == UMI_STATUS_OK);
        REQUIRE(strcmp(person.name, "Renamed at capacity") == 0);
    } else if (strcmp(argv[1], "roundtrip") == 0 || strcmp(argv[1], "schema") == 0) {
        Person(edits, "developer.one", "Zo\xc3\xab Morgan"); Apply(board, edits, 1U);
        size_t size = 0U; unsigned char *bytes = Encode(board, &size); UmiValueArchiveInfo info;
        REQUIRE(umi_value_archive_inspect(bytes, size, &info) == UMI_STATUS_OK);
        REQUIRE(info.schema == UINT64_C(0x554d494b414e4203));
        UmiKanbanBoard *restored = NULL; uint64_t revision = 0U;
        if (strcmp(argv[1], "schema") == 0) {
            UmiArchiveFinish(bytes, size, UINT64_C(0x554d494b414e4201));
            REQUIRE(umi_kanban_board_decode(bytes, size, &restored, &revision) != UMI_STATUS_OK);
            REQUIRE(restored == NULL && revision == 0U);
        } else {
            REQUIRE(umi_kanban_board_decode(bytes, size, &restored, &revision) == UMI_STATUS_OK);
            REQUIRE(revision == umi_kanban_board_revision(board));
            REQUIRE(umi_kanban_board_developer_count(restored) == 1U);
            REQUIRE(umi_kanban_board_developer_find(restored, umi_kanban_board_revision(restored), "developer.one", &person) == UMI_STATUS_OK);
            REQUIRE(strcmp(person.name, "Zo\xc3\xab Morgan") == 0);
            umi_kanban_board_destroy(restored);
        }
        free(bytes);
    } else if (strcmp(argv[1], "legacy") == 0) {
        Create(edits, "card"); strcpy(edits->card.assignee, "legacy.person"); Apply(board, edits, 1U);
        size_t size = 0U; unsigned char *bytes = Encode(board, &size); UmiValueArchiveInfo info;
        REQUIRE(umi_value_archive_inspect(bytes, size, &info) == UMI_STATUS_OK);
        REQUIRE(info.schema == UINT64_C(0x554d494b414e4201));
        UmiKanbanBoard *restored = NULL; uint64_t revision = 0U;
        REQUIRE(umi_kanban_board_decode(bytes, size, &restored, &revision) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_board_developer_count(restored) == 0U);
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(restored, "card", &row) == UMI_STATUS_OK);
        REQUIRE(strcmp(row.card.assignee, "legacy.person") == 0);
        free(bytes); umi_kanban_board_destroy(restored);
    } else if (strcmp(argv[1], "missing") == 0) {
        memset(&person, 0x5a, sizeof(person)); UmiKanbanDeveloper before = person;
        uint64_t revision = umi_kanban_board_revision(board);
        REQUIRE(umi_kanban_board_developer_at(board, revision, 0U, &person) == UMI_STATUS_NOT_FOUND);
        REQUIRE(memcmp(&before, &person, sizeof(person)) == 0);
        REQUIRE(umi_kanban_board_developer_find(board, revision, "missing", &person) == UMI_STATUS_NOT_FOUND);
        REQUIRE(memcmp(&before, &person, sizeof(person)) == 0);
    } else REQUIRE(0);
    free(edits); umi_kanban_board_destroy(board); return 0;
}
