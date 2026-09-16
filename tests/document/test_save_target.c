/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_save_target.c
 * PURPOSE: Exercise saved document identity and destination protection with
 *          real document stores, workbench views and filesystem writes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define PROCESS_ID _getpid
#else
#include <unistd.h>
#define PROCESS_ID getpid
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *coordinator;
    char root[UMI_PATH_CAPACITY];
    char first[UMI_PATH_CAPACITY];
    char second[UMI_PATH_CAPACITY];
} Fixture;

/* Every CTest invocation has its own temporary directory, including in
 * parallel runs. Nothing is written into a developer's actual workspace. */
static int Start(Fixture *fixture, const char *name)
{
    char temporary[UMI_PATH_CAPACITY];
    char directory[160];
    CHECK(snprintf(directory, sizeof directory, "umicom-save-target-%ld-%s",
                   (long)PROCESS_ID(), name) > 0);
    CHECK(umi_fs_temp_directory(temporary, sizeof temporary) == UMI_STATUS_OK);
    CHECK(umi_fs_join(fixture->root, sizeof fixture->root, temporary, directory) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(fixture->root) == UMI_STATUS_OK);
    CHECK(umi_fs_join(fixture->first, sizeof fixture->first, fixture->root, "first.c") == UMI_STATUS_OK);
    CHECK(umi_fs_join(fixture->second, sizeof fixture->second, fixture->root, "second.c") == UMI_STATUS_OK);
    CHECK(umi_command_registry_create(&fixture->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.save-target", fixture->commands, &fixture->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&fixture->store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(fixture->store, fixture->workbench, NULL,
                                         &fixture->coordinator) == UMI_STATUS_OK);
    return 0;
}

static void Stop(Fixture *fixture)
{
    umi_document_coordinator_destroy(fixture->coordinator);
    umi_document_store_destroy(fixture->store);
    umi_ui_workbench_destroy(fixture->workbench);
    umi_command_registry_destroy(fixture->commands);
    (void)umi_fs_remove_tree(fixture->root);
}

/* Edit through the same view draft that a native editor updates. */
static int Edit(Fixture *fixture, const char *text,
                UmiDocumentWorkingCopySnapshot *snapshot)
{
    UmiUiDocumentViewSnapshot *view = calloc(1, sizeof *view);
    CHECK(view != NULL);
    CHECK(umi_document_coordinator_active_snapshot(fixture->coordinator, snapshot) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(fixture->workbench),
                                         snapshot->view_id, view) == UMI_STATUS_OK);
    CHECK(strlen(text) < sizeof view->source_text);
    strcpy(view->source_text, text);
    view->dirty = 1;
    CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(fixture->workbench), view) == UMI_STATUS_OK);
    free(view);
    return 0;
}

static int Contents(const char *path, const char *expected)
{
    char *text = NULL;
    size_t length = 0;
    CHECK(umi_fs_read_text(path, &text, &length) == UMI_STATUS_OK);
    CHECK(length == strlen(expected) && strcmp(text, expected) == 0);
    umi_fs_free_text(text);
    return 0;
}

static int Run(Fixture *fixture, const char *name)
{
    UmiDocumentWorkingCopySnapshot first, second, active;
    CHECK(umi_document_coordinator_new(fixture->coordinator, "first.c", NULL, 0) == UMI_STATUS_OK);
    CHECK(Edit(fixture, "int first = 1;\n", &first) == 0);
    if (strcmp(name, "external-conflict") == 0 || strcmp(name, "normalised-conflict") == 0) {
        char equivalent[UMI_PATH_CAPACITY];
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->coordinator, first.document_id,
                                           fixture->first) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(fixture->first, "external modification\n") == UMI_STATUS_OK);
        CHECK(Edit(fixture, "int first = 2;\n", &first) == 0);
        CHECK(snprintf(equivalent, sizeof equivalent, "%s/./first.c", fixture->root) > 0);
        const char *target = strcmp(name, "normalised-conflict") == 0 ? equivalent : fixture->first;
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->coordinator, first.document_id, target) == UMI_STATUS_INVALID_STATE);
        CHECK(Contents(fixture->first, "external modification\n") == 0);
        return 0;
    }
    if (strcmp(name, "invalid-path") == 0) {
        char longPath[UMI_PATH_CAPACITY + 50U];
        memset(longPath, 'p', sizeof longPath - 1U); longPath[sizeof longPath - 1U] = '\0';
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->coordinator, first.document_id, longPath) != UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->coordinator, 0, fixture->first) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_document_coordinator_active_snapshot(fixture->coordinator, &active) == UMI_STATUS_OK);
        CHECK(!active.has_path && !umi_fs_exists(fixture->first));
        return 0;
    }
    if (strcmp(name, "closed-document") == 0) {
        CHECK(umi_document_coordinator_close_active(fixture->coordinator, 1) == UMI_STATUS_OK);
    }
    CHECK(umi_document_coordinator_new(fixture->coordinator, "second.c", NULL, 0) == UMI_STATUS_OK);
    CHECK(Edit(fixture, "int second = 2;\n", &second) == 0);
    if (strcmp(name, "captured-document") == 0) {
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->coordinator, first.document_id,
                                           fixture->first) == UMI_STATUS_OK);
        CHECK(Contents(fixture->first, "int first = 1;\n") == 0);
        CHECK(umi_document_coordinator_active_snapshot(fixture->coordinator, &active) == UMI_STATUS_OK);
        CHECK(active.document_id == second.document_id && !active.has_path);
        CHECK(!umi_fs_exists(fixture->second));
    } else if (strcmp(name, "closed-document") == 0) {
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->coordinator, first.document_id,
                                           fixture->first) == UMI_STATUS_NOT_FOUND);
        CHECK(!umi_fs_exists(fixture->first));
        CHECK(umi_document_coordinator_active_snapshot(fixture->coordinator, &active) == UMI_STATUS_OK);
        CHECK(active.document_id == second.document_id && !active.has_path);
    } else if (strcmp(name, "occupied-destination") == 0) {
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->coordinator, second.document_id,
                                           fixture->second) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorSaveAs(fixture->coordinator, first.document_id,
                                           fixture->second) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(Contents(fixture->second, "int second = 2;\n") == 0);
        CHECK(umi_document_coordinator_at(fixture->coordinator, 0, &active) == UMI_STATUS_OK);
        CHECK(active.document_id == first.document_id && !active.has_path);
    } else return 2;
    return 0;
}

int main(int argc, char **argv)
{
    Fixture fixture = {0};
    if (argc != 2 || Start(&fixture, argv[1]) != 0) return 1;
    int status = Run(&fixture, argv[1]);
    Stop(&fixture);
    return status;
}
