/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/projection_record.h
 *
 * PURPOSE:
 *   Define the reusable provider projection record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROJECTION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROJECTION_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderProjectionRecord {
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
} UmiWorkbenchSelectionProviderProjectionRecord;

void umi_workbench_selection_provider_projection_record_init(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_projection_record_validate(
    const UmiWorkbenchSelectionProviderProjectionRecord *record);
UmiStatus umi_workbench_selection_provider_projection_record_set_provider(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_projection_record_set_source(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_projection_record_set_subject(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_projection_record_set_related(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_projection_record_set_group(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_projection_record_set_description(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_projection_record_hash(
    const UmiWorkbenchSelectionProviderProjectionRecord *record);
void umi_workbench_selection_provider_projection_record_touch(
    UmiWorkbenchSelectionProviderProjectionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
