/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/search_engine.h
 *
 * PURPOSE:
 *   Aggregate Search Everywhere providers and return one ranked result stream.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_SEARCH_ENGINE_H
#define UMICOM_DEVELOPER_WORKBENCH_SEARCH_ENGINE_H

#include "umicom/developer_workbench/search_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_WORKBENCH_SEARCH_PROVIDER_CAPACITY 16U

typedef struct UmiDeveloperWorkbenchSearchEngine
    UmiDeveloperWorkbenchSearchEngine;

UmiStatus umi_developer_workbench_search_engine_create(
    UmiDeveloperWorkbenchSearchEngine **out_engine);

void umi_developer_workbench_search_engine_destroy(
    UmiDeveloperWorkbenchSearchEngine *engine);

UmiStatus umi_developer_workbench_search_engine_register(
    UmiDeveloperWorkbenchSearchEngine *engine,
    const UmiDeveloperWorkbenchSearchProvider *provider);

UmiStatus umi_developer_workbench_search_engine_query(
    UmiDeveloperWorkbenchSearchEngine *engine,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count);

size_t umi_developer_workbench_search_engine_provider_count(
    const UmiDeveloperWorkbenchSearchEngine *engine);

#ifdef __cplusplus
}
#endif

#endif
