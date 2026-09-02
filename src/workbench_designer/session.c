/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/session.c
 *
 * PURPOSE:
 *   Coordinate one complete designer session while keeping all frontend adapters
 *   as projections and command sources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Provide the workbench designer session config default operation used by this module and
 * its client applications.
 */
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

/*
 * Provide the session history push operation used by this module and its client
 * applications.
 */
static UmiStatus session_history_push(
    UmiWorkbenchDesignerSession *session,
    const UmiWorkbenchDesignerCommand *command)
{
    char entry_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    const char *label;
    int written;
    written = snprintf(entry_id, sizeof(entry_id), "history-%llu",
        (unsigned long long)session->document.version.revision);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the session build status operation used by this module and its client
 * applications.
 */
static UmiStatus session_build_status(UmiWorkbenchDesignerSession *session)
{
    UmiWorkbenchDesignerLockState lock_state;
    const char *message;
    umi_workbench_designer_lock_state_build(
        &lock_state, &session->document,
        session->config.administrator_override);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->diagnostics.error_count > 0U) message = "Layout contains errors";
    else /* Apply this branch only when its contract condition is satisfied. */ if (session->dirty) message = "Unsaved layout changes";
    /* Use this fallback path when the earlier condition does not apply. */
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

/*
 * Initialise workbench designer session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_designer_session_create(
    const char *session_id,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSessionConfig *config,
    UmiWorkbenchDesignerSession **out_session)
{
    UmiWorkbenchDesignerSession *session;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session_id == NULL || session_id[0] == '\0' || document == NULL ||
        out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session = (UmiWorkbenchDesignerSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->config = config != NULL
        ? *config : umi_workbench_designer_session_config_default();
    status = umi_workbench_designer_copy_text(
        session->session_id, sizeof(session->session_id), session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_copy(
            &session->document, document);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_layout_history_init(&session->history);
    status = umi_workbench_layout_history_seed(
        &session->history, &session->document,
        "session-open", "Session opened");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_property_model_init(&session->properties);
    umi_workbench_designer_clipboard_init(&session->clipboard);
    umi_workbench_designer_responsive_preview_init(&session->responsive_preview);
    status = umi_workbench_designer_responsive_preview_seed(
        &session->responsive_preview);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_toolbar_init(&session->toolbar);
    status = umi_workbench_designer_toolbar_seed(&session->toolbar);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    umi_workbench_designer_panel_catalogue_init(&session->panel_catalogue);
    status = umi_workbench_designer_panel_catalogue_seed(
        &session->panel_catalogue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by workbench designer session so the same storage can be
 * reused safely.
 */
void umi_workbench_designer_session_destroy(UmiWorkbenchDesignerSession *session)
{
    free(session);
}

/*
 * Provide the workbench designer session rebuild operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_session_rebuild(
    UmiWorkbenchDesignerSession *session)
{
    const char *primary;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_designer_canvas_build(
        &session->canvas,
        &session->document,
        &session->viewport,
        &session->selection,
        session->config.root_bounds);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_drop_targets_build(
        &session->drop_targets, &session->canvas, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    primary = umi_workbench_designer_selection_primary(&session->selection);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (primary != NULL) {
        status = umi_workbench_designer_docking_guides_build(
            &session->docking_guides, &session->drop_targets, primary);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        umi_workbench_designer_docking_guides_init(
            &session->docking_guides);
    }
    status = umi_workbench_designer_tree_build(
        &session->tree, &session->document, &session->selection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    primary = umi_workbench_designer_selection_primary(&session->selection);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (primary != NULL) {
        status = umi_workbench_designer_property_model_build(
            &session->properties, &session->document, primary);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_designer_workspace_build(
        &session->workspace,
        &session->panel_catalogue,
        session->mode,
        true,
        session->selection.count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = session_build_status(session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) session->revision += 1U;
    return status;
}

/*
 * Provide the session apply structural operation used by this module and its client
 * applications.
 */
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
    /* Use the shared build helper when it is available from the parent composition. */
    if (command->component_id[0] != '\0') {
        (void)umi_workbench_layout_node_set_component(
            &new_node,
            command->component_id,
            command->owner_application_id[0] != '\0'
                ? command->owner_application_id : "org.umicom.framework");
    }
    /* Use the shared build helper when it is available from the parent composition. */
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
    } else /* Apply this branch only when its contract condition is satisfied. */ if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_CREATE_TAB_GROUP) {
        status = umi_workbench_designer_tab_wrap_node(
            &session->document,
            command->target_node_id,
            &new_node,
            command->bool_value,
            container_id,
            sizeof(container_id));
    } else /* Apply this branch only when its contract condition is satisfied. */ if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_FLOAT_NODE) {
        status = umi_workbench_designer_float_node(
            &session->document,
            command->target_node_id,
            command->parent_node_id,
            command->rect_value,
            (int32_t)command->index_value);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_DOCK_NODE) {
        status = umi_workbench_designer_dock_node(
            &session->document,
            command->target_node_id,
            (UmiWorkbenchLayoutDockRegion)command->index_value,
            command->parent_node_id,
            0U);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_REORDER_TAB) {
        status = umi_workbench_designer_tab_reorder(
            &session->document,
            command->parent_node_id,
            command->target_node_id,
            command->index_value);
    } else /* Apply this operation only while the related capability or state is available. */ if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_SET_ACTIVE_TAB) {
        status = umi_workbench_designer_tab_activate(
            &session->document,
            command->parent_node_id,
            command->target_node_id);
    }
    result->status = status;
    result->document_changed = status == UMI_STATUS_OK;
    result->previous_revision = command->expected_revision;
    result->resulting_revision = session->document.version.revision;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Perform workbench designer session through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_designer_session_apply(
    UmiWorkbenchDesignerSession *session,
    const UmiWorkbenchDesignerCommand *command,
    UmiWorkbenchDesignerCommandResult *out_result)
{
    UmiWorkbenchDesignerLockState lock_state;
    UmiStatus status;
    bool structural;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || command == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_lock_state_build(
        &lock_state, &session->document,
        session->config.administrator_override);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_designer_lock_allows(&lock_state, command->kind)) {
        (void)memset(out_result, 0, sizeof(*out_result));
        out_result->status = UMI_STATUS_PERMISSION_DENIED;
        (void)umi_workbench_designer_copy_text(
            out_result->message, sizeof(out_result->message), lock_state.reason);
        return out_result->status;
    }
    structural = command->kind >= UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_HORIZONTAL &&
        command->kind <= UMI_WORKBENCH_DESIGNER_COMMAND_SET_ACTIVE_TAB;
    /* Use the shared build helper when it is available from the parent composition. */
    if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_UNDO) {
        status = umi_workbench_designer_session_undo(session);
        (void)memset(out_result, 0, sizeof(*out_result));
        out_result->status = status;
        out_result->document_changed = status == UMI_STATUS_OK;
        return status;
    }
    /* Use the shared build helper when it is available from the parent composition. */
    if (command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_REDO) {
        status = umi_workbench_designer_session_redo(session);
        (void)memset(out_result, 0, sizeof(*out_result));
        out_result->status = status;
        out_result->document_changed = status == UMI_STATUS_OK;
        return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (structural) {
        status = session_apply_structural(session, command, out_result);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_workbench_designer_command_execute(
            &session->document, command, out_result);
    }
    (void)umi_workbench_designer_command_history_push(
        &session->command_history, command, out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && out_result->document_changed) {
        session->dirty = true;
        (void)umi_workbench_layout_document_set_flag(
            &session->document, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY, true);
        umi_workbench_designer_autosave_mark_dirty(
            &session->autosave, command->timestamp_ms);
        status = session_history_push(session, command);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_designer_session_rebuild(session);
    return status;
}

/*
 * Provide the workbench designer session undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_session_undo(UmiWorkbenchDesignerSession *session)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_history_undo(
        &session->history, &session->document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->dirty = true;
    umi_workbench_designer_selection_clear(&session->selection);
    return umi_workbench_designer_session_rebuild(session);
}

/*
 * Provide the workbench designer session redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_session_redo(UmiWorkbenchDesignerSession *session)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_layout_history_redo(
        &session->history, &session->document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->dirty = true;
    umi_workbench_designer_selection_clear(&session->selection);
    return umi_workbench_designer_session_rebuild(session);
}

/*
 * Provide the workbench designer session set mode operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_session_set_mode(
    UmiWorkbenchDesignerSession *session,
    UmiWorkbenchDesignerMode mode)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || mode < UMI_WORKBENCH_DESIGNER_MODE_BROWSE ||
        mode > UMI_WORKBENCH_DESIGNER_MODE_CONFLICT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->mode = mode;
    return umi_workbench_designer_session_rebuild(session);
}

/*
 * Provide the workbench designer session set tool operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_session_set_tool(
    UmiWorkbenchDesignerSession *session,
    UmiWorkbenchDesignerTool tool)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || tool < UMI_WORKBENCH_DESIGNER_TOOL_SELECT ||
        tool > UMI_WORKBENCH_DESIGNER_TOOL_LINK_CONTEXT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->tool = tool;
    session->revision += 1U;
    return session_build_status(session);
}

/*
 * Provide the workbench designer session select operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_session_select(
    UmiWorkbenchDesignerSession *session,
    const char *node_id,
    bool extend,
    bool toggle)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || node_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_workbench_layout_document_find_node(&session->document, node_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!extend) umi_workbench_designer_selection_clear(&session->selection);
    status = toggle
        ? umi_workbench_designer_selection_toggle(&session->selection, node_id, true)
        : umi_workbench_designer_selection_add(&session->selection, node_id, true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_designer_session_rebuild(session);
    return status;
}

/*
 * Provide the workbench designer session clear selection operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_session_clear_selection(
    UmiWorkbenchDesignerSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    umi_workbench_designer_selection_clear(&session->selection);
    (void)umi_workbench_designer_session_rebuild(session);
}

/*
 * Provide the workbench designer session id operation used by this module and its client
 * applications.
 */
const char *umi_workbench_designer_session_id(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? session->session_id : NULL; }
/*
 * Provide the workbench designer session document operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutDocument *umi_workbench_designer_session_document(UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->document : NULL; }
/*
 * Provide the workbench designer session document const operation used by this module and
 * its client applications.
 */
const UmiWorkbenchLayoutDocument *umi_workbench_designer_session_document_const(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->document : NULL; }
/*
 * Provide the workbench designer session selection operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerSelection *umi_workbench_designer_session_selection(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->selection : NULL; }
/*
 * Provide the workbench designer session canvas operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerCanvas *umi_workbench_designer_session_canvas(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->canvas : NULL; }
/*
 * Provide the workbench designer session viewport operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerViewport *umi_workbench_designer_session_viewport(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->viewport : NULL; }
/*
 * Provide the workbench designer session palette operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerPalette *umi_workbench_designer_session_palette(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->palette : NULL; }
/*
 * Provide the workbench designer session collaboration operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerCollaborationModel *umi_workbench_designer_session_collaboration(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->collaboration : NULL; }
/*
 * Provide the workbench designer session drop targets operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerDropTargetSet *umi_workbench_designer_session_drop_targets(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->drop_targets : NULL; }
/*
 * Provide the workbench designer session docking guides operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerDockingGuideModel *umi_workbench_designer_session_docking_guides(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->docking_guides : NULL; }
/*
 * Provide the workbench designer session tree operation used by this module and its client
 * applications.
 */
const UmiWorkbenchDesignerTree *umi_workbench_designer_session_tree(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->tree : NULL; }
/*
 * Provide the workbench designer session properties operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerPropertyModel *umi_workbench_designer_session_properties(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->properties : NULL; }
/*
 * Provide the workbench designer session diagnostics operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerDiagnostics *umi_workbench_designer_session_diagnostics(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->diagnostics : NULL; }
/*
 * Provide the workbench designer session status operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerStatusModel *umi_workbench_designer_session_status(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->status : NULL; }
/*
 * Provide the workbench designer session autosave operation used by this module and its
 * client applications.
 */
UmiWorkbenchDesignerAutosave *umi_workbench_designer_session_autosave(UmiWorkbenchDesignerSession *session)
{ return session != NULL ? &session->autosave : NULL; }
/*
 * Provide the workbench designer session mode operation used by this module and its client
 * applications.
 */
UmiWorkbenchDesignerMode umi_workbench_designer_session_mode(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? session->mode : UMI_WORKBENCH_DESIGNER_MODE_BROWSE; }
/*
 * Provide the workbench designer session tool operation used by this module and its client
 * applications.
 */
UmiWorkbenchDesignerTool umi_workbench_designer_session_tool(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? session->tool : UMI_WORKBENCH_DESIGNER_TOOL_SELECT; }
/*
 * Provide the workbench designer session dirty operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_session_dirty(const UmiWorkbenchDesignerSession *session)
{ return session != NULL && session->dirty; }
/*
 * Provide the workbench designer session revision operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_designer_session_revision(const UmiWorkbenchDesignerSession *session)
{ return session != NULL ? session->revision : 0U; }
