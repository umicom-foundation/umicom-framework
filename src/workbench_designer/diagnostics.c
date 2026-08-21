/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/diagnostics.c
 *
 * PURPOSE:
 *   Translate semantic validation evidence into actionable designer issues.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/diagnostics.h"
#include "internal.h"
#include <stdio.h>


void umi_workbench_designer_diagnostics_init(
    UmiWorkbenchDesignerDiagnostics *diagnostics)
{
    if (diagnostics == NULL) return;
    (void)memset(diagnostics, 0, sizeof(*diagnostics));
}

UmiStatus umi_workbench_designer_diagnostics_add(
    UmiWorkbenchDesignerDiagnostics *diagnostics,
    UmiWorkbenchDesignerIssueCode code,
    UmiWorkbenchDesignerIssueSeverity severity,
    const char *issue_id,
    const char *node_id,
    const char *message,
    const char *remediation)
{
    UmiWorkbenchDesignerIssue *issue;
    if (diagnostics == NULL || issue_id == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (diagnostics->count >= UMI_WORKBENCH_DESIGNER_MAX_ISSUES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    issue = &diagnostics->issues[diagnostics->count++];
    (void)memset(issue, 0, sizeof(*issue));
    issue->code = code;
    issue->severity = severity;
    (void)umi_workbench_designer_copy_text(issue->issue_id, sizeof(issue->issue_id), issue_id);
    if (node_id != NULL) {
        (void)umi_workbench_designer_copy_text(issue->node_id, sizeof(issue->node_id), node_id);
    }
    (void)umi_workbench_designer_copy_text(issue->message, sizeof(issue->message), message);
    if (remediation != NULL) {
        (void)umi_workbench_designer_copy_text(issue->remediation, sizeof(issue->remediation), remediation);
    }
    if (severity == UMI_WORKBENCH_DESIGNER_ISSUE_INFO) diagnostics->info_count += 1U;
    else if (severity == UMI_WORKBENCH_DESIGNER_ISSUE_WARNING) diagnostics->warning_count += 1U;
    else diagnostics->error_count += 1U;
    diagnostics->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_diagnostics_build(
    UmiWorkbenchDesignerDiagnostics *diagnostics,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    const UmiWorkbenchDesignerViewport *viewport,
    const UmiWorkbenchDesignerLeaseView *lease,
    UmiWorkbenchDesignerSaveState save_state)
{
    UmiWorkbenchLayoutValidationOptions options;
    UmiWorkbenchLayoutValidationReport report;
    size_t index;
    UmiStatus status;
    if (diagnostics == NULL || document == NULL || selection == NULL || viewport == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_diagnostics_init(diagnostics);
    options = umi_workbench_layout_validation_options_default();
    options.validate_hash = true;
    status = umi_workbench_layout_validate(document, &options, &report);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_INVALID_STATE) return status;
    for (index = 0U; index < report.diagnostic_count; ++index) {
        const UmiWorkbenchLayoutDiagnostic *source = &report.diagnostics[index];
        char issue_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
        int written = snprintf(issue_id, sizeof(issue_id), "layout-%zu", index);
        UmiWorkbenchDesignerIssueSeverity severity = source->severity == UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR
            ? UMI_WORKBENCH_DESIGNER_ISSUE_ERROR
            : source->severity == UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING
                ? UMI_WORKBENCH_DESIGNER_ISSUE_WARNING
                : UMI_WORKBENCH_DESIGNER_ISSUE_INFO;
        if (written < 0 || (size_t)written >= sizeof(issue_id)) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = umi_workbench_designer_diagnostics_add(
            diagnostics, UMI_WORKBENCH_DESIGNER_ISSUE_LAYOUT_INVALID,
            severity, issue_id, source->node_id, source->message,
            source->remediation);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < selection->count; ++index) {
        if (umi_workbench_layout_document_find_node(document, selection->items[index].value) == NULL) {
            status = umi_workbench_designer_diagnostics_add(
                diagnostics, UMI_WORKBENCH_DESIGNER_ISSUE_SELECTION_MISSING,
                UMI_WORKBENCH_DESIGNER_ISSUE_WARNING,
                "selection-missing", selection->items[index].value,
                "A selected node no longer exists in the layout.",
                "Clear or rebuild the selection after the document changes.");
            if (status != UMI_STATUS_OK) return status;
        }
    }
    if (viewport->zoom < viewport->minimum_zoom || viewport->zoom > viewport->maximum_zoom) {
        status = umi_workbench_designer_diagnostics_add(
            diagnostics, UMI_WORKBENCH_DESIGNER_ISSUE_VIEWPORT_INVALID,
            UMI_WORKBENCH_DESIGNER_ISSUE_ERROR,
            "viewport-zoom", NULL,
            "The canvas zoom is outside its configured range.",
            "Reset the viewport or clamp the requested zoom.");
        if (status != UMI_STATUS_OK) return status;
    }
    if (lease != NULL && !umi_workbench_designer_lease_view_editable(lease)) {
        status = umi_workbench_designer_diagnostics_add(
            diagnostics, UMI_WORKBENCH_DESIGNER_ISSUE_EDIT_LEASE_REQUIRED,
            UMI_WORKBENCH_DESIGNER_ISSUE_WARNING,
            "edit-lease", NULL,
            "Another client owns the authoritative edit lease.",
            "Wait for the lease to expire or request ownership.");
        if (status != UMI_STATUS_OK) return status;
    }
    if (save_state == UMI_WORKBENCH_DESIGNER_SAVE_DIRTY ||
        save_state == UMI_WORKBENCH_DESIGNER_SAVE_PENDING) {
        status = umi_workbench_designer_diagnostics_add(
            diagnostics, UMI_WORKBENCH_DESIGNER_ISSUE_UNSAVED_CHANGES,
            UMI_WORKBENCH_DESIGNER_ISSUE_INFO,
            "unsaved", NULL,
            "The layout contains unsaved semantic changes.",
            "Save the layout or wait for autosave.");
    } else if (save_state == UMI_WORKBENCH_DESIGNER_SAVE_FAILED) {
        status = umi_workbench_designer_diagnostics_add(
            diagnostics, UMI_WORKBENCH_DESIGNER_ISSUE_SAVE_FAILED,
            UMI_WORKBENCH_DESIGNER_ISSUE_ERROR,
            "save-failed", NULL,
            "The last layout save failed.",
            "Inspect the persistence status and retry after resolving the cause.");
    } else if (save_state == UMI_WORKBENCH_DESIGNER_SAVE_CONFLICT) {
        status = umi_workbench_designer_diagnostics_add(
            diagnostics, UMI_WORKBENCH_DESIGNER_ISSUE_CONFLICT,
            UMI_WORKBENCH_DESIGNER_ISSUE_ERROR,
            "save-conflict", NULL,
            "The stored layout contains a newer revision.",
            "Open the semantic merge review before saving.");
    }
    return status;
}

const UmiWorkbenchDesignerIssue *umi_workbench_designer_diagnostics_at(
    const UmiWorkbenchDesignerDiagnostics *diagnostics,
    size_t index)
{
    if (diagnostics == NULL || index >= diagnostics->count) return NULL;
    return &diagnostics->issues[index];
}
