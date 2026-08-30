/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/session_record.h
 *
 * PURPOSE:
 *   Define the reusable selection session record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SESSION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_SESSION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionSessionRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionSessionRecord;

void umi_workbench_selection_session_record_init(
    UmiWorkbenchSelectionSessionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_session_record_validate(
    const UmiWorkbenchSelectionSessionRecord *record);
UmiStatus umi_workbench_selection_session_record_set_source(
    UmiWorkbenchSelectionSessionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_session_record_set_subject(
    UmiWorkbenchSelectionSessionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_session_record_set_secondary(
    UmiWorkbenchSelectionSessionRecord *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_session_record_set_group(
    UmiWorkbenchSelectionSessionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_session_record_set_label(
    UmiWorkbenchSelectionSessionRecord *record,
    const char *label);
uint64_t umi_workbench_selection_session_record_hash(
    const UmiWorkbenchSelectionSessionRecord *record);
void umi_workbench_selection_session_record_touch(
    UmiWorkbenchSelectionSessionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
