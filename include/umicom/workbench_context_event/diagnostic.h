/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/diagnostic.h
 *
 * PURPOSE:
 *   Define the reusable event diagnostic contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event diagnostic data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventDiagnostic {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
    UmiWorkbenchContextEventKind event_kind;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventPriority priority;
    UmiWorkbenchContextEventState state;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextEventDiagnostic;

/**
 * Initialise workbench context event diagnostic from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_diagnostic_init(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *record_id);
/**
 * Check that workbench context event diagnostic satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_diagnostic_validate(
    const UmiWorkbenchContextEventDiagnostic *record);
/**
 * Provide the workbench context event diagnostic set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_diagnostic_set_source(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *source_id);
/**
 * Provide the workbench context event diagnostic set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_diagnostic_set_subject(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *subject_id);
/**
 * Provide the workbench context event diagnostic set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_diagnostic_set_group(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *group_id);
/**
 * Provide the workbench context event diagnostic set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_diagnostic_set_label(
    UmiWorkbenchContextEventDiagnostic *record,
    const char *label);
/**
 * Provide the workbench context event diagnostic hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_diagnostic_hash(
    const UmiWorkbenchContextEventDiagnostic *record);
/**
 * Provide the workbench context event diagnostic touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_event_diagnostic_touch(
    UmiWorkbenchContextEventDiagnostic *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
