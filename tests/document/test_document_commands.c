/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_commands.c
 * PURPOSE: Verify canonical document commands through the runtime registry.
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
#include <string.h>

int main(void)
{
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *coordinator = NULL;
    char message[512];
    assert(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    assert(umi_ui_workbench_create("test.commands.workbench", commands, &workbench) == UMI_STATUS_OK);
    assert(umi_document_store_create(&store) == UMI_STATUS_OK);
    assert(umi_document_coordinator_create(store, workbench, NULL, &coordinator) == UMI_STATUS_OK);
    assert(umi_document_commands_register(commands, coordinator) == UMI_STATUS_OK);
    assert(umi_command_registry_count(commands) == UMI_DOCUMENT_COMMAND_COUNT);
    assert(umi_command_registry_execute(commands, UMI_DOCUMENT_COMMAND_NEW,
                                        "Command.c", message, sizeof(message)) == UMI_STATUS_OK);
    assert(strstr(message, "created") != NULL);
    assert(umi_command_registry_execute(commands, UMI_DOCUMENT_COMMAND_GO_TO_LINE,
                                        "1", message, sizeof(message)) == UMI_STATUS_OK);
    assert(umi_command_registry_execute(commands, UMI_DOCUMENT_COMMAND_FIND,
                                        "missing", message, sizeof(message)) == UMI_STATUS_NOT_FOUND);
    umi_document_coordinator_destroy(coordinator);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    (void)printf("document commands passed\n");
    return 0;
}
