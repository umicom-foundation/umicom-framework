/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/history_record.h
 *
 * PURPOSE:
 *   Define the reusable source history record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_HISTORY_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_HISTORY_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceHistoryRecord {
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
} UmiWorkbenchContextSourceHistoryRecord;

void umi_workbench_context_source_history_record_init(
    UmiWorkbenchContextSourceHistoryRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_history_record_validate(
    const UmiWorkbenchContextSourceHistoryRecord *record);
UmiStatus umi_workbench_context_source_history_record_set_source(
    UmiWorkbenchContextSourceHistoryRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_history_record_set_panel(
    UmiWorkbenchContextSourceHistoryRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_history_record_set_subject(
    UmiWorkbenchContextSourceHistoryRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_history_record_set_group(
    UmiWorkbenchContextSourceHistoryRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_history_record_set_label(
    UmiWorkbenchContextSourceHistoryRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_history_record_hash(
    const UmiWorkbenchContextSourceHistoryRecord *record);
void umi_workbench_context_source_history_record_touch(
    UmiWorkbenchContextSourceHistoryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
