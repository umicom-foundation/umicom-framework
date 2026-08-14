/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/runtime_dependency_v2.c
 *
 * PURPOSE:
 *   Classify runtime dependencies found while preparing portable products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/runtime_dependency_v2.h"
#include "delivery_internal.h"
#include <string.h>

void umi_runtime_dependency_report_v2_init(UmiRuntimeDependencyReportV2 *report)
{
    if (report != NULL) (void)memset(report, 0, sizeof(*report));
}

UmiStatus umi_runtime_dependency_report_v2_add(
    UmiRuntimeDependencyReportV2 *report,
    const char *name,
    const char *resolved_path,
    UmiDependencyDispositionV2 disposition,
    int resolved)
{
    UmiRuntimeDependencyV2 *dependency;
    UmiStatus status;
    size_t index;
    if (report == NULL || name == NULL || name[0] == '\0' ||
        resolved_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (report->count >= UMI_DELIVERY_MAX_COMPONENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < report->count; ++index) {
        if (strcmp(report->dependencies[index].name, name) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    dependency = &report->dependencies[report->count];
    (void)memset(dependency, 0, sizeof(*dependency));
    status = umi_delivery_copy_text(dependency->name,
                                    sizeof(dependency->name), name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(dependency->resolved_path,
                                    sizeof(dependency->resolved_path),
                                    resolved_path);
    if (status != UMI_STATUS_OK) return status;
    dependency->disposition = disposition;
    dependency->resolved = resolved != 0;
    ++report->count;
    return UMI_STATUS_OK;
}

size_t umi_runtime_dependency_report_v2_unresolved_count(
    const UmiRuntimeDependencyReportV2 *report)
{
    size_t count = 0U;
    size_t index;
    if (report == NULL) return 0U;
    for (index = 0U; index < report->count; ++index) {
        const UmiRuntimeDependencyV2 *dependency = &report->dependencies[index];
        if (dependency->disposition == UMI_DEPENDENCY_BUNDLE_V2 &&
            !dependency->resolved) ++count;
    }
    return count;
}

int umi_runtime_dependency_report_v2_ready(
    const UmiRuntimeDependencyReportV2 *report)
{
    return report != NULL && report->count <= UMI_DELIVERY_MAX_COMPONENTS &&
           umi_runtime_dependency_report_v2_unresolved_count(report) == 0U;
}
