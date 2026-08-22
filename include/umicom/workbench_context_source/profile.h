/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/profile.h
 *
 * PURPOSE:
 *   Define the reusable source profile contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PROFILE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceProfile {
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
} UmiWorkbenchContextSourceProfile;

void umi_workbench_context_source_profile_init(
    UmiWorkbenchContextSourceProfile *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_profile_validate(
    const UmiWorkbenchContextSourceProfile *record);
UmiStatus umi_workbench_context_source_profile_set_source(
    UmiWorkbenchContextSourceProfile *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_profile_set_panel(
    UmiWorkbenchContextSourceProfile *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_profile_set_subject(
    UmiWorkbenchContextSourceProfile *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_profile_set_group(
    UmiWorkbenchContextSourceProfile *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_profile_set_label(
    UmiWorkbenchContextSourceProfile *record,
    const char *label);
uint64_t umi_workbench_context_source_profile_hash(
    const UmiWorkbenchContextSourceProfile *record);
void umi_workbench_context_source_profile_touch(
    UmiWorkbenchContextSourceProfile *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
