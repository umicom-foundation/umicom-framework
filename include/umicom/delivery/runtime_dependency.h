/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/runtime_dependency.h
 *
 * PURPOSE:
 *   Classify runtime dependencies found while preparing portable products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_RUNTIME_DEPENDENCY_H
#define UMICOM_DELIVERY_RUNTIME_DEPENDENCY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDependencyDisposition {
    UMI_DEPENDENCY_SYSTEM = 1,
    UMI_DEPENDENCY_BUNDLE = 2,
    UMI_DEPENDENCY_EXCLUDE = 3
} UmiDependencyDisposition;

typedef struct UmiRuntimeDependency {
    char name[UMI_DELIVERY_ID_CAPACITY];
    char resolved_path[UMI_DELIVERY_PATH_CAPACITY];
    UmiDependencyDisposition disposition;
    int resolved;
} UmiRuntimeDependency;

typedef struct UmiRuntimeDependencyReport {
    UmiRuntimeDependency dependencies[UMI_DELIVERY_MAX_COMPONENTS];
    size_t count;
} UmiRuntimeDependencyReport;

void umi_runtime_dependency_report_init(UmiRuntimeDependencyReport *report);
UmiStatus umi_runtime_dependency_report_add(
    UmiRuntimeDependencyReport *report,
    const char *name,
    const char *resolved_path,
    UmiDependencyDisposition disposition,
    int resolved);
size_t umi_runtime_dependency_report_unresolved_count(
    const UmiRuntimeDependencyReport *report);
int umi_runtime_dependency_report_ready(
    const UmiRuntimeDependencyReport *report);

#ifdef __cplusplus
}
#endif
#endif
