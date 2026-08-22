/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/validation_record.h
 *
 * PURPOSE:
 *   Define the reusable provider validation record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_VALIDATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_VALIDATION_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderValidationRecord {
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
} UmiWorkbenchSelectionProviderValidationRecord;

void umi_workbench_selection_provider_validation_record_init(
    UmiWorkbenchSelectionProviderValidationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_validation_record_validate(
    const UmiWorkbenchSelectionProviderValidationRecord *record);
UmiStatus umi_workbench_selection_provider_validation_record_set_provider(
    UmiWorkbenchSelectionProviderValidationRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_validation_record_set_source(
    UmiWorkbenchSelectionProviderValidationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_validation_record_set_subject(
    UmiWorkbenchSelectionProviderValidationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_validation_record_set_related(
    UmiWorkbenchSelectionProviderValidationRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_validation_record_set_group(
    UmiWorkbenchSelectionProviderValidationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_validation_record_set_description(
    UmiWorkbenchSelectionProviderValidationRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_validation_record_hash(
    const UmiWorkbenchSelectionProviderValidationRecord *record);
void umi_workbench_selection_provider_validation_record_touch(
    UmiWorkbenchSelectionProviderValidationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
