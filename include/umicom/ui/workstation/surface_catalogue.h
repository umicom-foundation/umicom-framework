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

/**
 * Represent the ws surface catalogue data shared with callers of this public contract.
 */
typedef struct UmiWsSurfaceCatalogue {
    UmiWsSurfaceDescriptor items[UMI_WS_MAX_SURFACES];
    size_t count;
    uint64_t revision;
} UmiWsSurfaceCatalogue;

/**
 * Initialise ws surface catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_ws_surface_catalogue_init(UmiWsSurfaceCatalogue *catalogue);
/**
 * Provide the ws surface catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_surface_catalogue_upsert(UmiWsSurfaceCatalogue *catalogue,
                                          const UmiWsSurfaceDescriptor *descriptor);
/**
 * Find ws surface catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsSurfaceDescriptor *umi_ws_surface_catalogue_find(const UmiWsSurfaceCatalogue *catalogue,
                                                            const char *surface_id);
/**
 * Provide the ws surface catalogue count domain operation used by this module and its
 * client applications.
 */
size_t umi_ws_surface_catalogue_count_domain(const UmiWsSurfaceCatalogue *catalogue,
                                             UmiWsApplicationDomain domain);

#ifdef __cplusplus
}
#endif

#endif
