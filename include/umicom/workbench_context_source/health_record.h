/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/health_record.h
 *
 * PURPOSE:
 *   Define the reusable source health record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_HEALTH_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_HEALTH_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceHealthRecord {
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
} UmiWorkbenchContextSourceHealthRecord;

void umi_workbench_context_source_health_record_init(
    UmiWorkbenchContextSourceHealthRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_health_record_validate(
    const UmiWorkbenchContextSourceHealthRecord *record);
UmiStatus umi_workbench_context_source_health_record_set_source(
    UmiWorkbenchContextSourceHealthRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_health_record_set_panel(
    UmiWorkbenchContextSourceHealthRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_health_record_set_subject(
    UmiWorkbenchContextSourceHealthRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_health_record_set_group(
    UmiWorkbenchContextSourceHealthRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_health_record_set_label(
    UmiWorkbenchContextSourceHealthRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_health_record_hash(
    const UmiWorkbenchContextSourceHealthRecord *record);
void umi_workbench_context_source_health_record_touch(
    UmiWorkbenchContextSourceHealthRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
