/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/subscription.h
 *
 * PURPOSE:
 *   Define the reusable source subscription contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SUBSCRIPTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SUBSCRIPTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceSubscription {
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
} UmiWorkbenchContextSourceSubscription;

void umi_workbench_context_source_subscription_init(
    UmiWorkbenchContextSourceSubscription *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_subscription_validate(
    const UmiWorkbenchContextSourceSubscription *record);
UmiStatus umi_workbench_context_source_subscription_set_source(
    UmiWorkbenchContextSourceSubscription *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_subscription_set_panel(
    UmiWorkbenchContextSourceSubscription *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_subscription_set_subject(
    UmiWorkbenchContextSourceSubscription *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_subscription_set_group(
    UmiWorkbenchContextSourceSubscription *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_subscription_set_label(
    UmiWorkbenchContextSourceSubscription *record,
    const char *label);
uint64_t umi_workbench_context_source_subscription_hash(
    const UmiWorkbenchContextSourceSubscription *record);
void umi_workbench_context_source_subscription_touch(
    UmiWorkbenchContextSourceSubscription *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
