/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/lease_view.h
 *
 * PURPOSE:
 *   Present edit-lease ownership, expiry and fencing evidence without exposing
 *   persistence backend records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_LEASE_VIEW_H
#define UMICOM_WORKBENCH_DESIGNER_LEASE_VIEW_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerLeaseState {
    UMI_WORKBENCH_DESIGNER_LEASE_NONE = 1,
    UMI_WORKBENCH_DESIGNER_LEASE_HELD_LOCALLY = 2,
    UMI_WORKBENCH_DESIGNER_LEASE_HELD_REMOTELY = 3,
    UMI_WORKBENCH_DESIGNER_LEASE_RENEWING = 4,
    UMI_WORKBENCH_DESIGNER_LEASE_EXPIRED = 5,
    UMI_WORKBENCH_DESIGNER_LEASE_REJECTED = 6
} UmiWorkbenchDesignerLeaseState;

typedef struct UmiWorkbenchDesignerLeaseView {
    UmiWorkbenchDesignerLeaseState state;
    char layout_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char holder_user_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char holder_client_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t fencing_token;
    uint64_t acquired_at_ms;
    uint64_t expires_at_ms;
    uint64_t revision;
} UmiWorkbenchDesignerLeaseView;

void umi_workbench_designer_lease_view_init(UmiWorkbenchDesignerLeaseView *view);
UmiStatus umi_workbench_designer_lease_view_update(UmiWorkbenchDesignerLeaseView *view, const char *layout_id, const char *holder_user_id, const char *holder_client_id, uint64_t fencing_token, uint64_t acquired_at_ms, uint64_t expires_at_ms, bool local_holder);
void umi_workbench_designer_lease_view_expire(UmiWorkbenchDesignerLeaseView *view, uint64_t now_ms);
bool umi_workbench_designer_lease_view_editable(const UmiWorkbenchDesignerLeaseView *view);

#ifdef __cplusplus
}
#endif

#endif
