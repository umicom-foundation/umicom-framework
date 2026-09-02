/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/event_record.h
 *
 * PURPOSE:
 *   Define the reusable normalised event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_EVENT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_EVENT_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source event record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceEventRecord {
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
} UmiWorkbenchContextSourceEventRecord;

/**
 * Initialise workbench context source event record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_event_record_init(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *record_id);
/**
 * Check that workbench context source event record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_event_record_validate(
    const UmiWorkbenchContextSourceEventRecord *record);
/**
 * Provide the workbench context source event record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_event_record_set_source(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source event record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_event_record_set_panel(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source event record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_event_record_set_subject(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source event record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_event_record_set_group(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source event record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_event_record_set_label(
    UmiWorkbenchContextSourceEventRecord *record,
    const char *label);
/**
 * Provide the workbench context source event record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_event_record_hash(
    const UmiWorkbenchContextSourceEventRecord *record);
/**
 * Provide the workbench context source event record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_event_record_touch(
    UmiWorkbenchContextSourceEventRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
