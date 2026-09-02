/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/surface_catalogue.h
 *
 * PURPOSE:
 *   bounded catalogue of semantic surface requirements shared by all frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_SURFACE_CATALOGUE_H
#define UMICOM_FRONTEND_CONFORMANCE_SURFACE_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/surface_requirement.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc surface catalogue data shared with callers of this public contract.
 */
typedef struct UmiFcSurfaceCatalogue { UmiFcSurfaceRequirement items[UMI_FC_MAX_ITEMS]; size_t count; } UmiFcSurfaceCatalogue;
/**
 * Initialise fc surface catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_surface_catalogue_init(UmiFcSurfaceCatalogue *catalogue);
/**
 * Add fc surface catalogue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_fc_surface_catalogue_add(UmiFcSurfaceCatalogue *catalogue,const UmiFcSurfaceRequirement *requirement);
/**
 * Find fc surface catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_fc_surface_catalogue_find(const UmiFcSurfaceCatalogue *catalogue,const char *surface_id,UmiFcSurfaceRequirement *out_requirement);

#ifdef __cplusplus
}
#endif
#endif
