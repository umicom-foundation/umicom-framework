/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/cross_application_panel/session.h
 *
 * PURPOSE:
 *   Define cross-application panel session state and bounded storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CROSS_APPLICATION_PANEL_SESSION_H
#define UMICOM_CROSS_APPLICATION_PANEL_SESSION_H
#include "umicom/cross_application_panel/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the panel session data shared with callers of this public contract.
 */
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
/**
 * Represent the panel session store data shared with callers of this public contract.
 */
typedef struct UmiPanelSessionStore { UmiPanelSession items[UMI_PANEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiPanelSessionStore;
/**
 * Initialise panel session from caller-provided values so later operations receive a known
 * state.
 */
void umi_panel_session_init(UmiPanelSession *record);
/**
 * Check that panel session satisfies its contract before another service relies on it.
 */
UmiStatus umi_panel_session_validate(const UmiPanelSession *record);
/**
 * Initialise panel session store from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_session_store_init(UmiPanelSessionStore *store);
/**
 * Provide the panel session store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_session_store_put(UmiPanelSessionStore *store,const UmiPanelSession *record);
/**
 * Remove panel session store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_session_store_remove(UmiPanelSessionStore *store,const char *identity);
/**
 * Find panel session store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelSession *umi_panel_session_store_find(UmiPanelSessionStore *store,const char *identity);
/**
 * Provide the panel session store find const operation used by this module and its client
 * applications.
 */
const UmiPanelSession *umi_panel_session_store_find_const(const UmiPanelSessionStore *store,const char *identity);
/**
 * Provide the panel session store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_session_store_snapshot(const UmiPanelSessionStore *store,UmiPanelSession *records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
