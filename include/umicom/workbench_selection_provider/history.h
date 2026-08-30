/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/history.h
 *
 * PURPOSE:
 *   Retain bounded structured-selection provider history for diagnostics, replay and UI inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_HISTORY_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_HISTORY_H
#include "umicom/workbench_selection_provider/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderHistoryItem {
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    UmiWorkbenchSelection selection;
    UmiWorkbenchSelectionProviderResult result;
    uint64_t sequence;
    uint64_t timestamp_ms;
} UmiWorkbenchSelectionProviderHistoryItem;

typedef struct UmiWorkbenchSelectionProviderHistory {
    UmiWorkbenchSelectionProviderHistoryItem *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchSelectionProviderHistory;

void umi_workbench_selection_provider_history_init(
    UmiWorkbenchSelectionProviderHistory *history);
void umi_workbench_selection_provider_history_destroy(
    UmiWorkbenchSelectionProviderHistory *history);
UmiStatus umi_workbench_selection_provider_history_append(
    UmiWorkbenchSelectionProviderHistory *history,
    const char *provider_id,
    const UmiWorkbenchSelection *selection,
    UmiWorkbenchSelectionProviderResult result,
    uint64_t sequence,
    uint64_t timestamp_ms);
const UmiWorkbenchSelectionProviderHistoryItem *
umi_workbench_selection_provider_history_latest(
    const UmiWorkbenchSelectionProviderHistory *history,
    const char *provider_id);
void umi_workbench_selection_provider_history_clear(
    UmiWorkbenchSelectionProviderHistory *history);

#ifdef __cplusplus
}
#endif
#endif
