/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/sort_record.h
 *
 * PURPOSE:
 *   Define the reusable provider sort record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SORT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SORT_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderSortRecord {
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
} UmiWorkbenchSelectionProviderSortRecord;

void umi_workbench_selection_provider_sort_record_init(
    UmiWorkbenchSelectionProviderSortRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_sort_record_validate(
    const UmiWorkbenchSelectionProviderSortRecord *record);
UmiStatus umi_workbench_selection_provider_sort_record_set_provider(
    UmiWorkbenchSelectionProviderSortRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_sort_record_set_source(
    UmiWorkbenchSelectionProviderSortRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_sort_record_set_subject(
    UmiWorkbenchSelectionProviderSortRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_sort_record_set_related(
    UmiWorkbenchSelectionProviderSortRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_sort_record_set_group(
    UmiWorkbenchSelectionProviderSortRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_sort_record_set_description(
    UmiWorkbenchSelectionProviderSortRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_sort_record_hash(
    const UmiWorkbenchSelectionProviderSortRecord *record);
void umi_workbench_selection_provider_sort_record_touch(
    UmiWorkbenchSelectionProviderSortRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
