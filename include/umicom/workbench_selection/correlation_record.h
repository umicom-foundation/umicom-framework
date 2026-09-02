/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/correlation_record.h
 *
 * PURPOSE:
 *   Define the reusable selection correlation record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_CORRELATION_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_CORRELATION_RECORD_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection correlation record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionCorrelationRecord {
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
} UmiWorkbenchSelectionCorrelationRecord;

/**
 * Initialise workbench selection correlation record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_correlation_record_init(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *record_id);
/**
 * Check that workbench selection correlation record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_correlation_record_validate(
    const UmiWorkbenchSelectionCorrelationRecord *record);
/**
 * Provide the workbench selection correlation record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_correlation_record_set_source(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection correlation record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_correlation_record_set_subject(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection correlation record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_correlation_record_set_secondary(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *secondary_id);
/**
 * Provide the workbench selection correlation record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_correlation_record_set_group(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection correlation record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_correlation_record_set_label(
    UmiWorkbenchSelectionCorrelationRecord *record,
    const char *label);
/**
 * Provide the workbench selection correlation record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_correlation_record_hash(
    const UmiWorkbenchSelectionCorrelationRecord *record);
/**
 * Provide the workbench selection correlation record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_correlation_record_touch(
    UmiWorkbenchSelectionCorrelationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
