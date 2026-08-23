/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/commands/layout.h
 *
 * PURPOSE:
 *   Publish the reusable Layout command definitions used by professional
 *   Umicom developer workbenches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMANDS_LAYOUT_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMANDS_LAYOUT_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_layout_commands(void);

size_t umi_developer_workbench_layout_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
