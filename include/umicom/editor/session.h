/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/session.h
 *
 * PURPOSE:
 *   Define an owned reusable editor session that groups document, cursor, selection, diagnostics, completion, actions, diff and configuration models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_EDITOR_SESSION_H
#define UMICOM_EDITOR_SESSION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/editor/document.h"
#include "umicom/editor/cursor.h"
#include "umicom/editor/selection_range.h"
#include "umicom/editor/marker.h"
#include "umicom/editor/fold_region.h"
#include "umicom/editor/symbol.h"
#include "umicom/editor/diagnostic.h"
#include "umicom/editor/completion.h"
#include "umicom/editor/code_action.h"
#include "umicom/editor/diff_hunk.h"
#include "umicom/editor/configuration.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the editor session data shared with callers of this public contract.
 */
typedef struct UmiEditorSession UmiEditorSession;
/**
 * Represent the editor session snapshot data shared with callers of this public contract.
 */
typedef struct UmiEditorSessionSnapshot { uint32_t struct_size; uint32_t api_version;
    size_t document_count;
    size_t cursor_count;
    size_t selection_range_count;
    size_t marker_count;
    size_t fold_region_count;
    size_t symbol_count;
    size_t diagnostic_count;
    size_t completion_count;
    size_t code_action_count;
    size_t diff_hunk_count;
    size_t configuration_count;
    uint64_t revision;
} UmiEditorSessionSnapshot;
/**
 * Initialise editor session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_session_create(UmiEditorSession **out_session);
/**
 * Release or reset state held by editor session so the same storage can be reused safely.
 */
void umi_editor_session_destroy(UmiEditorSession *session);
/**
 * Provide the editor session snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_session_snapshot(const UmiEditorSession *session,UmiEditorSessionSnapshot *out_snapshot);
/**
 * Provide the editor session document operation used by this module and its client
 * applications.
 */
UmiEditorDocumentRegistry *umi_editor_session_document(UmiEditorSession *session);
/**
 * Provide the editor session cursor operation used by this module and its client
 * applications.
 */
UmiEditorCursorRegistry *umi_editor_session_cursor(UmiEditorSession *session);
/**
 * Provide the editor session selection range operation used by this module and its client
 * applications.
 */
UmiEditorSelectionRangeRegistry *umi_editor_session_selection_range(UmiEditorSession *session);
/**
 * Provide the editor session marker operation used by this module and its client
 * applications.
 */
UmiEditorMarkerRegistry *umi_editor_session_marker(UmiEditorSession *session);
/**
 * Provide the editor session fold region operation used by this module and its client
 * applications.
 */
UmiEditorFoldRegionRegistry *umi_editor_session_fold_region(UmiEditorSession *session);
/**
 * Provide the editor session symbol operation used by this module and its client
 * applications.
 */
UmiEditorSymbolRegistry *umi_editor_session_symbol(UmiEditorSession *session);
/**
 * Provide the editor session diagnostic operation used by this module and its client
 * applications.
 */
UmiEditorDiagnosticRegistry *umi_editor_session_diagnostic(UmiEditorSession *session);
/**
 * Provide the editor session completion operation used by this module and its client
 * applications.
 */
UmiEditorCompletionRegistry *umi_editor_session_completion(UmiEditorSession *session);
/**
 * Provide the editor session code action operation used by this module and its client
 * applications.
 */
UmiEditorCodeActionRegistry *umi_editor_session_code_action(UmiEditorSession *session);
/**
 * Provide the editor session diff hunk operation used by this module and its client
 * applications.
 */
UmiEditorDiffHunkRegistry *umi_editor_session_diff_hunk(UmiEditorSession *session);
/**
 * Provide the editor session configuration operation used by this module and its client
 * applications.
 */
UmiEditorConfigurationRegistry *umi_editor_session_configuration(UmiEditorSession *session);
#ifdef __cplusplus
}
#endif
#endif
