/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/builder_session.h
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

/*
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

/**
 * Represent the designer builder session data shared with callers of this public contract.
 */
typedef struct UmiDesignerBuilderSession UmiDesignerBuilderSession;

/**
 * Represent the designer builder session snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise designer builder session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_builder_session_create(const char *application_id,
                                                  UmiDesignerBuilderSession **out_session);
/**
 * Release or reset state held by designer builder session so the same storage can be
 * reused safely.
 */
void umi_designer_builder_session_destroy(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session add component operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_builder_session_add_component(UmiDesignerBuilderSession *session,
                                                         const char *node_id,
                                                         const char *component_type,
                                                         const char *parent_id);
/**
 * Provide the designer builder session select operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_builder_session_select(UmiDesignerBuilderSession *session,
                                                  const char *node_id,
                                                  int extend_selection);
/**
 * Provide the designer builder session tree operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_builder_session_tree(const UmiDesignerBuilderSession *session,
                                                UmiDesignerComponentTree *out_tree);
/**
 * Provide the designer builder session preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_builder_session_preview(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session generate operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_builder_session_generate(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_builder_session_snapshot(const UmiDesignerBuilderSession *session,
                                                    UmiDesignerBuilderSessionSnapshot *out_snapshot);

/**
 * Provide the designer builder session document operation used by this module and its
 * client applications.
 */
UmiDesignerDocument *umi_designer_builder_session_document(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session history operation used by this module and its
 * client applications.
 */
UmiDesignerHistory *umi_designer_builder_session_history(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session transactions operation used by this module and its
 * client applications.
 */
UmiDesignerTransactionHistory *umi_designer_builder_session_transactions(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session selection operation used by this module and its
 * client applications.
 */
UmiDesignerSelection *umi_designer_builder_session_selection(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session components operation used by this module and its
 * client applications.
 */
UmiDeclComponentRegistry *umi_designer_builder_session_components(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session schema operation used by this module and its client
 * applications.
 */
UmiDeclSchema *umi_designer_builder_session_schema(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session bindings operation used by this module and its
 * client applications.
 */
UmiDesignerBindingEditor *umi_designer_builder_session_bindings(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session interactions operation used by this module and its
 * client applications.
 */
UmiDesignerInteractionEditor *umi_designer_builder_session_interactions(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session clipboard operation used by this module and its
 * client applications.
 */
UmiDesignerSemanticClipboard *umi_designer_builder_session_clipboard(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session templates operation used by this module and its
 * client applications.
 */
UmiDesignerTemplateRegistry *umi_designer_builder_session_templates(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session live preview operation used by this module and its
 * client applications.
 */
UmiDesignerLivePreview *umi_designer_builder_session_live_preview(UmiDesignerBuilderSession *session);
/**
 * Provide the designer builder session generated source operation used by this module and
 * its client applications.
 */
const UmiDesignerGeneratedSource *umi_designer_builder_session_generated_source(const UmiDesignerBuilderSession *session);

#ifdef __cplusplus
}
#endif
#endif
