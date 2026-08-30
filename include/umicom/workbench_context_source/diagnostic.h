/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/diagnostic.h
 *
 * PURPOSE:
 *   Define the reusable source diagnostic contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_DIAGNOSTIC_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_DIAGNOSTIC_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceDiagnostic {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceDiagnostic;

void umi_workbench_context_source_diagnostic_init(
    UmiWorkbenchContextSourceDiagnostic *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_diagnostic_validate(
    const UmiWorkbenchContextSourceDiagnostic *record);
UmiStatus umi_workbench_context_source_diagnostic_set_source(
    UmiWorkbenchContextSourceDiagnostic *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_diagnostic_set_panel(
    UmiWorkbenchContextSourceDiagnostic *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_diagnostic_set_subject(
    UmiWorkbenchContextSourceDiagnostic *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_diagnostic_set_group(
    UmiWorkbenchContextSourceDiagnostic *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_diagnostic_set_label(
    UmiWorkbenchContextSourceDiagnostic *record,
    const char *label);
uint64_t umi_workbench_context_source_diagnostic_hash(
    const UmiWorkbenchContextSourceDiagnostic *record);
void umi_workbench_context_source_diagnostic_touch(
    UmiWorkbenchContextSourceDiagnostic *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
