/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/permission.h
 *
 * PURPOSE:
 *   Define the reusable event permission contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PERMISSION_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PERMISSION_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventPermission {
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
} UmiWorkbenchContextEventPermission;

void umi_workbench_context_event_permission_init(
    UmiWorkbenchContextEventPermission *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_permission_validate(
    const UmiWorkbenchContextEventPermission *record);
UmiStatus umi_workbench_context_event_permission_set_source(
    UmiWorkbenchContextEventPermission *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_permission_set_subject(
    UmiWorkbenchContextEventPermission *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_permission_set_group(
    UmiWorkbenchContextEventPermission *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_permission_set_label(
    UmiWorkbenchContextEventPermission *record,
    const char *label);
uint64_t umi_workbench_context_event_permission_hash(
    const UmiWorkbenchContextEventPermission *record);
void umi_workbench_context_event_permission_touch(
    UmiWorkbenchContextEventPermission *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
