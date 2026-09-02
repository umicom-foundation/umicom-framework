/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/commands/extensions_settings.h
 *
 * PURPOSE:
 *   Publish the reusable Extensions Settings command definitions used by professional
 *   Umicom developer workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMANDS_EXTENSIONS_SETTINGS_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMANDS_EXTENSIONS_SETTINGS_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench extensions settings commands operation used by this
 * module and its client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_extensions_settings_commands(void);

/**
 * Return the number of records represented by developer workbench extensions settings
 * command without changing their state.
 */
size_t umi_developer_workbench_extensions_settings_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
