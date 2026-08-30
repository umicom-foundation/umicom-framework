/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/result_record.h
 *
 * PURPOSE:
 *   Define the reusable provider result record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_RESULT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_RESULT_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderResultRecord {
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
} UmiWorkbenchSelectionProviderResultRecord;

void umi_workbench_selection_provider_result_record_init(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_result_record_validate(
    const UmiWorkbenchSelectionProviderResultRecord *record);
UmiStatus umi_workbench_selection_provider_result_record_set_provider(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_result_record_set_source(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_result_record_set_subject(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_result_record_set_related(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_result_record_set_group(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_result_record_set_description(
    UmiWorkbenchSelectionProviderResultRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_result_record_hash(
    const UmiWorkbenchSelectionProviderResultRecord *record);
void umi_workbench_selection_provider_result_record_touch(
    UmiWorkbenchSelectionProviderResultRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
