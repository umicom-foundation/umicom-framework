/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/surface_descriptor.c
 *
 * PURPOSE:
 *   Implement reusable semantic metadata for dockable workstation surfaces across every Umicom application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/surface_descriptor.h"

UmiStatus umi_ws_surface_descriptor_init(UmiWsSurfaceDescriptor *descriptor,
                                         const char *surface_id,
                                         const char *label,
                                         UmiWsApplicationDomain domain,
                                         UmiWsSurfaceKind kind) {
    UmiStatus status;
    if (descriptor == NULL || !umi_ws_id_valid(surface_id) || label == NULL || label[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *descriptor = (UmiWsSurfaceDescriptor){0};
    descriptor->api_version = UMI_WS_API_VERSION;
    status = umi_ws_copy_text(descriptor->surface_id, sizeof(descriptor->surface_id), surface_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ws_copy_text(descriptor->label, sizeof(descriptor->label), label);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ws_copy_text(descriptor->category, sizeof(descriptor->category), "General");
    if (status != UMI_STATUS_OK) return status;
    status = umi_ws_copy_text(descriptor->icon_name, sizeof(descriptor->icon_name), "view-grid-symbolic");
    if (status != UMI_STATUS_OK) return status;
    descriptor->domain = domain;
    descriptor->kind = kind;
    descriptor->default_region = UMI_WS_DOCK_CENTRE;
    descriptor->minimum_width = 160;
    descriptor->minimum_height = 120;
    descriptor->closable = true;
    descriptor->movable = true;
    descriptor->detachable = true;
    descriptor->multi_instance = false;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_surface_descriptor_validate(const UmiWsSurfaceDescriptor *descriptor) {
    if (descriptor == NULL || descriptor->api_version != UMI_WS_API_VERSION) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_ws_id_valid(descriptor->surface_id) || descriptor->label[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (descriptor->kind < UMI_WS_SURFACE_PANEL || descriptor->kind > UMI_WS_SURFACE_TOOLBAR) return UMI_STATUS_INVALID_ARGUMENT;
    if (descriptor->default_region < UMI_WS_DOCK_LEFT || descriptor->default_region > UMI_WS_DOCK_FLOATING) return UMI_STATUS_INVALID_ARGUMENT;
    if (descriptor->minimum_width < 0 || descriptor->minimum_height < 0) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
