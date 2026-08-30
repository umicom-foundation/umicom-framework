/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/context_record.h
 *
 * PURPOSE:
 *   Define the reusable published context record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_CONTEXT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_CONTEXT_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceContextRecord {
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
} UmiWorkbenchContextSourceContextRecord;

void umi_workbench_context_source_context_record_init(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_context_record_validate(
    const UmiWorkbenchContextSourceContextRecord *record);
UmiStatus umi_workbench_context_source_context_record_set_source(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_context_record_set_panel(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_context_record_set_subject(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_context_record_set_group(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_context_record_set_label(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_context_record_hash(
    const UmiWorkbenchContextSourceContextRecord *record);
void umi_workbench_context_source_context_record_touch(
    UmiWorkbenchContextSourceContextRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
