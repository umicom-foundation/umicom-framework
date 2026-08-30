/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/remediation/remediation_builder.c
 *
 * PURPOSE:
 *   Implement translate inspection findings into non-mutating remediation proposals.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/remediation_builder.h"

#include <string.h>
#include "umicom/repository/remediation_attributes.h"
#include "umicom/repository/remediation_branch.h"
#include "umicom/repository/remediation_gitlink.h"
#include "umicom/repository/remediation_ignore.h"
#include "umicom/repository/remediation_remote.h"
#include "umicom/repository/remediation_submodule.h"
#include "umicom/repository/remediation_upstream.h"
#include "umicom/repository/remediation_worktree.h"

/* Add one focused remediation action to a plan after its planner succeeds. */
static UmiStatus add_action(
    UmiRepositoryRemediationPlan *plan,
    UmiStatus planner_status,
    const UmiRepositoryRemediationAction *action)
{
    if (planner_status != UMI_STATUS_OK) {
        return planner_status;
    }
    return umi_repository_remediation_plan_add(plan, action);
}

/* Map one established doctor code to a plan-only action without duplicating doctor rules. */
static UmiStatus add_doctor_action(
    const UmiRepositoryDoctorIssue *issue,
    UmiRepositoryRemediationPlan *plan)
{
    UmiRepositoryRemediationAction action;

    if (strcmp(issue->code, "repository.dirty") == 0) {
        return add_action(plan, umi_repository_remediation_worktree_plan(&action), &action);
    }
    if (strcmp(issue->code, "repository.remote.origin") == 0) {
        return add_action(plan, umi_repository_remediation_remote_plan(&action), &action);
    }
    if (strcmp(issue->code, "repository.branch.upstream") == 0) {
        return add_action(plan, umi_repository_remediation_upstream_plan(&action), &action);
    }
    if (strcmp(issue->code, "repository.submodule.missing") == 0 ||
        strcmp(issue->code, "repository.submodule.head") == 0) {
        return add_action(plan, umi_repository_remediation_submodule_plan(&action), &action);
    }
    if (strcmp(issue->code, "repository.branch.diverged") == 0) {
        return add_action(plan, umi_repository_remediation_branch_plan(&action), &action);
    }
    return UMI_STATUS_OK;
}

/* Map one structural extension issue to a plan-only action. */
static UmiStatus add_structural_action(
    const UmiRepositoryInspectionIssue *issue,
    UmiRepositoryRemediationPlan *plan)
{
    UmiRepositoryRemediationAction action;

    switch (issue->kind) {
    case UMI_REPOSITORY_INSPECTION_FETCH_UNAVAILABLE:
        return add_action(plan, umi_repository_remediation_remote_plan(&action), &action);
    case UMI_REPOSITORY_INSPECTION_GITLINK_COUNT_MISMATCH:
        return add_action(plan, umi_repository_remediation_gitlink_plan(&action), &action);
    case UMI_REPOSITORY_INSPECTION_MISSING_GITIGNORE:
        return add_action(plan, umi_repository_remediation_ignore_plan(&action), &action);
    case UMI_REPOSITORY_INSPECTION_MISSING_GITATTRIBUTES:
        return add_action(plan, umi_repository_remediation_attributes_plan(&action), &action);
    default:
        return UMI_STATUS_OK;
    }
}

/* Build one remediation plan from existing doctor findings and additive structural evidence. */
UmiStatus umi_repository_remediation_builder_build(
    const UmiRepositoryDoctorReport *doctor_report,
    const UmiRepositoryInspectionReport *inspection_report,
    UmiRepositoryRemediationPlan *out_plan)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;

    if (doctor_report == NULL || inspection_report == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_repository_remediation_plan_init(out_plan);

    /* Translate the existing doctor output rather than reevaluating its policy. */
    for (index = 0U; index < doctor_report->issues.count && status == UMI_STATUS_OK;
         ++index) {
        status = add_doctor_action(&doctor_report->issues.items[index], out_plan);
    }

    /* Add only structural findings that the doctor intentionally does not model. */
    for (index = 0U; index < inspection_report->count && status == UMI_STATUS_OK;
         ++index) {
        status = add_structural_action(&inspection_report->issues[index], out_plan);
    }
    return status;
}
