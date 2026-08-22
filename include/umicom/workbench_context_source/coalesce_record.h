/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/coalesce_record.h
 *
 * PURPOSE:
 *   Define the reusable coalesced interaction record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_COALESCE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_COALESCE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceCoalesceRecord {
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
} UmiWorkbenchContextSourceCoalesceRecord;

void umi_workbench_context_source_coalesce_record_init(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_coalesce_record_validate(
    const UmiWorkbenchContextSourceCoalesceRecord *record);
UmiStatus umi_workbench_context_source_coalesce_record_set_source(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_coalesce_record_set_panel(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_coalesce_record_set_subject(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_coalesce_record_set_group(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_coalesce_record_set_label(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_coalesce_record_hash(
    const UmiWorkbenchContextSourceCoalesceRecord *record);
void umi_workbench_context_source_coalesce_record_touch(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
