/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/caret_state.h
 *
 * PURPOSE:
 *   Define the reusable caret state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_CARET_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_CARET_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceCaretState {
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
} UmiWorkbenchContextSourceCaretState;

void umi_workbench_context_source_caret_state_init(
    UmiWorkbenchContextSourceCaretState *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_caret_state_validate(
    const UmiWorkbenchContextSourceCaretState *record);
UmiStatus umi_workbench_context_source_caret_state_set_source(
    UmiWorkbenchContextSourceCaretState *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_caret_state_set_panel(
    UmiWorkbenchContextSourceCaretState *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_caret_state_set_subject(
    UmiWorkbenchContextSourceCaretState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_caret_state_set_group(
    UmiWorkbenchContextSourceCaretState *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_caret_state_set_label(
    UmiWorkbenchContextSourceCaretState *record,
    const char *label);
uint64_t umi_workbench_context_source_caret_state_hash(
    const UmiWorkbenchContextSourceCaretState *record);
void umi_workbench_context_source_caret_state_touch(
    UmiWorkbenchContextSourceCaretState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
