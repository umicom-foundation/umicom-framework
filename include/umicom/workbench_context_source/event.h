/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/event.h
 *
 * PURPOSE:
 *   Define the reusable source lifecycle event contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_EVENT_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_EVENT_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source event data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextSourceEvent {
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
} UmiWorkbenchContextSourceEvent;

/**
 * Initialise workbench context source event from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_event_init(
    UmiWorkbenchContextSourceEvent *record,
    const char *record_id);
/**
 * Check that workbench context source event satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_source_event_validate(
    const UmiWorkbenchContextSourceEvent *record);
/**
 * Provide the workbench context source event set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_event_set_source(
    UmiWorkbenchContextSourceEvent *record,
    const char *source_id);
/**
 * Provide the workbench context source event set panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_event_set_panel(
    UmiWorkbenchContextSourceEvent *record,
    const char *panel_id);
/**
 * Provide the workbench context source event set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_event_set_subject(
    UmiWorkbenchContextSourceEvent *record,
    const char *subject_id);
/**
 * Provide the workbench context source event set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_event_set_group(
    UmiWorkbenchContextSourceEvent *record,
    const char *group_id);
/**
 * Provide the workbench context source event set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_event_set_label(
    UmiWorkbenchContextSourceEvent *record,
    const char *label);
/**
 * Provide the workbench context source event hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_event_hash(
    const UmiWorkbenchContextSourceEvent *record);
/**
 * Provide the workbench context source event touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_source_event_touch(
    UmiWorkbenchContextSourceEvent *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
