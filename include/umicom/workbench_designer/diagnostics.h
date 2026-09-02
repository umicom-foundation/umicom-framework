/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/diagnostics.h
 *
 * PURPOSE:
 *   Combine semantic-layout validation with designer-specific editability,
 *   viewport, selection and persistence diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_DIAGNOSTICS_H
#define UMICOM_WORKBENCH_DESIGNER_DIAGNOSTICS_H

#include "umicom/workbench_designer/lease_view.h"
#include "umicom/workbench_designer/selection.h"
#include "umicom/workbench_designer/viewport.h"
#include "umicom/workbench_layout/validation.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * List the named workbench designer issue code values accepted by this public contract.
 */
typedef enum UmiWorkbenchDesignerIssueCode {
    UMI_WORKBENCH_DESIGNER_ISSUE_NONE = 0,
    UMI_WORKBENCH_DESIGNER_ISSUE_LAYOUT_INVALID = 1,
    UMI_WORKBENCH_DESIGNER_ISSUE_SELECTION_MISSING = 2,
    UMI_WORKBENCH_DESIGNER_ISSUE_VIEWPORT_INVALID = 3,
    UMI_WORKBENCH_DESIGNER_ISSUE_EDIT_LEASE_REQUIRED = 4,
    UMI_WORKBENCH_DESIGNER_ISSUE_UNSAVED_CHANGES = 5,
    UMI_WORKBENCH_DESIGNER_ISSUE_SAVE_FAILED = 6,
    UMI_WORKBENCH_DESIGNER_ISSUE_CONFLICT = 7,
    UMI_WORKBENCH_DESIGNER_ISSUE_COMPONENT_MISSING = 8,
    UMI_WORKBENCH_DESIGNER_ISSUE_MONITOR_MISSING = 9
} UmiWorkbenchDesignerIssueCode;

/**
 * Represent the workbench designer issue data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchDesignerIssue {
    UmiWorkbenchDesignerIssueCode code;
    UmiWorkbenchDesignerIssueSeverity severity;
    char issue_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char remediation[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
} UmiWorkbenchDesignerIssue;

/**
 * Represent the workbench designer diagnostics data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerDiagnostics {
    UmiWorkbenchDesignerIssue issues[UMI_WORKBENCH_DESIGNER_MAX_ISSUES];
    size_t count;
    size_t info_count;
    size_t warning_count;
    size_t error_count;
    uint64_t revision;
} UmiWorkbenchDesignerDiagnostics;

/**
 * Initialise workbench designer diagnostics from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_diagnostics_init(UmiWorkbenchDesignerDiagnostics *diagnostics);
/**
 * Add workbench designer diagnostics only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_designer_diagnostics_add(UmiWorkbenchDesignerDiagnostics *diagnostics, UmiWorkbenchDesignerIssueCode code, UmiWorkbenchDesignerIssueSeverity severity, const char *issue_id, const char *node_id, const char *message, const char *remediation);
/**
 * Provide the workbench designer diagnostics build operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_diagnostics_build(UmiWorkbenchDesignerDiagnostics *diagnostics, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSelection *selection, const UmiWorkbenchDesignerViewport *viewport, const UmiWorkbenchDesignerLeaseView *lease, UmiWorkbenchDesignerSaveState save_state);
/**
 * Find workbench designer diagnostics while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerIssue *umi_workbench_designer_diagnostics_at(const UmiWorkbenchDesignerDiagnostics *diagnostics, size_t index);

#ifdef __cplusplus
}
#endif

#endif
