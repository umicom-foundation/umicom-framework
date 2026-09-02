/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/search_session.h
 *
 * PURPOSE:
 *   Retain Search Everywhere query/results/selection independently from a UI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_SEARCH_SESSION_H
#define UMICOM_DEVELOPER_WORKBENCH_SEARCH_SESSION_H

#include "umicom/developer_workbench/search_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer workbench search session data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkbenchSearchSession {
    char query[UMI_DEVELOPER_WORKBENCH_TEXT_CAPACITY];
    UmiDeveloperWorkbenchSearchResult
        results[UMI_DEVELOPER_WORKBENCH_MAX_RESULTS];
    size_t result_count;
    size_t selected_index;
    int has_selection;
    uint64_t revision;
} UmiDeveloperWorkbenchSearchSession;

/**
 * Initialise developer workbench search session from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_workbench_search_session_init(
    UmiDeveloperWorkbenchSearchSession *session);

/**
 * Provide the developer workbench search session query operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_search_session_query(
    UmiDeveloperWorkbenchSearchSession *session,
    UmiDeveloperWorkbenchSearchEngine *engine,
    const char *query);

/**
 * Provide the developer workbench search session select operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_search_session_select(
    UmiDeveloperWorkbenchSearchSession *session,
    size_t index);

/**
 * Find developer workbench search session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_workbench_search_session_selected(
    const UmiDeveloperWorkbenchSearchSession *session,
    UmiDeveloperWorkbenchSearchResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
