/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/causation_record.h
 *
 * PURPOSE:
 *   Define the reusable provider causation record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_CAUSATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_CAUSATION_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderCausationRecord {
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
} UmiWorkbenchSelectionProviderCausationRecord;

void umi_workbench_selection_provider_causation_record_init(
    UmiWorkbenchSelectionProviderCausationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_causation_record_validate(
    const UmiWorkbenchSelectionProviderCausationRecord *record);
UmiStatus umi_workbench_selection_provider_causation_record_set_provider(
    UmiWorkbenchSelectionProviderCausationRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_causation_record_set_source(
    UmiWorkbenchSelectionProviderCausationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_causation_record_set_subject(
    UmiWorkbenchSelectionProviderCausationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_causation_record_set_related(
    UmiWorkbenchSelectionProviderCausationRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_causation_record_set_group(
    UmiWorkbenchSelectionProviderCausationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_causation_record_set_description(
    UmiWorkbenchSelectionProviderCausationRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_causation_record_hash(
    const UmiWorkbenchSelectionProviderCausationRecord *record);
void umi_workbench_selection_provider_causation_record_touch(
    UmiWorkbenchSelectionProviderCausationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
