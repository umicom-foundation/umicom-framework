/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/capability.h
 *
 * PURPOSE:
 *   Define the reusable source capability contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_CAPABILITY_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_CAPABILITY_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceCapability {
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
} UmiWorkbenchContextSourceCapability;

void umi_workbench_context_source_capability_init(
    UmiWorkbenchContextSourceCapability *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_capability_validate(
    const UmiWorkbenchContextSourceCapability *record);
UmiStatus umi_workbench_context_source_capability_set_source(
    UmiWorkbenchContextSourceCapability *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_capability_set_panel(
    UmiWorkbenchContextSourceCapability *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_capability_set_subject(
    UmiWorkbenchContextSourceCapability *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_capability_set_group(
    UmiWorkbenchContextSourceCapability *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_capability_set_label(
    UmiWorkbenchContextSourceCapability *record,
    const char *label);
uint64_t umi_workbench_context_source_capability_hash(
    const UmiWorkbenchContextSourceCapability *record);
void umi_workbench_context_source_capability_touch(
    UmiWorkbenchContextSourceCapability *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
