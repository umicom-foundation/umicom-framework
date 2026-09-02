/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/validation_record.h
 *
 * PURPOSE:
 *   Define the reusable selection validation record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_VALIDATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_VALIDATION_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection validation record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionValidationRecord {
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
} UmiWorkbenchSelectionValidationRecord;

/**
 * Initialise workbench selection validation record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_validation_record_init(
    UmiWorkbenchSelectionValidationRecord *record,
    const char *record_id);
/**
 * Check that workbench selection validation record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_validation_record_validate(
    const UmiWorkbenchSelectionValidationRecord *record);
/**
 * Provide the workbench selection validation record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_validation_record_set_source(
    UmiWorkbenchSelectionValidationRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection validation record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_validation_record_set_subject(
    UmiWorkbenchSelectionValidationRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection validation record set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_validation_record_set_related(
    UmiWorkbenchSelectionValidationRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection validation record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_validation_record_set_group(
    UmiWorkbenchSelectionValidationRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection validation record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_validation_record_set_description(
    UmiWorkbenchSelectionValidationRecord *record,
    const char *description);
/**
 * Provide the workbench selection validation record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_validation_record_hash(
    const UmiWorkbenchSelectionValidationRecord *record);
/**
 * Provide the workbench selection validation record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_validation_record_touch(
    UmiWorkbenchSelectionValidationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
