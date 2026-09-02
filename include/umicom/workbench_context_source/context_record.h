/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/context_record.h
 *
 * PURPOSE:
 *   Define the reusable published context record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_CONTEXT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_CONTEXT_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source context record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceContextRecord {
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
} UmiWorkbenchContextSourceContextRecord;

/**
 * Initialise workbench context source context record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_context_record_init(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *record_id);
/**
 * Check that workbench context source context record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_context_record_validate(
    const UmiWorkbenchContextSourceContextRecord *record);
/**
 * Provide the workbench context source context record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_context_record_set_source(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source context record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_context_record_set_panel(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source context record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_context_record_set_subject(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source context record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_context_record_set_group(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source context record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_context_record_set_label(
    UmiWorkbenchContextSourceContextRecord *record,
    const char *label);
/**
 * Provide the workbench context source context record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_context_record_hash(
    const UmiWorkbenchContextSourceContextRecord *record);
/**
 * Provide the workbench context source context record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_context_record_touch(
    UmiWorkbenchContextSourceContextRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
