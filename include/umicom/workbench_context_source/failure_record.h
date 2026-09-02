/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/failure_record.h
 *
 * PURPOSE:
 *   Define the reusable source failure record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_FAILURE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_FAILURE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source failure record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceFailureRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceFailureRecord;

/**
 * Initialise workbench context source failure record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_failure_record_init(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *record_id);
/**
 * Check that workbench context source failure record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_failure_record_validate(
    const UmiWorkbenchContextSourceFailureRecord *record);
/**
 * Provide the workbench context source failure record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_source(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source failure record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_panel(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source failure record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_subject(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source failure record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_group(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source failure record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_failure_record_set_label(
    UmiWorkbenchContextSourceFailureRecord *record,
    const char *label);
/**
 * Provide the workbench context source failure record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_failure_record_hash(
    const UmiWorkbenchContextSourceFailureRecord *record);
/**
 * Provide the workbench context source failure record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_failure_record_touch(
    UmiWorkbenchContextSourceFailureRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
