/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/workspace_record.h
 *
 * PURPOSE:
 *   Define the reusable workspace event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_WORKSPACE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_WORKSPACE_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event workspace record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventWorkspaceRecord {
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
} UmiWorkbenchContextEventWorkspaceRecord;

/**
 * Initialise workbench context event workspace record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_workspace_record_init(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *record_id);
/**
 * Check that workbench context event workspace record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_workspace_record_validate(
    const UmiWorkbenchContextEventWorkspaceRecord *record);
/**
 * Provide the workbench context event workspace record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_workspace_record_set_source(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event workspace record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_workspace_record_set_subject(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event workspace record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_workspace_record_set_group(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event workspace record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_workspace_record_set_label(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    const char *label);
/**
 * Provide the workbench context event workspace record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_workspace_record_hash(
    const UmiWorkbenchContextEventWorkspaceRecord *record);
/**
 * Provide the workbench context event workspace record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_workspace_record_touch(
    UmiWorkbenchContextEventWorkspaceRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
