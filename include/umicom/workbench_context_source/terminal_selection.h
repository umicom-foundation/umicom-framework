/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/terminal_selection.h
 *
 * PURPOSE:
 *   Define the reusable terminal selection snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_TERMINAL_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_TERMINAL_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceTerminalSelection {
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
} UmiWorkbenchContextSourceTerminalSelection;

void umi_workbench_context_source_terminal_selection_init(
    UmiWorkbenchContextSourceTerminalSelection *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_terminal_selection_validate(
    const UmiWorkbenchContextSourceTerminalSelection *record);
UmiStatus umi_workbench_context_source_terminal_selection_set_source(
    UmiWorkbenchContextSourceTerminalSelection *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_terminal_selection_set_panel(
    UmiWorkbenchContextSourceTerminalSelection *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_terminal_selection_set_subject(
    UmiWorkbenchContextSourceTerminalSelection *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_terminal_selection_set_group(
    UmiWorkbenchContextSourceTerminalSelection *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_terminal_selection_set_label(
    UmiWorkbenchContextSourceTerminalSelection *record,
    const char *label);
uint64_t umi_workbench_context_source_terminal_selection_hash(
    const UmiWorkbenchContextSourceTerminalSelection *record);
void umi_workbench_context_source_terminal_selection_touch(
    UmiWorkbenchContextSourceTerminalSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
