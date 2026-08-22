/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/paging_record.h
 *
 * PURPOSE:
 *   Define the reusable provider paging record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PAGING_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PAGING_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderPagingRecord {
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
} UmiWorkbenchSelectionProviderPagingRecord;

void umi_workbench_selection_provider_paging_record_init(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_paging_record_validate(
    const UmiWorkbenchSelectionProviderPagingRecord *record);
UmiStatus umi_workbench_selection_provider_paging_record_set_provider(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_paging_record_set_source(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_paging_record_set_subject(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_paging_record_set_related(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_paging_record_set_group(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_paging_record_set_description(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_paging_record_hash(
    const UmiWorkbenchSelectionProviderPagingRecord *record);
void umi_workbench_selection_provider_paging_record_touch(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
