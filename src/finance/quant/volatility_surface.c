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

/*
 * Initialise quant volatility surface from caller-provided values so later operations
 * receive a known state.
 */
void umi_quant_volatility_surface_init(UmiQuantVolatilitySurface *surface) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (surface != NULL) memset(surface,0,sizeof *surface); }
/*
 * Add quant volatility surface only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_quant_volatility_surface_add(UmiQuantVolatilitySurface *surface, double expiry_years, double strike, double volatility)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || expiry_years < 0.0 || strike < 0.0 || volatility < 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (surface->count >= UMI_QUANT_MAX_SURFACE_POINTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    surface->points[surface->count++] = (UmiQuantVolatilitySurfacePoint){expiry_years,strike,volatility};
    return UMI_STATUS_OK;
}
/*
 * Provide the quant volatility surface nearest operation used by this module and its
 * client applications.
 */
UmiStatus umi_quant_volatility_surface_nearest(const UmiQuantVolatilitySurface *surface, double expiry_years, double strike, double *out_volatility)
{
    size_t i,best=0U; double best_distance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || out_volatility == NULL || surface->count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    best_distance=fabs(surface->points[0].expiry_years-expiry_years)+fabs(surface->points[0].strike-strike);
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=1U;i<surface->count;++i){ double d=fabs(surface->points[i].expiry_years-expiry_years)+fabs(surface->points[i].strike-strike); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d<best_distance){best=i;best_distance=d;} }
    *out_volatility=surface->points[best].volatility; return UMI_STATUS_OK;
}
