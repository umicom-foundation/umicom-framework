/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/greek_set.h
 *
 * PURPOSE:
 *   Store first-order and second-order option sensitivity evidence.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_GREEK_SET_H
#define INCLUDE_UMICOM_FINANCE_QUANT_GREEK_SET_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantGreekSet {
    double delta;
    double gamma;
    double vega;
    double theta;
    double rho;
} UmiQuantGreekSet;

/* Initialise and validate the greek set contract. */
UmiStatus umi_quant_greek_set_init(UmiQuantGreekSet *record, double delta, double gamma, double vega, double theta, double rho);

/* Return aggregate absolute first-order risk magnitude. */
double umi_quant_greek_set_absolute_first_order(const UmiQuantGreekSet *record);

#ifdef __cplusplus
}
#endif

#endif
