/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/doctor.c
 *
 * PURPOSE:
 *   Implement non-mutating repository policy evaluation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/doctor.h"

/* Provide the add operation used by this module and its client applications. */
static void add(UmiRepositoryDoctorReport *report, UmiRepositoryDoctorSeverity severity,
                const char *code, const char *message)
{
    (void)umi_repository_doctor_issue_add(&report->issues, severity, code, message);
}

/*
 * Provide the repository doctor evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_doctor_evaluate(
    const UmiRepositoryStatusSummary *summary,
    const UmiRepositoryDoctorPolicy *policy,
    UmiRepositoryDoctorReport *out_report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL || policy == NULL || out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_repository_doctor_policy_validate(policy) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    umi_repository_doctor_report_init(out_report);
    out_report->summary = *summary;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!policy->allow_dirty_worktree &&
        (umi_repository_worktree_status_dirty(&summary->worktree) ||
         umi_repository_index_status_dirty(&summary->index))) {
        add(out_report, UMI_REPOSITORY_DOCTOR_WARNING, "repository.dirty",
            "Repository contains working-tree or staged changes.");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_origin && !summary->remotes.has_origin) {
        add(out_report, UMI_REPOSITORY_DOCTOR_ERROR, "repository.remote.origin",
            "Required origin remote is not configured.");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_upstream && !summary->remotes.upstream_configured) {
        add(out_report, UMI_REPOSITORY_DOCTOR_ERROR, "repository.branch.upstream",
            "Current branch has no configured upstream.");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_initialised_submodules && summary->submodules.missing > 0U) {
        add(out_report, UMI_REPOSITORY_DOCTOR_ERROR, "repository.submodule.missing",
            "One or more configured submodules are not initialised.");
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (policy->require_matching_submodule_heads && summary->submodules.head_mismatch > 0U) {
        add(out_report, UMI_REPOSITORY_DOCTOR_WARNING, "repository.submodule.head",
            "One or more submodule HEADs differ from the parent gitlink.");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (summary->branch.ahead > 0U && summary->branch.behind > 0U) {
        add(out_report, UMI_REPOSITORY_DOCTOR_WARNING, "repository.branch.diverged",
            "Current branch has diverged from its upstream.");
    }
    return umi_repository_doctor_report_finalize(out_report);
}
