/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/surface_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded reusable catalogue for Framework-owned workstation surface descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/surface_catalogue.h"

/*
 * Initialise ws surface catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_ws_surface_catalogue_init(UmiWsSurfaceCatalogue *catalogue) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) *catalogue = (UmiWsSurfaceCatalogue){0};
}

/*
 * Provide the ws surface catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_surface_catalogue_upsert(UmiWsSurfaceCatalogue *catalogue,
                                          const UmiWsSurfaceDescriptor *descriptor) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || umi_ws_surface_descriptor_validate(descriptor) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].surface_id, descriptor->surface_id) == 0) {
            catalogue->items[index] = *descriptor;
            ++catalogue->revision;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_WS_MAX_SURFACES) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->items[catalogue->count++] = *descriptor;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

/*
 * Find ws surface catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsSurfaceDescriptor *umi_ws_surface_catalogue_find(const UmiWsSurfaceCatalogue *catalogue,
                                                            const char *surface_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || !umi_ws_id_valid(surface_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].surface_id, surface_id) == 0) return &catalogue->items[index];
    }
    return NULL;
}

/*
 * Provide the ws surface catalogue count domain operation used by this module and its
 * client applications.
 */
size_t umi_ws_surface_catalogue_count_domain(const UmiWsSurfaceCatalogue *catalogue,
                                             UmiWsApplicationDomain domain) {
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalogue->items[index].domain == domain || catalogue->items[index].domain == UMI_WS_DOMAIN_GENERIC) ++count;
    }
    return count;
}
