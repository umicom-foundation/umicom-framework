/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/event.h
 *
 * PURPOSE:
 *   Define cross-application panel event state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_EVENT_H
#define UMICOM_CROSS_APPLICATION_PANEL_EVENT_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel event data shared with callers of this public contract.
 */
typedef struct UmiPanelEvent {
    uint32_t structure_size;
    char event_id[UMI_PANEL_TEXT_CAPACITY];
    char panel_id[UMI_PANEL_TEXT_CAPACITY];
    char instance_id[UMI_PANEL_TEXT_CAPACITY];
    char event_type[UMI_PANEL_TEXT_CAPACITY];
    char context_id[UMI_PANEL_TEXT_CAPACITY];
    uint64_t timestamp_ms;
    UmiStatus status;
    uint64_t revision;
} UmiPanelEvent;
/**
 * Represent the panel event store data shared with callers of this public contract.
 */
typedef struct UmiPanelEventStore { UmiPanelEvent items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelEventStore;
/**
 * Initialise panel event from caller-provided values so later operations receive a known
 * state.
 */
void umi_panel_event_init(UmiPanelEvent *record);
/**
 * Check that panel event satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_event_validate(const UmiPanelEvent *record);
/**
 * Initialise panel event store from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_event_store_init(UmiPanelEventStore *store);
/**
 * Provide the panel event store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_event_store_put(UmiPanelEventStore *store,const UmiPanelEvent *record);
/**
 * Remove panel event store while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_panel_event_store_remove(UmiPanelEventStore *store,const char *identity);
/**
 * Find panel event store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelEvent *umi_panel_event_store_find(UmiPanelEventStore *store,const char *identity);
/**
 * Provide the panel event store find const operation used by this module and its client
 * applications.
 */
const UmiPanelEvent *umi_panel_event_store_find_const(const UmiPanelEventStore *store,const char *identity);
/**
 * Provide the panel event store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_event_store_snapshot(const UmiPanelEventStore *store,UmiPanelEvent *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
