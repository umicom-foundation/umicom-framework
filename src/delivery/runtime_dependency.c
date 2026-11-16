/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/runtime_dependency.c
 *
 * PURPOSE:
 *   Classify runtime dependencies found while preparing portable products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/runtime_dependency.h"
#include "delivery_internal.h"
#include <string.h>
#include "umicom/base/snapshot_validation.h"

/*
 * Initialise runtime dependency report from caller-provided values so later operations
 * receive a known state.
 */
void umi_runtime_dependency_report_init(UmiRuntimeDependencyReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report != NULL) (void)memset(report, 0, sizeof(*report));
}

/*
 * Add runtime dependency report only after its inputs and available capacity have been
 * checked.
 */
/* Dependency reports now validate complete bounded records before reading counts and stage additions before publication. A malformed report cannot appear ready. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_runtime_dependency_report_add(
    UmiRuntimeDependencyReport *report,
    const char *name,
    const char *resolved_path,
    UmiDependencyDisposition disposition,
    int resolved)
{
    UmiRuntimeDependency *dependency;
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || name == NULL || name[0] == '\0' ||
        resolved_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (report->count >= UMI_DELIVERY_MAX_COMPONENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < report->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(report->dependencies[index].name, name) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    dependency = &report->dependencies[report->count];
    (void)memset(dependency, 0, sizeof(*dependency));
    status = umi_delivery_copy_text(dependency->name,
                                    sizeof(dependency->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(dependency->resolved_path,
                                    sizeof(dependency->resolved_path),
                                    resolved_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    dependency->disposition = disposition;
    dependency->resolved = resolved != 0;
    ++report->count;
    return UMI_STATUS_OK;
}

#endif

static int RuntimeDependencyValid(const UmiRuntimeDependency *dependency)
{
    static const UmiSnapshotTextField fields[] = {
        {"name", offsetof(UmiRuntimeDependency, name), sizeof dependency->name, 1},
        {"resolved_path", offsetof(UmiRuntimeDependency, resolved_path), sizeof dependency->resolved_path, 0}
    };
    if (UmiSnapshotValidateTextFields(dependency, sizeof *dependency, fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        dependency->disposition < UMI_DEPENDENCY_SYSTEM ||
        dependency->disposition > UMI_DEPENDENCY_EXCLUDE) return 0;
    return dependency->disposition != UMI_DEPENDENCY_BUNDLE || !dependency->resolved ||
        dependency->resolved_path[0] != '\0';
}

UmiStatus umi_runtime_dependency_report_validate(const UmiRuntimeDependencyReport *report)
{
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (report->count > UMI_DELIVERY_MAX_COMPONENTS) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < report->count; ++i)
        if (!RuntimeDependencyValid(&report->dependencies[i])) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < report->count; ++i)
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(report->dependencies[i].name, report->dependencies[j].name) == 0)
                return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus umi_runtime_dependency_report_add(UmiRuntimeDependencyReport *report,
    const char *name, const char *resolved_path, UmiDependencyDisposition disposition, int resolved)
{
    if (report == NULL || name == NULL || name[0] == '\0' || resolved_path == NULL ||
        disposition < UMI_DEPENDENCY_SYSTEM || disposition > UMI_DEPENDENCY_EXCLUDE ||
        (disposition == UMI_DEPENDENCY_BUNDLE && resolved && resolved_path[0] == '\0'))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_runtime_dependency_report_validate(report) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;
    if (report->count == UMI_DELIVERY_MAX_COMPONENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < report->count; ++i)
        if (strcmp(report->dependencies[i].name, name) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Reuse the domain copy policy in temporary storage. Rejected input leaves
     * every report byte intact, including prepared data in the unused slot. */
    UmiRuntimeDependency candidate = {0};
    UmiStatus status = umi_delivery_copy_text(candidate.name, sizeof candidate.name, name);
    if (status == UMI_STATUS_OK)
        status = umi_delivery_copy_text(candidate.resolved_path, sizeof candidate.resolved_path, resolved_path);
    if (status != UMI_STATUS_OK) return status;
    candidate.disposition = disposition;
    candidate.resolved = resolved != 0;
    report->dependencies[report->count] = candidate;
    ++report->count;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by runtime dependency report unresolved without
 * changing their state.
 */
/* Dependency reports now validate complete bounded records before reading counts and stage additions before publication. A malformed report cannot appear ready. The former implementation is retained for owner review. */
#if 0
size_t umi_runtime_dependency_report_unresolved_count(
    const UmiRuntimeDependencyReport *report)
{
    size_t count = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < report->count; ++index) {
        const UmiRuntimeDependency *dependency = &report->dependencies[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (dependency->disposition == UMI_DEPENDENCY_BUNDLE &&
            !dependency->resolved) ++count;
    }
    return count;
}

#endif
UmiStatus umi_runtime_dependency_report_unresolved_count_checked(
    const UmiRuntimeDependencyReport *report, size_t *out_count)
{
    if (out_count == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_runtime_dependency_report_validate(report);
    if (status != UMI_STATUS_OK) return status;
    size_t count = 0U;
    for (size_t i = 0U; i < report->count; ++i)
        if (report->dependencies[i].disposition == UMI_DEPENDENCY_BUNDLE &&
            !report->dependencies[i].resolved) ++count;
    *out_count = count;
    return UMI_STATUS_OK;
}

size_t umi_runtime_dependency_report_unresolved_count(const UmiRuntimeDependencyReport *report)
{
    if (report == NULL) return 0U; /* Retain the established null-input result. */
    size_t count;
    return umi_runtime_dependency_report_unresolved_count_checked(report, &count) == UMI_STATUS_OK ?
        count : SIZE_MAX;
}
/*
 * Provide the runtime dependency report ready operation used by this module and its client
 * applications.
 */
/* Dependency reports now validate complete bounded records before reading counts and stage additions before publication. A malformed report cannot appear ready. The former implementation is retained for owner review. */
#if 0
int umi_runtime_dependency_report_ready(
    const UmiRuntimeDependencyReport *report)
{
    return report != NULL && report->count <= UMI_DELIVERY_MAX_COMPONENTS &&
           umi_runtime_dependency_report_unresolved_count(report) == 0U;
}

#endif
int umi_runtime_dependency_report_ready(const UmiRuntimeDependencyReport *report)
{
    size_t count;
    return umi_runtime_dependency_report_unresolved_count_checked(report, &count) == UMI_STATUS_OK &&
        count == 0U;
}