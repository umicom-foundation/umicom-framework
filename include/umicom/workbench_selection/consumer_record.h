/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/consumer_record.h
 *
 * PURPOSE:
 *   Define the reusable selection consumer record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_CONSUMER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_CONSUMER_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionConsumerRecord {
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
} UmiWorkbenchSelectionConsumerRecord;

void umi_workbench_selection_consumer_record_init(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_consumer_record_validate(
    const UmiWorkbenchSelectionConsumerRecord *record);
UmiStatus umi_workbench_selection_consumer_record_set_source(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_consumer_record_set_subject(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_consumer_record_set_secondary(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_consumer_record_set_group(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_consumer_record_set_label(
    UmiWorkbenchSelectionConsumerRecord *record,
    const char *label);
uint64_t umi_workbench_selection_consumer_record_hash(
    const UmiWorkbenchSelectionConsumerRecord *record);
void umi_workbench_selection_consumer_record_touch(
    UmiWorkbenchSelectionConsumerRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
