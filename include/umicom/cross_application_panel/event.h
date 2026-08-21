/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/event.h
 *
 * PURPOSE:
 *   Define cross-application panel event state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_EVENT_H
#define UMICOM_CROSS_APPLICATION_PANEL_EVENT_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
typedef struct UmiPanelEventStore { UmiPanelEvent items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelEventStore;
void umi_panel_event_init(UmiPanelEvent *record);
UmiStatus umi_panel_event_validate(const UmiPanelEvent *record);
void umi_panel_event_store_init(UmiPanelEventStore *store);
UmiStatus umi_panel_event_store_put(UmiPanelEventStore *store,const UmiPanelEvent *record);
UmiStatus umi_panel_event_store_remove(UmiPanelEventStore *store,const char *identity);
UmiPanelEvent *umi_panel_event_store_find(UmiPanelEventStore *store,const char *identity);
const UmiPanelEvent *umi_panel_event_store_find_const(const UmiPanelEventStore *store,const char *identity);
UmiStatus umi_panel_event_store_snapshot(const UmiPanelEventStore *store,UmiPanelEvent *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
