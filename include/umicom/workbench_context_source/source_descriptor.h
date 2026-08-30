/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/source_descriptor.h
 *
 * PURPOSE:
 *   Define the reusable live source descriptor contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SOURCE_DESCRIPTOR_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SOURCE_DESCRIPTOR_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceSourceDescriptor {
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
} UmiWorkbenchContextSourceSourceDescriptor;

void umi_workbench_context_source_source_descriptor_init(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_source_descriptor_validate(
    const UmiWorkbenchContextSourceSourceDescriptor *record);
UmiStatus umi_workbench_context_source_source_descriptor_set_source(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_source_descriptor_set_panel(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_source_descriptor_set_subject(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_source_descriptor_set_group(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_source_descriptor_set_label(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    const char *label);
uint64_t umi_workbench_context_source_source_descriptor_hash(
    const UmiWorkbenchContextSourceSourceDescriptor *record);
void umi_workbench_context_source_source_descriptor_touch(
    UmiWorkbenchContextSourceSourceDescriptor *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
