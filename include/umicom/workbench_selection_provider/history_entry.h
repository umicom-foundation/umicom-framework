/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/history_entry.h
 *
 * PURPOSE:
 *   Define the reusable provider history entry contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_HISTORY_ENTRY_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_HISTORY_ENTRY_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderHistoryEntry {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderHistoryEntry;

void umi_workbench_selection_provider_history_entry_init(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_history_entry_validate(
    const UmiWorkbenchSelectionProviderHistoryEntry *record);
UmiStatus umi_workbench_selection_provider_history_entry_set_provider(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_history_entry_set_source(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_history_entry_set_subject(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_history_entry_set_related(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_history_entry_set_group(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_history_entry_set_description(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    const char *description);
uint64_t umi_workbench_selection_provider_history_entry_hash(
    const UmiWorkbenchSelectionProviderHistoryEntry *record);
void umi_workbench_selection_provider_history_entry_touch(
    UmiWorkbenchSelectionProviderHistoryEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
