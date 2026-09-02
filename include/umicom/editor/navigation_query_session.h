/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/navigation_query_session.h
 *
 * PURPOSE:
 *   Execute one definition, declaration, implementation, reference or symbol
 *   query across all applicable providers; aggregate, rank and de-duplicate
 *   results; preserve provider evidence; and expose deterministic selection and
 *   cancellation semantics to every Umicom frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_QUERY_SESSION_H
#define UMICOM_EDITOR_NAVIGATION_QUERY_SESSION_H

#include "umicom/editor/navigation_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_QUERY_SESSION_API_VERSION 1U

/**
 * List the named editor navigation query session state values accepted by this public
 * contract.
 */
typedef enum UmiEditorNavigationQuerySessionState {
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_EMPTY = 1,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_READY = 2,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_RUNNING = 3,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_COMPLETE = 4,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_CANCELLED = 5,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_FAILED = 6
} UmiEditorNavigationQuerySessionState;

/**
 * Represent the editor navigation query options data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorNavigationQueryOptions {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_results;
    int allow_partial_results;
    int stop_on_first_primary;
    int include_stale_results;
    int wrap_selection;
} UmiEditorNavigationQueryOptions;

/**
 * Represent the editor navigation query session snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorNavigationQuerySessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorNavigationQuerySessionState state;
    UmiEditorNavigationQueryKind query_kind;
    size_t result_count;
    size_t provider_count;
    size_t failed_provider_count;
    size_t stale_result_count;
    size_t duplicate_result_count;
    size_t selected_index;
    uint64_t request_id;
    uint64_t registry_revision;
    uint64_t revision;
    int has_selection;
    int truncated;
    int cancelled;
} UmiEditorNavigationQuerySessionSnapshot;

/**
 * Represent the editor navigation query session data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorNavigationQuerySession
    UmiEditorNavigationQuerySession;

/**
 * Initialise editor navigation query options from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_query_options_initialize(
    UmiEditorNavigationQueryOptions *options);
/**
 * Initialise editor navigation query session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_navigation_query_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorNavigationQuerySession **out_session);
/**
 * Release or reset state held by editor navigation query session so the same storage can
 * be reused safely.
 */
void umi_editor_navigation_query_session_destroy(
    UmiEditorNavigationQuerySession *session);
/**
 * Release or reset state held by editor navigation query session so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_navigation_query_session_clear(
    UmiEditorNavigationQuerySession *session);
/**
 * Perform editor navigation query session through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_editor_navigation_query_session_execute(
    UmiEditorNavigationQuerySession *session,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationQueryOptions *options);
/**
 * Provide the editor navigation query session refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_query_session_refresh(
    UmiEditorNavigationQuerySession *session);
/**
 * Provide the editor navigation query session cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_query_session_cancel(
    UmiEditorNavigationQuerySession *session);
/**
 * Provide the editor navigation query session select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_navigation_query_session_select(
    UmiEditorNavigationQuerySession *session,
    size_t position);
/**
 * Provide the editor navigation query session select next operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_navigation_query_session_select_next(
    UmiEditorNavigationQuerySession *session);
/**
 * Provide the editor navigation query session select previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_navigation_query_session_select_previous(
    UmiEditorNavigationQuerySession *session);
/**
 * Find editor navigation query session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_navigation_query_session_at(
    const UmiEditorNavigationQuerySession *session,
    size_t position,
    UmiEditorNavigationResult *out_result);
/**
 * Find editor navigation query session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_navigation_query_session_selected(
    const UmiEditorNavigationQuerySession *session,
    UmiEditorNavigationResult *out_result);
/**
 * Find editor navigation query session provider report while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_navigation_query_session_provider_report_at(
    const UmiEditorNavigationQuerySession *session,
    size_t position,
    UmiEditorNavigationProviderReport *out_report);
/**
 * Provide the editor navigation query session snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_navigation_query_session_snapshot(
    const UmiEditorNavigationQuerySession *session,
    UmiEditorNavigationQuerySessionSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor navigation query session without
 * changing their state.
 */
size_t umi_editor_navigation_query_session_count(
    const UmiEditorNavigationQuerySession *session);
/**
 * Return the number of records represented by editor navigation query session provider
 * report without changing their state.
 */
size_t umi_editor_navigation_query_session_provider_report_count(
    const UmiEditorNavigationQuerySession *session);
/**
 * Provide the editor navigation query session revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_navigation_query_session_revision(
    const UmiEditorNavigationQuerySession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_QUERY_SESSION_H */
