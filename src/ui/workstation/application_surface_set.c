/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/application_surface_set.c
 *
 * PURPOSE:
 *   Implement per-application surface registration sets and cross-application compatibility checks for Umicom Desk mosaics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/application_surface_set.h"

/*
 * Initialise ws application surface set from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ws_application_surface_set_init(UmiWsApplicationSurfaceSet *set,
                                              const char *application_id,
                                              UmiWsApplicationDomain domain) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || !umi_ws_id_valid(application_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *set = (UmiWsApplicationSurfaceSet){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(set->application_id, sizeof(set->application_id), application_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    set->domain = domain;
    set->allow_cross_application_hosting = true;
    return UMI_STATUS_OK;
}

/*
 * Add ws application surface set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ws_application_surface_set_add(UmiWsApplicationSurfaceSet *set, const char *surface_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(set->surface_ids[index], surface_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_WS_MAX_SURFACES) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_copy_text(set->surface_ids[set->count], sizeof(set->surface_ids[set->count]), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++set->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws application surface set contains operation used by this module and its
 * client applications.
 */
bool umi_ws_application_surface_set_contains(const UmiWsApplicationSurfaceSet *set, const char *surface_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || !umi_ws_id_valid(surface_id)) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(set->surface_ids[index], surface_id) == 0) return true;
    return false;
}

/*
 * Provide the ws application surface set can host operation used by this module and its
 * client applications.
 */
bool umi_ws_application_surface_set_can_host(const UmiWsApplicationSurfaceSet *host,
                                             const UmiWsSurfaceDescriptor *surface) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || surface == NULL || umi_ws_surface_descriptor_validate(surface) != UMI_STATUS_OK) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (surface->domain == host->domain || surface->domain == UMI_WS_DOMAIN_GENERIC) return true;
    return host->allow_cross_application_hosting;
}
