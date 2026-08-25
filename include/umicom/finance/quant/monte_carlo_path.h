/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/monte_carlo_path.h
 *
 * PURPOSE:
 *   Generate one geometric-Brownian terminal path from an explicit normal draw.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_MONTE_CARLO_PATH_H
#define INCLUDE_UMICOM_FINANCE_QUANT_MONTE_CARLO_PATH_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generate terminal geometric-Brownian value from one standard-normal draw. */
UmiStatus umi_quant_monte_carlo_path_terminal(double spot, double drift, double volatility, double years, double normal_draw, double *out_terminal);

#ifdef __cplusplus
}
#endif

#endif
