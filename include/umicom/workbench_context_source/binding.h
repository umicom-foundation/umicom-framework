/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/binding.h
 *
 * PURPOSE:
 *   Define the reusable live source binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_BINDING_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceBinding {
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
} UmiWorkbenchContextSourceBinding;

void umi_workbench_context_source_binding_init(
    UmiWorkbenchContextSourceBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_binding_validate(
    const UmiWorkbenchContextSourceBinding *record);
UmiStatus umi_workbench_context_source_binding_set_source(
    UmiWorkbenchContextSourceBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_binding_set_panel(
    UmiWorkbenchContextSourceBinding *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_binding_set_subject(
    UmiWorkbenchContextSourceBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_binding_set_group(
    UmiWorkbenchContextSourceBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_binding_set_label(
    UmiWorkbenchContextSourceBinding *record,
    const char *label);
uint64_t umi_workbench_context_source_binding_hash(
    const UmiWorkbenchContextSourceBinding *record);
void umi_workbench_context_source_binding_touch(
    UmiWorkbenchContextSourceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
