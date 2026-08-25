/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/surface_catalogue.h
 *
 * PURPOSE:
 *   bounded catalogue of semantic surface requirements shared by all frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcSurfaceCatalogue { UmiFcSurfaceRequirement items[UMI_FC_MAX_ITEMS]; size_t count; } UmiFcSurfaceCatalogue;
void umi_fc_surface_catalogue_init(UmiFcSurfaceCatalogue *catalogue);
UmiStatus umi_fc_surface_catalogue_add(UmiFcSurfaceCatalogue *catalogue,const UmiFcSurfaceRequirement *requirement);
UmiStatus umi_fc_surface_catalogue_find(const UmiFcSurfaceCatalogue *catalogue,const char *surface_id,UmiFcSurfaceRequirement *out_requirement);

#ifdef __cplusplus
}
#endif
#endif
