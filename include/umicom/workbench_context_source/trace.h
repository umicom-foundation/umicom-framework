/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/trace.h
 *
 * PURPOSE:
 *   Define the reusable source trace contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_TRACE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_TRACE_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source trace data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextSourceTrace {
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
} UmiWorkbenchContextSourceTrace;

/**
 * Initialise workbench context source trace from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_trace_init(
    UmiWorkbenchContextSourceTrace *record,
    const char *record_id);
/**
 * Check that workbench context source trace satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_source_trace_validate(
    const UmiWorkbenchContextSourceTrace *record);
/**
 * Provide the workbench context source trace set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_trace_set_source(
    UmiWorkbenchContextSourceTrace *record,
    const char *source_id);
/**
 * Provide the workbench context source trace set panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_trace_set_panel(
    UmiWorkbenchContextSourceTrace *record,
    const char *panel_id);
/**
 * Provide the workbench context source trace set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_trace_set_subject(
    UmiWorkbenchContextSourceTrace *record,
    const char *subject_id);
/**
 * Provide the workbench context source trace set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_trace_set_group(
    UmiWorkbenchContextSourceTrace *record,
    const char *group_id);
/**
 * Provide the workbench context source trace set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_trace_set_label(
    UmiWorkbenchContextSourceTrace *record,
    const char *label);
/**
 * Provide the workbench context source trace hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_trace_hash(
    const UmiWorkbenchContextSourceTrace *record);
/**
 * Provide the workbench context source trace touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_source_trace_touch(
    UmiWorkbenchContextSourceTrace *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
