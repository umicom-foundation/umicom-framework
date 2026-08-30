/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/export_record.h
 *
 * PURPOSE:
 *   Define the reusable selection export record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_EXPORT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_EXPORT_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionExportRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionExportRecord;

void umi_workbench_selection_export_record_init(
    UmiWorkbenchSelectionExportRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_export_record_validate(
    const UmiWorkbenchSelectionExportRecord *record);
UmiStatus umi_workbench_selection_export_record_set_source(
    UmiWorkbenchSelectionExportRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_export_record_set_subject(
    UmiWorkbenchSelectionExportRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_export_record_set_related(
    UmiWorkbenchSelectionExportRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_export_record_set_group(
    UmiWorkbenchSelectionExportRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_export_record_set_description(
    UmiWorkbenchSelectionExportRecord *record,
    const char *description);
uint64_t umi_workbench_selection_export_record_hash(
    const UmiWorkbenchSelectionExportRecord *record);
void umi_workbench_selection_export_record_touch(
    UmiWorkbenchSelectionExportRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
