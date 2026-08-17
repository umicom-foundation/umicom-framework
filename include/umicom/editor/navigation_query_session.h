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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_NAVIGATION_QUERY_SESSION_H
#define UMICOM_EDITOR_NAVIGATION_QUERY_SESSION_H

#include "umicom/editor/navigation_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_NAVIGATION_QUERY_SESSION_API_VERSION 1U

typedef enum UmiEditorNavigationQuerySessionState {
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_EMPTY = 1,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_READY = 2,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_RUNNING = 3,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_COMPLETE = 4,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_CANCELLED = 5,
    UMI_EDITOR_NAVIGATION_QUERY_SESSION_FAILED = 6
} UmiEditorNavigationQuerySessionState;

typedef struct UmiEditorNavigationQueryOptions {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_results;
    int allow_partial_results;
    int stop_on_first_primary;
    int include_stale_results;
    int wrap_selection;
} UmiEditorNavigationQueryOptions;

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

typedef struct UmiEditorNavigationQuerySession
    UmiEditorNavigationQuerySession;

UmiStatus umi_editor_navigation_query_options_initialize(
    UmiEditorNavigationQueryOptions *options);
UmiStatus umi_editor_navigation_query_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorNavigationQuerySession **out_session);
void umi_editor_navigation_query_session_destroy(
    UmiEditorNavigationQuerySession *session);
UmiStatus umi_editor_navigation_query_session_clear(
    UmiEditorNavigationQuerySession *session);
UmiStatus umi_editor_navigation_query_session_execute(
    UmiEditorNavigationQuerySession *session,
    const UmiEditorNavigationRequest *request,
    const UmiEditorNavigationQueryOptions *options);
UmiStatus umi_editor_navigation_query_session_refresh(
    UmiEditorNavigationQuerySession *session);
UmiStatus umi_editor_navigation_query_session_cancel(
    UmiEditorNavigationQuerySession *session);
UmiStatus umi_editor_navigation_query_session_select(
    UmiEditorNavigationQuerySession *session,
    size_t position);
UmiStatus umi_editor_navigation_query_session_select_next(
    UmiEditorNavigationQuerySession *session);
UmiStatus umi_editor_navigation_query_session_select_previous(
    UmiEditorNavigationQuerySession *session);
UmiStatus umi_editor_navigation_query_session_at(
    const UmiEditorNavigationQuerySession *session,
    size_t position,
    UmiEditorNavigationResult *out_result);
UmiStatus umi_editor_navigation_query_session_selected(
    const UmiEditorNavigationQuerySession *session,
    UmiEditorNavigationResult *out_result);
UmiStatus umi_editor_navigation_query_session_provider_report_at(
    const UmiEditorNavigationQuerySession *session,
    size_t position,
    UmiEditorNavigationProviderReport *out_report);
UmiStatus umi_editor_navigation_query_session_snapshot(
    const UmiEditorNavigationQuerySession *session,
    UmiEditorNavigationQuerySessionSnapshot *out_snapshot);
size_t umi_editor_navigation_query_session_count(
    const UmiEditorNavigationQuerySession *session);
size_t umi_editor_navigation_query_session_provider_report_count(
    const UmiEditorNavigationQuerySession *session);
uint64_t umi_editor_navigation_query_session_revision(
    const UmiEditorNavigationQuerySession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_NAVIGATION_QUERY_SESSION_H */
