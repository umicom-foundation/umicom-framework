/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/commands/view.h
 *
 * PURPOSE:
 *   Publish the reusable View command definitions used by professional
 *   Umicom developer workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMANDS_VIEW_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMANDS_VIEW_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench view commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_view_commands(void);

/**
 * Return the number of records represented by developer workbench view command without
 * changing their state.
 */
size_t umi_developer_workbench_view_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
