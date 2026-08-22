/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/projection_record.h
 *
 * PURPOSE:
 *   Define the reusable source projection record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PROJECTION_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PROJECTION_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceProjectionRecord {
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
} UmiWorkbenchContextSourceProjectionRecord;

void umi_workbench_context_source_projection_record_init(
    UmiWorkbenchContextSourceProjectionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_projection_record_validate(
    const UmiWorkbenchContextSourceProjectionRecord *record);
UmiStatus umi_workbench_context_source_projection_record_set_source(
    UmiWorkbenchContextSourceProjectionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_projection_record_set_panel(
    UmiWorkbenchContextSourceProjectionRecord *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_projection_record_set_subject(
    UmiWorkbenchContextSourceProjectionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_projection_record_set_group(
    UmiWorkbenchContextSourceProjectionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_projection_record_set_label(
    UmiWorkbenchContextSourceProjectionRecord *record,
    const char *label);
uint64_t umi_workbench_context_source_projection_record_hash(
    const UmiWorkbenchContextSourceProjectionRecord *record);
void umi_workbench_context_source_projection_record_touch(
    UmiWorkbenchContextSourceProjectionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
