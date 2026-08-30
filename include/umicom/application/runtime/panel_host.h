/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/panel_host.h
 *
 * PURPOSE:
 *   Register frontend-owned panel instances against Framework-owned panel identifiers without transferring product logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_PANEL_HOST_H
#define UMICOM_APPLICATION_RUNTIME_PANEL_HOST_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationPanelHostEntry {
    const char *panel_id;
    void *frontend_handle;
    int available;
} UmiApplicationPanelHostEntry;

typedef struct UmiApplicationPanelHost {
    uint32_t structure_size;
    UmiApplicationPanelHostEntry entries[UMI_APPLICATION_RUNTIME_MAX_PANELS];
    size_t entry_count;
} UmiApplicationPanelHost;

void umi_application_panel_host_init(UmiApplicationPanelHost *host);
UmiStatus umi_application_panel_host_register(
    UmiApplicationPanelHost *host,
    const UmiApplicationExperienceDefinition *experience,
    const char *panel_id,
    void *frontend_handle);
UmiStatus umi_application_panel_host_unregister(
    UmiApplicationPanelHost *host,
    const char *panel_id);
const UmiApplicationPanelHostEntry *umi_application_panel_host_find(
    const UmiApplicationPanelHost *host,
    const char *panel_id);

#ifdef __cplusplus
}
#endif

#endif
