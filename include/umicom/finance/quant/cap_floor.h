/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/cap_floor.h
 *
 * PURPOSE:
 *   Represent a single caplet or floorlet payoff.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CAP_FLOOR_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CAP_FLOOR_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant cap floor data shared with callers of this public contract.
 */
typedef struct UmiQuantCapFloor {
    double notional;
    double strike_rate;
    double observed_rate;
    double year_fraction;
    int32_t right;
} UmiQuantCapFloor;

/* Initialise and validate the cap floor contract. */
UmiStatus umi_quant_cap_floor_init(UmiQuantCapFloor *record, double notional, double strike_rate, double observed_rate, double year_fraction, int32_t right);

/* Return undiscounted caplet or floorlet payoff. */
double umi_quant_cap_floor_payoff(const UmiQuantCapFloor *record);

#ifdef __cplusplus
}
#endif

#endif
