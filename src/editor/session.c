/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/session.c
 *
 * PURPOSE:
 *   Implement the reusable editor session and ownership of its toolkit-neutral editor models.
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
#include "umicom/editor/session.h"
#include <stdlib.h>
#include <string.h>
struct UmiEditorSession{UmiEditorDocumentRegistry *document; UmiEditorCursorRegistry *cursor; UmiEditorSelectionRangeRegistry *selection_range; UmiEditorMarkerRegistry *marker; UmiEditorFoldRegionRegistry *fold_region; UmiEditorSymbolRegistry *symbol; UmiEditorDiagnosticRegistry *diagnostic; UmiEditorCompletionRegistry *completion; UmiEditorCodeActionRegistry *code_action; UmiEditorDiffHunkRegistry *diff_hunk; UmiEditorConfigurationRegistry *configuration; uint64_t revision;};
/*
 * Initialise editor session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_session_create(UmiEditorSession **out){UmiEditorSession*p;UmiStatus s=UMI_STATUS_OK;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;p->revision=1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_document_registry_create(&p->document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_cursor_registry_create(&p->cursor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_selection_range_registry_create(&p->selection_range);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_marker_registry_create(&p->marker);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_fold_region_registry_create(&p->fold_region);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_symbol_registry_create(&p->symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_diagnostic_registry_create(&p->diagnostic);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_completion_registry_create(&p->completion);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_code_action_registry_create(&p->code_action);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_diff_hunk_registry_create(&p->diff_hunk);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s==UMI_STATUS_OK)s=umi_editor_configuration_registry_create(&p->configuration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(s!=UMI_STATUS_OK){umi_editor_session_destroy(p);return s;}*out=p;return UMI_STATUS_OK;}
/* Release or reset state held by editor session so the same storage can be reused safely. */
void umi_editor_session_destroy(UmiEditorSession*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return;
    umi_editor_configuration_registry_destroy(p->configuration);
    umi_editor_diff_hunk_registry_destroy(p->diff_hunk);
    umi_editor_code_action_registry_destroy(p->code_action);
    umi_editor_completion_registry_destroy(p->completion);
    umi_editor_diagnostic_registry_destroy(p->diagnostic);
    umi_editor_symbol_registry_destroy(p->symbol);
    umi_editor_fold_region_registry_destroy(p->fold_region);
    umi_editor_marker_registry_destroy(p->marker);
    umi_editor_selection_range_registry_destroy(p->selection_range);
    umi_editor_cursor_registry_destroy(p->cursor);
    umi_editor_document_registry_destroy(p->document);
    free(p);}
/*
 * Provide the editor session snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_session_snapshot(const UmiEditorSession*p,UmiEditorSessionSnapshot*o){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;
    o->document_count=umi_editor_document_registry_count(p->document);
    o->cursor_count=umi_editor_cursor_registry_count(p->cursor);
    o->selection_range_count=umi_editor_selection_range_registry_count(p->selection_range);
    o->marker_count=umi_editor_marker_registry_count(p->marker);
    o->fold_region_count=umi_editor_fold_region_registry_count(p->fold_region);
    o->symbol_count=umi_editor_symbol_registry_count(p->symbol);
    o->diagnostic_count=umi_editor_diagnostic_registry_count(p->diagnostic);
    o->completion_count=umi_editor_completion_registry_count(p->completion);
    o->code_action_count=umi_editor_code_action_registry_count(p->code_action);
    o->diff_hunk_count=umi_editor_diff_hunk_registry_count(p->diff_hunk);
    o->configuration_count=umi_editor_configuration_registry_count(p->configuration);
    o->revision=p->revision;return UMI_STATUS_OK;}
/*
 * Provide the editor session document operation used by this module and its client
 * applications.
 */
UmiEditorDocumentRegistry *umi_editor_session_document(UmiEditorSession*p){return p!=NULL?p->document:NULL;}
/*
 * Provide the editor session cursor operation used by this module and its client
 * applications.
 */
UmiEditorCursorRegistry *umi_editor_session_cursor(UmiEditorSession*p){return p!=NULL?p->cursor:NULL;}
/*
 * Provide the editor session selection range operation used by this module and its client
 * applications.
 */
UmiEditorSelectionRangeRegistry *umi_editor_session_selection_range(UmiEditorSession*p){return p!=NULL?p->selection_range:NULL;}
/*
 * Provide the editor session marker operation used by this module and its client
 * applications.
 */
UmiEditorMarkerRegistry *umi_editor_session_marker(UmiEditorSession*p){return p!=NULL?p->marker:NULL;}
/*
 * Provide the editor session fold region operation used by this module and its client
 * applications.
 */
UmiEditorFoldRegionRegistry *umi_editor_session_fold_region(UmiEditorSession*p){return p!=NULL?p->fold_region:NULL;}
/*
 * Provide the editor session symbol operation used by this module and its client
 * applications.
 */
UmiEditorSymbolRegistry *umi_editor_session_symbol(UmiEditorSession*p){return p!=NULL?p->symbol:NULL;}
/*
 * Provide the editor session diagnostic operation used by this module and its client
 * applications.
 */
UmiEditorDiagnosticRegistry *umi_editor_session_diagnostic(UmiEditorSession*p){return p!=NULL?p->diagnostic:NULL;}
/*
 * Provide the editor session completion operation used by this module and its client
 * applications.
 */
UmiEditorCompletionRegistry *umi_editor_session_completion(UmiEditorSession*p){return p!=NULL?p->completion:NULL;}
/*
 * Provide the editor session code action operation used by this module and its client
 * applications.
 */
UmiEditorCodeActionRegistry *umi_editor_session_code_action(UmiEditorSession*p){return p!=NULL?p->code_action:NULL;}
/*
 * Provide the editor session diff hunk operation used by this module and its client
 * applications.
 */
UmiEditorDiffHunkRegistry *umi_editor_session_diff_hunk(UmiEditorSession*p){return p!=NULL?p->diff_hunk:NULL;}
/*
 * Provide the editor session configuration operation used by this module and its client
 * applications.
 */
UmiEditorConfigurationRegistry *umi_editor_session_configuration(UmiEditorSession*p){return p!=NULL?p->configuration:NULL;}
