/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/event.h
 *
 * PURPOSE:
 *   Define one normalised workbench interaction event before conversion into a typed context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_EVENT_H

#include "umicom/workbench_context_event/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_event_init(
    UmiWorkbenchContextEvent *event,
    UmiWorkbenchContextEventKind kind,
    const char *event_id);
UmiStatus umi_workbench_context_event_validate(
    const UmiWorkbenchContextEvent *event);
UmiStatus umi_workbench_context_event_add_metadata(
    UmiWorkbenchContextEvent *event,
    const char *name,
    const char *value);
const UmiWorkbenchContextEventMetadata *
umi_workbench_context_event_find_metadata(
    const UmiWorkbenchContextEvent *event,
    const char *name);
uint64_t umi_workbench_context_event_refresh_hash(
    UmiWorkbenchContextEvent *event);

#ifdef __cplusplus
}
#endif
#endif
