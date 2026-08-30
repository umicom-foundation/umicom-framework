/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/validation_gate.c
 *
 * PURPOSE:
 *   Combine semantic document validation and designer issues into stable action
 *   availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/validation_gate.h"
#include "internal.h"


void umi_workbench_designer_validation_gate_init(
    UmiWorkbenchDesignerValidationGate *gate)
{
    if (gate == NULL) return;
    (void)memset(gate, 0, sizeof(*gate));
}

UmiStatus umi_workbench_designer_validation_gate_evaluate(
    UmiWorkbenchDesignerValidationGate *gate,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerDiagnostics *designer_diagnostics,
    bool allow_warning_export)
{
    UmiWorkbenchLayoutValidationOptions validation_options;
    UmiWorkbenchLayoutValidationReport semantic_diagnostics;
    size_t index;
    UmiStatus status;
    if (gate == NULL || document == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_validation_gate_init(gate);
    validation_options = umi_workbench_layout_validation_options_default();
    umi_workbench_layout_validation_report_init(&semantic_diagnostics);
    status = umi_workbench_layout_validate(
        document, &validation_options, &semantic_diagnostics);
    for (index = 0U; index < semantic_diagnostics.diagnostic_count; ++index) {
        const UmiWorkbenchLayoutDiagnostic *issue =
            &semantic_diagnostics.diagnostics[index];
        if (issue->severity == UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR) {
            gate->error_count += 1U;
        } else if (issue->severity == UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING) {
            gate->warning_count += 1U;
        } else {
            gate->info_count += 1U;
        }
    }
    if (designer_diagnostics != NULL) {
        for (index = 0U; index < designer_diagnostics->count; ++index) {
            switch (designer_diagnostics->issues[index].severity) {
                case UMI_WORKBENCH_DESIGNER_ISSUE_ERROR:
                    gate->error_count += 1U; break;
                case UMI_WORKBENCH_DESIGNER_ISSUE_WARNING:
                    gate->warning_count += 1U; break;
                case UMI_WORKBENCH_DESIGNER_ISSUE_INFO:
                    gate->info_count += 1U; break;
                default: break;
            }
        }
    }
    gate->can_preview = gate->error_count == 0U;
    gate->can_save = gate->error_count == 0U;
    gate->can_export = gate->error_count == 0U &&
        (allow_warning_export || gate->warning_count == 0U);
    gate->requires_confirmation = gate->error_count == 0U &&
        gate->warning_count > 0U;
    gate->document_revision = document->version.revision;
    (void)snprintf(
        gate->summary, sizeof(gate->summary),
        "%zu errors, %zu warnings and %zu informational findings.",
        gate->error_count, gate->warning_count, gate->info_count);
    gate->revision += 1U;
    return status == UMI_STATUS_OK || gate->error_count > 0U
        ? UMI_STATUS_OK : status;
}
