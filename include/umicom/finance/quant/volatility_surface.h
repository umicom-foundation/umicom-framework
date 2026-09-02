/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/volatility_surface.h
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_VOLATILITY_SURFACE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_VOLATILITY_SURFACE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant volatility surface point data shared with callers of this public
 * contract.
 */
typedef struct UmiQuantVolatilitySurfacePoint { double expiry_years; double strike; double volatility; } UmiQuantVolatilitySurfacePoint;
/**
 * Represent the quant volatility surface data shared with callers of this public contract.
 */
typedef struct UmiQuantVolatilitySurface { UmiQuantVolatilitySurfacePoint points[UMI_QUANT_MAX_SURFACE_POINTS]; size_t count; } UmiQuantVolatilitySurface;
/* Initialise an empty volatility surface. */
void umi_quant_volatility_surface_init(UmiQuantVolatilitySurface *surface);
/* Add one validated volatility observation. */
UmiStatus umi_quant_volatility_surface_add(UmiQuantVolatilitySurface *surface, double expiry_years, double strike, double volatility);
/* Return the nearest stored volatility by normalised expiry/strike distance. */
UmiStatus umi_quant_volatility_surface_nearest(const UmiQuantVolatilitySurface *surface, double expiry_years, double strike, double *out_volatility);

#ifdef __cplusplus
}
#endif

#endif
