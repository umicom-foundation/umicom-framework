/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/producer_state.h
 *
 * PURPOSE:
 *   Define the reusable selection producer state contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PRODUCER_STATE_H
#define UMICOM_WORKBENCH_SELECTION_PRODUCER_STATE_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProducerState {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProducerState;

void umi_workbench_selection_producer_state_init(
    UmiWorkbenchSelectionProducerState *record,
    const char *record_id);
UmiStatus umi_workbench_selection_producer_state_validate(
    const UmiWorkbenchSelectionProducerState *record);
UmiStatus umi_workbench_selection_producer_state_set_source(
    UmiWorkbenchSelectionProducerState *record,
    const char *source_id);
UmiStatus umi_workbench_selection_producer_state_set_subject(
    UmiWorkbenchSelectionProducerState *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_producer_state_set_related(
    UmiWorkbenchSelectionProducerState *record,
    const char *related_id);
UmiStatus umi_workbench_selection_producer_state_set_group(
    UmiWorkbenchSelectionProducerState *record,
    const char *group_id);
UmiStatus umi_workbench_selection_producer_state_set_description(
    UmiWorkbenchSelectionProducerState *record,
    const char *description);
uint64_t umi_workbench_selection_producer_state_hash(
    const UmiWorkbenchSelectionProducerState *record);
void umi_workbench_selection_producer_state_touch(
    UmiWorkbenchSelectionProducerState *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
