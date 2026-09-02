/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/builder_session.c
 *
 * PURPOSE:
 *   Compose all Visual Application Builder v2 services into one owned session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/builder_session.h"

#include "umicom/designer/catalogue.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerBuilderSession {
    UmiDeclComponentRegistry *components;
    UmiDeclSchema schema;
    UmiDesignerDocument *document;
    UmiDesignerHistory *history;
    UmiDesignerTransactionHistory *transactions;
    UmiDesignerSelection selection;
    UmiDesignerBindingEditor *bindings;
    UmiDesignerInteractionEditor *interactions;
    UmiDesignerSemanticClipboard clipboard;
    UmiDesignerTemplateRegistry *templates;
    UmiDesignerLivePreview live_preview;
    UmiDesignerGeneratedSource generated_source;
};

/*
 * Provide the register default templates operation used by this module and its client
 * applications.
 */
static UmiStatus register_default_templates(UmiDesignerTemplateRegistry *registry)
{
    UmiDesignerTemplate item;
    UmiDeclNode panel;
    UmiDeclNode title;
    UmiDeclNode action;
    UmiStatus status;
    status = umi_designer_template_init(&item, "form-card", "Form card", "Forms");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&panel, "card", "pane", "-");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&title, "heading", "label", "card");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(&title, "title", UMI_DECL_VALUE_STRING, "New form");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&action, "submit", "button", "card");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(&action, "title", UMI_DECL_VALUE_STRING, "Submit");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_template_add_node(&item, &panel);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_template_add_node(&item, &title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_template_add_node(&item, &action);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_template_registry_register(registry, &item);
    return status;
}

/*
 * Initialise designer builder session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_builder_session_create(const char *application_id,
                                                  UmiDesignerBuilderSession **out_session)
{
    UmiDesignerBuilderSession *session;
    UmiDeclNode root;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = (UmiDesignerBuilderSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_designer_catalogue_create(&session->components, &session->schema);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_document_create(application_id, &session->document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&root, "root", "window", "-");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_document_add_node(umi_designer_document_declarative(session->document), &root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_history_create(session->document, &session->history);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_transaction_history_create(session->document, &session->transactions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_binding_editor_create(&session->bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_interaction_editor_create(&session->interactions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_template_registry_create(&session->templates);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = register_default_templates(session->templates);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_selection_set_primary(&session->selection, "root");
    umi_designer_clipboard_init(&session->clipboard);
    umi_designer_live_preview_init(&session->live_preview);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_designer_builder_session_destroy(session);
        return status;
    }
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by designer builder session so the same storage can be
 * reused safely.
 */
void umi_designer_builder_session_destroy(UmiDesignerBuilderSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    umi_designer_template_registry_destroy(session->templates);
    umi_designer_interaction_editor_destroy(session->interactions);
    umi_designer_binding_editor_destroy(session->bindings);
    umi_designer_transaction_history_destroy(session->transactions);
    umi_designer_history_destroy(session->history);
    umi_designer_document_destroy(session->document);
    umi_decl_component_registry_destroy(session->components);
    free(session);
}

/*
 * Provide the designer builder session add component operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_builder_session_add_component(UmiDesignerBuilderSession *session,
                                                         const char *node_id,
                                                         const char *component_type,
                                                         const char *parent_id)
{
    UmiDeclNode node;
    UmiDesignerOperation operation;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_decl_node_init(&node, node_id, component_type, parent_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_operation_add(&node, &operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_history_execute(session->history, &operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) umi_designer_live_preview_mark_stale(&session->live_preview);
    return status;
}

/*
 * Provide the designer builder session select operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_builder_session_select(UmiDesignerBuilderSession *session,
                                                  const char *node_id,
                                                  int extend_selection)
{
    UmiDeclNode node;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_decl_document_find_node(umi_designer_document_declarative(session->document), node_id, &node) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (extend_selection == 0) {
        umi_designer_selection_clear(&session->selection);
        return umi_designer_selection_set_primary(&session->selection, node_id);
    }
    return umi_designer_selection_add(&session->selection, node_id);
}

/*
 * Provide the designer builder session tree operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_builder_session_tree(const UmiDesignerBuilderSession *session,
                                                UmiDesignerComponentTree *out_tree)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_component_tree_build(session->document, &session->selection, out_tree);
}

/*
 * Provide the designer builder session preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_builder_session_preview(UmiDesignerBuilderSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_live_preview_refresh(&session->live_preview, session->document, &session->schema);
}

/*
 * Provide the designer builder session generate operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_builder_session_generate(UmiDesignerBuilderSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_source_generation_generate(session->document, &session->generated_source);
}

/*
 * Provide the designer builder session snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_builder_session_snapshot(const UmiDesignerBuilderSession *session,
                                                    UmiDesignerBuilderSessionSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)umi_designer_document_snapshot(session->document, &out_snapshot->document);
    out_snapshot->selected_count = session->selection.count;
    out_snapshot->binding_count = umi_designer_binding_editor_count(session->bindings);
    out_snapshot->interaction_count = umi_designer_interaction_editor_count(session->interactions);
    out_snapshot->template_count = umi_designer_template_registry_count(session->templates);
    out_snapshot->undo_count = umi_designer_transaction_history_undo_count(session->transactions);
    out_snapshot->redo_count = umi_designer_transaction_history_redo_count(session->transactions);
    out_snapshot->preview_health = session->live_preview.health;
    out_snapshot->generated_revision = session->generated_source.source_revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the designer builder session document operation used by this module and its
 * client applications.
 */
UmiDesignerDocument *umi_designer_builder_session_document(UmiDesignerBuilderSession *session) { return session != NULL ? session->document : NULL; }
/*
 * Provide the designer builder session history operation used by this module and its
 * client applications.
 */
UmiDesignerHistory *umi_designer_builder_session_history(UmiDesignerBuilderSession *session) { return session != NULL ? session->history : NULL; }
/*
 * Provide the designer builder session transactions operation used by this module and its
 * client applications.
 */
UmiDesignerTransactionHistory *umi_designer_builder_session_transactions(UmiDesignerBuilderSession *session) { return session != NULL ? session->transactions : NULL; }
/*
 * Provide the designer builder session selection operation used by this module and its
 * client applications.
 */
UmiDesignerSelection *umi_designer_builder_session_selection(UmiDesignerBuilderSession *session) { return session != NULL ? &session->selection : NULL; }
/*
 * Provide the designer builder session components operation used by this module and its
 * client applications.
 */
UmiDeclComponentRegistry *umi_designer_builder_session_components(UmiDesignerBuilderSession *session) { return session != NULL ? session->components : NULL; }
/*
 * Provide the designer builder session schema operation used by this module and its client
 * applications.
 */
UmiDeclSchema *umi_designer_builder_session_schema(UmiDesignerBuilderSession *session) { return session != NULL ? &session->schema : NULL; }
/*
 * Provide the designer builder session bindings operation used by this module and its
 * client applications.
 */
UmiDesignerBindingEditor *umi_designer_builder_session_bindings(UmiDesignerBuilderSession *session) { return session != NULL ? session->bindings : NULL; }
/*
 * Provide the designer builder session interactions operation used by this module and its
 * client applications.
 */
UmiDesignerInteractionEditor *umi_designer_builder_session_interactions(UmiDesignerBuilderSession *session) { return session != NULL ? session->interactions : NULL; }
/*
 * Provide the designer builder session clipboard operation used by this module and its
 * client applications.
 */
UmiDesignerSemanticClipboard *umi_designer_builder_session_clipboard(UmiDesignerBuilderSession *session) { return session != NULL ? &session->clipboard : NULL; }
/*
 * Provide the designer builder session templates operation used by this module and its
 * client applications.
 */
UmiDesignerTemplateRegistry *umi_designer_builder_session_templates(UmiDesignerBuilderSession *session) { return session != NULL ? session->templates : NULL; }
/*
 * Provide the designer builder session live preview operation used by this module and its
 * client applications.
 */
UmiDesignerLivePreview *umi_designer_builder_session_live_preview(UmiDesignerBuilderSession *session) { return session != NULL ? &session->live_preview : NULL; }
/*
 * Provide the designer builder session generated source operation used by this module and
 * its client applications.
 */
const UmiDesignerGeneratedSource *umi_designer_builder_session_generated_source(const UmiDesignerBuilderSession *session) { return session != NULL ? &session->generated_source : NULL; }
