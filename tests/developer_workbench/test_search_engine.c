/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_search_engine.c
 *
 * PURPOSE:
 *   Verify Search Everywhere combines and ranks providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_workbench/search_engine.h"

static UmiStatus provider_search(
    void *user_data,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    (void)user_data;
    (void)query;

    if (capacity < 2U) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memset(out_results, 0, 2U * sizeof(out_results[0]));
    out_results[0].kind = UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND;
    (void)strcpy(out_results[0].title, "Lower");
    out_results[0].score = 10;

    out_results[1].kind = UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND;
    (void)strcpy(out_results[1].title, "Higher");
    out_results[1].score = 100;

    *out_count = 2U;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiDeveloperWorkbenchSearchEngine *engine = NULL;
    UmiDeveloperWorkbenchSearchProvider provider;
    UmiDeveloperWorkbenchSearchResult results[4];
    size_t count = 0U;

    assert(umi_developer_workbench_search_engine_create(&engine) ==
           UMI_STATUS_OK);

    (void)memset(&provider, 0, sizeof(provider));
    (void)strcpy(provider.provider_id, "test.provider");
    (void)strcpy(provider.title, "Test Provider");
    provider.kind = UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND;
    provider.search = provider_search;

    assert(umi_developer_workbench_search_engine_register(
        engine, &provider) == UMI_STATUS_OK);
    assert(umi_developer_workbench_search_engine_query(
        engine, "test", results, 4U, &count) == UMI_STATUS_OK);
    assert(count == 2U);
    assert(strcmp(results[0].title, "Higher") == 0);

    umi_developer_workbench_search_engine_destroy(engine);
    return 0;
}
