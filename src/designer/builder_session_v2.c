/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/builder_session_v2.c
 *
 * PURPOSE:
 *   Compose all Visual Application Builder v2 services into one owned session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/builder_session_v2.h"

#include "umicom/designer/catalogue.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerBuilderSessionV2 {
    UmiDeclComponentRegistry *components;
    UmiDeclSchema schema;
    UmiDesignerDocument *document;
    UmiDesignerHistory *history;
    UmiDesignerTransactionHistoryV2 *transactions;
    UmiDesignerSelection selection;
    UmiDesignerBindingEditorV2 *bindings;
    UmiDesignerInteractionEditorV2 *interactions;
    UmiDesignerClipboardV2 clipboard;
    UmiDesignerTemplateRegistryV2 *templates;
    UmiDesignerLivePreviewV2 live_preview;
    UmiDesignerGeneratedSourceV2 generated_source;
};

static UmiStatus register_default_templates(UmiDesignerTemplateRegistryV2 *registry)
{
    UmiDesignerTemplateV2 item;
    UmiDeclNode panel;
    UmiDeclNode title;
    UmiDeclNode action;
    UmiStatus status;
    status = umi_designer_template_v2_init(&item, "form-card", "Form card", "Forms");
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&panel, "card", "pane", "-");
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&title, "heading", "label", "card");
    if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(&title, "title", UMI_DECL_VALUE_STRING, "New form");
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&action, "submit", "button", "card");
    if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(&action, "title", UMI_DECL_VALUE_STRING, "Submit");
    if (status == UMI_STATUS_OK) status = umi_designer_template_v2_add_node(&item, &panel);
    if (status == UMI_STATUS_OK) status = umi_designer_template_v2_add_node(&item, &title);
    if (status == UMI_STATUS_OK) status = umi_designer_template_v2_add_node(&item, &action);
    if (status == UMI_STATUS_OK) status = umi_designer_template_registry_v2_register(registry, &item);
    return status;
}

UmiStatus umi_designer_builder_session_v2_create(const char *application_id,
                                                  UmiDesignerBuilderSessionV2 **out_session)
{
    UmiDesignerBuilderSessionV2 *session;
    UmiDeclNode root;
    UmiStatus status;
    if (application_id == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = (UmiDesignerBuilderSessionV2 *)calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_designer_catalogue_create(&session->components, &session->schema);
    if (status == UMI_STATUS_OK) status = umi_designer_document_create(application_id, &session->document);
    if (status == UMI_STATUS_OK) status = umi_decl_node_init(&root, "root", "window", "-");
    if (status == UMI_STATUS_OK) status = umi_decl_document_add_node(umi_designer_document_declarative(session->document), &root);
    if (status == UMI_STATUS_OK) status = umi_designer_history_create(session->document, &session->history);
    if (status == UMI_STATUS_OK) status = umi_designer_transaction_history_v2_create(session->document, &session->transactions);
    if (status == UMI_STATUS_OK) status = umi_designer_binding_editor_v2_create(&session->bindings);
    if (status == UMI_STATUS_OK) status = umi_designer_interaction_editor_v2_create(&session->interactions);
    if (status == UMI_STATUS_OK) status = umi_designer_template_registry_v2_create(&session->templates);
    if (status == UMI_STATUS_OK) status = register_default_templates(session->templates);
    if (status == UMI_STATUS_OK) status = umi_designer_selection_set_primary(&session->selection, "root");
    umi_designer_clipboard_v2_init(&session->clipboard);
    umi_designer_live_preview_v2_init(&session->live_preview);
    if (status != UMI_STATUS_OK) {
        umi_designer_builder_session_v2_destroy(session);
        return status;
    }
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_designer_builder_session_v2_destroy(UmiDesignerBuilderSessionV2 *session)
{
    if (session == NULL) return;
    umi_designer_template_registry_v2_destroy(session->templates);
    umi_designer_interaction_editor_v2_destroy(session->interactions);
    umi_designer_binding_editor_v2_destroy(session->bindings);
    umi_designer_transaction_history_v2_destroy(session->transactions);
    umi_designer_history_destroy(session->history);
    umi_designer_document_destroy(session->document);
    umi_decl_component_registry_destroy(session->components);
    free(session);
}

UmiStatus umi_designer_builder_session_v2_add_component(UmiDesignerBuilderSessionV2 *session,
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
    if (status == UMI_STATUS_OK) umi_designer_live_preview_v2_mark_stale(&session->live_preview);
    return status;
}

UmiStatus umi_designer_builder_session_v2_select(UmiDesignerBuilderSessionV2 *session,
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

UmiStatus umi_designer_builder_session_v2_tree(const UmiDesignerBuilderSessionV2 *session,
                                                UmiDesignerComponentTreeV2 *out_tree)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_component_tree_v2_build(session->document, &session->selection, out_tree);
}

UmiStatus umi_designer_builder_session_v2_preview(UmiDesignerBuilderSessionV2 *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_live_preview_v2_refresh(&session->live_preview, session->document, &session->schema);
}

UmiStatus umi_designer_builder_session_v2_generate(UmiDesignerBuilderSessionV2 *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_designer_source_generation_v2_generate(session->document, &session->generated_source);
}

UmiStatus umi_designer_builder_session_v2_snapshot(const UmiDesignerBuilderSessionV2 *session,
                                                    UmiDesignerBuilderSessionSnapshotV2 *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)umi_designer_document_snapshot(session->document, &out_snapshot->document);
    out_snapshot->selected_count = session->selection.count;
    out_snapshot->binding_count = umi_designer_binding_editor_v2_count(session->bindings);
    out_snapshot->interaction_count = umi_designer_interaction_editor_v2_count(session->interactions);
    out_snapshot->template_count = umi_designer_template_registry_v2_count(session->templates);
    out_snapshot->undo_count = umi_designer_transaction_history_v2_undo_count(session->transactions);
    out_snapshot->redo_count = umi_designer_transaction_history_v2_redo_count(session->transactions);
    out_snapshot->preview_health = session->live_preview.health;
    out_snapshot->generated_revision = session->generated_source.source_revision;
    return UMI_STATUS_OK;
}

UmiDesignerDocument *umi_designer_builder_session_v2_document(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? session->document : NULL; }
UmiDesignerHistory *umi_designer_builder_session_v2_history(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? session->history : NULL; }
UmiDesignerTransactionHistoryV2 *umi_designer_builder_session_v2_transactions(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? session->transactions : NULL; }
UmiDesignerSelection *umi_designer_builder_session_v2_selection(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? &session->selection : NULL; }
UmiDeclComponentRegistry *umi_designer_builder_session_v2_components(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? session->components : NULL; }
UmiDeclSchema *umi_designer_builder_session_v2_schema(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? &session->schema : NULL; }
UmiDesignerBindingEditorV2 *umi_designer_builder_session_v2_bindings(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? session->bindings : NULL; }
UmiDesignerInteractionEditorV2 *umi_designer_builder_session_v2_interactions(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? session->interactions : NULL; }
UmiDesignerClipboardV2 *umi_designer_builder_session_v2_clipboard(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? &session->clipboard : NULL; }
UmiDesignerTemplateRegistryV2 *umi_designer_builder_session_v2_templates(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? session->templates : NULL; }
UmiDesignerLivePreviewV2 *umi_designer_builder_session_v2_live_preview(UmiDesignerBuilderSessionV2 *session) { return session != NULL ? &session->live_preview : NULL; }
const UmiDesignerGeneratedSourceV2 *umi_designer_builder_session_v2_generated_source(const UmiDesignerBuilderSessionV2 *session) { return session != NULL ? &session->generated_source : NULL; }
