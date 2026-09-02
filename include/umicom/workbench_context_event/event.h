/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/event.h
 *
 * PURPOSE:
 *   Define one normalised workbench interaction event before conversion into a typed context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_H

#include "umicom/workbench_context_event/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchContextEvent {
    uint32_t structure_size;
    char event_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char path[UMI_WORKBENCH_CONTEXT_EVENT_PATH_CAPACITY];
    char secondary_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    UmiWorkbenchContextEventKind kind;
    UmiWorkbenchContextEventSourceKind source_kind;
    UmiWorkbenchContextEventState state;
    UmiWorkbenchContextEventPriority priority;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventMetadata metadata[UMI_WORKBENCH_CONTEXT_EVENT_MAX_METADATA];
    size_t metadata_count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
    uint64_t content_hash;
    uint32_t line;
    uint32_t column;
    uint32_t selection_length;
    int64_t signed_value;
    uint64_t unsigned_value;
    bool boolean_value;
} UmiWorkbenchContextEvent;

/**
 * Initialise workbench context event from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_event_init(
    UmiWorkbenchContextEvent *event,
    UmiWorkbenchContextEventKind kind,
    const char *event_id);
/**
 * Check that workbench context event satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_workbench_context_event_validate(
    const UmiWorkbenchContextEvent *event);
/**
 * Provide the workbench context event add metadata operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_add_metadata(
    UmiWorkbenchContextEvent *event,
    const char *name,
    const char *value);
/**
 * Provide the workbench context event find metadata operation used by this module and its
 * client applications.
 */
const UmiWorkbenchContextEventMetadata *
umi_workbench_context_event_find_metadata(
    const UmiWorkbenchContextEvent *event,
    const char *name);
/**
 * Provide the workbench context event refresh hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_refresh_hash(
    UmiWorkbenchContextEvent *event);

#ifdef __cplusplus
}
#endif
#endif
