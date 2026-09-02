/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/duplicate_record.h
 *
 * PURPOSE:
 *   Define the reusable duplicate selection record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_DUPLICATE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_DUPLICATE_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection duplicate record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionDuplicateRecord {
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
} UmiWorkbenchSelectionDuplicateRecord;

/**
 * Initialise workbench selection duplicate record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_duplicate_record_init(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *record_id);
/**
 * Check that workbench selection duplicate record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_duplicate_record_validate(
    const UmiWorkbenchSelectionDuplicateRecord *record);
/**
 * Provide the workbench selection duplicate record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_duplicate_record_set_source(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection duplicate record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_duplicate_record_set_subject(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection duplicate record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_duplicate_record_set_secondary(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *secondary_id);
/**
 * Provide the workbench selection duplicate record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_duplicate_record_set_group(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection duplicate record set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_duplicate_record_set_label(
    UmiWorkbenchSelectionDuplicateRecord *record,
    const char *label);
/**
 * Provide the workbench selection duplicate record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_duplicate_record_hash(
    const UmiWorkbenchSelectionDuplicateRecord *record);
/**
 * Provide the workbench selection duplicate record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_duplicate_record_touch(
    UmiWorkbenchSelectionDuplicateRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
