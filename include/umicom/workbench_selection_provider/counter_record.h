/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/counter_record.h
 *
 * PURPOSE:
 *   Define the reusable provider counter record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_COUNTER_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_COUNTER_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderCounterRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderCounterRecord;

void umi_workbench_selection_provider_counter_record_init(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_counter_record_validate(
    const UmiWorkbenchSelectionProviderCounterRecord *record);
UmiStatus umi_workbench_selection_provider_counter_record_set_provider(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_counter_record_set_source(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_counter_record_set_subject(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_counter_record_set_related(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_counter_record_set_group(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_counter_record_set_description(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_counter_record_hash(
    const UmiWorkbenchSelectionProviderCounterRecord *record);
void umi_workbench_selection_provider_counter_record_touch(
    UmiWorkbenchSelectionProviderCounterRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
