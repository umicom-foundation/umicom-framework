/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/latency_record.h
 *
 * PURPOSE:
 *   Define the reusable selection latency record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_LATENCY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_LATENCY_RECORD_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection latency record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionLatencyRecord {
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
} UmiWorkbenchSelectionLatencyRecord;

/**
 * Initialise workbench selection latency record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_latency_record_init(
    UmiWorkbenchSelectionLatencyRecord *record,
    const char *record_id);
/**
 * Check that workbench selection latency record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_latency_record_validate(
    const UmiWorkbenchSelectionLatencyRecord *record);
/**
 * Provide the workbench selection latency record set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_latency_record_set_source(
    UmiWorkbenchSelectionLatencyRecord *record,
    const char *source_id);
/**
 * Provide the workbench selection latency record set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_latency_record_set_subject(
    UmiWorkbenchSelectionLatencyRecord *record,
    const char *subject_id);
/**
 * Provide the workbench selection latency record set related operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_latency_record_set_related(
    UmiWorkbenchSelectionLatencyRecord *record,
    const char *related_id);
/**
 * Provide the workbench selection latency record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_latency_record_set_group(
    UmiWorkbenchSelectionLatencyRecord *record,
    const char *group_id);
/**
 * Provide the workbench selection latency record set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_latency_record_set_description(
    UmiWorkbenchSelectionLatencyRecord *record,
    const char *description);
/**
 * Provide the workbench selection latency record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_latency_record_hash(
    const UmiWorkbenchSelectionLatencyRecord *record);
/**
 * Provide the workbench selection latency record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_latency_record_touch(
    UmiWorkbenchSelectionLatencyRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
