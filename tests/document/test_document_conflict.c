/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_conflict.c
 * PURPOSE: Verify external changes prevent silent overwrite of a dirty document.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *coordinator = NULL;
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    int changed = 0;
    assert(umi_fs_temp_directory(root, sizeof(root)) == UMI_STATUS_OK);
    assert(umi_fs_join(path, sizeof(path), root, "umicom-b39-conflict.txt") == UMI_STATUS_OK);
    assert(umi_fs_write_text(path, "baseline") == UMI_STATUS_OK);
    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("test.conflict.workbench", commands, &workbench) == UMI_STATUS_OK);
    assert(umi_document_store_create(&store) == UMI_STATUS_OK);
    assert(umi_document_coordinator_create(store, workbench, NULL, &coordinator) == UMI_STATUS_OK);
    assert(umi_document_coordinator_open(coordinator, path, NULL, 0U) == UMI_STATUS_OK);
    assert(umi_fs_write_text(path, "changed externally") == UMI_STATUS_OK);
    assert(umi_document_coordinator_check_external_change(coordinator, &changed) == UMI_STATUS_OK);
    assert(changed);
    assert(umi_document_coordinator_save_active(coordinator) == UMI_STATUS_INVALID_STATE);
    assert(remove(path) == 0);
    umi_document_coordinator_destroy(coordinator);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    (void)printf("document conflict passed\n");
    return 0;
}
