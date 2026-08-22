/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/model_binding.h
 *
 * PURPOSE:
 *   Define the reusable toolkit-neutral model binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_MODEL_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_MODEL_BINDING_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceModelBinding {
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
} UmiWorkbenchContextSourceModelBinding;

void umi_workbench_context_source_model_binding_init(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_model_binding_validate(
    const UmiWorkbenchContextSourceModelBinding *record);
UmiStatus umi_workbench_context_source_model_binding_set_source(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_model_binding_set_panel(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_model_binding_set_subject(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_model_binding_set_group(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_model_binding_set_label(
    UmiWorkbenchContextSourceModelBinding *record,
    const char *label);
uint64_t umi_workbench_context_source_model_binding_hash(
    const UmiWorkbenchContextSourceModelBinding *record);
void umi_workbench_context_source_model_binding_touch(
    UmiWorkbenchContextSourceModelBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
