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

/*
 * Return the number of records represented by runtime dependency report unresolved without
 * changing their state.
 */
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

/*
 * Provide the runtime dependency report ready operation used by this module and its client
 * applications.
 */
int umi_runtime_dependency_report_ready(
    const UmiRuntimeDependencyReport *report)
{
    return report != NULL && report->count <= UMI_DELIVERY_MAX_COMPONENTS &&
           umi_runtime_dependency_report_unresolved_count(report) == 0U;
}
