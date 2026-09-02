/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/result_record.h
 *
 * PURPOSE:
 *   Define the reusable selection result record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_RESULT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_RESULT_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection result record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionResultRecord {
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
} UmiWorkbenchSelectionResultRecord;

/**
 * Initialise workbench selection result record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_result_record_init(
    UmiWorkbenchSelectionResultRecord *record,
    const char *record_id);
/**
 * Check that workbench selection result record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_result_record_validate(
    const UmiWorkbenchSelectionResultRecord *record);
/**
 * Provide the workbench selection result record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_result_record_set_source(
    UmiWorkbenchSelectionResultRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection result record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_result_record_set_subject(
    UmiWorkbenchSelectionResultRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection result record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_result_record_set_related(
    UmiWorkbenchSelectionResultRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection result record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_result_record_set_group(
    UmiWorkbenchSelectionResultRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection result record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_result_record_set_description(
    UmiWorkbenchSelectionResultRecord *record,
    const char *description);
/**
 * Provide the workbench selection result record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_result_record_hash(
    const UmiWorkbenchSelectionResultRecord *record);
/**
 * Provide the workbench selection result record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_result_record_touch(
    UmiWorkbenchSelectionResultRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
