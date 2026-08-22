/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/account_selection.h
 *
 * PURPOSE:
 *   Define the reusable account selection snapshot contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_ACCOUNT_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_ACCOUNT_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceAccountSelection {
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
} UmiWorkbenchContextSourceAccountSelection;

void umi_workbench_context_source_account_selection_init(
    UmiWorkbenchContextSourceAccountSelection *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_account_selection_validate(
    const UmiWorkbenchContextSourceAccountSelection *record);
UmiStatus umi_workbench_context_source_account_selection_set_source(
    UmiWorkbenchContextSourceAccountSelection *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_account_selection_set_panel(
    UmiWorkbenchContextSourceAccountSelection *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_account_selection_set_subject(
    UmiWorkbenchContextSourceAccountSelection *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_account_selection_set_group(
    UmiWorkbenchContextSourceAccountSelection *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_account_selection_set_label(
    UmiWorkbenchContextSourceAccountSelection *record,
    const char *label);
uint64_t umi_workbench_context_source_account_selection_hash(
    const UmiWorkbenchContextSourceAccountSelection *record);
void umi_workbench_context_source_account_selection_touch(
    UmiWorkbenchContextSourceAccountSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
