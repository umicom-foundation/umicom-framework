/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/search_engine.c
 *
 * PURPOSE:
 *   Implement Search Everywhere provider aggregation and ranking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/search_engine.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbenchSearchEngine {
    UmiDeveloperWorkbenchSearchProvider
        providers[UMI_DEVELOPER_WORKBENCH_SEARCH_PROVIDER_CAPACITY];
    size_t provider_count;
};

/* Provide the result compare operation used by this module and its client applications. */
static int result_compare(
    const UmiDeveloperWorkbenchSearchResult *left,
    const UmiDeveloperWorkbenchSearchResult *right)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score != right->score) {
        return left->score > right->score ? -1 : 1;
    }

    return strcmp(left->title, right->title);
}

/* Provide the sort results operation used by this module and its client applications. */
static void sort_results(
    UmiDeveloperWorkbenchSearchResult *results,
    size_t count)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < count; ++index) {
        UmiDeveloperWorkbenchSearchResult current = results[index];
        size_t position = index;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (position > 0U &&
               result_compare(&current, &results[position - 1U]) < 0) {
            results[position] = results[position - 1U];
            --position;
        }

        results[position] = current;
    }
}

/*
 * Initialise developer workbench search engine from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_search_engine_create(
    UmiDeveloperWorkbenchSearchEngine **out_engine)
{
    UmiDeveloperWorkbenchSearchEngine *engine;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_engine = NULL;

    engine = (UmiDeveloperWorkbenchSearchEngine *)calloc(
        1U, sizeof(*engine));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (engine == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_engine = engine;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer workbench search engine so the same storage can
 * be reused safely.
 */
void umi_developer_workbench_search_engine_destroy(
    UmiDeveloperWorkbenchSearchEngine *engine)
{
    free(engine);
}

/*
 * Add developer workbench search engine only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_developer_workbench_search_engine_register(
    UmiDeveloperWorkbenchSearchEngine *engine,
    const UmiDeveloperWorkbenchSearchProvider *provider)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (engine == NULL || provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_search_provider_validate(provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < engine->provider_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(engine->providers[index].provider_id,
                   provider->provider_id) == 0) {
            engine->providers[index] = *provider;
            return UMI_STATUS_OK;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (engine->provider_count >=
        UMI_DEVELOPER_WORKBENCH_SEARCH_PROVIDER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    engine->providers[engine->provider_count++] = *provider;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer workbench search engine query operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_search_engine_query(
    UmiDeveloperWorkbenchSearchEngine *engine,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    size_t provider_index;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (engine == NULL || query == NULL ||
        out_results == NULL || capacity == 0U || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_count = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (provider_index = 0U;
         provider_index < engine->provider_count &&
         used < capacity;
         ++provider_index) {
        size_t count = 0U;
        UmiStatus status = engine->providers[provider_index].search(
            engine->providers[provider_index].user_data,
            query,
            out_results + used,
            capacity - used,
            &count);

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
            return status;
        }

        used += count;
    }

    sort_results(out_results, used);
    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by developer workbench search engine provider
 * without changing their state.
 */
size_t umi_developer_workbench_search_engine_provider_count(
    const UmiDeveloperWorkbenchSearchEngine *engine)
{
    return engine != NULL ? engine->provider_count : 0U;
}
