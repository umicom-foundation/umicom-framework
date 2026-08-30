/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/application_state.h
 *
 * PURPOSE:
 *   Define the reusable application state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_APPLICATION_STATE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_APPLICATION_STATE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceApplicationState {
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
} UmiWorkbenchContextSourceApplicationState;

void umi_workbench_context_source_application_state_init(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_application_state_validate(
    const UmiWorkbenchContextSourceApplicationState *record);
UmiStatus umi_workbench_context_source_application_state_set_source(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_application_state_set_panel(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_application_state_set_subject(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_application_state_set_group(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_application_state_set_label(
    UmiWorkbenchContextSourceApplicationState *record,
    const char *label);
uint64_t umi_workbench_context_source_application_state_hash(
    const UmiWorkbenchContextSourceApplicationState *record);
void umi_workbench_context_source_application_state_touch(
    UmiWorkbenchContextSourceApplicationState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
