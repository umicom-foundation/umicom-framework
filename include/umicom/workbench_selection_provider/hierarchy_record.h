/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/hierarchy_record.h
 *
 * PURPOSE:
 *   Define the reusable structured hierarchy record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_HIERARCHY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_HIERARCHY_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderHierarchyRecord {
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
} UmiWorkbenchSelectionProviderHierarchyRecord;

void umi_workbench_selection_provider_hierarchy_record_init(
    UmiWorkbenchSelectionProviderHierarchyRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_hierarchy_record_validate(
    const UmiWorkbenchSelectionProviderHierarchyRecord *record);
UmiStatus umi_workbench_selection_provider_hierarchy_record_set_provider(
    UmiWorkbenchSelectionProviderHierarchyRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_hierarchy_record_set_source(
    UmiWorkbenchSelectionProviderHierarchyRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_hierarchy_record_set_subject(
    UmiWorkbenchSelectionProviderHierarchyRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_hierarchy_record_set_related(
    UmiWorkbenchSelectionProviderHierarchyRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_hierarchy_record_set_group(
    UmiWorkbenchSelectionProviderHierarchyRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_hierarchy_record_set_description(
    UmiWorkbenchSelectionProviderHierarchyRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_hierarchy_record_hash(
    const UmiWorkbenchSelectionProviderHierarchyRecord *record);
void umi_workbench_selection_provider_hierarchy_record_touch(
    UmiWorkbenchSelectionProviderHierarchyRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
