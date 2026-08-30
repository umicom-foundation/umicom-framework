/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/adapter_record.h
 *
 * PURPOSE:
 *   Define the reusable selection adapter record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_ADAPTER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_ADAPTER_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionAdapterRecord {
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
} UmiWorkbenchSelectionAdapterRecord;

void umi_workbench_selection_adapter_record_init(
    UmiWorkbenchSelectionAdapterRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_adapter_record_validate(
    const UmiWorkbenchSelectionAdapterRecord *record);
UmiStatus umi_workbench_selection_adapter_record_set_source(
    UmiWorkbenchSelectionAdapterRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_adapter_record_set_subject(
    UmiWorkbenchSelectionAdapterRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_adapter_record_set_secondary(
    UmiWorkbenchSelectionAdapterRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_adapter_record_set_group(
    UmiWorkbenchSelectionAdapterRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_adapter_record_set_label(
    UmiWorkbenchSelectionAdapterRecord *record,
    const char *label);
uint64_t umi_workbench_selection_adapter_record_hash(
    const UmiWorkbenchSelectionAdapterRecord *record);
void umi_workbench_selection_adapter_record_touch(
    UmiWorkbenchSelectionAdapterRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
