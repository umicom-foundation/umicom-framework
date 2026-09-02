/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/binding_record.h
 *
 * PURPOSE:
 *   Define the reusable selection binding record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_BINDING_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_BINDING_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection binding record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionBindingRecord {
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
} UmiWorkbenchSelectionBindingRecord;

/**
 * Initialise workbench selection binding record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_binding_record_init(
    UmiWorkbenchSelectionBindingRecord *record,
    const char *record_id);
/**
 * Check that workbench selection binding record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_binding_record_validate(
    const UmiWorkbenchSelectionBindingRecord *record);
/**
 * Provide the workbench selection binding record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_binding_record_set_source(
    UmiWorkbenchSelectionBindingRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection binding record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_binding_record_set_subject(
    UmiWorkbenchSelectionBindingRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection binding record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_binding_record_set_secondary(
    UmiWorkbenchSelectionBindingRecord *record,
    const char *secondary_id);
/**
 * Provide the workbench selection binding record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_binding_record_set_group(
    UmiWorkbenchSelectionBindingRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection binding record set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_binding_record_set_label(
    UmiWorkbenchSelectionBindingRecord *record,
    const char *label);
/**
 * Provide the workbench selection binding record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_binding_record_hash(
    const UmiWorkbenchSelectionBindingRecord *record);
/**
 * Provide the workbench selection binding record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_binding_record_touch(
    UmiWorkbenchSelectionBindingRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
