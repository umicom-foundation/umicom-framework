/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/provider_record.h
 *
 * PURPOSE:
 *   Define the reusable selection provider record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderRecord {
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
} UmiWorkbenchSelectionProviderRecord;

void umi_workbench_selection_provider_record_init(
    UmiWorkbenchSelectionProviderRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_record_validate(
    const UmiWorkbenchSelectionProviderRecord *record);
UmiStatus umi_workbench_selection_provider_record_set_source(
    UmiWorkbenchSelectionProviderRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_record_set_subject(
    UmiWorkbenchSelectionProviderRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_record_set_secondary(
    UmiWorkbenchSelectionProviderRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_provider_record_set_group(
    UmiWorkbenchSelectionProviderRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_record_set_label(
    UmiWorkbenchSelectionProviderRecord *record,
    const char *label);
uint64_t umi_workbench_selection_provider_record_hash(
    const UmiWorkbenchSelectionProviderRecord *record);
void umi_workbench_selection_provider_record_touch(
    UmiWorkbenchSelectionProviderRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
