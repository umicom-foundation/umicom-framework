/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/commands/workspace.h
 *
 * PURPOSE:
 *   Publish the reusable Workspace command definitions used by professional
 *   Umicom developer workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMANDS_WORKSPACE_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMANDS_WORKSPACE_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench workspace commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_workspace_commands(void);

/**
 * Return the number of records represented by developer workbench workspace command
 * without changing their state.
 */
size_t umi_developer_workbench_workspace_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
