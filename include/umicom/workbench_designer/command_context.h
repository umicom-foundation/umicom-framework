/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/command_context.h
 *
 * PURPOSE:
 *   Compute command visibility and enablement from selection, clipboard,
 *   history, lock, mode and save state so every frontend enforces the same
 *   designer policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COMMAND_CONTEXT_H
#define UMICOM_WORKBENCH_DESIGNER_COMMAND_CONTEXT_H

#include "umicom/workbench_designer/clipboard.h"
#include "umicom/workbench_designer/command_history.h"
#include "umicom/workbench_designer/lock.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_COMMAND_STATES 64U

/**
 * Represent the workbench designer command state data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCommandState {
    UmiWorkbenchDesignerCommandKind kind;
    char command_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char disabled_reason[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    bool visible;
    bool enabled;
    bool checked;
} UmiWorkbenchDesignerCommandState;

/**
 * Represent the workbench designer command context data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCommandContext {
    UmiWorkbenchDesignerCommandState states[UMI_WORKBENCH_DESIGNER_MAX_COMMAND_STATES];
    size_t count;
    UmiWorkbenchDesignerMode mode;
    size_t selection_count;
    bool clipboard_empty;
    bool layout_locked;
    bool can_undo;
    bool can_redo;
    UmiWorkbenchDesignerSaveState save_state;
    uint64_t revision;
} UmiWorkbenchDesignerCommandContext;

/**
 * Initialise workbench designer command context from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_command_context_init(UmiWorkbenchDesignerCommandContext *context);
/**
 * Provide the workbench designer command context build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_command_context_build(UmiWorkbenchDesignerCommandContext *context, UmiWorkbenchDesignerMode mode, const UmiWorkbenchDesignerSelection *selection, const UmiWorkbenchDesignerClipboard *clipboard, const UmiWorkbenchDesignerCommandHistory *history, const UmiWorkbenchDesignerLockState *lock_state, UmiWorkbenchDesignerSaveState save_state);
/**
 * Find workbench designer command context while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerCommandState *umi_workbench_designer_command_context_find(const UmiWorkbenchDesignerCommandContext *context, UmiWorkbenchDesignerCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif
