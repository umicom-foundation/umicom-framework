/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/binding_state.h
 *
 * PURPOSE:
 *   Define the reusable binding runtime state contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_BINDING_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_BINDING_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceBindingState {
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
} UmiWorkbenchContextSourceBindingState;

void umi_workbench_context_source_binding_state_init(
    UmiWorkbenchContextSourceBindingState *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_binding_state_validate(
    const UmiWorkbenchContextSourceBindingState *record);
UmiStatus umi_workbench_context_source_binding_state_set_source(
    UmiWorkbenchContextSourceBindingState *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_binding_state_set_panel(
    UmiWorkbenchContextSourceBindingState *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_binding_state_set_subject(
    UmiWorkbenchContextSourceBindingState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_binding_state_set_group(
    UmiWorkbenchContextSourceBindingState *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_binding_state_set_label(
    UmiWorkbenchContextSourceBindingState *record,
    const char *label);
uint64_t umi_workbench_context_source_binding_state_hash(
    const UmiWorkbenchContextSourceBindingState *record);
void umi_workbench_context_source_binding_state_touch(
    UmiWorkbenchContextSourceBindingState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
