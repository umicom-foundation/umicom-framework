/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/query_record.h
 *
 * PURPOSE:
 *   Define the reusable provider query record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_QUERY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_QUERY_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderQueryRecord {
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
} UmiWorkbenchSelectionProviderQueryRecord;

void umi_workbench_selection_provider_query_record_init(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_query_record_validate(
    const UmiWorkbenchSelectionProviderQueryRecord *record);
UmiStatus umi_workbench_selection_provider_query_record_set_provider(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_query_record_set_source(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_query_record_set_subject(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_query_record_set_related(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_query_record_set_group(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_query_record_set_description(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_query_record_hash(
    const UmiWorkbenchSelectionProviderQueryRecord *record);
void umi_workbench_selection_provider_query_record_touch(
    UmiWorkbenchSelectionProviderQueryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
