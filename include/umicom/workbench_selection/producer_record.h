/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/producer_record.h
 *
 * PURPOSE:
 *   Define the reusable selection producer record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PRODUCER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PRODUCER_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProducerRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProducerRecord;

void umi_workbench_selection_producer_record_init(
    UmiWorkbenchSelectionProducerRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_producer_record_validate(
    const UmiWorkbenchSelectionProducerRecord *record);
UmiStatus umi_workbench_selection_producer_record_set_source(
    UmiWorkbenchSelectionProducerRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_producer_record_set_subject(
    UmiWorkbenchSelectionProducerRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_producer_record_set_secondary(
    UmiWorkbenchSelectionProducerRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_producer_record_set_group(
    UmiWorkbenchSelectionProducerRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_producer_record_set_label(
    UmiWorkbenchSelectionProducerRecord *record,
    const char *label);
uint64_t umi_workbench_selection_producer_record_hash(
    const UmiWorkbenchSelectionProducerRecord *record);
void umi_workbench_selection_producer_record_touch(
    UmiWorkbenchSelectionProducerRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
