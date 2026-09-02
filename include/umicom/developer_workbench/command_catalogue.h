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

/**
 * Return the number of records represented by developer workbench command catalogue
 * without changing their state.
 */
size_t umi_developer_workbench_command_catalogue_count(void);

/**
 * Find developer workbench command catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_command_catalogue_at(size_t index);

/**
 * Find developer workbench command catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_command_catalogue_find(const char *command_id);

/**
 * Check that developer workbench command catalogue satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_developer_workbench_command_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
