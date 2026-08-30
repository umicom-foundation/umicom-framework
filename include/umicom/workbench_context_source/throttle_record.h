/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/throttle_record.h
 *
 * PURPOSE:
 *   Define the reusable throttled interaction record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_THROTTLE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_THROTTLE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceThrottleRecord {
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
} UmiWorkbenchContextSourceThrottleRecord;

void umi_workbench_context_source_throttle_record_init(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_throttle_record_validate(
    const UmiWorkbenchContextSourceThrottleRecord *record);
UmiStatus umi_workbench_context_source_throttle_record_set_source(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_throttle_record_set_panel(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_throttle_record_set_subject(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_throttle_record_set_group(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_throttle_record_set_label(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_throttle_record_hash(
    const UmiWorkbenchContextSourceThrottleRecord *record);
void umi_workbench_context_source_throttle_record_touch(
    UmiWorkbenchContextSourceThrottleRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
