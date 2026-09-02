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

/*
 * Initialise developer workbench search session from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_workbench_search_session_init(
    UmiDeveloperWorkbenchSearchSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    (void)memset(session, 0, sizeof(*session));
    session->revision = 1U;
}

/*
 * Provide the developer workbench search session query operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_search_session_query(
    UmiDeveloperWorkbenchSearchSession *session,
    UmiDeveloperWorkbenchSearchEngine *engine,
    const char *query)
{
    UmiStatus status;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || engine == NULL || query == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(query);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) {
        status = UMI_STATUS_OK;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && session->result_count > 0U) {
        session->selected_index = 0U;
        session->has_selection = 1;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) session->revision += 1U;
    return status;
}

/*
 * Provide the developer workbench search session select operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_search_session_select(
    UmiDeveloperWorkbenchSearchSession *session,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= session->result_count) return UMI_STATUS_NOT_FOUND;

    session->selected_index = index;
    session->has_selection = 1;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find developer workbench search session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_workbench_search_session_selected(
    const UmiDeveloperWorkbenchSearchSession *session,
    UmiDeveloperWorkbenchSearchResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!session->has_selection ||
        session->selected_index >= session->result_count) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_result = session->results[session->selected_index];
    return UMI_STATUS_OK;
}
