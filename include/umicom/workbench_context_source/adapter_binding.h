/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/adapter_binding.h
 *
 * PURPOSE:
 *   Define the reusable frontend adapter binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_ADAPTER_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_ADAPTER_BINDING_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceAdapterBinding {
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
} UmiWorkbenchContextSourceAdapterBinding;

void umi_workbench_context_source_adapter_binding_init(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_adapter_binding_validate(
    const UmiWorkbenchContextSourceAdapterBinding *record);
UmiStatus umi_workbench_context_source_adapter_binding_set_source(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_adapter_binding_set_panel(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_adapter_binding_set_subject(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_adapter_binding_set_group(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_adapter_binding_set_label(
    UmiWorkbenchContextSourceAdapterBinding *record,
    const char *label);
uint64_t umi_workbench_context_source_adapter_binding_hash(
    const UmiWorkbenchContextSourceAdapterBinding *record);
void umi_workbench_context_source_adapter_binding_touch(
    UmiWorkbenchContextSourceAdapterBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
