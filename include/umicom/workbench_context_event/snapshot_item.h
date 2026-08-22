/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/snapshot_item.h
 *
 * PURPOSE:
 *   Define the reusable event snapshot item contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_SNAPSHOT_ITEM_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_SNAPSHOT_ITEM_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventSnapshotItem {
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
} UmiWorkbenchContextEventSnapshotItem;

void umi_workbench_context_event_snapshot_item_init(
    UmiWorkbenchContextEventSnapshotItem *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_snapshot_item_validate(
    const UmiWorkbenchContextEventSnapshotItem *record);
UmiStatus umi_workbench_context_event_snapshot_item_set_source(
    UmiWorkbenchContextEventSnapshotItem *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_snapshot_item_set_subject(
    UmiWorkbenchContextEventSnapshotItem *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_snapshot_item_set_group(
    UmiWorkbenchContextEventSnapshotItem *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_snapshot_item_set_label(
    UmiWorkbenchContextEventSnapshotItem *record,
    const char *label);
uint64_t umi_workbench_context_event_snapshot_item_hash(
    const UmiWorkbenchContextEventSnapshotItem *record);
void umi_workbench_context_event_snapshot_item_touch(
    UmiWorkbenchContextEventSnapshotItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
