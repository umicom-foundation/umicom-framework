/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/group_hint.h
 *
 * PURPOSE:
 *   Define the reusable context group routing hint contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_GROUP_HINT_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_GROUP_HINT_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceGroupHint {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceGroupHint;

void umi_workbench_context_source_group_hint_init(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_group_hint_validate(
    const UmiWorkbenchContextSourceGroupHint *record);
UmiStatus umi_workbench_context_source_group_hint_set_source(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_group_hint_set_panel(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_group_hint_set_subject(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_group_hint_set_group(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_group_hint_set_label(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *label);
uint64_t umi_workbench_context_source_group_hint_hash(
    const UmiWorkbenchContextSourceGroupHint *record);
void umi_workbench_context_source_group_hint_touch(
    UmiWorkbenchContextSourceGroupHint *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
