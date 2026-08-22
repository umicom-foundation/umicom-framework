/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/reveal_record.h
 *
 * PURPOSE:
 *   Define the reusable selection reveal record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_REVEAL_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_REVEAL_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionRevealRecord {
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
} UmiWorkbenchSelectionRevealRecord;

void umi_workbench_selection_reveal_record_init(
    UmiWorkbenchSelectionRevealRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_reveal_record_validate(
    const UmiWorkbenchSelectionRevealRecord *record);
UmiStatus umi_workbench_selection_reveal_record_set_source(
    UmiWorkbenchSelectionRevealRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_reveal_record_set_subject(
    UmiWorkbenchSelectionRevealRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_reveal_record_set_related(
    UmiWorkbenchSelectionRevealRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_reveal_record_set_group(
    UmiWorkbenchSelectionRevealRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_reveal_record_set_description(
    UmiWorkbenchSelectionRevealRecord *record,
    const char *description);
uint64_t umi_workbench_selection_reveal_record_hash(
    const UmiWorkbenchSelectionRevealRecord *record);
void umi_workbench_selection_reveal_record_touch(
    UmiWorkbenchSelectionRevealRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
