/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/validation.h
 *
 * PURPOSE:
 *   Validate layout identity, topology, component ownership, geometry, monitor placement and persistence invariants with actionable diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_VALIDATION_H
#define UMICOM_WORKBENCH_LAYOUT_VALIDATION_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named workbench layout diagnostic code values accepted by this public contract.
 */
typedef enum UmiWorkbenchLayoutDiagnosticCode {
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_NONE = 0,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_LAYOUT_ID = 1,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_LAYOUT_NAME = 2,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_UNSUPPORTED_SCHEMA = 3,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_ROOT = 4,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_ROOT = 5,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_DUPLICATE_NODE_ID = 6,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_PARENT = 7,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_CHILD_NOT_RECIPROCAL = 8,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_CYCLE = 9,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_UNREACHABLE_NODE = 10,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_CHILD_KIND = 11,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_TOO_MANY_CHILDREN = 12,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_SPLIT_RATIO = 13,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_COMPONENT = 14,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_MISSING_OWNER = 15,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_BOUNDS = 16,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_ACTIVE_CHILD = 17,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_MONITOR = 18,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_EMPTY_CONTAINER = 19,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_LOCKED_DIRTY_LAYOUT = 20,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INVALID_TAG = 21,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_HASH_MISMATCH = 22
} UmiWorkbenchLayoutDiagnosticCode;

/**
 * Represent the workbench layout diagnostic data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutDiagnostic {
    uint32_t structure_size;
    UmiWorkbenchLayoutSeverity severity;
    UmiWorkbenchLayoutDiagnosticCode code;
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char path[UMI_WORKBENCH_LAYOUT_PATH_CAPACITY];
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
    char remediation[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutDiagnostic;

/**
 * Represent the workbench layout validation options data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutValidationOptions {
    uint32_t structure_size;
    bool require_connected_tree;
    bool require_non_empty_containers;
    bool require_components_for_leaf_nodes;
    bool require_owner_for_components;
    bool validate_geometry;
    bool validate_hash;
    bool warnings_as_errors;
} UmiWorkbenchLayoutValidationOptions;

/**
 * Represent the workbench layout validation report data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutValidationReport {
    uint32_t structure_size;
    UmiWorkbenchLayoutDiagnostic
        diagnostics[UMI_WORKBENCH_LAYOUT_MAX_DIAGNOSTICS];
    size_t diagnostic_count;
    size_t info_count;
    size_t warning_count;
    size_t error_count;
    bool valid;
} UmiWorkbenchLayoutValidationReport;

/**
 * Provide the workbench layout validation options default operation used by this module
 * and its client applications.
 */
UmiWorkbenchLayoutValidationOptions
umi_workbench_layout_validation_options_default(void);

/**
 * Initialise workbench layout validation report from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_layout_validation_report_init(
    UmiWorkbenchLayoutValidationReport *report);

/**
 * Check that workbench layout satisfies its contract before another service relies on it.
 */
UmiStatus umi_workbench_layout_validate(
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutValidationOptions *options,
    UmiWorkbenchLayoutValidationReport *out_report);

/**
 * Add workbench layout validation report only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_layout_validation_report_add(
    UmiWorkbenchLayoutValidationReport *report,
    UmiWorkbenchLayoutSeverity severity,
    UmiWorkbenchLayoutDiagnosticCode code,
    const char *node_id,
    const char *path,
    const char *message,
    const char *remediation);

/**
 * Provide the workbench layout diagnostic code text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_layout_diagnostic_code_text(
    UmiWorkbenchLayoutDiagnosticCode code);

/**
 * Provide the workbench layout validation report has code operation used by this module
 * and its client applications.
 */
bool umi_workbench_layout_validation_report_has_code(
    const UmiWorkbenchLayoutValidationReport *report,
    UmiWorkbenchLayoutDiagnosticCode code);

#ifdef __cplusplus
}
#endif

#endif
