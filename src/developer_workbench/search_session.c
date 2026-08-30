/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/search_session.c
 *
 * PURPOSE:
 *   Implement reusable Search Everywhere session state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/search_session.h"

#include <string.h>

void umi_developer_workbench_search_session_init(
    UmiDeveloperWorkbenchSearchSession *session)
{
    if (session == NULL) return;
    (void)memset(session, 0, sizeof(*session));
    session->revision = 1U;
}

UmiStatus umi_developer_workbench_search_session_query(
    UmiDeveloperWorkbenchSearchSession *session,
    UmiDeveloperWorkbenchSearchEngine *engine,
    const char *query)
{
    UmiStatus status;
    size_t length;

    if (session == NULL || engine == NULL || query == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(query);
    if (length >= sizeof(session->query)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(session->results, 0, sizeof(session->results));
    session->result_count = 0U;
    session->has_selection = 0;
    (void)memcpy(session->query, query, length + 1U);

    status = umi_developer_workbench_search_engine_query(
        engine,
        query,
        session->results,
        UMI_DEVELOPER_WORKBENCH_MAX_RESULTS,
        &session->result_count);

    if (status == UMI_STATUS_NOT_FOUND) {
        status = UMI_STATUS_OK;
    }

    if (status == UMI_STATUS_OK && session->result_count > 0U) {
        session->selected_index = 0U;
        session->has_selection = 1;
    }

    if (status == UMI_STATUS_OK) session->revision += 1U;
    return status;
}

UmiStatus umi_developer_workbench_search_session_select(
    UmiDeveloperWorkbenchSearchSession *session,
    size_t index)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= session->result_count) return UMI_STATUS_NOT_FOUND;

    session->selected_index = index;
    session->has_selection = 1;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_search_session_selected(
    const UmiDeveloperWorkbenchSearchSession *session,
    UmiDeveloperWorkbenchSearchResult *out_result)
{
    if (session == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!session->has_selection ||
        session->selected_index >= session->result_count) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_result = session->results[session->selected_index];
    return UMI_STATUS_OK;
}
