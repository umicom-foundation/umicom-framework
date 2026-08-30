/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/evidence_record.h
 *
 * PURPOSE:
 *   Define the reusable selection evidence record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_EVIDENCE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_EVIDENCE_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionEvidenceRecord {
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
} UmiWorkbenchSelectionEvidenceRecord;

void umi_workbench_selection_evidence_record_init(
    UmiWorkbenchSelectionEvidenceRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_evidence_record_validate(
    const UmiWorkbenchSelectionEvidenceRecord *record);
UmiStatus umi_workbench_selection_evidence_record_set_source(
    UmiWorkbenchSelectionEvidenceRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_evidence_record_set_subject(
    UmiWorkbenchSelectionEvidenceRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_evidence_record_set_related(
    UmiWorkbenchSelectionEvidenceRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_evidence_record_set_group(
    UmiWorkbenchSelectionEvidenceRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_evidence_record_set_description(
    UmiWorkbenchSelectionEvidenceRecord *record,
    const char *description);
uint64_t umi_workbench_selection_evidence_record_hash(
    const UmiWorkbenchSelectionEvidenceRecord *record);
void umi_workbench_selection_evidence_record_touch(
    UmiWorkbenchSelectionEvidenceRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
