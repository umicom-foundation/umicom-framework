/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/builder_session_v2.h
 *
 * PURPOSE:
 *   Own the complete toolkit-neutral Visual Application Builder v2 workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A session is the reusable application service behind any visual frontend. It
 * owns document, selection, history, editors, templates, preview and source.
 */
#ifndef UMICOM_DESIGNER_BUILDER_SESSION_V2_H
#define UMICOM_DESIGNER_BUILDER_SESSION_V2_H

#include "umicom/designer/binding_editor_v2.h"
#include "umicom/designer/clipboard_v2.h"
#include "umicom/designer/component_tree_v2.h"
#include "umicom/designer/interaction_editor_v2.h"
#include "umicom/designer/live_preview_v2.h"
#include "umicom/designer/source_generation_v2.h"
#include "umicom/designer/template_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerBuilderSessionV2 UmiDesignerBuilderSessionV2;

typedef struct UmiDesignerBuilderSessionSnapshotV2 {
    UmiDesignerDocumentSnapshot document;
    size_t selected_count;
    size_t binding_count;
    size_t interaction_count;
    size_t template_count;
    size_t undo_count;
    size_t redo_count;
    UmiDesignerPreviewHealthV2 preview_health;
    uint64_t generated_revision;
} UmiDesignerBuilderSessionSnapshotV2;

UmiStatus umi_designer_builder_session_v2_create(const char *application_id,
                                                  UmiDesignerBuilderSessionV2 **out_session);
void umi_designer_builder_session_v2_destroy(UmiDesignerBuilderSessionV2 *session);
UmiStatus umi_designer_builder_session_v2_add_component(UmiDesignerBuilderSessionV2 *session,
                                                         const char *node_id,
                                                         const char *component_type,
                                                         const char *parent_id);
UmiStatus umi_designer_builder_session_v2_select(UmiDesignerBuilderSessionV2 *session,
                                                  const char *node_id,
                                                  int extend_selection);
UmiStatus umi_designer_builder_session_v2_tree(const UmiDesignerBuilderSessionV2 *session,
                                                UmiDesignerComponentTreeV2 *out_tree);
UmiStatus umi_designer_builder_session_v2_preview(UmiDesignerBuilderSessionV2 *session);
UmiStatus umi_designer_builder_session_v2_generate(UmiDesignerBuilderSessionV2 *session);
UmiStatus umi_designer_builder_session_v2_snapshot(const UmiDesignerBuilderSessionV2 *session,
                                                    UmiDesignerBuilderSessionSnapshotV2 *out_snapshot);

UmiDesignerDocument *umi_designer_builder_session_v2_document(UmiDesignerBuilderSessionV2 *session);
UmiDesignerHistory *umi_designer_builder_session_v2_history(UmiDesignerBuilderSessionV2 *session);
UmiDesignerTransactionHistoryV2 *umi_designer_builder_session_v2_transactions(UmiDesignerBuilderSessionV2 *session);
UmiDesignerSelection *umi_designer_builder_session_v2_selection(UmiDesignerBuilderSessionV2 *session);
UmiDeclComponentRegistry *umi_designer_builder_session_v2_components(UmiDesignerBuilderSessionV2 *session);
UmiDeclSchema *umi_designer_builder_session_v2_schema(UmiDesignerBuilderSessionV2 *session);
UmiDesignerBindingEditorV2 *umi_designer_builder_session_v2_bindings(UmiDesignerBuilderSessionV2 *session);
UmiDesignerInteractionEditorV2 *umi_designer_builder_session_v2_interactions(UmiDesignerBuilderSessionV2 *session);
UmiDesignerClipboardV2 *umi_designer_builder_session_v2_clipboard(UmiDesignerBuilderSessionV2 *session);
UmiDesignerTemplateRegistryV2 *umi_designer_builder_session_v2_templates(UmiDesignerBuilderSessionV2 *session);
UmiDesignerLivePreviewV2 *umi_designer_builder_session_v2_live_preview(UmiDesignerBuilderSessionV2 *session);
const UmiDesignerGeneratedSourceV2 *umi_designer_builder_session_v2_generated_source(const UmiDesignerBuilderSessionV2 *session);

#ifdef __cplusplus
}
#endif
#endif
