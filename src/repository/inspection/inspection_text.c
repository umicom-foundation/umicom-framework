/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_text.c
 *
 * PURPOSE:
 *   Implement format inspection evidence for CLI or UI text surfaces without embedding presentation logic in probes.
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

#include "umicom/repository/inspection_text.h"

#include <stdio.h>
#include <inttypes.h>

/* Format repository evidence while keeping canonical doctor and structural findings distinct. */
/* Retain the earlier report for comparison. The active formatter includes
 * evidence state so a failed refresh cannot leave an apparently current report. */
#if 0
UmiStatus umi_repository_inspection_text_format(
    const UmiRepositoryInspectionService *service,
    char *out_text,
    size_t capacity)
{
    int written;

    /* Require complete caller-owned output storage. */
    if (service == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Preserve exact state counts; presentation never narrows authoritative values. */
    written = snprintf(
        out_text,
        capacity,
        "Repository: %s\n"
        "HEAD: %s\n"
        "Branch: %s\n"
        "Upstream: %s\n"
        "Ahead/behind: %zu/%zu\n"
        "Changes: %zu\n"
        "Submodules: %zu configured, %zu missing, %zu mismatched\n"
        "Doctor findings: %zu (%zu warnings, %zu errors)\n"
        "Structural findings: %zu (%zu warnings, %zu errors)\n",
        service->snapshot.identity.root,
        service->snapshot.identity.head,
        service->snapshot.maintenance.branch.branch,
        service->snapshot.maintenance.branch.upstream,
        service->snapshot.maintenance.branch.ahead,
        service->snapshot.maintenance.branch.behind,
        service->snapshot.maintenance.total_changes,
        service->snapshot.configured_submodules,
        service->snapshot.maintenance.submodules.missing,
        service->snapshot.maintenance.submodules.head_mismatch,
        service->maintenance.report.issues.count,
        service->maintenance.report.warnings,
        service->maintenance.report.errors,
        service->report.count,
        service->report.warnings,
        service->report.errors);
    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0) {
        return UMI_STATUS_IO_ERROR;
    }
    return (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

#endif
UmiStatus umi_repository_inspection_text_format(
    const UmiRepositoryInspectionService *service,
    char *out_text,
    size_t capacity)
{
    int written;

    /* Require complete caller-owned output storage. */
    if (service == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Preserve exact state counts; presentation never narrows authoritative values. */
    written = snprintf(
        out_text,
        capacity,
        "Observation: %s\n"
        "Accepted revision: %" PRIu64 "\n"
        "Last refresh: %s\n"
        "Repository: %s\n"
        "HEAD: %s\n"
        "Branch: %s\n"
        "Upstream: %s\n"
        "Ahead/behind: %zu/%zu\n"
        "Changes: %zu\n"
        "Submodules: %zu configured, %zu missing, %zu mismatched\n"
        "Doctor findings: %zu (%zu warnings, %zu errors)\n"
        "Structural findings: %zu (%zu warnings, %zu errors)\n",
        UmiRepositoryInspectionEvidenceText(UmiRepositoryInspectionEvidenceRead(service)),
        service->revision,
        umi_status_text(service->last_status),
        service->snapshot.identity.root,
        service->snapshot.identity.head,
        service->snapshot.maintenance.branch.branch,
        service->snapshot.maintenance.branch.upstream,
        service->snapshot.maintenance.branch.ahead,
        service->snapshot.maintenance.branch.behind,
        service->snapshot.maintenance.total_changes,
        service->snapshot.configured_submodules,
        service->snapshot.maintenance.submodules.missing,
        service->snapshot.maintenance.submodules.head_mismatch,
        service->maintenance.report.issues.count,
        service->maintenance.report.warnings,
        service->maintenance.report.errors,
        service->report.count,
        service->report.warnings,
        service->report.errors);
    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0) {
        return UMI_STATUS_IO_ERROR;
    }
    return (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}
