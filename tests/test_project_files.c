/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_files.c
 *
 * PURPOSE:
 *   Exercise real project-entry creation and coherent Explorer snapshots,
 *   including failure preservation, paging and concurrent readers/writers.
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
#include "umicom/platform/workspace_files.h"
#include "umicom/platform/file_index.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    return EXIT_FAILURE; } } while (0)

/* Every test runs in a separate, CTest-owned working directory. */
static int Root(char *root)
{
    char cwd[UMI_PATH_CAPACITY];
    CHECK(umi_fs_current_directory(cwd, sizeof(cwd)) == UMI_STATUS_OK);
    CHECK(umi_path_join(cwd, "project", root, UMI_PATH_CAPACITY) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}

static int ReadEquals(const char *path, const char *expected)
{
    char *text = NULL;
    CHECK(umi_fs_read_text(path, &text, NULL) == UMI_STATUS_OK);
    int same = strcmp(text, expected) == 0;
    umi_fs_free_text(text);
    CHECK(same);
    return EXIT_SUCCESS;
}

typedef struct CreationWorker { const char *root; UmiStatus status; } CreationWorker;
/* Each worker owns its graph; only the OS destination is shared. */
static int CreateWorker(void *data)
{
    CreationWorker *worker = data;
    UmiWorkspaceGraph *graph = NULL;
    UmiWorkspaceEntryResult result;
    CHECK(umi_workspace_graph_create(&graph) == UMI_STATUS_OK);
    CHECK(umi_workspace_graph_open(graph, worker->root, 1) == UMI_STATUS_OK);
    worker->status = UmiWorkspaceCreateEntry(graph, 0U, "race.c", UMI_WORKSPACE_ENTRY_FILE, &result);
    umi_workspace_graph_destroy(graph);
    return EXIT_SUCCESS;
}

static int Creation(const char *test)
{
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    UmiWorkspaceGraph *graph = NULL;
    UmiWorkspaceGraphSnapshot snapshot;
    UmiWorkspaceEntryResult result;
    CHECK(Root(root) == EXIT_SUCCESS);
    CHECK(umi_workspace_graph_create(&graph) == UMI_STATUS_OK);
    CHECK(umi_workspace_graph_open(graph, root, 1) == UMI_STATUS_OK);
    CHECK(umi_workspace_graph_snapshot(graph, &snapshot) == UMI_STATUS_OK);
    if (strcmp(test, "create-file") == 0) {
        CHECK(UmiWorkspaceCreateEntry(graph, snapshot.revision, "notes.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_OK);
        CHECK(result.created && umi_fs_is_file(result.path));
        CHECK(ReadEquals(result.path, "") == EXIT_SUCCESS);
    } else if (strcmp(test, "create-directory") == 0) {
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "src", UMI_WORKSPACE_ENTRY_DIRECTORY, &result) == UMI_STATUS_OK);
        CHECK(result.created && umi_fs_is_directory(result.path));
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "src/notes.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_OK);
        CHECK(result.created);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "src", UMI_WORKSPACE_ENTRY_DIRECTORY, &result) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(!result.created);
    } else if (strcmp(test, "no-overwrite") == 0) {
        CHECK(umi_path_join(root, "notes.c", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(path, "important source") == UMI_STATUS_OK);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "notes.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(!result.created && ReadEquals(path, "important source") == EXIT_SUCCESS);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "notes.c", UMI_WORKSPACE_ENTRY_DIRECTORY, &result) == UMI_STATUS_ALREADY_EXISTS);
    } else if (strcmp(test, "parent-required") == 0) {
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "missing/notes.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_NOT_FOUND);
        CHECK(!result.created);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "missing/src", UMI_WORKSPACE_ENTRY_DIRECTORY, &result) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(test, "untrusted") == 0) {
        CHECK(umi_workspace_graph_set_trusted(graph, 0) == UMI_STATUS_OK);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "notes.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(!result.created && result.path[0] == '\0');
    } else if (strcmp(test, "stale") == 0) {
        CHECK(umi_workspace_graph_set_trusted(graph, 0) == UMI_STATUS_OK);
        CHECK(umi_workspace_graph_set_trusted(graph, 1) == UMI_STATUS_OK);
        CHECK(UmiWorkspaceCreateEntry(graph, snapshot.revision, "notes.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_BUSY);
        CHECK(!result.created);
    } else if (strcmp(test, "closed") == 0) {
        CHECK(umi_workspace_graph_close(graph) == UMI_STATUS_OK);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "notes.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(test, "path-rules") == 0) {
        const char *invalid[] = {"", ".", "..", "../outside.c", "src/../../outside.c", "/outside.c", "C:/outside.c", "C:notes.c", "src\\..\\outside.c", "src/", "a//b", "a/./b", "a.", "a ", "CON", "NUL.c", "COM1.txt", "LPT9.c", "bad:name", "bad?name", "a\nname", "a\"name"};
        for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
            CHECK(UmiWorkspaceCreateEntry(graph, 0U, invalid[i], UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!result.created);
        }
    } else if (strcmp(test, "metadata") == 0) {
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, ".git", UMI_WORKSPACE_ENTRY_DIRECTORY, &result) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, ".GIT/config", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, ".gitignore", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_OK);
    } else if (strcmp(test, "invalid-input") == 0) {
        CHECK(UmiWorkspaceCreateEntry(NULL, 0U, "a.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, NULL, UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "a.c", (UmiWorkspaceEntryKind)19, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, "a.c", UMI_WORKSPACE_ENTRY_FILE, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        char oversized[UMI_PATH_CAPACITY + 1U];
        memset(oversized, 'x', sizeof(oversized)); oversized[sizeof(oversized) - 1U] = '\0';
        CHECK(UmiWorkspaceCreateEntry(graph, 0U, oversized, UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(test, "symlink-parent") == 0 || strcmp(test, "symlink-leaf") == 0) {
#ifndef _WIN32
        char outside[UMI_PATH_CAPACITY];
        CHECK(umi_path_join(root, "link", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_path_join(root, "../outside", outside, sizeof(outside)) == UMI_STATUS_OK);
        CHECK(umi_fs_make_directories(outside) == UMI_STATUS_OK);
        CHECK(symlink(outside, path) == 0);
        if (strcmp(test, "symlink-parent") == 0) {
            CHECK(UmiWorkspaceCreateEntry(graph, 0U, "link/escape.c", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_PERMISSION_DENIED);
        } else {
            CHECK(UmiWorkspaceCreateEntry(graph, 0U, "link", UMI_WORKSPACE_ENTRY_FILE, &result) == UMI_STATUS_ALREADY_EXISTS);
        }
        CHECK(!result.created);
        CHECK(unlink(path) == 0);
        CHECK(umi_fs_remove_tree(outside) == UMI_STATUS_OK);
#else
        umi_workspace_graph_destroy(graph);
        CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK);
        return 77; /* Reparse-point creation needs a separate native Windows fixture. */
#endif
    } else if (strcmp(test, "parallel-create") == 0) {
        CreationWorker workers[8]; UmiThread *threads[8]; size_t created = 0U;
        for (size_t i = 0U; i < 8U; ++i) {
            workers[i].root = root; workers[i].status = UMI_STATUS_INTERNAL_ERROR;
            CHECK(umi_thread_start(CreateWorker, &workers[i], &threads[i]) == UMI_STATUS_OK);
        }
        for (size_t i = 0U; i < 8U; ++i) {
            int code = -1;
            CHECK(umi_thread_join(threads[i], &code) == UMI_STATUS_OK && code == 0);
            umi_thread_destroy(threads[i]);
            CHECK(workers[i].status == UMI_STATUS_OK || workers[i].status == UMI_STATUS_ALREADY_EXISTS);
            if (workers[i].status == UMI_STATUS_OK) ++created;
        }
        CHECK(created == 1U);
    } else return EXIT_FAILURE;
    umi_workspace_graph_destroy(graph);
    CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}

typedef struct IndexWorker { UmiFileIndex *index; const char *path; } IndexWorker;
static int WriteIndex(void *data)
{
    IndexWorker *worker = data;
    for (unsigned i = 0U; i < 300U; ++i) {
        CHECK(umi_file_index_remove(worker->index, worker->path) == UMI_STATUS_OK);
        CHECK(umi_file_index_update(worker->index, worker->path) == UMI_STATUS_OK);
    }
    return EXIT_SUCCESS;
}

static int Index(const char *test)
{
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    UmiFileIndex *index = NULL;
    UmiFileIndexPage page;
    UmiFileIndexEntry entries[3];
    CHECK(Root(root) == EXIT_SUCCESS);
    UmiFileIndexConfig config = umi_file_index_config_default(root);
    config.maximum_files = strcmp(test, "refresh-rollback") == 0 ? 2U : 20U;
    if (strcmp(test, "budget") == 0) {
        config.maximum_files = SIZE_MAX;
        CHECK(umi_file_index_create(&config, &index) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(index == NULL);
        CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK);
        return EXIT_SUCCESS;
    }
    CHECK(umi_file_index_create(&config, &index) == UMI_STATUS_OK);
    CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
    if (strcmp(test, "empty") == 0) {
        CHECK(UmiFileIndexReadPage(index, "", 0, 0U, 0U, NULL, 0U, &page) == UMI_STATUS_OK);
        CHECK(page.count == 0U && page.matched == 0U && !page.has_more);
    } else if (strcmp(test, "refresh-rollback") == 0 || strcmp(test, "no-change") == 0) {
        CHECK(umi_path_join(root, "y.c", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(path, "y") == UMI_STATUS_OK);
        CHECK(umi_path_join(root, "z.c", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(path, "z") == UMI_STATUS_OK);
        CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
        UmiFileIndexEntry before[2]; UmiFileIndexPage previous;
        CHECK(UmiFileIndexReadPage(index, "", 0, 0U, 0U, before, 2U, &previous) == UMI_STATUS_OK);
        if (strcmp(test, "refresh-rollback") == 0) {
            CHECK(umi_path_join(root, "a.c", path, sizeof(path)) == UMI_STATUS_OK);
            CHECK(umi_fs_write_text(path, "a") == UMI_STATUS_OK);
            CHECK(umi_file_index_rebuild(index) == UMI_STATUS_CAPACITY_EXCEEDED);
        } else {
            CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
            CHECK(umi_file_index_update(index, path) == UMI_STATUS_OK);
        }
        CHECK(UmiFileIndexReadPage(index, "", 0, 0U, previous.stats.revision, entries, 3U, &page) == UMI_STATUS_OK);
        CHECK(page.count == 2U && memcmp(before, entries, sizeof(before)) == 0);
    } else if (strcmp(test, "read-write") == 0) {
        CHECK(umi_path_join(root, "notes.c", path, sizeof(path)) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(path, "notes") == UMI_STATUS_OK);
        CHECK(umi_file_index_update(index, path) == UMI_STATUS_OK);
        IndexWorker worker = {index, path}; UmiThread *thread = NULL;
        CHECK(umi_thread_start(WriteIndex, &worker, &thread) == UMI_STATUS_OK);
        for (unsigned i = 0U; i < 500U; ++i) {
            CHECK(UmiFileIndexReadPage(index, "", 0, 0U, 0U, entries, 3U, &page) == UMI_STATUS_OK);
            CHECK(page.count == page.stats.files && page.matched == page.count && page.count <= 1U);
        }
        int code = -1; CHECK(umi_thread_join(thread, &code) == UMI_STATUS_OK && code == 0);
        umi_thread_destroy(thread);
    } else {
        for (unsigned i = 0U; i < 7U; ++i) {
            char name[32]; snprintf(name, sizeof(name), "note%u.%s", i, i % 2U == 0U ? "c" : "h");
            CHECK(umi_path_join(root, name, path, sizeof(path)) == UMI_STATUS_OK);
            CHECK(umi_fs_write_text(path, "notes") == UMI_STATUS_OK);
        }
        CHECK(umi_file_index_rebuild(index) == UMI_STATUS_OK);
        if (strcmp(test, "page-boundaries") == 0) {
            CHECK(UmiFileIndexReadPage(index, "", 0, 0U, 0U, entries, 3U, &page) == UMI_STATUS_OK);
            CHECK(page.count == 3U && page.matched == 7U && page.has_more);
            CHECK(strcmp(entries[0].name, "note0.c") == 0);
            CHECK(UmiFileIndexReadPage(index, "", 0, 3U, page.stats.revision, entries, 3U, &page) == UMI_STATUS_OK);
            CHECK(page.count == 3U && page.has_more && strcmp(entries[0].name, "note3.h") == 0);
            CHECK(UmiFileIndexReadPage(index, "", 0, 6U, page.stats.revision, entries, 3U, &page) == UMI_STATUS_OK);
            CHECK(page.count == 1U && !page.has_more);
            CHECK(UmiFileIndexReadPage(index, "", 0, SIZE_MAX, 0U, entries, 3U, &page) == UMI_STATUS_OK);
            CHECK(page.count == 0U && !page.has_more);
        } else if (strcmp(test, "filter-pages") == 0) {
            CHECK(UmiFileIndexReadPage(index, ".C", 0, 0U, 0U, entries, 3U, &page) == UMI_STATUS_OK);
            CHECK(page.matched == 4U && page.count == 3U && page.has_more);
            CHECK(UmiFileIndexReadPage(index, ".C", 0, 3U, page.stats.revision, entries, 3U, &page) == UMI_STATUS_OK);
            CHECK(page.count == 1U && strcmp(entries[0].name, "note6.c") == 0 && !page.has_more);
            CHECK(UmiFileIndexReadPage(index, ".C", 1, 0U, 0U, NULL, 0U, &page) == UMI_STATUS_OK && page.matched == 0U);
        } else if (strcmp(test, "stale-page") == 0) {
            CHECK(UmiFileIndexReadPage(index, "", 0, 0U, 0U, entries, 3U, &page) == UMI_STATUS_OK);
            UmiFileIndexPage before = page; UmiFileIndexEntry rows[3]; memcpy(rows, entries, sizeof(rows));
            CHECK(umi_file_index_clear(index) == UMI_STATUS_OK);
            CHECK(UmiFileIndexReadPage(index, "", 0, 3U, before.stats.revision, entries, 3U, &page) == UMI_STATUS_BUSY);
            CHECK(memcmp(&page, &before, sizeof(page)) == 0 && memcmp(entries, rows, sizeof(rows)) == 0);
        } else if (strcmp(test, "page-input") == 0) {
            CHECK(UmiFileIndexReadPage(NULL, "", 0, 0U, 0U, entries, 3U, &page) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiFileIndexReadPage(index, NULL, 0, 0U, 0U, entries, 3U, &page) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiFileIndexReadPage(index, "", 0, 0U, 0U, NULL, 3U, &page) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiFileIndexReadPage(index, "", 0, 0U, 0U, NULL, 0U, &page) == UMI_STATUS_OK && page.matched == 7U);
        } else return EXIT_FAILURE;
    }
    umi_file_index_destroy(index);
    CHECK(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    CHECK(argc == 3);
    if (strcmp(argv[1], "creation") == 0) return Creation(argv[2]);
    if (strcmp(argv[1], "index") == 0) return Index(argv[2]);
    return EXIT_FAILURE;
}
