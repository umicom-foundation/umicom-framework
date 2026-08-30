/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/presentation_record.h
 *
 * PURPOSE:
 *   Define the reusable provider presentation record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PRESENTATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PRESENTATION_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderPresentationRecord {
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
} UmiWorkbenchSelectionProviderPresentationRecord;

void umi_workbench_selection_provider_presentation_record_init(
    UmiWorkbenchSelectionProviderPresentationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_presentation_record_validate(
    const UmiWorkbenchSelectionProviderPresentationRecord *record);
UmiStatus umi_workbench_selection_provider_presentation_record_set_provider(
    UmiWorkbenchSelectionProviderPresentationRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_presentation_record_set_source(
    UmiWorkbenchSelectionProviderPresentationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_presentation_record_set_subject(
    UmiWorkbenchSelectionProviderPresentationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_presentation_record_set_related(
    UmiWorkbenchSelectionProviderPresentationRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_presentation_record_set_group(
    UmiWorkbenchSelectionProviderPresentationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_presentation_record_set_description(
    UmiWorkbenchSelectionProviderPresentationRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_presentation_record_hash(
    const UmiWorkbenchSelectionProviderPresentationRecord *record);
void umi_workbench_selection_provider_presentation_record_touch(
    UmiWorkbenchSelectionProviderPresentationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
