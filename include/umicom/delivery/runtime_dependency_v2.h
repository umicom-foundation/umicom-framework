/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/runtime_dependency_v2.h
 *
 * PURPOSE:
 *   Classify runtime dependencies found while preparing portable products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_RUNTIME_DEPENDENCY_V2_H
#define UMICOM_DELIVERY_RUNTIME_DEPENDENCY_V2_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDependencyDispositionV2 {
    UMI_DEPENDENCY_SYSTEM_V2 = 1,
    UMI_DEPENDENCY_BUNDLE_V2 = 2,
    UMI_DEPENDENCY_EXCLUDE_V2 = 3
} UmiDependencyDispositionV2;

typedef struct UmiRuntimeDependencyV2 {
    char name[UMI_DELIVERY_ID_CAPACITY];
    char resolved_path[UMI_DELIVERY_PATH_CAPACITY];
    UmiDependencyDispositionV2 disposition;
    int resolved;
} UmiRuntimeDependencyV2;

typedef struct UmiRuntimeDependencyReportV2 {
    UmiRuntimeDependencyV2 dependencies[UMI_DELIVERY_MAX_COMPONENTS];
    size_t count;
} UmiRuntimeDependencyReportV2;

void umi_runtime_dependency_report_v2_init(UmiRuntimeDependencyReportV2 *report);
UmiStatus umi_runtime_dependency_report_v2_add(
    UmiRuntimeDependencyReportV2 *report,
    const char *name,
    const char *resolved_path,
    UmiDependencyDispositionV2 disposition,
    int resolved);
size_t umi_runtime_dependency_report_v2_unresolved_count(
    const UmiRuntimeDependencyReportV2 *report);
int umi_runtime_dependency_report_v2_ready(
    const UmiRuntimeDependencyReportV2 *report);

#ifdef __cplusplus
}
#endif
#endif
