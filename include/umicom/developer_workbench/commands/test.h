/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/commands/test.h
 *
 * PURPOSE:
 *   Publish the reusable Test command definitions used by professional
 *   Umicom developer workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMANDS_TEST_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMANDS_TEST_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench test commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_test_commands(void);

/**
 * Return the number of records represented by developer workbench test command without
 * changing their state.
 */
size_t umi_developer_workbench_test_command_count(void);

#ifdef __cplusplus
}
#endif

#endif
