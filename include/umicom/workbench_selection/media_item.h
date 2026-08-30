/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/media_item.h
 *
 * PURPOSE:
 *   Define the reusable structured media item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_MEDIA_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_MEDIA_ITEM_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionMediaItem {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionMediaItem;

void umi_workbench_selection_media_item_init(
    UmiWorkbenchSelectionMediaItem *record,
    const char *record_id);
UmiStatus umi_workbench_selection_media_item_validate(
    const UmiWorkbenchSelectionMediaItem *record);
UmiStatus umi_workbench_selection_media_item_set_source(
    UmiWorkbenchSelectionMediaItem *record,
    const char *source_id);
UmiStatus umi_workbench_selection_media_item_set_subject(
    UmiWorkbenchSelectionMediaItem *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_media_item_set_secondary(
    UmiWorkbenchSelectionMediaItem *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_media_item_set_group(
    UmiWorkbenchSelectionMediaItem *record,
    const char *group_id);
UmiStatus umi_workbench_selection_media_item_set_label(
    UmiWorkbenchSelectionMediaItem *record,
    const char *label);
uint64_t umi_workbench_selection_media_item_hash(
    const UmiWorkbenchSelectionMediaItem *record);
void umi_workbench_selection_media_item_touch(
    UmiWorkbenchSelectionMediaItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
