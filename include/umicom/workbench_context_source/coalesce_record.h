/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/coalesce_record.h
 *
 * PURPOSE:
 *   Define the reusable coalesced interaction record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_COALESCE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_COALESCE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source coalesce record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceCoalesceRecord {
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
} UmiWorkbenchContextSourceCoalesceRecord;

/**
 * Initialise workbench context source coalesce record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_coalesce_record_init(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *record_id);
/**
 * Check that workbench context source coalesce record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_coalesce_record_validate(
    const UmiWorkbenchContextSourceCoalesceRecord *record);
/**
 * Provide the workbench context source coalesce record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_coalesce_record_set_source(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source coalesce record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_coalesce_record_set_panel(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source coalesce record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_coalesce_record_set_subject(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source coalesce record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_coalesce_record_set_group(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source coalesce record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_coalesce_record_set_label(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    const char *label);
/**
 * Provide the workbench context source coalesce record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_coalesce_record_hash(
    const UmiWorkbenchContextSourceCoalesceRecord *record);
/**
 * Provide the workbench context source coalesce record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_coalesce_record_touch(
    UmiWorkbenchContextSourceCoalesceRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
