/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/presence.h
 *
 * PURPOSE:
 *   Apply heartbeat expiry and active-node updates to the collaboration
 *   projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PRESENCE_H
#define UMICOM_WORKBENCH_DESIGNER_PRESENCE_H

#include "umicom/workbench_designer/collaboration.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerPresencePolicy {
    uint64_t idle_after_ms;
    uint64_t offline_after_ms;
    bool retain_offline_users;
} UmiWorkbenchDesignerPresencePolicy;

UmiWorkbenchDesignerPresencePolicy umi_workbench_designer_presence_policy_default(void);
UmiStatus umi_workbench_designer_presence_heartbeat(UmiWorkbenchDesignerCollaborationModel *model, const char *user_id, const char *client_id, const char *active_node_id, bool editing, uint64_t timestamp_ms);
size_t umi_workbench_designer_presence_expire(UmiWorkbenchDesignerCollaborationModel *model, const UmiWorkbenchDesignerPresencePolicy *policy, uint64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
