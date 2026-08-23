/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/search_session.h
 *
 * PURPOSE:
 *   Retain Search Everywhere query/results/selection independently from a UI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_SEARCH_SESSION_H
#define UMICOM_DEVELOPER_WORKBENCH_SEARCH_SESSION_H

#include "umicom/developer_workbench/search_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperWorkbenchSearchSession {
    char query[UMI_DEVELOPER_WORKBENCH_TEXT_CAPACITY];
    UmiDeveloperWorkbenchSearchResult
        results[UMI_DEVELOPER_WORKBENCH_MAX_RESULTS];
    size_t result_count;
    size_t selected_index;
    int has_selection;
    uint64_t revision;
} UmiDeveloperWorkbenchSearchSession;

void umi_developer_workbench_search_session_init(
    UmiDeveloperWorkbenchSearchSession *session);

UmiStatus umi_developer_workbench_search_session_query(
    UmiDeveloperWorkbenchSearchSession *session,
    UmiDeveloperWorkbenchSearchEngine *engine,
    const char *query);

UmiStatus umi_developer_workbench_search_session_select(
    UmiDeveloperWorkbenchSearchSession *session,
    size_t index);

UmiStatus umi_developer_workbench_search_session_selected(
    const UmiDeveloperWorkbenchSearchSession *session,
    UmiDeveloperWorkbenchSearchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
