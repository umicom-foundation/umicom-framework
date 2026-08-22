/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/event_record.h
 *
 * PURPOSE:
 *   Define the reusable provider event record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_EVENT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_EVENT_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderEventRecord {
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
} UmiWorkbenchSelectionProviderEventRecord;

void umi_workbench_selection_provider_event_record_init(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_event_record_validate(
    const UmiWorkbenchSelectionProviderEventRecord *record);
UmiStatus umi_workbench_selection_provider_event_record_set_provider(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_event_record_set_source(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_event_record_set_subject(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_event_record_set_related(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_event_record_set_group(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_event_record_set_description(
    UmiWorkbenchSelectionProviderEventRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_event_record_hash(
    const UmiWorkbenchSelectionProviderEventRecord *record);
void umi_workbench_selection_provider_event_record_touch(
    UmiWorkbenchSelectionProviderEventRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
