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
void umi_vcs_advanced_partial_stage_plan_init(UmiVcsAdvancedPartialStagePlan *plan)
{
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size = (uint32_t)sizeof(*plan);
    plan->api_version = UMI_VCS_ADVANCED_API_VERSION;
    plan->check_only = 1;
}
UmiStatus umi_vcs_advanced_partial_stage_plan_set_root(UmiVcsAdvancedPartialStagePlan *plan,
                                                        const char *root)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_vcs_advanced_copy_text(plan->repository_root, sizeof(plan->repository_root), root);
}
UmiStatus umi_vcs_advanced_partial_stage_plan_add(UmiVcsAdvancedPartialStagePlan *plan,
                                                   const UmiVcsAdvancedStagingHunk *hunk)
{
    if (plan == NULL || umi_vcs_advanced_staging_hunk_validate(hunk) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->hunk_count >= UMI_VCS_ADVANCED_LIST_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    plan->hunks[plan->hunk_count] = *hunk;
    if (hunk->selected) plan->selected_count += 1U;
    plan->fingerprint ^= hunk->fingerprint + (uint64_t)plan->hunk_count;
    plan->hunk_count += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_vcs_advanced_partial_stage_plan_select(UmiVcsAdvancedPartialStagePlan *plan,
                                                      size_t index,
                                                      int selected)
{
    int previous;
    if (plan == NULL || index >= plan->hunk_count) return UMI_STATUS_NOT_FOUND;
    previous = plan->hunks[index].selected;
    plan->hunks[index].selected = selected != 0;
    if (!previous && selected) plan->selected_count += 1U;
    else if (previous && !selected && plan->selected_count > 0U) plan->selected_count -= 1U;
    return UMI_STATUS_OK;
}
int umi_vcs_advanced_partial_stage_plan_ready(const UmiVcsAdvancedPartialStagePlan *plan)
{
    return plan != NULL && umi_vcs_advanced_text_present(plan->repository_root) &&
           plan->hunk_count > 0U && plan->selected_count > 0U;
}
