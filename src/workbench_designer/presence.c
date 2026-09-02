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


/*
 * Provide the workbench designer presence policy default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerPresencePolicy umi_workbench_designer_presence_policy_default(void)
{
    UmiWorkbenchDesignerPresencePolicy policy;
    policy.idle_after_ms = 30000U;
    policy.offline_after_ms = 90000U;
    policy.retain_offline_users = false;
    return policy;
}

/*
 * Provide the workbench designer presence heartbeat operation used by this module and its
 * client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || user_id == NULL || client_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    existing = umi_workbench_designer_collaboration_find(model, user_id, client_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing != NULL) collaborator = *existing;
    /* Use this fallback path when the earlier condition does not apply. */
    else {
        (void)memset(&collaborator, 0, sizeof(collaborator));
        (void)umi_workbench_designer_copy_text(collaborator.user_id, sizeof(collaborator.user_id), user_id);
        (void)umi_workbench_designer_copy_text(collaborator.client_id, sizeof(collaborator.client_id), client_id);
        (void)umi_workbench_designer_copy_text(collaborator.display_name, sizeof(collaborator.display_name), user_id);
        collaborator.connected_at_ms = timestamp_ms;
        collaborator.colour_index = (uint32_t)(model->count % 12U);
    }
    collaborator.active_node_id[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the workbench designer presence expire operation used by this module and its
 * client applications.
 */
size_t umi_workbench_designer_presence_expire(
    UmiWorkbenchDesignerCollaborationModel *model,
    const UmiWorkbenchDesignerPresencePolicy *policy,
    uint64_t now_ms)
{
    size_t index = 0U;
    size_t changed = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || policy == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < model->count) {
        UmiWorkbenchDesignerCollaborator *collaborator = &model->collaborators[index];
        uint64_t age = now_ms >= collaborator->last_activity_ms
            ? now_ms - collaborator->last_activity_ms : 0U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (age >= policy->offline_after_ms) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (policy->retain_offline_users) {
                /* Apply this branch only when its contract condition is satisfied. */
                if (collaborator->state != UMI_WORKBENCH_DESIGNER_COLLABORATOR_OFFLINE) {
                    collaborator->state = UMI_WORKBENCH_DESIGNER_COLLABORATOR_OFFLINE;
                    changed += 1U;
                }
                index += 1U;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                (void)umi_workbench_designer_collaboration_remove(
                    model, collaborator->user_id, collaborator->client_id);
                changed += 1U;
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (age >= policy->idle_after_ms &&
                collaborator->state != UMI_WORKBENCH_DESIGNER_COLLABORATOR_IDLE) {
                collaborator->state = UMI_WORKBENCH_DESIGNER_COLLABORATOR_IDLE;
                changed += 1U;
            }
            index += 1U;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (changed > 0U) model->revision += 1U;
    return changed;
}
