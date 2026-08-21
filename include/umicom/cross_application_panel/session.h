/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/session.h
 *
 * PURPOSE:
 *   Define cross-application panel session state and bounded storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_SESSION_H
#define UMICOM_CROSS_APPLICATION_PANEL_SESSION_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPanelSession {
    uint32_t structure_size;
    char session_id[UMI_PANEL_TEXT_CAPACITY];
    char workspace_id[UMI_PANEL_TEXT_CAPACITY];
    char active_instance_id[UMI_PANEL_TEXT_CAPACITY];
    char layout_id[UMI_PANEL_TEXT_CAPACITY];
    uint32_t panel_count;
    bool clean_shutdown;
    uint64_t revision;
} UmiPanelSession;
typedef struct UmiPanelSessionStore { UmiPanelSession items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelSessionStore;
void umi_panel_session_init(UmiPanelSession *record);
UmiStatus umi_panel_session_validate(const UmiPanelSession *record);
void umi_panel_session_store_init(UmiPanelSessionStore *store);
UmiStatus umi_panel_session_store_put(UmiPanelSessionStore *store,const UmiPanelSession *record);
UmiStatus umi_panel_session_store_remove(UmiPanelSessionStore *store,const char *identity);
UmiPanelSession *umi_panel_session_store_find(UmiPanelSessionStore *store,const char *identity);
const UmiPanelSession *umi_panel_session_store_find_const(const UmiPanelSessionStore *store,const char *identity);
UmiStatus umi_panel_session_store_snapshot(const UmiPanelSessionStore *store,UmiPanelSession *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
