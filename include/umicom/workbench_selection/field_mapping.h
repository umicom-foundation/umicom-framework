/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/field_mapping.h
 *
 * PURPOSE:
 *   Define the reusable selection field mapping contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FIELD_MAPPING_H
#define UMICOM_WORKBENCH_SELECTION_FIELD_MAPPING_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionFieldMapping {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionFieldMapping;

void umi_workbench_selection_field_mapping_init(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *record_id);
UmiStatus umi_workbench_selection_field_mapping_validate(
    const UmiWorkbenchSelectionFieldMapping *record);
UmiStatus umi_workbench_selection_field_mapping_set_source(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *source_id);
UmiStatus umi_workbench_selection_field_mapping_set_subject(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_field_mapping_set_related(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *related_id);
UmiStatus umi_workbench_selection_field_mapping_set_group(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *group_id);
UmiStatus umi_workbench_selection_field_mapping_set_description(
    UmiWorkbenchSelectionFieldMapping *record,
    const char *description);
uint64_t umi_workbench_selection_field_mapping_hash(
    const UmiWorkbenchSelectionFieldMapping *record);
void umi_workbench_selection_field_mapping_touch(
    UmiWorkbenchSelectionFieldMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
