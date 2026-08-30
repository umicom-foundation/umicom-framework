/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/session_record.h
 *
 * PURPOSE:
 *   Define the reusable source session record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SESSION_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SESSION_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceSessionRecord {
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
} UmiWorkbenchContextSourceSessionRecord;

void umi_workbench_context_source_session_record_init(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_session_record_validate(
    const UmiWorkbenchContextSourceSessionRecord *record);
UmiStatus umi_workbench_context_source_session_record_set_source(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_session_record_set_panel(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_session_record_set_subject(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_session_record_set_group(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_session_record_set_label(
    UmiWorkbenchContextSourceSessionRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_session_record_hash(
    const UmiWorkbenchContextSourceSessionRecord *record);
void umi_workbench_context_source_session_record_touch(
    UmiWorkbenchContextSourceSessionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
