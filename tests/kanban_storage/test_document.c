/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_storage/test_document.c
 * PURPOSE: Exercise native board storage, stale views, competing owners and recovery without changing user files.
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

static void Configuration(UmiKanbanConfig *config)
{
    UmiKanbanBoard *board = Board();
    REQUIRE(umi_kanban_board_config(board, config) == UMI_STATUS_OK);
    umi_kanban_board_destroy(board);
}
static void AddCard(UmiKanbanDocument *document)
{
    UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "save-dialog");
    Apply(umi_kanban_document_board(document), edit, 1U);
    free(edit);
}
static UmiKanbanDocumentState State(const UmiKanbanDocument *document)
{
    UmiKanbanDocumentState state;
    REQUIRE(umi_kanban_document_state(document, &state) == UMI_STATUS_OK);
    return state;
}
static void SameFile(const char *path, const unsigned char *before, size_t size)
{
    size_t after_size = 0U;
    unsigned char *after = FixtureRead(path, &after_size);
    REQUIRE(size == after_size && memcmp(before, after, size) == 0);
    free(after);
}
static void SameState(const UmiKanbanDocument *document, const UmiKanbanDocumentState *before)
{
    UmiKanbanDocumentState after = State(document);
    REQUIRE(after.board_revision == before->board_revision);
    REQUIRE(after.saved_live_revision == before->saved_live_revision);
    REQUIRE(after.archive_revision == before->archive_revision);
    REQUIRE(after.unsaved_changes == before->unsaved_changes);
    REQUIRE(strcmp(after.filename, before->filename) == 0);
    REQUIRE(strcmp(after.directory, before->directory) == 0);
}
static void ChangeDirectory(const char *root)
{
#ifdef _WIN32
    wchar_t wide[UMI_PATH_CAPACITY];
    REQUIRE(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, root, -1,
        wide, (int)UMI_PATH_CAPACITY) != 0);
    REQUIRE(SetCurrentDirectoryW(wide));
#else
    REQUIRE(chdir(root) == 0);
#endif
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], full[UMI_PATH_CAPACITY], guard[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    const char *name = strcmp(mode, "unicode") == 0 ? "plan-\xc3\xa9 space.umikb" : "plan.umikb";
    FixturePath(full, root, name);
    int guard_length = snprintf(guard, sizeof(guard), "%s.lock", full);
    REQUIRE(guard_length > 0 && (size_t)guard_length < sizeof(guard));
    UmiKanbanConfig config;
    Configuration(&config);
    UmiKanbanDocument *document = NULL, *other = NULL;
    UmiKanbanSaveProblem problem = UMI_KANBAN_SAVE_NONE;
    if (strcmp(mode, "invalid-path") == 0) {
        const char *bad[] = {"../plan.umikb", "sub/plan.umikb", "plan.umikb.lock", ".umikb", "CON.umikb", "plan.txt"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
            REQUIRE(umi_kanban_document_create(root, bad[i], &config, &document) != UMI_STATUS_OK);
            REQUIRE(document == NULL);
        }
        REQUIRE(umi_kanban_document_open("relative", name, &document) != UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_create(root, name, NULL, &document) != UMI_STATUS_OK);
        REQUIRE(document == NULL);
        int exists = 1;
        REQUIRE(UmiRootedFileExists(root, name, &exists) == UMI_STATUS_OK && exists == 0);
        REQUIRE(UmiRootedFileExists(root, "plan.umikb.lock", &exists) == UMI_STATUS_OK && exists == 0);
        REQUIRE(umi_kanban_document_save(NULL, 1U, &problem) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(problem == UMI_KANBAN_SAVE_ARGUMENT);
        REQUIRE(umi_kanban_document_close(NULL, false) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_OK);
        return 0;
    }
    if (strcmp(mode, "missing-parent") == 0) {
        char missing[UMI_PATH_CAPACITY];
        FixturePath(missing, root, "absent");
        REQUIRE(umi_kanban_document_create(missing, name, &config, &document) == UMI_STATUS_NOT_FOUND);
        REQUIRE(umi_kanban_document_open(root, name, &document) == UMI_STATUS_NOT_FOUND);
        REQUIRE(document == NULL);
        return 0;
    }
    if (strcmp(mode, "malformed") == 0) {
        REQUIRE(UmiRootedFileWrite(root, name, "broken", 6U) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_open(root, name, &document) == UMI_STATUS_PARSE_ERROR);
        REQUIRE(document == NULL);
        SameFile(full, (const unsigned char *)"broken", 6U);
        UmiFileLease *lease = NULL;
        REQUIRE(UmiFileLeaseTryAcquire(guard, &lease) == UMI_STATUS_OK);
        REQUIRE(UmiFileLeaseRelease(&lease) == UMI_STATUS_OK);
        return 0;
    }
    if (strcmp(mode, "symlink-board") == 0 || strcmp(mode, "symlink-guard") == 0) {
        char target[UMI_PATH_CAPACITY];
        FixturePath(target, root, "untouched.txt");
        REQUIRE(UmiRootedFileWrite(root, "untouched.txt", "keep", 4U) == UMI_STATUS_OK);
        const char *alias = strcmp(mode, "symlink-board") == 0 ? full : guard;
#ifdef _WIN32
        if (!FixtureSymbolicLink(alias, target, 0)) return 77;
#else
        REQUIRE(symlink(target, alias) == 0);
#endif
        REQUIRE(umi_kanban_document_create(root, name, &config, &document) != UMI_STATUS_OK);
        REQUIRE(document == NULL);
        SameFile(target, (const unsigned char *)"keep", 4U);
        return 0;
    }
    if (strcmp(mode, "working-directory") == 0) {
        char different[UMI_PATH_CAPACITY];
        FixtureDirectory(different);
        ChangeDirectory(different);
    }
    REQUIRE(umi_kanban_document_create(root, name, &config, &document) == UMI_STATUS_OK);
    UmiKanbanDocumentState initial = State(document);
    REQUIRE(!initial.unsaved_changes && initial.board_revision == 1U && initial.archive_revision == 1U);
    REQUIRE(strcmp(initial.directory, root) == 0 && strcmp(initial.filename, name) == 0);
    size_t original_size = 0U;
    unsigned char *original = FixtureRead(full, &original_size);
    if (strcmp(mode, "existing") == 0) {
        REQUIRE(umi_kanban_document_create(root, name, &config, &other) == UMI_STATUS_ALREADY_EXISTS);
        REQUIRE(other == NULL);
        UmiKanbanDocument *saved = document;
        REQUIRE(umi_kanban_document_open(root, name, &document) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(document == saved);
        SameFile(full, original, original_size);
    } else if (strcmp(mode, "ownership") == 0) {
        REQUIRE(umi_kanban_document_open(root, name, &other) == UMI_STATUS_BUSY && other == NULL);
        REQUIRE(umi_kanban_document_create(root, "independent.umikb", &config, &other) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_close(&other, false) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_open(root, name, &document) == UMI_STATUS_OK);
        SameFile(full, original, original_size);
    } else if (strcmp(mode, "stale") == 0) {
        AddCard(document);
        UmiKanbanDocumentState before = State(document);
        REQUIRE(umi_kanban_document_save(document, 1U, &problem) == UMI_STATUS_INVALID_STATE);
        REQUIRE(problem == UMI_KANBAN_SAVE_STALE_REVISION);
        REQUIRE(umi_kanban_document_save_copy(document, 1U, root, "copy.umikb") == UMI_STATUS_INVALID_STATE);
        SameState(document, &before);
        SameFile(full, original, original_size);
    } else if (strcmp(mode, "external-change") == 0 || strcmp(mode, "external-delete") == 0) {
        AddCard(document);
        UmiKanbanDocumentState before = State(document);
        if (strcmp(mode, "external-change") == 0)
            REQUIRE(UmiRootedFileWrite(root, name, "external", 8U) == UMI_STATUS_OK);
        else REQUIRE(UmiRootedFileRemove(root, name) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_save(document, before.board_revision, &problem) != UMI_STATUS_OK);
        REQUIRE(problem == UMI_KANBAN_SAVE_EXTERNAL_CHANGE);
        SameState(document, &before);
        /* Refused saves keep recoverable edits; a copy never claims to fix the
         * original conflict or marks the original session clean. */
        REQUIRE(umi_kanban_document_save_copy(document, before.board_revision, root, "recovered.umikb") == UMI_STATUS_OK);
        SameState(document, &before);
        REQUIRE(umi_kanban_document_open(root, "recovered.umikb", &other) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_board_count(umi_kanban_document_board(other)) == 1U);
        REQUIRE(umi_kanban_document_close(&other, false) == UMI_STATUS_OK);
        if (strcmp(mode, "external-change") == 0) SameFile(full, (const unsigned char *)"external", 8U);
        else {
            int exists = 1;
            REQUIRE(UmiRootedFileExists(root, name, &exists) == UMI_STATUS_OK && exists == 0);
        }
        REQUIRE(UmiRootedFileWrite(root, name, original, original_size) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_save(document, before.board_revision, &problem) == UMI_STATUS_OK);
    } else if (strcmp(mode, "discard") == 0) {
        AddCard(document);
        UmiKanbanDocument *saved = document;
        UmiKanbanDocumentState before = State(document);
        REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_INVALID_STATE);
        REQUIRE(document == saved);
        SameState(document, &before);
        REQUIRE(umi_kanban_document_close(&document, true) == UMI_STATUS_OK && document == NULL);
        REQUIRE(umi_kanban_document_open(root, name, &document) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_board_count(umi_kanban_document_board(document)) == 0U);
        SameFile(full, original, original_size);
    } else if (strcmp(mode, "copy") == 0) {
        AddCard(document);
        UmiKanbanDocumentState before = State(document);
        REQUIRE(umi_kanban_document_save_copy(document, before.board_revision, root, name) == UMI_STATUS_ALREADY_EXISTS);
        REQUIRE(umi_kanban_document_save_copy(document, before.board_revision, root, "copy.umikb") == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_save_copy(document, before.board_revision, root, "copy.umikb") == UMI_STATUS_ALREADY_EXISTS);
        SameState(document, &before);
        SameFile(full, original, original_size);
        REQUIRE(umi_kanban_document_open(root, "copy.umikb", &other) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_board_count(umi_kanban_document_board(other)) == 1U);
        REQUIRE(State(other).archive_revision == before.board_revision);
        REQUIRE(umi_kanban_document_close(&other, false) == UMI_STATUS_OK);
    } else if (strcmp(mode, "write-refusal") == 0) {
        AddCard(document);
        UmiKanbanDocumentState before = State(document);
#ifdef _WIN32
        wchar_t wide[UMI_PATH_CAPACITY];
        REQUIRE(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, full, -1, wide, (int)UMI_PATH_CAPACITY) != 0);
        REQUIRE(SetFileAttributesW(wide, FILE_ATTRIBUTE_READONLY));
#else
        /* A non-regular replacement is a deterministic refusal even as root. */
        REQUIRE(UmiRootedFileRemove(root, name) == UMI_STATUS_OK);
        REQUIRE(mkdir(full, 0700) == 0);
#endif
        REQUIRE(umi_kanban_document_save(document, before.board_revision, &problem) != UMI_STATUS_OK);
        REQUIRE(problem == UMI_KANBAN_SAVE_STORAGE);
        SameState(document, &before);
#ifdef _WIN32
        REQUIRE(SetFileAttributesW(wide, FILE_ATTRIBUTE_NORMAL));
        SameFile(full, original, original_size);
#endif
    } else if (strcmp(mode, "roundtrip") == 0 || strcmp(mode, "unicode") == 0 || strcmp(mode, "working-directory") == 0) {
        AddCard(document);
        uint64_t revision = State(document).board_revision;
        REQUIRE(State(document).unsaved_changes);
        REQUIRE(umi_kanban_document_save(document, revision, &problem) == UMI_STATUS_OK);
        REQUIRE(problem == UMI_KANBAN_SAVE_NONE && !State(document).unsaved_changes);
        REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_open(root, name, &document) == UMI_STATUS_OK);
        REQUIRE(State(document).board_revision == 1U && State(document).archive_revision == revision);
        REQUIRE(umi_kanban_board_count(umi_kanban_document_board(document)) == 1U);
        size_t saved_size = 0U;
        unsigned char *saved = FixtureRead(full, &saved_size);
        REQUIRE(umi_kanban_document_save(document, 1U, &problem) == UMI_STATUS_OK);
        SameFile(full, saved, saved_size);
        REQUIRE(State(document).archive_revision == revision);
        free(saved);
    } else return 2;
    free(original);
    REQUIRE(umi_kanban_document_close(&document, true) == UMI_STATUS_OK && document == NULL);
    REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_OK);
    /* Persistent guards remain after release and can be reused safely. */
    UmiFileLease *lease = NULL;
    REQUIRE(UmiFileLeaseTryAcquire(guard, &lease) == UMI_STATUS_OK);
    REQUIRE(UmiFileLeaseRelease(&lease) == UMI_STATUS_OK);
    return 0;
}
