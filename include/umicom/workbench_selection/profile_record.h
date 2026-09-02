/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/profile_record.h
 *
 * PURPOSE:
 *   Define the reusable selection profile record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROFILE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROFILE_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection profile record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionProfileRecord {
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
} UmiWorkbenchSelectionProfileRecord;

/**
 * Initialise workbench selection profile record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_profile_record_init(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *record_id);
/**
 * Check that workbench selection profile record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_profile_record_validate(
    const UmiWorkbenchSelectionProfileRecord *record);
/**
 * Provide the workbench selection profile record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_profile_record_set_source(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection profile record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_profile_record_set_subject(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection profile record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_profile_record_set_secondary(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *secondary_id);
/**
 * Provide the workbench selection profile record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_profile_record_set_group(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection profile record set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_profile_record_set_label(
    UmiWorkbenchSelectionProfileRecord *record,
    const char *label);
/**
 * Provide the workbench selection profile record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_profile_record_hash(
    const UmiWorkbenchSelectionProfileRecord *record);
/**
 * Provide the workbench selection profile record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_profile_record_touch(
    UmiWorkbenchSelectionProfileRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
