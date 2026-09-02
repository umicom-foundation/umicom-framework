/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/commands/search.h
 *
 * PURPOSE:
 *   Publish the reusable Search command definitions used by professional
 *   Umicom developer workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMANDS_SEARCH_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMANDS_SEARCH_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench search commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_search_commands(void);

/**
 * Return the number of records represented by developer workbench search command without
 * changing their state.
 */
size_t umi_developer_workbench_search_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
