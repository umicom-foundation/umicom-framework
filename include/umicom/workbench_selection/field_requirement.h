/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/field_requirement.h
 *
 * PURPOSE:
 *   Define the reusable selection field requirement contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FIELD_REQUIREMENT_H
#define UMICOM_WORKBENCH_SELECTION_FIELD_REQUIREMENT_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionFieldRequirement {
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
} UmiWorkbenchSelectionFieldRequirement;

void umi_workbench_selection_field_requirement_init(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *record_id);
UmiStatus umi_workbench_selection_field_requirement_validate(
    const UmiWorkbenchSelectionFieldRequirement *record);
UmiStatus umi_workbench_selection_field_requirement_set_source(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *source_id);
UmiStatus umi_workbench_selection_field_requirement_set_subject(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_field_requirement_set_related(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *related_id);
UmiStatus umi_workbench_selection_field_requirement_set_group(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *group_id);
UmiStatus umi_workbench_selection_field_requirement_set_description(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *description);
uint64_t umi_workbench_selection_field_requirement_hash(
    const UmiWorkbenchSelectionFieldRequirement *record);
void umi_workbench_selection_field_requirement_touch(
    UmiWorkbenchSelectionFieldRequirement *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
