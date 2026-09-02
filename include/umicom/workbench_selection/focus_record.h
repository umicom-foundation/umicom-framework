/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/focus_record.h
 *
 * PURPOSE:
 *   Define the reusable selection focus record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FOCUS_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_FOCUS_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection focus record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionFocusRecord {
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
} UmiWorkbenchSelectionFocusRecord;

/**
 * Initialise workbench selection focus record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_focus_record_init(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *record_id);
/**
 * Check that workbench selection focus record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_focus_record_validate(
    const UmiWorkbenchSelectionFocusRecord *record);
/**
 * Provide the workbench selection focus record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_focus_record_set_source(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection focus record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_focus_record_set_subject(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection focus record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_focus_record_set_related(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection focus record set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_focus_record_set_group(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection focus record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_focus_record_set_description(
    UmiWorkbenchSelectionFocusRecord *record,
    const char *description);
/**
 * Provide the workbench selection focus record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_focus_record_hash(
    const UmiWorkbenchSelectionFocusRecord *record);
/**
 * Provide the workbench selection focus record touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_focus_record_touch(
    UmiWorkbenchSelectionFocusRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
