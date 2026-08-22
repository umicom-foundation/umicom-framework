/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/bookmark.h
 *
 * PURPOSE:
 *   Define the reusable event bookmark contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_BOOKMARK_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_BOOKMARK_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventBookmark {
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
} UmiWorkbenchContextEventBookmark;

void umi_workbench_context_event_bookmark_init(
    UmiWorkbenchContextEventBookmark *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_bookmark_validate(
    const UmiWorkbenchContextEventBookmark *record);
UmiStatus umi_workbench_context_event_bookmark_set_source(
    UmiWorkbenchContextEventBookmark *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_bookmark_set_subject(
    UmiWorkbenchContextEventBookmark *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_bookmark_set_group(
    UmiWorkbenchContextEventBookmark *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_bookmark_set_label(
    UmiWorkbenchContextEventBookmark *record,
    const char *label);
uint64_t umi_workbench_context_event_bookmark_hash(
    const UmiWorkbenchContextEventBookmark *record);
void umi_workbench_context_event_bookmark_touch(
    UmiWorkbenchContextEventBookmark *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
