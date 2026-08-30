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

static UmiStatus register_default_templates(UmiDesignerTemplateRegistry *registry)
{
    UmiDesignerTemplate item;
    UmiDeclNode panel;
    UmiDeclNode title;
    UmiDeclNode action;
    UmiStatus status;
    status = umi_designer_template_init(&item, "form-card", "Form card", "Forms");
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&panel, "card", "pane", "-");
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&title, "heading", "label", "card");
    if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(&title, "title", UMI_DECL_VALUE_STRING, "New form");
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&action, "submit", "button", "card");
    if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(&action, "title", UMI_DECL_VALUE_STRING, "Submit");
    if (status == UMI_STATUS_OK) status = umi_designer_template_add_node(&item, &panel);
    if (status == UMI_STATUS_OK) status = umi_designer_template_add_node(&item, &title);
    if (status == UMI_STATUS_OK) status = umi_designer_template_add_node(&item, &action);
    if (status == UMI_STATUS_OK) status = umi_designer_template_registry_register(registry, &item);
    return status;
}

UmiStatus umi_designer_builder_session_create(const char *application_id,
                                                  UmiDesignerBuilderSession **out_session)
{
    UmiDesignerBuilderSession *session;
    UmiDeclNode root;
    UmiStatus status;
    if (application_id == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = (UmiDesignerBuilderSession *)calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_designer_catalogue_create(&session->components, &session->schema);
    if (status == UMI_STATUS_OK) status = umi_designer_document_create(application_id, &session->document);
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&root, "root", "window", "-");
    if (status == UMI_STATUS_OK) status = umi_decl_document_add_node(umi_designer_document_declarative(session->document), &root);
    if (status == UMI_STATUS_OK) status = umi_designer_history_create(session->document, &session->history);
    if (status == UMI_STATUS_OK) status = umi_designer_transaction_history_create(session->document, &session->transactions);
    if (status == UMI_STATUS_OK) status = umi_designer_binding_editor_create(&session->bindings);
    if (status == UMI_STATUS_OK) status = umi_designer_interaction_editor_create(&session->interactions);
    if (status == UMI_STATUS_OK) status = umi_designer_template_registry_create(&session->templates);
    if (status == UMI_STATUS_OK) status = register_default_templates(session->templates);
    if (status == UMI_STATUS_OK) status = umi_designer_selection_set_primary(&session->selection, "root");
    umi_designer_clipboard_init(&session->clipboard);
    umi_designer_live_preview_init(&session->live_preview);
    if (status != UMI_STATUS_OK) {
        umi_designer_builder_session_destroy(session);
        return status;
    }
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_designer_builder_session_destroy(UmiDesignerBuilderSession *session)
{
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

UmiStatus umi_designer_builder_session_add_component(UmiDesignerBuilderSession *session,
                                                         const char *node_id,
                                                         const char *component_type,
                                                         const char *parent_id)
{
    UmiDeclNode node;
    UmiDesignerOperation operation;
    UmiStatus status;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_decl_node_init(&node, node_id, component_type, parent_id);
    if (status == UMI_STATUS_OK) status = umi_designer_operation_add(&node, &operation);
    if (status == UMI_STATUS_OK) status = umi_designer_history_execute(session->history, &operation);
    if (status == UMI_STATUS_OK) umi_designer_live_preview_mark_stale(&session->live_preview);
    return status;
}

UmiStatus umi_designer_builder_session_select(UmiDesignerBuilderSession *session,
                                                  const char *node_id,
                                                  int extend_selection)
{
    UmiDeclNode node;
    if (session == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_decl_document_find_node(umi_designer_document_declarative(session->document), node_id, &node) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (extend_selection == 0) {
        umi_designer_selection_clear(&session->selection);
        return umi_designer_selection_set_primary(&session->selection, node_id);
    }
    return umi_designer_selection_add(&session->selection, node_id);
}

UmiStatus umi_designer_builder_session_tree(const UmiDesignerBuilderSession *session,
                                                UmiDesignerComponentTree *out_tree)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_component_tree_build(session->document, &session->selection, out_tree);
}

UmiStatus umi_designer_builder_session_preview(UmiDesignerBuilderSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_live_preview_refresh(&session->live_preview, session->document, &session->schema);
}

UmiStatus umi_designer_builder_session_generate(UmiDesignerBuilderSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_source_generation_generate(session->document, &session->generated_source);
}

UmiStatus umi_designer_builder_session_snapshot(const UmiDesignerBuilderSession *session,
                                                    UmiDesignerBuilderSessionSnapshot *out_snapshot)
{
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

UmiDesignerDocument *umi_designer_builder_session_document(UmiDesignerBuilderSession *session) { return session != NULL ? session->document : NULL; }
UmiDesignerHistory *umi_designer_builder_session_history(UmiDesignerBuilderSession *session) { return session != NULL ? session->history : NULL; }
UmiDesignerTransactionHistory *umi_designer_builder_session_transactions(UmiDesignerBuilderSession *session) { return session != NULL ? session->transactions : NULL; }
UmiDesignerSelection *umi_designer_builder_session_selection(UmiDesignerBuilderSession *session) { return session != NULL ? &session->selection : NULL; }
UmiDeclComponentRegistry *umi_designer_builder_session_components(UmiDesignerBuilderSession *session) { return session != NULL ? session->components : NULL; }
UmiDeclSchema *umi_designer_builder_session_schema(UmiDesignerBuilderSession *session) { return session != NULL ? &session->schema : NULL; }
UmiDesignerBindingEditor *umi_designer_builder_session_bindings(UmiDesignerBuilderSession *session) { return session != NULL ? session->bindings : NULL; }
UmiDesignerInteractionEditor *umi_designer_builder_session_interactions(UmiDesignerBuilderSession *session) { return session != NULL ? session->interactions : NULL; }
UmiDesignerSemanticClipboard *umi_designer_builder_session_clipboard(UmiDesignerBuilderSession *session) { return session != NULL ? &session->clipboard : NULL; }
UmiDesignerTemplateRegistry *umi_designer_builder_session_templates(UmiDesignerBuilderSession *session) { return session != NULL ? session->templates : NULL; }
UmiDesignerLivePreview *umi_designer_builder_session_live_preview(UmiDesignerBuilderSession *session) { return session != NULL ? &session->live_preview : NULL; }
const UmiDesignerGeneratedSource *umi_designer_builder_session_generated_source(const UmiDesignerBuilderSession *session) { return session != NULL ? &session->generated_source : NULL; }
