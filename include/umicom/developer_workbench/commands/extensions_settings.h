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

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_extensions_settings_commands(void);

size_t umi_developer_workbench_extensions_settings_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
