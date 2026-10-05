/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/factory_record.h
 *
 * PURPOSE:
 *   Define cross-application panel factory record state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_FACTORY_RECORD_H
#define UMICOM_CROSS_APPLICATION_PANEL_FACTORY_RECORD_H
#include "umicom/cross_application_panel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel factory record data shared with callers of this public contract.
 */
typedef struct UmiPanelFactoryRecord {
    uint32_t structure_size;
    char factory_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char provider_id[UMI_PANEL_TEXT_CAPACITY];
    char component_id[UMI_PANEL_TEXT_CAPACITY];
    bool enabled;
    uint32_t priority;
    uint64_t revision;
} UmiPanelFactoryRecord;
/**
 * Represent the panel factory record store data shared with callers of this public
 * contract.
 */
typedef struct UmiPanelFactoryRecordStore { UmiPanelFactoryRecord items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelFactoryRecordStore;
/**
 * Initialise panel factory record from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_factory_record_init(UmiPanelFactoryRecord *record);
/**
 * Check that panel factory record satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_panel_factory_record_validate(const UmiPanelFactoryRecord *record);
/**
 * Initialise panel factory record store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_factory_record_store_init(UmiPanelFactoryRecordStore *store);
/**
 * Provide the panel factory record store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_factory_record_store_put(UmiPanelFactoryRecordStore *store,const UmiPanelFactoryRecord *record);
/**
 * Remove panel factory record store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_factory_record_store_remove(UmiPanelFactoryRecordStore *store,const char *identity);
/**
 * Find panel factory record store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelFactoryRecord *umi_panel_factory_record_store_find(UmiPanelFactoryRecordStore *store,const char *identity);
/**
 * Provide the panel factory record store find const operation used by this module and its
 * client applications.
 */
const UmiPanelFactoryRecord *umi_panel_factory_record_store_find_const(const UmiPanelFactoryRecordStore *store,const char *identity);
/**
 * Provide the panel factory record store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_factory_record_store_snapshot(const UmiPanelFactoryRecordStore *store,UmiPanelFactoryRecord *records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_panel_factory_record_archive_encode(const UmiPanelFactoryRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_panel_factory_record_archive_decode(const void *bytes, size_t byte_count,
    UmiPanelFactoryRecord *value);

#ifdef __cplusplus
}
#endif
#endif
