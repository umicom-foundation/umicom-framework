/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/synchronization_record.h
 *
 * PURPOSE:
 *   Define the reusable provider synchronisation record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SYNCHRONIZATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SYNCHRONIZATION_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderSynchronizationRecord {
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
} UmiWorkbenchSelectionProviderSynchronizationRecord;

void umi_workbench_selection_provider_synchronization_record_init(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_synchronization_record_validate(
    const UmiWorkbenchSelectionProviderSynchronizationRecord *record);
UmiStatus umi_workbench_selection_provider_synchronization_record_set_provider(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_synchronization_record_set_source(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_synchronization_record_set_subject(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_synchronization_record_set_related(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_synchronization_record_set_group(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_synchronization_record_set_description(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_synchronization_record_hash(
    const UmiWorkbenchSelectionProviderSynchronizationRecord *record);
void umi_workbench_selection_provider_synchronization_record_touch(
    UmiWorkbenchSelectionProviderSynchronizationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
