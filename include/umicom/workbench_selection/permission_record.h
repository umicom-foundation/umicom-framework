/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/permission_record.h
 *
 * PURPOSE:
 *   Define the reusable selection permission record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PERMISSION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PERMISSION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection permission record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionPermissionRecord {
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
} UmiWorkbenchSelectionPermissionRecord;

/**
 * Initialise workbench selection permission record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_permission_record_init(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *record_id);
/**
 * Check that workbench selection permission record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_permission_record_validate(
    const UmiWorkbenchSelectionPermissionRecord *record);
/**
 * Provide the workbench selection permission record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_permission_record_set_source(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection permission record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_permission_record_set_subject(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection permission record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_permission_record_set_secondary(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *secondary_id);
/**
 * Provide the workbench selection permission record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_permission_record_set_group(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection permission record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_permission_record_set_label(
    UmiWorkbenchSelectionPermissionRecord *record,
    const char *label);
/**
 * Provide the workbench selection permission record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_permission_record_hash(
    const UmiWorkbenchSelectionPermissionRecord *record);
/**
 * Provide the workbench selection permission record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_permission_record_touch(
    UmiWorkbenchSelectionPermissionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
