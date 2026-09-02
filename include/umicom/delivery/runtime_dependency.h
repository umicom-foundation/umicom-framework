/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/runtime_dependency.h
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
#ifndef UMICOM_DELIVERY_RUNTIME_DEPENDENCY_H
#define UMICOM_DELIVERY_RUNTIME_DEPENDENCY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named dependency disposition values accepted by this public contract.
 */
typedef enum UmiDependencyDisposition {
    UMI_DEPENDENCY_SYSTEM = 1,
    UMI_DEPENDENCY_BUNDLE = 2,
    UMI_DEPENDENCY_EXCLUDE = 3
} UmiDependencyDisposition;

/**
 * Represent the runtime dependency data shared with callers of this public contract.
 */
typedef struct UmiRuntimeDependency {
    char name[UMI_DELIVERY_ID_CAPACITY];
    char resolved_path[UMI_DELIVERY_PATH_CAPACITY];
    UmiDependencyDisposition disposition;
    int resolved;
} UmiRuntimeDependency;

/**
 * Represent the runtime dependency report data shared with callers of this public
 * contract.
 */
typedef struct UmiRuntimeDependencyReport {
    UmiRuntimeDependency dependencies[UMI_DELIVERY_MAX_COMPONENTS];
    size_t count;
} UmiRuntimeDependencyReport;

/**
 * Initialise runtime dependency report from caller-provided values so later operations
 * receive a known state.
 */
void umi_runtime_dependency_report_init(UmiRuntimeDependencyReport *report);
/**
 * Add runtime dependency report only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_runtime_dependency_report_add(
    UmiRuntimeDependencyReport *report,
    const char *name,
    const char *resolved_path,
    UmiDependencyDisposition disposition,
    int resolved);
/**
 * Return the number of records represented by runtime dependency report unresolved without
 * changing their state.
 */
size_t umi_runtime_dependency_report_unresolved_count(
    const UmiRuntimeDependencyReport *report);
/**
 * Provide the runtime dependency report ready operation used by this module and its client
 * applications.
 */
int umi_runtime_dependency_report_ready(
    const UmiRuntimeDependencyReport *report);

#ifdef __cplusplus
}
#endif
#endif
