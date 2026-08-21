/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/command_context.c
 *
 * PURPOSE:
 *   Build a durable command-state catalogue and explain why an action is
 *   unavailable instead of silently disabling toolkit controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/command_context.h"
#include "internal.h"


void umi_workbench_designer_command_context_init(
    UmiWorkbenchDesignerCommandContext *context)
{
    if (context == NULL) return;
    (void)memset(context, 0, sizeof(*context));
}

static UmiStatus command_context_add(
    UmiWorkbenchDesignerCommandContext *context,
    UmiWorkbenchDesignerCommandKind kind,
    const char *command_id,
    const char *label,
    bool visible,
    bool enabled,
    bool checked,
    const char *disabled_reason)
{
    UmiWorkbenchDesignerCommandState *state;
    if (context->count >= UMI_WORKBENCH_DESIGNER_MAX_COMMAND_STATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    state = &context->states[context->count++];
    (void)memset(state, 0, sizeof(*state));
    state->kind = kind;
    (void)umi_workbench_designer_copy_text(
        state->command_id, sizeof(state->command_id), command_id);
    (void)umi_workbench_designer_copy_text(
        state->label, sizeof(state->label), label);
    if (disabled_reason != NULL) {
        (void)umi_workbench_designer_copy_text(
            state->disabled_reason, sizeof(state->disabled_reason),
            disabled_reason);
    }
    state->visible = visible;
    state->enabled = enabled;
    state->checked = checked;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_command_context_build(
    UmiWorkbenchDesignerCommandContext *context,
    UmiWorkbenchDesignerMode mode,
    const UmiWorkbenchDesignerSelection *selection,
    const UmiWorkbenchDesignerClipboard *clipboard,
    const UmiWorkbenchDesignerCommandHistory *history,
    const UmiWorkbenchDesignerLockState *lock_state,
    UmiWorkbenchDesignerSaveState save_state)
{
    const bool has_selection = selection != NULL && selection->count > 0U;
    const bool locked = lock_state != NULL && lock_state->locked &&
        !lock_state->administrator_override;
    const bool design_mode = mode == UMI_WORKBENCH_DESIGNER_MODE_DESIGN;
    const bool mutable = design_mode && !locked;
    UmiStatus status;
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_command_context_init(context);
    context->mode = mode;
    context->selection_count = selection != NULL ? selection->count : 0U;
    context->clipboard_empty = umi_workbench_designer_clipboard_empty(clipboard);
    context->layout_locked = locked;
    context->can_undo = history != NULL && history->cursor > 0U;
    context->can_redo = history != NULL && history->cursor < history->count;
    context->save_state = save_state;
#define ADD(kind,id,label,visible,enabled,checked,reason) \
    do { status = command_context_add(context, kind, id, label, visible, enabled, checked, reason); \
         if (status != UMI_STATUS_OK) return status; } while (0)
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_ADD_PANEL,
        "designer.add-panel", "Add Panel", true, mutable, false,
        mutable ? "" : "The layout is not editable in the current mode.");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_REMOVE_NODE,
        "designer.remove", "Remove", true, mutable && has_selection, false,
        !has_selection ? "Select a layout node first." :
        locked ? "Unlock the layout before removing nodes." : "");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_MOVE_NODE,
        "designer.move", "Move", design_mode, mutable && has_selection, false,
        has_selection ? "" : "Select a layout node first.");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_UNDO,
        "designer.undo", "Undo", true, mutable && context->can_undo, false,
        context->can_undo ? "" : "There is no command to undo.");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_REDO,
        "designer.redo", "Redo", true, mutable && context->can_redo, false,
        context->can_redo ? "" : "There is no command to redo.");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_SAVE,
        "designer.save", "Save", true,
        !locked && save_state != UMI_WORKBENCH_DESIGNER_SAVE_SAVING &&
        save_state != UMI_WORKBENCH_DESIGNER_SAVE_CLEAN,
        false,
        save_state == UMI_WORKBENCH_DESIGNER_SAVE_CLEAN
            ? "The layout has no unsaved changes." : "");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_LOCK_LAYOUT,
        "designer.lock", "Lock Layout", true, !locked, locked, "");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_UNLOCK_LAYOUT,
        "designer.unlock", "Unlock Layout", true, locked, !locked, "");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_IMPORT,
        "designer.import", "Import Layout", true, !locked, false,
        locked ? "Unlock the layout before importing." : "");
    ADD(UMI_WORKBENCH_DESIGNER_COMMAND_EXPORT,
        "designer.export", "Export Layout", true, true, false, "");
#undef ADD
    context->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiWorkbenchDesignerCommandState *umi_workbench_designer_command_context_find(
    const UmiWorkbenchDesignerCommandContext *context,
    UmiWorkbenchDesignerCommandKind kind)
{
    size_t index;
    if (context == NULL) return NULL;
    for (index = 0U; index < context->count; ++index) {
        if (context->states[index].kind == kind) return &context->states[index];
    }
    return NULL;
}
