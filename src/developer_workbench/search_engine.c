/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/search_engine.c
 *
 * PURPOSE:
 *   Implement Search Everywhere provider aggregation and ranking.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/search_engine.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbenchSearchEngine {
    UmiDeveloperWorkbenchSearchProvider
        providers[UMI_DEVELOPER_WORKBENCH_SEARCH_PROVIDER_CAPACITY];
    size_t provider_count;
};

static int result_compare(
    const UmiDeveloperWorkbenchSearchResult *left,
    const UmiDeveloperWorkbenchSearchResult *right)
{
    if (left->score != right->score) {
        return left->score > right->score ? -1 : 1;
    }

    return strcmp(left->title, right->title);
}

static void sort_results(
    UmiDeveloperWorkbenchSearchResult *results,
    size_t count)
{
    size_t index;

    for (index = 1U; index < count; ++index) {
        UmiDeveloperWorkbenchSearchResult current = results[index];
        size_t position = index;

        while (position > 0U &&
               result_compare(&current, &results[position - 1U]) < 0) {
            results[position] = results[position - 1U];
            --position;
        }

        results[position] = current;
    }
}

UmiStatus umi_developer_workbench_search_engine_create(
    UmiDeveloperWorkbenchSearchEngine **out_engine)
{
    UmiDeveloperWorkbenchSearchEngine *engine;

    if (out_engine == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_engine = NULL;

    engine = (UmiDeveloperWorkbenchSearchEngine *)calloc(
        1U, sizeof(*engine));
    if (engine == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_engine = engine;
    return UMI_STATUS_OK;
}

void umi_developer_workbench_search_engine_destroy(
    UmiDeveloperWorkbenchSearchEngine *engine)
{
    free(engine);
}

UmiStatus umi_developer_workbench_search_engine_register(
    UmiDeveloperWorkbenchSearchEngine *engine,
    const UmiDeveloperWorkbenchSearchProvider *provider)
{
    size_t index;
    UmiStatus status;

    if (engine == NULL || provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_search_provider_validate(provider);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < engine->provider_count; ++index) {
        if (strcmp(engine->providers[index].provider_id,
                   provider->provider_id) == 0) {
            engine->providers[index] = *provider;
            return UMI_STATUS_OK;
        }
    }

    if (engine->provider_count >=
        UMI_DEVELOPER_WORKBENCH_SEARCH_PROVIDER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    engine->providers[engine->provider_count++] = *provider;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_search_engine_query(
    UmiDeveloperWorkbenchSearchEngine *engine,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count)
{
    size_t provider_index;
    size_t used = 0U;

    if (engine == NULL || query == NULL ||
        out_results == NULL || capacity == 0U || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_count = 0U;

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

        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
            return status;
        }

        used += count;
    }

    sort_results(out_results, used);
    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

size_t umi_developer_workbench_search_engine_provider_count(
    const UmiDeveloperWorkbenchSearchEngine *engine)
{
    return engine != NULL ? engine->provider_count : 0U;
}
