/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/instance.h
 *
 * PURPOSE:
 *   Define cross-application panel instance state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_INSTANCE_H
#define UMICOM_CROSS_APPLICATION_PANEL_INSTANCE_H
#include "umicom/cross_application_panel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel instance data shared with callers of this public contract.
 */
typedef struct UmiPanelInstance {
    uint32_t structure_size;
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char application_id[UMI_PANEL_TEXT_CAPACITY];
    char layout_node_id[UMI_PANEL_TEXT_CAPACITY];
    char workspace_id[UMI_PANEL_TEXT_CAPACITY];
    char channel_id[UMI_PANEL_TEXT_CAPACITY];
    UmiPanelPlacement placement;
    bool visible;
    bool active;
    bool locked;
    uint64_t revision;
} UmiPanelInstance;
/**
 * Represent the panel instance store data shared with callers of this public contract.
 */
typedef struct UmiPanelInstanceStore { UmiPanelInstance items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelInstanceStore;
/**
 * Initialise panel instance from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_instance_init(UmiPanelInstance *record);
/**
 * Check that panel instance satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_instance_validate(const UmiPanelInstance *record);
/**
 * Initialise panel instance store from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_instance_store_init(UmiPanelInstanceStore *store);
/**
 * Provide the panel instance store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_instance_store_put(UmiPanelInstanceStore *store,const UmiPanelInstance *record);
/**
 * Remove panel instance store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_instance_store_remove(UmiPanelInstanceStore *store,const char *identity);
/**
 * Find panel instance store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelInstance *umi_panel_instance_store_find(UmiPanelInstanceStore *store,const char *identity);
/**
 * Provide the panel instance store find const operation used by this module and its client
 * applications.
 */
const UmiPanelInstance *umi_panel_instance_store_find_const(const UmiPanelInstanceStore *store,const char *identity);
/**
 * Provide the panel instance store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_instance_store_snapshot(const UmiPanelInstanceStore *store,UmiPanelInstance *records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_panel_instance_archive_encode(const UmiPanelInstance *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_panel_instance_archive_decode(const void *bytes, size_t byte_count,
    UmiPanelInstance *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_panel_instance_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPanelInstance *expected, UmiPanelInstance *value);


#ifdef __cplusplus
}
#endif
#endif
