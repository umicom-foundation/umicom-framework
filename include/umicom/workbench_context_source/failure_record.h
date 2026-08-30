/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/failure_record.h
 *
 * PURPOSE:
 *   Define the reusable source failure record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_FAILURE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_FAILURE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceFailureRecord {
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
} UmiWorkbenchContextSourceFailureRecord;

void umi_workbench_context_source_failure_record_init(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_failure_record_validate(
    const UmiWorkbenchContextSourceFailureRecord *record);
UmiStatus umi_workbench_context_source_failure_record_set_source(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_failure_record_set_panel(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_failure_record_set_subject(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_failure_record_set_group(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_failure_record_set_label(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_failure_record_hash(
    const UmiWorkbenchContextSourceFailureRecord *record);
void umi_workbench_context_source_failure_record_touch(
    UmiWorkbenchContextSourceFailureRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
