/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/event_record.h
 *
 * PURPOSE:
 *   Define the reusable selection event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_EVENT_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_EVENT_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection event record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionEventRecord {
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
} UmiWorkbenchSelectionEventRecord;

/**
 * Initialise workbench selection event record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_event_record_init(
    UmiWorkbenchSelectionEventRecord *record,
    const char *record_id);
/**
 * Check that workbench selection event record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_event_record_validate(
    const UmiWorkbenchSelectionEventRecord *record);
/**
 * Provide the workbench selection event record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_event_record_set_source(
    UmiWorkbenchSelectionEventRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection event record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_event_record_set_subject(
    UmiWorkbenchSelectionEventRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection event record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_event_record_set_related(
    UmiWorkbenchSelectionEventRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection event record set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_event_record_set_group(
    UmiWorkbenchSelectionEventRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection event record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_event_record_set_description(
    UmiWorkbenchSelectionEventRecord *record,
    const char *description);
/**
 * Provide the workbench selection event record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_event_record_hash(
    const UmiWorkbenchSelectionEventRecord *record);
/**
 * Provide the workbench selection event record touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_event_record_touch(
    UmiWorkbenchSelectionEventRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
