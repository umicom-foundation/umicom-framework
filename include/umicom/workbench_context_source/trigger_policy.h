/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/trigger_policy.h
 *
 * PURPOSE:
 *   Define the reusable trigger policy contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_TRIGGER_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_TRIGGER_POLICY_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceTriggerPolicy {
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
} UmiWorkbenchContextSourceTriggerPolicy;

void umi_workbench_context_source_trigger_policy_init(
    UmiWorkbenchContextSourceTriggerPolicy *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_trigger_policy_validate(
    const UmiWorkbenchContextSourceTriggerPolicy *record);
UmiStatus umi_workbench_context_source_trigger_policy_set_source(
    UmiWorkbenchContextSourceTriggerPolicy *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_trigger_policy_set_panel(
    UmiWorkbenchContextSourceTriggerPolicy *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_trigger_policy_set_subject(
    UmiWorkbenchContextSourceTriggerPolicy *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_trigger_policy_set_group(
    UmiWorkbenchContextSourceTriggerPolicy *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_trigger_policy_set_label(
    UmiWorkbenchContextSourceTriggerPolicy *record,
    const char *label);
uint64_t umi_workbench_context_source_trigger_policy_hash(
    const UmiWorkbenchContextSourceTriggerPolicy *record);
void umi_workbench_context_source_trigger_policy_touch(
    UmiWorkbenchContextSourceTriggerPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
