/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_storage/test_boundaries.c
 * PURPOSE: Check bounded archive reads and unchanged-save conflict detection against real isolated files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../kanban/fixture.h"
#include "../build_log/fixture.h"
#include "umicom/kanban/document.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/file_lease.h"
#include "umicom/base/value_archive.h"

static void OversizedFile(const char *path)
{
    UmiOutputFile *file = NULL;
    REQUIRE(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    unsigned char chunk[4096] = {0};
    size_t remaining = (size_t)UMI_VALUE_ARCHIVE_BYTE_LIMIT + 1U;
    while (remaining != 0U) {
        size_t count = remaining < sizeof(chunk) ? remaining : sizeof(chunk);
        REQUIRE(UmiOutputFileWrite(file, chunk, count) == UMI_STATUS_OK);
        remaining -= count;
    }
    REQUIRE(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], full[UMI_PATH_CAPACITY], guard[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(full, root, "plan.umikb");
    FixturePath(guard, root, "plan.umikb.lock");
    UmiKanbanDocument *document = NULL;
    if (strcmp(mode, "oversized-open") == 0) {
        OversizedFile(full);
        REQUIRE(umi_kanban_document_open(root, "plan.umikb", &document) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(document == NULL);
        UmiFileLease *lease = NULL;
        REQUIRE(UmiFileLeaseTryAcquire(guard, &lease) == UMI_STATUS_OK);
        REQUIRE(UmiFileLeaseRelease(&lease) == UMI_STATUS_OK);
        return 0;
    }
    UmiKanbanBoard *seed = Board();
    UmiKanbanConfig config;
    REQUIRE(umi_kanban_board_config(seed, &config) == UMI_STATUS_OK);
    umi_kanban_board_destroy(seed);
    if (strcmp(mode, "guard-contents") == 0)
        REQUIRE(UmiRootedFileWrite(root, "plan.umikb.lock", "preserve", 8U) == UMI_STATUS_OK);
    REQUIRE(umi_kanban_document_create(root, "plan.umikb", &config, &document) == UMI_STATUS_OK);
    UmiKanbanSaveProblem problem;
    if (strcmp(mode, "clean-change") == 0) {
        REQUIRE(UmiRootedFileWrite(root, "plan.umikb", "other", 5U) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_save(document, 1U, &problem) == UMI_STATUS_INVALID_STATE);
        REQUIRE(problem == UMI_KANBAN_SAVE_EXTERNAL_CHANGE);
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(full, &size);
        REQUIRE(size == 5U && memcmp(bytes, "other", size) == 0);
        free(bytes);
    } else if (strcmp(mode, "oversized-save") == 0) {
        UmiKanbanEdit *edit = Edits(1U);
        Create(edit, "memory-budget");
        Apply(umi_kanban_document_board(document), edit, 1U);
        free(edit);
        REQUIRE(UmiRootedFileRemove(root, "plan.umikb") == UMI_STATUS_OK);
        OversizedFile(full);
        REQUIRE(umi_kanban_document_save(document, 2U, &problem) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(problem == UMI_KANBAN_SAVE_EXTERNAL_CHANGE);
        UmiKanbanDocumentState state;
        REQUIRE(umi_kanban_document_state(document, &state) == UMI_STATUS_OK);
        REQUIRE(state.unsaved_changes && state.saved_live_revision == 1U && state.board_revision == 2U);
        REQUIRE(umi_kanban_board_count(umi_kanban_document_board(document)) == 1U);
    } else if (strcmp(mode, "guard-contents") != 0) return 2;
    REQUIRE(umi_kanban_document_close(&document, true) == UMI_STATUS_OK);
    if (strcmp(mode, "guard-contents") == 0) {
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(guard, &size);
        REQUIRE(size == 8U && memcmp(bytes, "preserve", size) == 0);
        free(bytes);
    }
    return 0;
}
