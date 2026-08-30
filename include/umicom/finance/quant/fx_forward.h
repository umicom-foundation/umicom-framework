/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/fx_forward.h
 *
 * PURPOSE:
 *   Model an FX forward using spot, forward points and maturity.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_FX_FORWARD_H
#define INCLUDE_UMICOM_FINANCE_QUANT_FX_FORWARD_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantFxForward {
    double spot;
    double forward_points;
    int32_t maturity_days;
} UmiQuantFxForward;

/* Initialise and validate the fx forward contract. */
UmiStatus umi_quant_fx_forward_init(UmiQuantFxForward *record, double spot, double forward_points, int32_t maturity_days);

/* Return the outright forward rate represented by spot plus forward points. */
double umi_quant_fx_forward_outright(const UmiQuantFxForward *record);

#ifdef __cplusplus
}
#endif

#endif
