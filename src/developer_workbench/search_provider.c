/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/search_provider.c
 *
 * PURPOSE:
 *   Validate Search Everywhere provider contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/search_provider.h"

/*
 * Check that developer workbench search provider satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_developer_workbench_search_provider_validate(
    const UmiDeveloperWorkbenchSearchProvider *provider)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL ||
        provider->provider_id[0] == '\0' ||
        provider->title[0] == '\0' ||
        provider->kind < UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND ||
        provider->kind > UMI_DEVELOPER_WORKBENCH_SEARCH_SOURCE_CONTROL ||
        provider->search == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
