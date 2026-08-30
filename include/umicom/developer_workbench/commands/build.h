/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/commands/build.h
 *
 * PURPOSE:
 *   Publish the reusable Build command definitions used by professional
 *   Umicom developer workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMANDS_BUILD_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMANDS_BUILD_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_build_commands(void);

size_t umi_developer_workbench_build_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
