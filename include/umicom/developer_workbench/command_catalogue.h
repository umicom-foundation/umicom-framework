/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/command_catalogue.h
 *
 * PURPOSE:
 *   Provide one deterministic command catalogue for Studio, command palettes,
 *   menus, toolbars, automation and AI tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMAND_CATALOGUE_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMAND_CATALOGUE_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_developer_workbench_command_catalogue_count(void);

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_command_catalogue_at(size_t index);

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_command_catalogue_find(const char *command_id);

UmiStatus umi_developer_workbench_command_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
