/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/surface_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded reusable catalogue for Framework-owned workstation surface descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_SURFACE_CATALOGUE_H
#define UMICOM_UI_WORKSTATION_SURFACE_CATALOGUE_H

#include <stddef.h>
#include "umicom/ui/workstation/surface_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsSurfaceCatalogue {
    UmiWsSurfaceDescriptor items[UMI_WS_MAX_SURFACES];
    size_t count;
    uint64_t revision;
} UmiWsSurfaceCatalogue;

void umi_ws_surface_catalogue_init(UmiWsSurfaceCatalogue *catalogue);
UmiStatus umi_ws_surface_catalogue_upsert(UmiWsSurfaceCatalogue *catalogue,
                                          const UmiWsSurfaceDescriptor *descriptor);
const UmiWsSurfaceDescriptor *umi_ws_surface_catalogue_find(const UmiWsSurfaceCatalogue *catalogue,
                                                            const char *surface_id);
size_t umi_ws_surface_catalogue_count_domain(const UmiWsSurfaceCatalogue *catalogue,
                                             UmiWsApplicationDomain domain);

#ifdef __cplusplus
}
#endif

#endif
