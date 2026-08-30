/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/validation_gate.h
 *
 * PURPOSE:
 *   Summarise semantic validation and designer diagnostics into explicit
 *   preview, save and export gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_VALIDATION_GATE_H
#define UMICOM_WORKBENCH_DESIGNER_VALIDATION_GATE_H

#include "umicom/workbench_designer/diagnostics.h"
#include "umicom/workbench_layout/validation.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerValidationGate {
    bool can_preview;
    bool can_save;
    bool can_export;
    bool requires_confirmation;
    size_t info_count;
    size_t warning_count;
    size_t error_count;
    char summary[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    uint64_t document_revision;
    uint64_t revision;
} UmiWorkbenchDesignerValidationGate;

void umi_workbench_designer_validation_gate_init(
    UmiWorkbenchDesignerValidationGate *gate);
UmiStatus umi_workbench_designer_validation_gate_evaluate(
    UmiWorkbenchDesignerValidationGate *gate,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerDiagnostics *designer_diagnostics,
    bool allow_warning_export);

#ifdef __cplusplus
}
#endif

#endif
