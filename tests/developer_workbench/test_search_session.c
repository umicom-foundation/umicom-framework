/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_search_session.c
 *
 * PURPOSE:
 *   Verify Search Everywhere selection state.
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

#include "umicom/developer_workbench/search_session.h"

/*
 * Exercise provider search and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus provider_search(
    void *user_data,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    (void)user_data;
    (void)query;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity == 0U) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(&out_results[0], 0, sizeof(out_results[0]));
    (void)strcpy(out_results[0].title, "Build");
    out_results[0].kind = UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND;
    out_results[0].score = 100;
    *out_count = 1U;
    return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperWorkbenchSearchEngine *engine = NULL;
    UmiDeveloperWorkbenchSearchProvider provider;
    UmiDeveloperWorkbenchSearchSession session;
    UmiDeveloperWorkbenchSearchResult selected;

    assert(umi_developer_workbench_search_engine_create(&engine) ==
           UMI_STATUS_OK);

    (void)memset(&provider, 0, sizeof(provider));
    (void)strcpy(provider.provider_id, "test");
    (void)strcpy(provider.title, "Test");
    provider.kind = UMI_DEVELOPER_WORKBENCH_SEARCH_COMMAND;
    provider.search = provider_search;
    assert(umi_developer_workbench_search_engine_register(
        engine, &provider) == UMI_STATUS_OK);

    umi_developer_workbench_search_session_init(&session);
    assert(umi_developer_workbench_search_session_query(
        &session, engine, "build") == UMI_STATUS_OK);
    assert(session.result_count == 1U);
    assert(umi_developer_workbench_search_session_selected(
        &session, &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.title, "Build") == 0);

    umi_developer_workbench_search_engine_destroy(engine);
    return 0;
}
