/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/lease_view.c
 *
 * PURPOSE:
 *   Implement lease presentation and expiry semantics for the designer toolbar
 *   and status area.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/lease_view.h"
#include "internal.h"


/*
 * Initialise workbench designer lease view from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_lease_view_init(UmiWorkbenchDesignerLeaseView *view)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return;
    (void)memset(view, 0, sizeof(*view));
    view->state = UMI_WORKBENCH_DESIGNER_LEASE_NONE;
}

/*
 * Provide the workbench designer lease view update operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_lease_view_update(
    UmiWorkbenchDesignerLeaseView *view,
    const char *layout_id,
    const char *holder_user_id,
    const char *holder_client_id,
    uint64_t fencing_token,
    uint64_t acquired_at_ms,
    uint64_t expires_at_ms,
    bool local_holder)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL || layout_id == NULL || holder_user_id == NULL ||
        holder_client_id == NULL || fencing_token == 0U ||
        expires_at_ms <= acquired_at_ms) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_workbench_designer_copy_text(view->layout_id, sizeof(view->layout_id), layout_id);
    (void)umi_workbench_designer_copy_text(view->holder_user_id, sizeof(view->holder_user_id), holder_user_id);
    (void)umi_workbench_designer_copy_text(view->holder_client_id, sizeof(view->holder_client_id), holder_client_id);
    view->fencing_token = fencing_token;
    view->acquired_at_ms = acquired_at_ms;
    view->expires_at_ms = expires_at_ms;
    view->state = local_holder
        ? UMI_WORKBENCH_DESIGNER_LEASE_HELD_LOCALLY
        : UMI_WORKBENCH_DESIGNER_LEASE_HELD_REMOTELY;
    view->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer lease view expire operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_lease_view_expire(
    UmiWorkbenchDesignerLeaseView *view,
    uint64_t now_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view == NULL) return;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (view->expires_at_ms != 0U && now_ms >= view->expires_at_ms &&
        view->state != UMI_WORKBENCH_DESIGNER_LEASE_EXPIRED) {
        view->state = UMI_WORKBENCH_DESIGNER_LEASE_EXPIRED;
        view->revision += 1U;
    }
}

/*
 * Provide the workbench designer lease view editable operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_lease_view_editable(
    const UmiWorkbenchDesignerLeaseView *view)
{
    return view != NULL &&
        (view->state == UMI_WORKBENCH_DESIGNER_LEASE_NONE ||
         view->state == UMI_WORKBENCH_DESIGNER_LEASE_HELD_LOCALLY);
}
