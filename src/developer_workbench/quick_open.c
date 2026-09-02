/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/quick_open.c
 *
 * PURPOSE:
 *   Query the active workspace file index through the standard file provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/quick_open.h"

/*
 * Provide the developer workbench quick open query operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_workbench_quick_open_query(
    UmiFileIndex *file_index,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    UmiDeveloperWorkbenchSearchProvider provider;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_developer_workbench_file_search_provider_init(
        &provider, file_index);

    return provider.search(
        provider.user_data,
        query,
        out_results,
        capacity,
        out_count);
}
