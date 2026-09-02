/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/throttle_record.h
 *
 * PURPOSE:
 *   Define the reusable throttled interaction record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_THROTTLE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_THROTTLE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source throttle record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceThrottleRecord {
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
} UmiWorkbenchContextSourceThrottleRecord;

/**
 * Initialise workbench context source throttle record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_throttle_record_init(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *record_id);
/**
 * Check that workbench context source throttle record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_throttle_record_validate(
    const UmiWorkbenchContextSourceThrottleRecord *record);
/**
 * Provide the workbench context source throttle record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_throttle_record_set_source(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source throttle record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_throttle_record_set_panel(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source throttle record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_throttle_record_set_subject(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source throttle record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_throttle_record_set_group(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source throttle record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_throttle_record_set_label(
    UmiWorkbenchContextSourceThrottleRecord *record,
    const char *label);
/**
 * Provide the workbench context source throttle record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_throttle_record_hash(
    const UmiWorkbenchContextSourceThrottleRecord *record);
/**
 * Provide the workbench context source throttle record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_throttle_record_touch(
    UmiWorkbenchContextSourceThrottleRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
