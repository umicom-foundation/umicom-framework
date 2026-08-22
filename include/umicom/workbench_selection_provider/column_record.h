/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/column_record.h
 *
 * PURPOSE:
 *   Define the reusable structured column record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_COLUMN_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_COLUMN_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderColumnRecord {
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
} UmiWorkbenchSelectionProviderColumnRecord;

void umi_workbench_selection_provider_column_record_init(
    UmiWorkbenchSelectionProviderColumnRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_column_record_validate(
    const UmiWorkbenchSelectionProviderColumnRecord *record);
UmiStatus umi_workbench_selection_provider_column_record_set_provider(
    UmiWorkbenchSelectionProviderColumnRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_column_record_set_source(
    UmiWorkbenchSelectionProviderColumnRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_column_record_set_subject(
    UmiWorkbenchSelectionProviderColumnRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_column_record_set_related(
    UmiWorkbenchSelectionProviderColumnRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_column_record_set_group(
    UmiWorkbenchSelectionProviderColumnRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_column_record_set_description(
    UmiWorkbenchSelectionProviderColumnRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_column_record_hash(
    const UmiWorkbenchSelectionProviderColumnRecord *record);
void umi_workbench_selection_provider_column_record_touch(
    UmiWorkbenchSelectionProviderColumnRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
