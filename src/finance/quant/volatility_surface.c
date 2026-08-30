/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/volatility_surface.c
 *
 * PURPOSE:
 *   Store bounded volatility points and select nearest observations.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/volatility_surface.h"

#include <math.h>
#include <string.h>

void umi_quant_volatility_surface_init(UmiQuantVolatilitySurface *surface) { if (surface != NULL) memset(surface,0,sizeof *surface); }
UmiStatus umi_quant_volatility_surface_add(UmiQuantVolatilitySurface *surface, double expiry_years, double strike, double volatility)
{
    if (surface == NULL || expiry_years < 0.0 || strike < 0.0 || volatility < 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    if (surface->count >= UMI_QUANT_MAX_SURFACE_POINTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    surface->points[surface->count++] = (UmiQuantVolatilitySurfacePoint){expiry_years,strike,volatility};
    return UMI_STATUS_OK;
}
UmiStatus umi_quant_volatility_surface_nearest(const UmiQuantVolatilitySurface *surface, double expiry_years, double strike, double *out_volatility)
{
    size_t i,best=0U; double best_distance;
    if (surface == NULL || out_volatility == NULL || surface->count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    best_distance=fabs(surface->points[0].expiry_years-expiry_years)+fabs(surface->points[0].strike-strike);
    for(i=1U;i<surface->count;++i){ double d=fabs(surface->points[i].expiry_years-expiry_years)+fabs(surface->points[i].strike-strike); if(d<best_distance){best=i;best_distance=d;} }
    *out_volatility=surface->points[best].volatility; return UMI_STATUS_OK;
}
