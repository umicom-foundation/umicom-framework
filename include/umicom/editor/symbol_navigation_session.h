/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/symbol_navigation_session.h
 *
 * PURPOSE:
 *   Aggregate document and workspace symbols from native and remote providers,
 *   perform deterministic fuzzy/local ranking, preserve hierarchy depth,
 *   support follow-cursor selection and expose a breadcrumb-compatible path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SYMBOL_NAVIGATION_SESSION_H
#define UMICOM_EDITOR_SYMBOL_NAVIGATION_SESSION_H

#include "umicom/editor/navigation_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SYMBOL_NAVIGATION_SESSION_API_VERSION 1U
#define UMI_EDITOR_SYMBOL_NAVIGATION_FILTER_CAPACITY 256U
#define UMI_EDITOR_SYMBOL_NAVIGATION_PATH_CAPACITY 64U

typedef enum UmiEditorSymbolNavigationScope {
    UMI_EDITOR_SYMBOL_NAVIGATION_DOCUMENT = 1,
    UMI_EDITOR_SYMBOL_NAVIGATION_WORKSPACE = 2
} UmiEditorSymbolNavigationScope;

typedef enum UmiEditorSymbolNavigationState {
    UMI_EDITOR_SYMBOL_NAVIGATION_CLOSED = 1,
    UMI_EDITOR_SYMBOL_NAVIGATION_LOADING = 2,
    UMI_EDITOR_SYMBOL_NAVIGATION_OPEN = 3,
    UMI_EDITOR_SYMBOL_NAVIGATION_CANCELLED = 4,
    UMI_EDITOR_SYMBOL_NAVIGATION_FAILED = 5
} UmiEditorSymbolNavigationState;

typedef struct UmiEditorSymbolNavigationEntry {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorIndexedSymbol symbol;
    int32_t score;
    size_t depth;
    int visible;
    int selected;
} UmiEditorSymbolNavigationEntry;

typedef struct UmiEditorSymbolNavigationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorSymbolNavigationState state;
    UmiEditorSymbolNavigationScope scope;
    size_t symbol_count;
    size_t visible_symbol_count;
    size_t selected_index;
    size_t selected_path_count;
    size_t provider_count;
    uint64_t request_id;
    uint64_t revision;
    int has_selection;
    int follow_cursor;
    int truncated;
    char filter[UMI_EDITOR_SYMBOL_NAVIGATION_FILTER_CAPACITY];
} UmiEditorSymbolNavigationSnapshot;

typedef struct UmiEditorSymbolNavigationSession
    UmiEditorSymbolNavigationSession;

UmiStatus umi_editor_symbol_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorSymbolNavigationSession **out_session);
void umi_editor_symbol_navigation_session_destroy(
    UmiEditorSymbolNavigationSession *session);
UmiStatus umi_editor_symbol_navigation_session_open(
    UmiEditorSymbolNavigationSession *session,
    UmiEditorSymbolNavigationScope scope,
    const UmiEditorNavigationRequest *request);
UmiStatus umi_editor_symbol_navigation_session_refresh(
    UmiEditorSymbolNavigationSession *session);
UmiStatus umi_editor_symbol_navigation_session_cancel(
    UmiEditorSymbolNavigationSession *session);
UmiStatus umi_editor_symbol_navigation_session_close(
    UmiEditorSymbolNavigationSession *session);
UmiStatus umi_editor_symbol_navigation_session_set_filter(
    UmiEditorSymbolNavigationSession *session,
    const char *filter);
UmiStatus umi_editor_symbol_navigation_session_set_follow_cursor(
    UmiEditorSymbolNavigationSession *session,
    int follow_cursor);
UmiStatus umi_editor_symbol_navigation_session_select(
    UmiEditorSymbolNavigationSession *session,
    size_t symbol_index);
UmiStatus umi_editor_symbol_navigation_session_select_next(
    UmiEditorSymbolNavigationSession *session,
    int wrap);
UmiStatus umi_editor_symbol_navigation_session_select_previous(
    UmiEditorSymbolNavigationSession *session,
    int wrap);
UmiStatus umi_editor_symbol_navigation_session_select_enclosing(
    UmiEditorSymbolNavigationSession *session,
    const UmiEditorSourceLocation *location);
UmiStatus umi_editor_symbol_navigation_session_at(
    const UmiEditorSymbolNavigationSession *session,
    size_t position,
    UmiEditorSymbolNavigationEntry *out_entry);
UmiStatus umi_editor_symbol_navigation_session_visible_at(
    const UmiEditorSymbolNavigationSession *session,
    size_t visible_position,
    UmiEditorSymbolNavigationEntry *out_entry);
UmiStatus umi_editor_symbol_navigation_session_selected(
    const UmiEditorSymbolNavigationSession *session,
    UmiEditorSymbolNavigationEntry *out_entry);
size_t umi_editor_symbol_navigation_session_selected_path_count(
    const UmiEditorSymbolNavigationSession *session);
UmiStatus umi_editor_symbol_navigation_session_selected_path_at(
    const UmiEditorSymbolNavigationSession *session,
    size_t path_position,
    UmiEditorSymbolNavigationEntry *out_entry);
UmiStatus umi_editor_symbol_navigation_session_provider_report_at(
    const UmiEditorSymbolNavigationSession *session,
    size_t position,
    UmiEditorNavigationProviderReport *out_report);
UmiStatus umi_editor_symbol_navigation_session_snapshot(
    const UmiEditorSymbolNavigationSession *session,
    UmiEditorSymbolNavigationSnapshot *out_snapshot);
size_t umi_editor_symbol_navigation_session_count(
    const UmiEditorSymbolNavigationSession *session);
size_t umi_editor_symbol_navigation_session_visible_count(
    const UmiEditorSymbolNavigationSession *session);
uint64_t umi_editor_symbol_navigation_session_revision(
    const UmiEditorSymbolNavigationSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SYMBOL_NAVIGATION_SESSION_H */
