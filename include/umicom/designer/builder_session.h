/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/builder_session.h
 *
 * PURPOSE:
 *   Own the complete toolkit-neutral Visual Application Builder v2 workspace.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A session is the reusable application service behind any visual frontend. It
 * owns document, selection, history, editors, templates, preview and source.
 */
#ifndef UMICOM_DESIGNER_BUILDER_SESSION_H
#define UMICOM_DESIGNER_BUILDER_SESSION_H

#include "umicom/designer/binding_editor.h"
#include "umicom/designer/semantic_clipboard.h"
#include "umicom/designer/component_tree.h"
#include "umicom/designer/interaction_editor.h"
#include "umicom/designer/live_preview.h"
#include "umicom/designer/source_generation.h"
#include "umicom/designer/template.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerBuilderSession UmiDesignerBuilderSession;

typedef struct UmiDesignerBuilderSessionSnapshot {
    UmiDesignerDocumentSnapshot document;
    size_t selected_count;
    size_t binding_count;
    size_t interaction_count;
    size_t template_count;
    size_t undo_count;
    size_t redo_count;
    UmiDesignerPreviewHealth preview_health;
    uint64_t generated_revision;
} UmiDesignerBuilderSessionSnapshot;

UmiStatus umi_designer_builder_session_create(const char *application_id,
                                                  UmiDesignerBuilderSession **out_session);
void umi_designer_builder_session_destroy(UmiDesignerBuilderSession *session);
UmiStatus umi_designer_builder_session_add_component(UmiDesignerBuilderSession *session,
                                                         const char *node_id,
                                                         const char *component_type,
                                                         const char *parent_id);
UmiStatus umi_designer_builder_session_select(UmiDesignerBuilderSession *session,
                                                  const char *node_id,
                                                  int extend_selection);
UmiStatus umi_designer_builder_session_tree(const UmiDesignerBuilderSession *session,
                                                UmiDesignerComponentTree *out_tree);
UmiStatus umi_designer_builder_session_preview(UmiDesignerBuilderSession *session);
UmiStatus umi_designer_builder_session_generate(UmiDesignerBuilderSession *session);
UmiStatus umi_designer_builder_session_snapshot(const UmiDesignerBuilderSession *session,
                                                    UmiDesignerBuilderSessionSnapshot *out_snapshot);

UmiDesignerDocument *umi_designer_builder_session_document(UmiDesignerBuilderSession *session);
UmiDesignerHistory *umi_designer_builder_session_history(UmiDesignerBuilderSession *session);
UmiDesignerTransactionHistory *umi_designer_builder_session_transactions(UmiDesignerBuilderSession *session);
UmiDesignerSelection *umi_designer_builder_session_selection(UmiDesignerBuilderSession *session);
UmiDeclComponentRegistry *umi_designer_builder_session_components(UmiDesignerBuilderSession *session);
UmiDeclSchema *umi_designer_builder_session_schema(UmiDesignerBuilderSession *session);
UmiDesignerBindingEditor *umi_designer_builder_session_bindings(UmiDesignerBuilderSession *session);
UmiDesignerInteractionEditor *umi_designer_builder_session_interactions(UmiDesignerBuilderSession *session);
UmiDesignerSemanticClipboard *umi_designer_builder_session_clipboard(UmiDesignerBuilderSession *session);
UmiDesignerTemplateRegistry *umi_designer_builder_session_templates(UmiDesignerBuilderSession *session);
UmiDesignerLivePreview *umi_designer_builder_session_live_preview(UmiDesignerBuilderSession *session);
const UmiDesignerGeneratedSource *umi_designer_builder_session_generated_source(const UmiDesignerBuilderSession *session);

#ifdef __cplusplus
}
#endif
#endif
