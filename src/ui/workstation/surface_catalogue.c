/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/surface_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded reusable catalogue for Framework-owned workstation surface descriptors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/surface_catalogue.h"

void umi_ws_surface_catalogue_init(UmiWsSurfaceCatalogue *catalogue) {
    if (catalogue != NULL) *catalogue = (UmiWsSurfaceCatalogue){0};
}

UmiStatus umi_ws_surface_catalogue_upsert(UmiWsSurfaceCatalogue *catalogue,
                                          const UmiWsSurfaceDescriptor *descriptor) {
    size_t index;
    if (catalogue == NULL || umi_ws_surface_descriptor_validate(descriptor) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].surface_id, descriptor->surface_id) == 0) {
            catalogue->items[index] = *descriptor;
            ++catalogue->revision;
            return UMI_STATUS_OK;
        }
    }
    if (catalogue->count >= UMI_WS_MAX_SURFACES) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *descriptor;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

const UmiWsSurfaceDescriptor *umi_ws_surface_catalogue_find(const UmiWsSurfaceCatalogue *catalogue,
                                                            const char *surface_id) {
    size_t index;
    if (catalogue == NULL || !umi_ws_id_valid(surface_id)) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].surface_id, surface_id) == 0) return &catalogue->items[index];
    }
    return NULL;
}

size_t umi_ws_surface_catalogue_count_domain(const UmiWsSurfaceCatalogue *catalogue,
                                             UmiWsApplicationDomain domain) {
    size_t index;
    size_t count = 0U;
    if (catalogue == NULL) return 0U;
    for (index = 0U; index < catalogue->count; ++index) {
        if (catalogue->items[index].domain == domain || catalogue->items[index].domain == UMI_WS_DOMAIN_GENERIC) ++count;
    }
    return count;
}
