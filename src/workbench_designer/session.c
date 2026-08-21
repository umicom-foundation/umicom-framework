/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/session.c
 *
 * PURPOSE:
 *   Coordinate one complete designer session while keeping all frontend adapters
 *   as projections and command sources.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/session.h"
#include "umicom/workbench_designer/floating_editor.h"
#include "umicom/workbench_designer/lock.h"
#include "umicom/workbench_designer/split_editor.h"
#include "umicom/workbench_designer/tab_editor.h"
#include "internal.h"


struct UmiWorkbenchDesignerSession {
    char session_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerSessionConfig config;
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchLayoutHistory history;
    UmiWorkbenchDesignerCommandHistory command_history;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerViewport viewport;
    UmiWorkbenchDesignerGrid grid;
    UmiWorkbenchDesignerCanvas canvas;
    UmiWorkbenchDesignerDropTargetSet drop_targets;
    UmiWorkbenchDesignerDockingGuideModel docking_guides;
    UmiWorkbenchDesignerTree tree;
    UmiWorkbenchDesignerPalette palette;
    UmiWorkbenchDesignerPropertyModel properties;
    UmiWorkbenchDesignerClipboard clipboard;
    UmiWorkbenchDesignerResponsivePreview responsive_preview;
    UmiWorkbenchDesignerMonitorCanvas monitors;
    UmiWorkbenchDesignerCollaborationModel collaboration;
    UmiWorkbenchDesignerLeaseView lease;
    UmiWorkbenchDesignerAutosave autosave;
    UmiWorkbenchDesignerDiagnostics diagnostics;
    UmiWorkbenchDesignerKeymap keymap;
    UmiWorkbenchDesignerToolbar toolbar;
    UmiWorkbenchDesignerPanelCatalogue panel_catalogue;
    UmiWorkbenchDesignerWorkspace workspace;
    UmiWorkbenchDesignerStatusModel status;
    UmiWorkbenchDesignerMode mode;
    UmiWorkbenchDesignerTool tool;
    UmiWorkbenchDesignerPoint pointer_world;
    bool dirty;
    uint64_t revision;
};

UmiWorkbenchDesignerSessionConfig umi_workbench_designer_session_config_default(void)
{
    UmiWorkbenchDesignerSessionConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.autosave_policy = umi_workbench_designer_autosave_policy_default();
    config.initial_canvas_width = 1280.0;
    config.initial_canvas_height = 720.0;
    config.root_bounds.x = 0.0;
    config.root_bounds.y = 0.0;
    config.root_bounds.width = 1280.0;
    config.root_bounds.height = 720.0;
    config.administrator_override = false;
    return config;
}

static UmiStatus session_history_push(
    UmiWorkbenchDesignerSession *session,
    const UmiWorkbenchDesignerCommand *command)
{
    char entry_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    const char *label;
    int written;
    written = snprintf(entry_id, sizeof(entry_id), "history-%llu",
        (unsigned long long)session->document.version.revision);
    if (written < 0 || (size_t)written >= sizeof(entry_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    label = command != NULL
        ? umi_workbench_designer_command_kind_text(command->kind)
        : "document update";
    return umi_workbench_layout_history_push(
        &session->history,
        &session->document,
        entry_id,
        label,
        command != NULL && command->actor_id[0] != '\0'
            ? command->actor_id : "designer",
        command != NULL && command->correlation_id[0] != '\0'
            ? command->correlation_id : entry_id,
        command != NULL ? command->timestamp_ms : 0U,
        false);
}

static UmiStatus session_build_status(UmiWorkbenchDesignerSession *session)
{
    UmiWorkbenchDesignerLockState lock_state;
    const char *message;
    umi_workbench_designer_lock_state_build(
        &lock_state, &session->document,
        session->config.administrator_override);
    if (session->diagnostics.error_count > 0U) message = "Layout contains errors";
    else if (session->dirty) message = "Unsaved layout changes";
    else message = "Layout ready";
    return umi_workbench_designer_status_update(
        &session->status,
        message,
        session->document.name,
        session->mode,
        session->tool,
        session->dirty
            ? UMI_WORKBENCH_DESIGNER_SAVE_DIRTY
            : UMI_WORKBENCH_DESIGNER_SAVE_CLEAN,
        session->pointer_world,
        session->viewport.zoom,
        &session->selection,
        &session->collaboration,
        session->diagnostics.count,
        lock_state.locked,
        session->document.version.revision);
}

UmiStatus umi_workbench_designer_session_create(
    const char *session_id,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSessionConfig *config,
    UmiWorkbenchDesignerSession **out_session)
{
    UmiWorkbenchDesignerSession *session;
    UmiStatus status;
    if (session_id == NULL || session_id[0] == '\0' || document == NULL ||
        out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session = (UmiWorkbenchDesignerSession *)calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->config = config != NULL
        ? *config : umi_workbench_designer_session_config_default();
    status = umi_workbench_designer_copy_text(
        session->session_id, sizeof(session->session_id), session_id);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_copy(
            &session->document, document);
    }
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_layout_history_init(&session->history);
    status = umi_workbench_layout_history_seed(
        &session->history, &session->document,
        "session-open", "Session opened");
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_command_history_init(&session->command_history);
    umi_workbench_designer_selection_init(&session->selection);
    umi_workbench_designer_viewport_init(
        &session->viewport,
        session->config.initial_canvas_width,
        session->config.initial_canvas_height);
    umi_workbench_designer_grid_init(&session->grid);
    umi_workbench_designer_canvas_init(&session->canvas);
    umi_workbench_designer_drop_targets_init(&session->drop_targets);
    umi_workbench_designer_docking_guides_init(&session->docking_guides);
    umi_workbench_designer_tree_init(&session->tree);
    umi_workbench_designer_palette_init(&session->palette);
    status = umi_workbench_designer_palette_seed_framework(&session->palette);
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_property_model_init(&session->properties);
    umi_workbench_designer_clipboard_init(&session->clipboard);
    umi_workbench_designer_responsive_preview_init(&session->responsive_preview);
    status = umi_workbench_designer_responsive_preview_seed(
        &session->responsive_preview);
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_monitor_canvas_init(&session->monitors);
    umi_workbench_designer_collaboration_init(&session->collaboration);
    umi_workbench_designer_lease_view_init(&session->lease);
    umi_workbench_designer_autosave_init(
        &session->autosave, &session->config.autosave_policy);
    umi_workbench_designer_diagnostics_init(&session->diagnostics);
    umi_workbench_designer_keymap_init(&session->keymap);
    status = umi_workbench_designer_keymap_seed_defaults(&session->keymap);
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_toolbar_init(&session->toolbar);
    status = umi_workbench_designer_toolbar_seed(&session->toolbar);
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_panel_catalogue_init(&session->panel_catalogue);
    status = umi_workbench_designer_panel_catalogue_seed(
        &session->panel_catalogue);
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_workspace_init(&session->workspace);
    umi_workbench_designer_status_init(&session->status);
    session->mode = UMI_WORKBENCH_DESIGNER_MODE_DESIGN;
    session->tool = UMI_WORKBENCH_DESIGNER_TOOL_SELECT;
    session->dirty = umi_workbench_layout_document_has_flag(
        &session->document, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY);
    status = umi_workbench_designer_session_rebuild(session);
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_workbench_designer_session_destroy(UmiWorkbenchDesignerSession *session)
{
    free(session);
}

UmiStatus umi_workbench_designer_session_rebuild(
    UmiWorkbenchDesignerSession *session)
{
    const char *primary;
    UmiStatus status;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_designer_canvas_build(
        &session->canvas,
        &session->document,
        &session->viewport,
        &session->selection,
        session->config.root_bounds);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_drop_targets_build(
        &session->drop_targets, &session->canvas, NULL);
    if (status != UMI_STATUS_OK) return status;
    primary = umi_workbench_designer_selection_primary(&session->selection);
    if (primary != NULL) {
        status = umi_workbench_designer_docking_guides_build(
            &session->docking_guides, &session->drop_targets, primary);
        if (status != UMI_STATUS_OK) return status;
    } else {
        umi_workbench_designer_docking_guides_init(
            &session->docking_guides);
    }
    status = umi_workbench_designer_tree_build(
        &session->tree, &session->document, &session->selection);
    if (status != UMI_STATUS_OK) return status;
    primary = umi_workbench_designer_selection_primary(&session->selection);
    if (primary != NULL) {
        status = umi_workbench_designer_property_model_build(
            &session->properties, &session->document, primary);
        if (status != UMI_STATUS_OK) return status;
    } else {
        umi_workbench_designer_property_model_init(&session->properties);
    }
    status = umi_workbench_designer_diagnostics_build(
        &session->diagnostics,
        &session->document,
        &session->selection,
        &session->viewport,
        &session->lease,
        session->dirty
            ? UMI_WORKBENCH_DESIGNER_SAVE_DIRTY
            : UMI_WORKBENCH_DESIGNER_SAVE_CLEAN);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_workspace_build(
        &session->workspace,
        &session->panel_catalogue,
        session->mode,
        true,
        session->selection.count > 0U);
    if (status != UMI_STATUS_OK) return status;
    status = session_build_status(session);
    if (status == UMI_STATUS_OK) session->revision += 1U;
    return status;
}

static UmiStatus session_apply_structural(
    UmiWorkbenchDesignerSession *session,
    const UmiWorkbenchDesignerCommand *command,
    UmiWorkbenchDesignerCommandResult *result)
{
    UmiWorkbenchLayoutNode new_node;
    char container_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiStatus status = UMI_STATUS_NOT_IMPLEMENTED;
    umi_workbench_layout_node_init(
        &new_node,
        command->component_id[0] != '\0'
            ? command->command_id : "new-panel",
        command->node_kind != 0
            ? command->node_kind : UMI_WORKBENCH_LAYOUT_NODE_PANEL);
    (void)umi_workbench_layout_node_set_title(
        &new_node,
        command->text_value[0] != '\0'
            ? command->text_value : "New Panel");
    if (command->component_id[0] != '\0') {
        (void)umi_workbench_layout_node_set_component(
            &new_node,
            command->component_id,
            command->owner_application_id[0] != '\0'
                ? command->owner_application_id : "org.umicom.framework");
    }
    if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_HORIZONTAL ||
        command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_VERTICAL) {
        status = umi_workbench_designer_split_wrap_node(
            &session->document,
            command->target_node_id,
            &new_node,
            command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_HORIZONTAL
                ? UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL
                : UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL,
            command->number_value > 0.0 ? command->number_value : 0.5,
            command->bool_value,
            container_id,
            sizeof(container_id));
    } else if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_CREATE_TAB_GROUP) {
        status = umi_workbench_designer_tab_wrap_node(
            &session->document,
            command->target_node_id,
            &new_node,
            command->bool_value,
            container_id,
            sizeof(container_id));
    } else if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_FLOAT_NODE) {
        status = umi_workbench_designer_float_node(
            &session->document,
            command->target_node_id,
            command->parent_node_id,
            command->rect_value,
            (int32_t)command->index_value);
    } else if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_DOCK_NODE) {
        status = umi_workbench_designer_dock_node(
            &session->document,
            command->target_node_id,
            (UmiWorkbenchLayoutDockRegion)command->index_value,
            command->parent_node_id,
            0U);
    } else if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_REORDER_TAB) {
        status = umi_workbench_designer_tab_reorder(
            &session->document,
            command->parent_node_id,
            command->target_node_id,
            command->index_value);
    } else if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_SET_ACTIVE_TAB) {
        status = umi_workbench_designer_tab_activate(
            &session->document,
            command->parent_node_id,
            command->target_node_id);
    }
    result->status = status;
    result->document_changed = status == UMI_STATUS_OK;
    result->previous_revision = command->expected_revision;
    result->resulting_revision = session->document.version.revision;
    if (status == UMI_STATUS_OK) {
        (void)umi_workbench_designer_copy_text(
            result->affected_node_id,
            sizeof(result->affected_node_id),
            container_id[0] != '\0' ? container_id : command->target_node_id);
        (void)umi_workbench_designer_copy_text(
            result->message, sizeof(result->message),
            "Structural designer command applied.");
    }
    return status;
}

UmiStatus umi_workbench_designer_session_apply(
    UmiWorkbenchDesignerSession *session,
    const UmiWorkbenchDesignerCommand *command,
    UmiWorkbenchDesignerCommandResult *out_result)
{
    UmiWorkbenchDesignerLockState lock_state;
    UmiStatus status;
    bool structural;
    if (session == NULL || command == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_lock_state_build(
        &lock_state, &session->document,
        session->config.administrator_override);
    if (!umi_workbench_designer_lock_allows(&lock_state, command->kind)) {
        (void)memset(out_result, 0, sizeof(*out_result));
        out_result->status = UMI_STATUS_PERMISSION_DENIED;
        (void)umi_workbench_designer_copy_text(
            out_result->message, sizeof(out_result->message), lock_state.reason);
        return out_result->status;
    }
    structural = command->kind >= UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_HORIZONTAL &&
        command->kind <= UMI_WORKBENCH_DESIGNER_COMMAND_SET_ACTIVE_TAB;
    if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_UNDO) {
        status = umi_workbench_designer_session_undo(session);
        (void)memset(out_result, 0, sizeof(*out_result));
        out_result->status = status;
        out_result->document_changed = status == UMI_STATUS_OK;
        return status;
    }
    if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_REDO) {
        status = umi_workbench_designer_session_redo(session);
        (void)memset(out_result, 0, sizeof(*out_result));
        out_result->status = status;
        out_result->document_changed = status == UMI_STATUS_OK;
        return status;
    }
    if (structural) {
        status = session_apply_structural(session, command, out_result);
    } else {
        status = umi_workbench_designer_command_execute(
            &session->document, command, out_result);
    }
    (void)umi_workbench_designer_command_history_push(
        &session->command_history, command, out_result);
    if (status == UMI_STATUS_OK && out_result->document_changed) {
        session->dirty = true;
        (void)umi_workbench_layout_document_set_flag(
            &session->document, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY, true);
        umi_workbench_designer_autosave_mark_dirty(
            &session->autosave, command->timestamp_ms);
        status = session_history_push(session, command);
        if (status != UMI_STATUS_OK) return status;
    }
    if (status == UMI_STATUS_OK) status = umi_workbench_designer_session_rebuild(session);
    return status;
}

UmiStatus umi_workbench_designer_session_undo(UmiWorkbenchDesignerSession *session)
{
    UmiStatus status;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_history_undo(
        &session->history, &session->document);
    if (status != UMI_STATUS_OK) return status;
    session->dirty = true;
    umi_workbench_designer_selection_clear(&session->selection);
    return umi_workbench_designer_session_rebuild(session);
}

UmiStatus umi_workbench_designer_session_redo(UmiWorkbenchDesignerSession *session)
{
    UmiStatus status;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_history_redo(
        &session->history, &session->document);
    if (status != UMI_STATUS_OK) return status;
    session->dirty = true;
    umi_workbench_designer_selection_clear(&session->selection);
    return umi_workbench_designer_session_rebuild(session);
}

UmiStatus umi_workbench_designer_session_set_mode(
    UmiWorkbenchDesignerSession *session,
    UmiWorkbenchDesignerMode mode)
{
    if (session == NULL || mode < UMI_WORKBENCH_DESIGNER_MODE_BROWSE ||
        mode > UMI_WORKBENCH_DESIGNER_MODE_CONFLICT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->mode = mode;
    return umi_workbench_designer_session_rebuild(session);
}

UmiStatus umi_workbench_designer_session_set_tool(
    UmiWorkbenchDesignerSession *session,
    UmiWorkbenchDesignerTool tool)
{
    if (session == NULL || tool < UMI_WORKBENCH_DESIGNER_TOOL_SELECT ||
        tool > UMI_WORKBENCH_DESIGNER_TOOL_LINK_CONTEXT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->tool = tool;
    session->revision += 1U;
    return session_build_status(session);
}

UmiStatus umi_workbench_designer_session_select(
    UmiWorkbenchDesignerSession *session,
    const char *node_id,
    bool extend,
    bool toggle)
{
    UmiStatus status;
    if (session == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_workbench_layout_document_find_node(&session->document, node_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (!extend) umi_workbench_designer_selection_clear(&session->selection);
    status = toggle
        ? umi_workbench_designer_selection_toggle(&session->selection, node_id, true)
        : umi_workbench_designer_selection_add(&session->selection, node_id, true);
    if (status == UMI_STATUS_OK) status = umi_workbench_designer_session_rebuild(session);
    return status;
}

void umi_workbench_designer_session_clear_selection(
    UmiWorkbenchDesignerSession *session)
{
    if (session == NULL) return;
    umi_workbench_designer_selection_clear(&session->selection);
    (void)umi_workbench_designer_session_rebuild(session);
}

const char *umi_workbench_designer_session_id(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? session->session_id : NULL; }
UmiWorkbenchLayoutDocument *umi_workbench_designer_session_document(UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->document : NULL; }
const UmiWorkbenchLayoutDocument *umi_workbench_designer_session_document_const(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->document : NULL; }
const UmiWorkbenchDesignerSelection *umi_workbench_designer_session_selection(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->selection : NULL; }
const UmiWorkbenchDesignerCanvas *umi_workbench_designer_session_canvas(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->canvas : NULL; }
const UmiWorkbenchDesignerViewport *umi_workbench_designer_session_viewport(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->viewport : NULL; }
const UmiWorkbenchDesignerPalette *umi_workbench_designer_session_palette(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->palette : NULL; }
const UmiWorkbenchDesignerCollaborationModel *umi_workbench_designer_session_collaboration(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->collaboration : NULL; }
const UmiWorkbenchDesignerDropTargetSet *umi_workbench_designer_session_drop_targets(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->drop_targets : NULL; }
const UmiWorkbenchDesignerDockingGuideModel *umi_workbench_designer_session_docking_guides(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->docking_guides : NULL; }
const UmiWorkbenchDesignerTree *umi_workbench_designer_session_tree(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->tree : NULL; }
const UmiWorkbenchDesignerPropertyModel *umi_workbench_designer_session_properties(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->properties : NULL; }
const UmiWorkbenchDesignerDiagnostics *umi_workbench_designer_session_diagnostics(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->diagnostics : NULL; }
const UmiWorkbenchDesignerStatusModel *umi_workbench_designer_session_status(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->status : NULL; }
UmiWorkbenchDesignerAutosave *umi_workbench_designer_session_autosave(UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->autosave : NULL; }
UmiWorkbenchDesignerMode umi_workbench_designer_session_mode(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? session->mode : UMI_WORKBENCH_DESIGNER_MODE_BROWSE; }
UmiWorkbenchDesignerTool umi_workbench_designer_session_tool(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? session->tool : UMI_WORKBENCH_DESIGNER_TOOL_SELECT; }
bool umi_workbench_designer_session_dirty(const UmiWorkbenchDesignerSession *session)
{ return session != NULL && session->dirty; }
uint64_t umi_workbench_designer_session_revision(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? session->revision : 0U; }
