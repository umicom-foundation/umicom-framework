/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/save_guard.c
 *
 * PURPOSE:
 *   Convert save lifecycle state into an explicit close decision that frontend
 *   confirmation dialogs can render consistently.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/save_guard.h"
#include "internal.h"


UmiStatus umi_workbench_designer_save_guard_evaluate(
    const UmiWorkbenchDesignerStatusModel *status,
    bool save_supported,
    bool discard_allowed,
    UmiWorkbenchDesignerSaveGuardResult *out_result)
{
    if (status == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    out_result->can_discard = discard_allowed;
    switch (status->save_state) {
        case UMI_WORKBENCH_DESIGNER_SAVE_CLEAN:
            out_result->decision = UMI_WORKBENCH_DESIGNER_CLOSE_ALLOWED;
            out_result->can_close_immediately = true;
            (void)umi_workbench_designer_copy_text(
                out_result->message, sizeof(out_result->message),
                "The layout has no unsaved changes.");
            break;
        case UMI_WORKBENCH_DESIGNER_SAVE_PENDING:
        case UMI_WORKBENCH_DESIGNER_SAVE_SAVING:
            out_result->decision = UMI_WORKBENCH_DESIGNER_CLOSE_WAIT_FOR_SAVE;
            out_result->must_preserve_recovery = true;
            (void)umi_workbench_designer_copy_text(
                out_result->message, sizeof(out_result->message),
                "A layout save is pending or currently in progress.");
            break;
        case UMI_WORKBENCH_DESIGNER_SAVE_CONFLICT:
            out_result->decision = UMI_WORKBENCH_DESIGNER_CLOSE_RESOLVE_CONFLICT;
            out_result->must_preserve_recovery = true;
            (void)umi_workbench_designer_copy_text(
                out_result->message, sizeof(out_result->message),
                "The layout has a revision conflict that must be resolved or discarded.");
            break;
        case UMI_WORKBENCH_DESIGNER_SAVE_DIRTY:
            out_result->decision = save_supported
                ? UMI_WORKBENCH_DESIGNER_CLOSE_PROMPT_SAVE
                : UMI_WORKBENCH_DESIGNER_CLOSE_CONFIRM_DISCARD;
            out_result->must_preserve_recovery = true;
            (void)umi_workbench_designer_copy_text(
                out_result->message, sizeof(out_result->message),
                save_supported
                    ? "Save changes before closing the layout?"
                    : "Saving is unavailable; confirm whether to discard changes.");
            break;
        case UMI_WORKBENCH_DESIGNER_SAVE_FAILED:
            out_result->decision = UMI_WORKBENCH_DESIGNER_CLOSE_CONFIRM_DISCARD;
            out_result->must_preserve_recovery = true;
            (void)umi_workbench_designer_copy_text(
                out_result->message, sizeof(out_result->message),
                "The most recent save failed; retry or explicitly discard changes.");
            break;
        default:
            return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
