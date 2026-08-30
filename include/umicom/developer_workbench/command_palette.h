/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/command_palette.h
 *
 * PURPOSE:
 *   Provide a command-only query model for familiar IDE command palettes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_COMMAND_PALETTE_H
#define UMICOM_DEVELOPER_WORKBENCH_COMMAND_PALETTE_H

#include "umicom/developer_workbench/command_search_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_developer_workbench_command_palette_query(
    UmiCommandRegistry *commands,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
