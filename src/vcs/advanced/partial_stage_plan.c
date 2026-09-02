/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/partial_stage_plan.c
 *
 * PURPOSE:
 *   Build reviewable partial-stage plans from selected diff hunks.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/partial_stage_plan.h"
#include <string.h>
/*
 * Initialise vcs advanced partial stage plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_partial_stage_plan_init(UmiVcsAdvancedPartialStagePlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size = (uint32_t)sizeof(*plan);
    plan->api_version = UMI_VCS_ADVANCED_API_VERSION;
    plan->check_only = 1;
}
/*
 * Provide the vcs advanced partial stage plan set root operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_partial_stage_plan_set_root(UmiVcsAdvancedPartialStagePlan *plan,
                                                        const char *root)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_vcs_advanced_copy_text(plan->repository_root, sizeof(plan->repository_root), root);
}
/*
 * Add vcs advanced partial stage plan only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_vcs_advanced_partial_stage_plan_add(UmiVcsAdvancedPartialStagePlan *plan,
                                                   const UmiVcsAdvancedStagingHunk *hunk)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || umi_vcs_advanced_staging_hunk_validate(hunk) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->hunk_count >= UMI_VCS_ADVANCED_LIST_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->hunks[plan->hunk_count] = *hunk;
    /* Apply this branch only when its contract condition is satisfied. */
    if (hunk->selected) plan->selected_count += 1U;
    plan->fingerprint ^= hunk->fingerprint + (uint64_t)plan->hunk_count;
    plan->hunk_count += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the vcs advanced partial stage plan select operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_partial_stage_plan_select(UmiVcsAdvancedPartialStagePlan *plan,
                                                      size_t index,
                                                      int selected)
{
    int previous;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || index >= plan->hunk_count) return UMI_STATUS_NOT_FOUND;
    previous = plan->hunks[index].selected;
    plan->hunks[index].selected = selected != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!previous && selected) plan->selected_count += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (previous && !selected && plan->selected_count > 0U) plan->selected_count -= 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the vcs advanced partial stage plan ready operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_partial_stage_plan_ready(const UmiVcsAdvancedPartialStagePlan *plan)
{
    return plan != NULL && umi_vcs_advanced_text_present(plan->repository_root) &&
           plan->hunk_count > 0U && plan->selected_count > 0U;
}
