/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/workflow.c
 *
 * PURPOSE:
 *   Advance Helix workflows through ordered candidate stages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The first implementation intentionally forbids skipping stages, which keeps release evidence complete and easy to reason about.
 */

#include "umicom/helix/workflow.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

/*
 * Initialise helix workflow from caller-provided values so later operations receive a
 * known state.
 */
void umi_helix_workflow_init(UmiHelixWorkflow *workflow, const char *workflow_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow != NULL) {
        (void)memset(workflow, 0, sizeof(*workflow));
        workflow->stage = UMI_HELIX_STAGE_OBSERVE;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (workflow_id != NULL) {
            (void)snprintf(workflow->workflow_id, sizeof(workflow->workflow_id),
                           "%s", workflow_id);
        }
    }
}

/*
 * Provide the helix workflow advance operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_workflow_advance(UmiHelixWorkflow *workflow,
                                     UmiHelixWorkflowStage next_stage)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL || workflow->failed) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((int)next_stage != (int)workflow->stage + 1) return UMI_STATUS_INVALID_STATE;
    workflow->stage = next_stage;
    return UMI_STATUS_OK;
}

/*
 * Provide the helix workflow stage text operation used by this module and its client
 * applications.
 */
const char *umi_helix_workflow_stage_text(UmiHelixWorkflowStage stage)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (stage) {
        case UMI_HELIX_STAGE_OBSERVE: return "observe";
        case UMI_HELIX_STAGE_DIAGNOSE: return "diagnose";
        case UMI_HELIX_STAGE_PLAN: return "plan";
        case UMI_HELIX_STAGE_BUILD: return "build";
        case UMI_HELIX_STAGE_VALIDATE: return "validate";
        case UMI_HELIX_STAGE_APPROVE: return "approve";
        case UMI_HELIX_STAGE_PROMOTE: return "promote";
        default: return "unknown";
    }
}
