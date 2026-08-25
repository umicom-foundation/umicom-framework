/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/application_surface_set.c
 *
 * PURPOSE:
 *   Implement per-application surface registration sets and cross-application compatibility checks for Umicom Desk mosaics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/application_surface_set.h"

UmiStatus umi_ws_application_surface_set_init(UmiWsApplicationSurfaceSet *set,
                                              const char *application_id,
                                              UmiWsApplicationDomain domain) {
    if (set == NULL || !umi_ws_id_valid(application_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *set = (UmiWsApplicationSurfaceSet){0};
    if (umi_ws_copy_text(set->application_id, sizeof(set->application_id), application_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    set->domain = domain;
    set->allow_cross_application_hosting = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_application_surface_set_add(UmiWsApplicationSurfaceSet *set, const char *surface_id) {
    size_t index;
    if (set == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < set->count; ++index) if (strcmp(set->surface_ids[index], surface_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (set->count >= UMI_WS_MAX_SURFACES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(set->surface_ids[set->count], sizeof(set->surface_ids[set->count]), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++set->count;
    return UMI_STATUS_OK;
}

bool umi_ws_application_surface_set_contains(const UmiWsApplicationSurfaceSet *set, const char *surface_id) {
    size_t index;
    if (set == NULL || !umi_ws_id_valid(surface_id)) return false;
    for (index = 0U; index < set->count; ++index) if (strcmp(set->surface_ids[index], surface_id) == 0) return true;
    return false;
}

bool umi_ws_application_surface_set_can_host(const UmiWsApplicationSurfaceSet *host,
                                             const UmiWsSurfaceDescriptor *surface) {
    if (host == NULL || surface == NULL || umi_ws_surface_descriptor_validate(surface) != UMI_STATUS_OK) return false;
    if (surface->domain == host->domain || surface->domain == UMI_WS_DOMAIN_GENERIC) return true;
    return host->allow_cross_application_hosting;
}
