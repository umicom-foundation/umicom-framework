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

/**
 * Represent the developer workbench search engine data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkbenchSearchEngine
    UmiDeveloperWorkbenchSearchEngine;

/**
 * Initialise developer workbench search engine from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_search_engine_create(
    UmiDeveloperWorkbenchSearchEngine **out_engine);

/**
 * Release or reset state held by developer workbench search engine so the same storage can
 * be reused safely.
 */
void umi_developer_workbench_search_engine_destroy(
    UmiDeveloperWorkbenchSearchEngine *engine);

/**
 * Add developer workbench search engine only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_developer_workbench_search_engine_register(
    UmiDeveloperWorkbenchSearchEngine *engine,
    const UmiDeveloperWorkbenchSearchProvider *provider);

/**
 * Provide the developer workbench search engine query operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_search_engine_query(
    UmiDeveloperWorkbenchSearchEngine *engine,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count);

/**
 * Return the number of records represented by developer workbench search engine provider
 * without changing their state.
 */
size_t umi_developer_workbench_search_engine_provider_count(
    const UmiDeveloperWorkbenchSearchEngine *engine);

#ifdef __cplusplus
}
#endif

#endif
