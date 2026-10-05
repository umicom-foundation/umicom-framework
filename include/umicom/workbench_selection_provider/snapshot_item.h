/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/snapshot_item.h
 *
 * PURPOSE:
 *   Define the reusable provider snapshot item contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SNAPSHOT_ITEM_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SNAPSHOT_ITEM_H

#include "umicom/workbench_selection_provider/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider snapshot item data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSnapshotItem {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderSnapshotItem;

/**
 * Initialise workbench selection provider snapshot item from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_snapshot_item_init(
    UmiWorkbenchSelectionProviderSnapshotItem *record,
    const char *record_id);
/**
 * Check that workbench selection provider snapshot item satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_snapshot_item_validate(
    const UmiWorkbenchSelectionProviderSnapshotItem *record);
/**
 * Provide the workbench selection provider snapshot item set provider operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_snapshot_item_set_provider(
    UmiWorkbenchSelectionProviderSnapshotItem *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider snapshot item set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_snapshot_item_set_source(
    UmiWorkbenchSelectionProviderSnapshotItem *record,
    const char *source_id);
/**
 * Provide the workbench selection provider snapshot item set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_snapshot_item_set_subject(
    UmiWorkbenchSelectionProviderSnapshotItem *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider snapshot item set related operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_snapshot_item_set_related(
    UmiWorkbenchSelectionProviderSnapshotItem *record,
    const char *related_id);
/**
 * Provide the workbench selection provider snapshot item set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_snapshot_item_set_group(
    UmiWorkbenchSelectionProviderSnapshotItem *record,
    const char *group_id);
/**
 * Provide the workbench selection provider snapshot item set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_snapshot_item_set_description(
    UmiWorkbenchSelectionProviderSnapshotItem *record,
    const char *description);
/**
 * Provide the workbench selection provider snapshot item hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_selection_provider_snapshot_item_hash(
    const UmiWorkbenchSelectionProviderSnapshotItem *record);
/**
 * Provide the workbench selection provider snapshot item touch operation used by this
 * module and its client applications.
 */
void umi_workbench_selection_provider_snapshot_item_touch(
    UmiWorkbenchSelectionProviderSnapshotItem *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_selection_provider_snapshot_item_archive_encode(const UmiWorkbenchSelectionProviderSnapshotItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_selection_provider_snapshot_item_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchSelectionProviderSnapshotItem *value);

#ifdef __cplusplus
}
#endif
#endif
