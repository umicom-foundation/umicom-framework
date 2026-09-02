/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/types.c
 *
 * PURPOSE:
 *   Implement stable developer-workbench command metadata validation and text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/types.h"

/*
 * Provide the developer workbench command action text operation used by this module and
 * its client applications.
 */
const char *umi_developer_workbench_command_action_text(
    UmiDeveloperWorkbenchCommandAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
        case UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL: return "external";
        case UMI_DEVELOPER_WORKBENCH_ACTION_CONFIGURE: return "configure";
        case UMI_DEVELOPER_WORKBENCH_ACTION_BUILD: return "build";
        case UMI_DEVELOPER_WORKBENCH_ACTION_REBUILD: return "rebuild";
        case UMI_DEVELOPER_WORKBENCH_ACTION_CLEAN: return "clean";
        case UMI_DEVELOPER_WORKBENCH_ACTION_TEST: return "test";
        case UMI_DEVELOPER_WORKBENCH_ACTION_RUN: return "run";
        case UMI_DEVELOPER_WORKBENCH_ACTION_INSTALL: return "install";
        case UMI_DEVELOPER_WORKBENCH_ACTION_PACKAGE: return "package";
        case UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL: return "cancel";
        default: return "unknown";
    }
}

/*
 * Check that developer workbench command definition satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_developer_workbench_command_definition_validate(
    const UmiDeveloperWorkbenchCommandDefinition *definition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL ||
        definition->command_id == NULL ||
        definition->command_id[0] == '\0' ||
        definition->title == NULL ||
        definition->title[0] == '\0' ||
        definition->category == NULL ||
        definition->category[0] == '\0' ||
        definition->description == NULL ||
        definition->action < UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL ||
        definition->action > UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
