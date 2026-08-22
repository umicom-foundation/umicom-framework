/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/profile_binding.h
 *
 * PURPOSE:
 *   Define the reusable source profile binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PROFILE_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PROFILE_BINDING_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceProfileBinding {
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
} UmiWorkbenchContextSourceProfileBinding;

void umi_workbench_context_source_profile_binding_init(
    UmiWorkbenchContextSourceProfileBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_profile_binding_validate(
    const UmiWorkbenchContextSourceProfileBinding *record);
UmiStatus umi_workbench_context_source_profile_binding_set_source(
    UmiWorkbenchContextSourceProfileBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_profile_binding_set_panel(
    UmiWorkbenchContextSourceProfileBinding *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_profile_binding_set_subject(
    UmiWorkbenchContextSourceProfileBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_profile_binding_set_group(
    UmiWorkbenchContextSourceProfileBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_profile_binding_set_label(
    UmiWorkbenchContextSourceProfileBinding *record,
    const char *label);
uint64_t umi_workbench_context_source_profile_binding_hash(
    const UmiWorkbenchContextSourceProfileBinding *record);
void umi_workbench_context_source_profile_binding_touch(
    UmiWorkbenchContextSourceProfileBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
