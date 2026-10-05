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
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection media item data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench selection media item from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_media_item_init(
    UmiWorkbenchSelectionMediaItem *record,
    const char *record_id);
/**
 * Check that workbench selection media item satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_selection_media_item_validate(
    const UmiWorkbenchSelectionMediaItem *record);
/**
 * Provide the workbench selection media item set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_media_item_set_source(
    UmiWorkbenchSelectionMediaItem *record,
    const char *source_id);
/**
 * Provide the workbench selection media item set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_media_item_set_subject(
    UmiWorkbenchSelectionMediaItem *record,
    const char *subject_id);
/**
 * Provide the workbench selection media item set secondary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_media_item_set_secondary(
    UmiWorkbenchSelectionMediaItem *record,
    const char *secondary_id);
/**
 * Provide the workbench selection media item set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_media_item_set_group(
    UmiWorkbenchSelectionMediaItem *record,
    const char *group_id);
/**
 * Provide the workbench selection media item set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_selection_media_item_set_label(
    UmiWorkbenchSelectionMediaItem *record,
    const char *label);
/**
 * Provide the workbench selection media item hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_media_item_hash(
    const UmiWorkbenchSelectionMediaItem *record);
/**
 * Provide the workbench selection media item touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_media_item_touch(
    UmiWorkbenchSelectionMediaItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_media_item_archive_encode(const UmiWorkbenchSelectionMediaItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_media_item_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionMediaItem *value);

#ifdef __cplusplus
}
#endif
#endif
