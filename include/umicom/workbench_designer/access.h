/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/access.h
 *
 * PURPOSE:
 *   Combine layout lock state, persistent permissions, edit leases and
 *   administrator override into one designer access decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_ACCESS_H
#define UMICOM_WORKBENCH_DESIGNER_ACCESS_H

#include "umicom/workbench_designer/lease_view.h"
#include "umicom/workbench_layout/access_policy.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerAccessReason {
    UMI_WORKBENCH_DESIGNER_ACCESS_GRANTED = 1,
    UMI_WORKBENCH_DESIGNER_ACCESS_LAYOUT_LOCKED = 2,
    UMI_WORKBENCH_DESIGNER_ACCESS_PERMISSION_MISSING = 3,
    UMI_WORKBENCH_DESIGNER_ACCESS_LEASE_REQUIRED = 4,
    UMI_WORKBENCH_DESIGNER_ACCESS_LEASE_HELD_BY_OTHER = 5,
    UMI_WORKBENCH_DESIGNER_ACCESS_READ_ONLY = 6
} UmiWorkbenchDesignerAccessReason;

typedef struct UmiWorkbenchDesignerAccessDecision {
    bool can_view;
    bool can_edit;
    bool can_share;
    bool can_delete;
    bool can_lock;
    bool administrator_override_used;
    UmiWorkbenchDesignerAccessReason reason;
    char explanation[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
} UmiWorkbenchDesignerAccessDecision;

UmiStatus umi_workbench_designer_access_evaluate(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutRole role,
    const UmiWorkbenchDesignerLeaseView *lease,
    const char *user_id,
    const char *client_id,
    bool administrator_override,
    UmiWorkbenchDesignerAccessDecision *out_decision);

#ifdef __cplusplus
}
#endif

#endif
