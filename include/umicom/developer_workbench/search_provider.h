/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/search_provider.h
 *
 * PURPOSE:
 *   Define pluggable Search Everywhere providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_SEARCH_PROVIDER_H
#define UMICOM_DEVELOPER_WORKBENCH_SEARCH_PROVIDER_H

#include "umicom/developer_workbench/search_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiDeveloperWorkbenchSearchFunction)(
    void *user_data,
    const char *query,
    UmiDeveloperWorkbenchSearchResult *out_results,
    size_t capacity,
    size_t *out_count);

typedef struct UmiDeveloperWorkbenchSearchProvider {
    char provider_id[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    char title[UMI_DEVELOPER_WORKBENCH_TITLE_CAPACITY];
    UmiDeveloperWorkbenchSearchKind kind;
    int priority;
    void *user_data;
    UmiDeveloperWorkbenchSearchFunction search;
} UmiDeveloperWorkbenchSearchProvider;

UmiStatus umi_developer_workbench_search_provider_validate(
    const UmiDeveloperWorkbenchSearchProvider *provider);

#ifdef __cplusplus
}
#endif

#endif
