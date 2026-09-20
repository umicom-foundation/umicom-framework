/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/navigation.c
 * PURPOSE:
 *   Find a line in an Umicom Notes draft, reject stale form input and verify
 *   that navigation changes neither the source text nor the saved file.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/navigation.h"
#include "umicom/platform/filesystem.h"
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

int main(void)
{
    const char *source = "#include <stdio.h>\nint main(void)\n{\n    puts(\"Umicom Notes\");\n    return 0;\n}\n";
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiStatus status = UMI_STATUS_OK;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    char leaf[100], viewId[UMI_UI_ID_CAPACITY], otherView[UMI_UI_ID_CAPACITY];
    char *saved = NULL, *draft = NULL;
    int ownsDirectory = 0;
#define TRY(call) do { status = (call); if (status != UMI_STATUS_OK) goto cleanup; } while (0)

    /* The example owns this temporary directory. Existing files are never used
     * as test fixtures, and only this directory is removed during cleanup. */
    (void)snprintf(leaf, sizeof leaf, "umicom-notes-navigation-%ld", (long)PROCESS_ID());
    TRY(umi_fs_temp_directory(temp, sizeof temp));
    TRY(umi_fs_join(root, sizeof root, temp, leaf));
    if (umi_fs_exists(root)) { status = UMI_STATUS_ALREADY_EXISTS; goto cleanup; }
    TRY(umi_fs_make_directories(root)); ownsDirectory = 1;
    TRY(umi_fs_join(path, sizeof path, root, "notes.c"));
    TRY(umi_fs_write_text(path, source));
    TRY(umi_command_registry_create(&commands));
    TRY(umi_ui_workbench_create("umicom.notes.navigation", commands, &workbench));
    TRY(umi_document_store_create(&store));
    TRY(umi_document_coordinator_create(store, workbench, NULL, &documents));
    TRY(umi_document_coordinator_open(documents, path, viewId, sizeof viewId));

    UmiDocumentWorkingCopySnapshot intended;
    TRY(umi_document_coordinator_active_snapshot(documents, &intended));
    UmiDocumentLocation location;
    TRY(UmiDocumentLocationParse("4:5", 3U, &location));
    printf("Requested line %zu, UTF-8 byte column %zu.\n", location.line, location.column);
    size_t offset = 0U;
    TRY(UmiDocumentCoordinatorNavigate(documents, intended.document_id, "4:5", 3U, &offset));
    if (source[offset] != 'p') { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    puts("The caret points to puts; navigation did not edit the source.");

    /* Model a form opened for notes.c, followed by a change of active tab.
     * Old input must not navigate the newly active review document. */
    TRY(umi_document_coordinator_new(documents, "Review notes", otherView, sizeof otherView));
    status = UmiDocumentCoordinatorNavigate(documents, intended.document_id, "2", 1U, &offset);
    if (status != UMI_STATUS_INVALID_STATE) { status = UMI_STATUS_INTERNAL_ERROR; goto cleanup; }
    status = UMI_STATUS_OK;
    UmiDocumentWorkingCopySnapshot active;
    TRY(umi_document_coordinator_active_snapshot(documents, &active));
    if (strcmp(active.view_id, otherView) != 0) { status = UMI_STATUS_INVALID_STATE; goto cleanup; }
    puts("A different active tab rejected the old request and remained active.");

    /* Check independent outcomes: both the complete draft and saved bytes. */
    size_t bytes = 0U;
    TRY(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(workbench), viewId, &draft, &bytes));
    if (bytes != strlen(source) || memcmp(draft, source, bytes) != 0) {
        status = UMI_STATUS_INVALID_STATE; goto cleanup;
    }
    TRY(umi_fs_read_text(path, &saved, &bytes));
    if (bytes != strlen(source) || memcmp(saved, source, bytes) != 0) {
        status = UMI_STATUS_INVALID_STATE; goto cleanup;
    }
    puts("The Notes draft and saved file are unchanged.");
cleanup:
    UmiUiDocumentViewModelFreeText(draft);
    umi_fs_free_text(saved);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if (ownsDirectory) {
        UmiStatus removed = umi_fs_remove_tree(root);
        if (status == UMI_STATUS_OK) status = removed;
    }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Notes navigation: %s\n", umi_status_text(status));
    return status == UMI_STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
