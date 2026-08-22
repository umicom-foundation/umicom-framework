/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/studio_profile_record.h
 *
 * PURPOSE:
 *   Define the reusable Studio provider profile record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_STUDIO_PROFILE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_STUDIO_PROFILE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderStudioProfileRecord {
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
} UmiWorkbenchSelectionProviderStudioProfileRecord;

void umi_workbench_selection_provider_studio_profile_record_init(
    UmiWorkbenchSelectionProviderStudioProfileRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_studio_profile_record_validate(
    const UmiWorkbenchSelectionProviderStudioProfileRecord *record);
UmiStatus umi_workbench_selection_provider_studio_profile_record_set_provider(
    UmiWorkbenchSelectionProviderStudioProfileRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_studio_profile_record_set_source(
    UmiWorkbenchSelectionProviderStudioProfileRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_studio_profile_record_set_subject(
    UmiWorkbenchSelectionProviderStudioProfileRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_studio_profile_record_set_related(
    UmiWorkbenchSelectionProviderStudioProfileRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_studio_profile_record_set_group(
    UmiWorkbenchSelectionProviderStudioProfileRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_studio_profile_record_set_description(
    UmiWorkbenchSelectionProviderStudioProfileRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_studio_profile_record_hash(
    const UmiWorkbenchSelectionProviderStudioProfileRecord *record);
void umi_workbench_selection_provider_studio_profile_record_touch(
    UmiWorkbenchSelectionProviderStudioProfileRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
