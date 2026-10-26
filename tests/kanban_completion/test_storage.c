/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_storage.c
 *
 * PURPOSE:
 *   Preserve earlier review files and refuse invalid private archives before board mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "fixture.h"
#include "umicom/kanban/completion_storage.h"

static void WriteBytes(const char *path, const void *bytes, size_t size)
{
    UmiOutputFile *file = NULL;
    REQUIRE(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    REQUIRE(UmiOutputFileWrite(file, bytes, size) == UMI_STATUS_OK);
    REQUIRE(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
}
int main(int argc, char **argv)
{
    const char *cases[] = {"roundtrip","existing","different-existing","malformed","truncated",
        "empty","oversized","missing","missing-parent","relative","suffix","null","invalid-plan",
        "unicode","owned","stale","board-unchanged","symlink-load","symlink-write"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, !strcmp(name, "unicode") ? "caf\xc3\xa9.umicompletion" : "review.umicompletion");
    UmiKanbanBoard *board = Ready();
    UmiKanbanCompletionRecord record; Record(&record, "alpha");
    UmiKanbanCompletionPlan *plan = NULL, *loaded = NULL;
    REQUIRE(Capture(board, &record, 1U, &plan) == UMI_STATUS_OK);
    size_t board_size = 0, activity = umi_kanban_board_activity_count(board);
    void *before = Snapshot(board, &board_size);
    int result = 0;
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanCompletionPlanWriteNew(NULL, plan) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(UmiKanbanCompletionPlanLoadFile(NULL, &loaded) == UMI_STATUS_INVALID_ARGUMENT && !loaded);
        REQUIRE(UmiKanbanCompletionPlanLoadFile(path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(name, "invalid-plan")) {
        REQUIRE(UmiKanbanCompletionPlanWriteNew(path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        /* A refused write must not reserve an empty destination. */
        REQUIRE(UmiKanbanCompletionPlanWriteNew(path, plan) == UMI_STATUS_OK);
    } else if (!strcmp(name, "missing")) {
        REQUIRE(UmiKanbanCompletionPlanLoadFile(path, &loaded) == UMI_STATUS_NOT_FOUND && !loaded);
    } else if (!strcmp(name, "relative") || !strcmp(name, "suffix") || !strcmp(name, "missing-parent")) {
        char refused[UMI_PATH_CAPACITY];
        if (!strcmp(name, "relative")) strcpy(refused, "review.umicompletion");
        else FixturePath(refused, directory, !strcmp(name, "suffix") ? "review.html" : "absent/review.umicompletion");
        REQUIRE(UmiKanbanCompletionPlanWriteNew(refused, plan) != UMI_STATUS_OK);
        REQUIRE(UmiKanbanCompletionPlanLoadFile(refused, &loaded) != UMI_STATUS_OK && !loaded);
    } else if (!strcmp(name, "malformed") || !strcmp(name, "truncated") ||
               !strcmp(name, "empty") || !strcmp(name, "oversized")) {
        size_t size = 0;
        REQUIRE(UmiKanbanCompletionPlanEncode(plan, NULL, 0U, &size) == UMI_STATUS_OK);
        unsigned char *bytes = malloc(UMI_KANBAN_COMPLETION_MAXIMUM_BYTES + 1U); REQUIRE(bytes);
        memset(bytes, 0xa5, UMI_KANBAN_COMPLETION_MAXIMUM_BYTES + 1U);
        REQUIRE(UmiKanbanCompletionPlanEncode(plan, bytes, size, &size) == UMI_STATUS_OK);
        if (!strcmp(name, "malformed")) bytes[size - 1U] ^= 1U;
        if (!strcmp(name, "truncated")) --size;
        if (!strcmp(name, "empty")) size = 0U;
        if (!strcmp(name, "oversized")) size = UMI_KANBAN_COMPLETION_MAXIMUM_BYTES + 1U;
        WriteBytes(path, bytes, size);
        REQUIRE(UmiKanbanCompletionPlanLoadFile(path, &loaded) != UMI_STATUS_OK && !loaded);
        size_t actual = 0; unsigned char *retained = FixtureRead(path, &actual);
        REQUIRE(actual == size && !memcmp(bytes, retained, size));
        free(retained); free(bytes);
    } else if (!strncmp(name, "symlink-", 8)) {
        REQUIRE(UmiKanbanCompletionPlanWriteNew(path, plan) == UMI_STATUS_OK);
        char alias[UMI_PATH_CAPACITY]; FixturePath(alias, directory, "alias.umicompletion");
#ifdef _WIN32
        if (!FixtureSymbolicLink(alias, path, 0)) { result = 77; goto done; }
#else
        REQUIRE(symlink(path, alias) == 0);
#endif
        if (!strcmp(name, "symlink-load"))
            REQUIRE(UmiKanbanCompletionPlanLoadFile(alias, &loaded) != UMI_STATUS_OK && !loaded);
        else REQUIRE(UmiKanbanCompletionPlanWriteNew(alias, plan) != UMI_STATUS_OK);
        REQUIRE(UmiKanbanCompletionPlanLoadFile(path, &loaded) == UMI_STATUS_OK);
        REQUIRE(UmiKanbanCompletionPlanCheck(loaded, board, umi_kanban_board_revision(board)) == UMI_STATUS_OK);
    } else {
        REQUIRE(UmiKanbanCompletionPlanWriteNew(path, plan) == UMI_STATUS_OK);
        if (!strcmp(name, "existing") || !strcmp(name, "different-existing")) {
            size_t size = 0; unsigned char *original = FixtureRead(path, &size);
            if (!strcmp(name, "different-existing")) {
                UmiKanbanCompletionPlanDestroy(plan); plan = NULL;
                strcpy(record.rationale, "A different later review must use a different destination.");
                REQUIRE(Capture(board, &record, 1U, &plan) == UMI_STATUS_OK);
            }
            REQUIRE(UmiKanbanCompletionPlanWriteNew(path, plan) == UMI_STATUS_ALREADY_EXISTS);
            size_t size_after = 0; unsigned char *retained = FixtureRead(path, &size_after);
            REQUIRE(size == size_after && !memcmp(original, retained, size));
            free(original); free(retained);
        }
        REQUIRE(UmiKanbanCompletionPlanLoadFile(path, &loaded) == UMI_STATUS_OK);
        REQUIRE(UmiKanbanCompletionPlanCheck(loaded, board, umi_kanban_board_revision(board)) == UMI_STATUS_OK);
        REQUIRE(!strcmp(UmiKanbanCompletionPlanAt(loaded, 0U)->card_id, "alpha"));
        if (!strcmp(name, "owned")) {
            UmiKanbanCompletionPlanDestroy(plan); plan = NULL;
            umi_kanban_board_destroy(board); board = NULL;
            REQUIRE(!strcmp(UmiKanbanCompletionPlanAt(loaded, 0U)->rationale,
                "Synthetic owner review of the complete acceptance criteria."));
        }
        if (!strcmp(name, "stale")) {
            Update(board, "alpha", "Changed acceptance criteria");
            free(before); before = Snapshot(board, &board_size); activity = umi_kanban_board_activity_count(board);
            REQUIRE(UmiKanbanCompletionPlanApply(loaded, board, umi_kanban_board_revision(board)) == UMI_STATUS_INVALID_STATE);
        }
    }
done:
    if (board) Unchanged(board, before, board_size, activity);
    free(before); UmiKanbanCompletionPlanDestroy(plan); UmiKanbanCompletionPlanDestroy(loaded);
    umi_kanban_board_destroy(board); return result;
}
