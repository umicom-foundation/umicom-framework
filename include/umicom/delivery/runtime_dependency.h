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
/** Stage a complete entry; rejected input leaves every report byte unchanged.
 * Inputs are readable terminated strings and may borrow report storage. A
 * resolved bundled dependency requires a nonempty path. Unknown dispositions
 * return INVALID_ARGUMENT; corrupt existing state returns INVALID_STATE. */
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
/** Validate bounded count, live strings, dispositions and unique names.
 * An empty initialized report is valid. A bundled entry marked resolved needs
 * a nonempty path. This is caller-supplied evidence, not a filesystem probe. */
UmiStatus umi_runtime_dependency_report_validate(const UmiRuntimeDependencyReport *report);

/** Publish an unresolved bundle count only after full validation. Refusal keeps
 * *out_count unchanged. Output must be separate from report storage. The caller
 * serializes access; no file discovery or dependency loading occurs. */
UmiStatus umi_runtime_dependency_report_unresolved_count_checked(
    const UmiRuntimeDependencyReport *report, size_t *out_count);

/** Legacy null input remains zero. Malformed non-null input returns SIZE_MAX,
 * which is a failure sentinel, never a valid unresolved count. */
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
