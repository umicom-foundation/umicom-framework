/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/command_palette.c
 *
 * PURPOSE:
 *   Query the command provider without requiring a complete Search Everywhere
 *   engine instance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/command_palette.h"

UmiStatus umi_developer_workbench_command_palette_query(
    UmiCommandRegistry *commands,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    UmiDeveloperWorkbenchSearchProvider provider;

    if (commands == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_developer_workbench_command_search_provider_init(
        &provider, commands);

    return provider.search(
        provider.user_data,
        query,
        out_results,
        capacity,
        out_count);
}
