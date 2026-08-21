/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/access.c
 *
 * PURPOSE:
 *   Evaluate the complete edit boundary before the visual designer enables
 *   mutating actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/access.h"
#include "internal.h"


UmiStatus umi_workbench_designer_access_evaluate(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutRole role,
    const UmiWorkbenchDesignerLeaseView *lease,
    const char *user_id,
    const char *client_id,
    bool administrator_override,
    UmiWorkbenchDesignerAccessDecision *out_decision)
{
    const bool administrator = role == UMI_WORKBENCH_LAYOUT_ROLE_ADMINISTRATOR;
    const bool owner = role == UMI_WORKBENCH_LAYOUT_ROLE_OWNER;
    const bool editor = role == UMI_WORKBENCH_LAYOUT_ROLE_EDITOR;
    const bool locked = umi_workbench_layout_document_has_flag(
        document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED);
    const bool read_only = umi_workbench_layout_document_has_flag(
        document, UMI_WORKBENCH_LAYOUT_DOCUMENT_READ_ONLY);
    if (document == NULL || user_id == NULL || client_id == NULL ||
        out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->can_view = true;
    out_decision->can_share = administrator || owner;
    out_decision->can_delete = administrator || owner;
    out_decision->can_lock = administrator || owner;
    if (administrator_override && administrator) {
        out_decision->can_edit = true;
        out_decision->administrator_override_used = true;
        out_decision->reason = UMI_WORKBENCH_DESIGNER_ACCESS_GRANTED;
        (void)umi_workbench_designer_copy_text(
            out_decision->explanation, sizeof(out_decision->explanation),
            "Administrator override grants temporary designer edit authority.");
        return UMI_STATUS_OK;
    }
    if (read_only) {
        out_decision->reason = UMI_WORKBENCH_DESIGNER_ACCESS_READ_ONLY;
        (void)umi_workbench_designer_copy_text(
            out_decision->explanation, sizeof(out_decision->explanation),
            "The layout document is marked read-only.");
        return UMI_STATUS_OK;
    }
    if (locked && !(administrator || owner)) {
        out_decision->reason = UMI_WORKBENCH_DESIGNER_ACCESS_LAYOUT_LOCKED;
        (void)umi_workbench_designer_copy_text(
            out_decision->explanation, sizeof(out_decision->explanation),
            "The layout is locked and the user cannot unlock it.");
        return UMI_STATUS_OK;
    }
    if (!(administrator || owner || editor)) {
        out_decision->reason = UMI_WORKBENCH_DESIGNER_ACCESS_PERMISSION_MISSING;
        (void)umi_workbench_designer_copy_text(
            out_decision->explanation, sizeof(out_decision->explanation),
            "The current role does not grant layout editing permission.");
        return UMI_STATUS_OK;
    }
    if (lease != NULL) {
        if (lease->state == UMI_WORKBENCH_DESIGNER_LEASE_NONE ||
            lease->state == UMI_WORKBENCH_DESIGNER_LEASE_EXPIRED ||
            lease->state == UMI_WORKBENCH_DESIGNER_LEASE_REJECTED) {
            out_decision->reason = UMI_WORKBENCH_DESIGNER_ACCESS_LEASE_REQUIRED;
            (void)umi_workbench_designer_copy_text(
                out_decision->explanation, sizeof(out_decision->explanation),
                "An active edit lease is required before modifying this layout.");
            return UMI_STATUS_OK;
        }
        if (lease->state != UMI_WORKBENCH_DESIGNER_LEASE_HELD_LOCALLY ||
            strcmp(lease->holder_user_id, user_id) != 0 ||
            strcmp(lease->holder_client_id, client_id) != 0) {
            out_decision->reason =
                UMI_WORKBENCH_DESIGNER_ACCESS_LEASE_HELD_BY_OTHER;
            (void)umi_workbench_designer_copy_text(
                out_decision->explanation, sizeof(out_decision->explanation),
                "Another user or client currently holds the layout edit lease.");
            return UMI_STATUS_OK;
        }
    }
    out_decision->can_edit = true;
    out_decision->reason = UMI_WORKBENCH_DESIGNER_ACCESS_GRANTED;
    (void)umi_workbench_designer_copy_text(
        out_decision->explanation, sizeof(out_decision->explanation),
        "Layout editing is permitted by role, lock state and lease authority.");
    return UMI_STATUS_OK;
}
