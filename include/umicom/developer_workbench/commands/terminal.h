/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/commands/terminal.h
 *
 * PURPOSE:
 *   Publish the reusable Terminal command definitions used by professional
 *   Umicom developer workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMANDS_TERMINAL_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMANDS_TERMINAL_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_terminal_commands(void);

size_t umi_developer_workbench_terminal_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
