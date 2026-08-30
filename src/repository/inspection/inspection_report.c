/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_report.c
 *
 * PURPOSE:
 *   Implement evaluate additive repository inspection rules while preserving the established doctor report.
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

#include "umicom/repository/inspection_report.h"

#include <string.h>

/* Append one structural finding and maintain its severity totals in one place. */
static UmiStatus append_issue(
    UmiRepositoryInspectionReport *report,
    UmiRepositoryInspectionIssueKind kind,
    UmiRepositoryInspectionSeverity severity,
    const char *summary,
    const char *hint)
{
    UmiStatus status;

    /* Preserve bounded report storage before writing the next issue. */
    if (report->count >= UMI_REPOSITORY_INSPECTION_ISSUE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_repository_inspection_issue_set(
        &report->issues[report->count], kind, severity, summary, hint);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Aggregate canonical doctor severities for operation-level health views. */
    ++report->count;
    if (severity == UMI_REPOSITORY_INSPECTION_ERROR) {
        ++report->errors;
    } else if (severity == UMI_REPOSITORY_INSPECTION_WARNING) {
        ++report->warnings;
    }
    return UMI_STATUS_OK;
}

/* Evaluate structural evidence that the established repository doctor does not own. */
UmiStatus umi_repository_inspection_report_build(
    const UmiRepositoryInspectionSnapshot *snapshot,
    const UmiRepositoryInspectionPolicy *policy,
    UmiRepositoryInspectionReport *out_report)
{
    UmiStatus status = UMI_STATUS_OK;

    /* Structural inspection requires a complete snapshot, policy and output model. */
    if (snapshot == NULL || policy == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));

    /* Fetch capability is useful for remote freshness but is not a doctor acceptance rule. */
    if (policy->require_fetch_capability &&
        snapshot->maintenance.remotes.remote_count > 0U &&
        !snapshot->maintenance.remotes.fetch_available) {
        status = append_issue(
            out_report,
            UMI_REPOSITORY_INSPECTION_FETCH_UNAVAILABLE,
            UMI_REPOSITORY_INSPECTION_WARNING,
            "Configured remotes are present but fetch capability is unavailable.",
            "Review network/authentication state before relying on remote freshness.");
    }

    /* Compare canonical .gitmodules inventory with stage-zero gitlinks only. */
    if (status == UMI_STATUS_OK && policy->require_gitlink_alignment &&
        snapshot->has_gitmodules &&
        snapshot->configured_submodules != snapshot->tracked_gitlinks) {
        status = append_issue(
            out_report,
            UMI_REPOSITORY_INSPECTION_GITLINK_COUNT_MISMATCH,
            UMI_REPOSITORY_INSPECTION_ERROR,
            "Configured submodule count differs from tracked gitlink count.",
            "Inspect .gitmodules and stage-zero gitlinks before locking versions.");
    }

    /* Source-control hygiene files are structural repository evidence, not doctor state. */
    if (status == UMI_STATUS_OK && policy->require_gitignore &&
        !snapshot->has_gitignore) {
        status = append_issue(
            out_report,
            UMI_REPOSITORY_INSPECTION_MISSING_GITIGNORE,
            UMI_REPOSITORY_INSPECTION_WARNING,
            "Repository has no root .gitignore.",
            "Add a reviewed project ignore policy; inspection does not create it.");
    }
    if (status == UMI_STATUS_OK && policy->require_gitattributes &&
        !snapshot->has_gitattributes) {
        status = append_issue(
            out_report,
            UMI_REPOSITORY_INSPECTION_MISSING_GITATTRIBUTES,
            UMI_REPOSITORY_INSPECTION_WARNING,
            "Repository has no root .gitattributes.",
            "Add reviewed line-ending and binary attribute rules.");
    }
    return status;
}
