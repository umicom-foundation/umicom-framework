/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/presence.c
 *
 * PURPOSE:
 *   Implement heartbeat, idle and offline transitions for collaborative editing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/presence.h"
#include "internal.h"


UmiWorkbenchDesignerPresencePolicy umi_workbench_designer_presence_policy_default(void)
{
    UmiWorkbenchDesignerPresencePolicy policy;
    policy.idle_after_ms = 30000U;
    policy.offline_after_ms = 90000U;
    policy.retain_offline_users = false;
    return policy;
}

UmiStatus umi_workbench_designer_presence_heartbeat(
    UmiWorkbenchDesignerCollaborationModel *model,
    const char *user_id,
    const char *client_id,
    const char *active_node_id,
    bool editing,
    uint64_t timestamp_ms)
{
    UmiWorkbenchDesignerCollaborator collaborator;
    const UmiWorkbenchDesignerCollaborator *existing;
    if (model == NULL || user_id == NULL || client_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    existing = umi_workbench_designer_collaboration_find(model, user_id, client_id);
    if (existing != NULL) collaborator = *existing;
    else {
        (void)memset(&collaborator, 0, sizeof(collaborator));
        (void)umi_workbench_designer_copy_text(collaborator.user_id, sizeof(collaborator.user_id), user_id);
        (void)umi_workbench_designer_copy_text(collaborator.client_id, sizeof(collaborator.client_id), client_id);
        (void)umi_workbench_designer_copy_text(collaborator.display_name, sizeof(collaborator.display_name), user_id);
        collaborator.connected_at_ms = timestamp_ms;
        collaborator.colour_index = (uint32_t)(model->count % 12U);
    }
    collaborator.active_node_id[0] = '\0';
    if (active_node_id != NULL) {
        (void)umi_workbench_designer_copy_text(
            collaborator.active_node_id,
            sizeof(collaborator.active_node_id), active_node_id);
    }
    collaborator.state = editing
        ? UMI_WORKBENCH_DESIGNER_COLLABORATOR_EDITING
        : UMI_WORKBENCH_DESIGNER_COLLABORATOR_VIEWING;
    collaborator.last_activity_ms = timestamp_ms;
    return umi_workbench_designer_collaboration_upsert(model, &collaborator);
}

size_t umi_workbench_designer_presence_expire(
    UmiWorkbenchDesignerCollaborationModel *model,
    const UmiWorkbenchDesignerPresencePolicy *policy,
    uint64_t now_ms)
{
    size_t index = 0U;
    size_t changed = 0U;
    if (model == NULL || policy == NULL) return 0U;
    while (index < model->count) {
        UmiWorkbenchDesignerCollaborator *collaborator = &model->collaborators[index];
        uint64_t age = now_ms >= collaborator->last_activity_ms
            ? now_ms - collaborator->last_activity_ms : 0U;
        if (age >= policy->offline_after_ms) {
            if (policy->retain_offline_users) {
                if (collaborator->state != UMI_WORKBENCH_DESIGNER_COLLABORATOR_OFFLINE) {
                    collaborator->state = UMI_WORKBENCH_DESIGNER_COLLABORATOR_OFFLINE;
                    changed += 1U;
                }
                index += 1U;
            } else {
                (void)umi_workbench_designer_collaboration_remove(
                    model, collaborator->user_id, collaborator->client_id);
                changed += 1U;
            }
        } else {
            if (age >= policy->idle_after_ms &&
                collaborator->state != UMI_WORKBENCH_DESIGNER_COLLABORATOR_IDLE) {
                collaborator->state = UMI_WORKBENCH_DESIGNER_COLLABORATOR_IDLE;
                changed += 1U;
            }
            index += 1U;
        }
    }
    if (changed > 0U) model->revision += 1U;
    return changed;
}
