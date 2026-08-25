/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/option_pricer.h
 *
 * PURPOSE:
 *   Provide intrinsic and time-value decomposition for vanilla options.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_OPTION_PRICER_H
#define INCLUDE_UMICOM_FINANCE_QUANT_OPTION_PRICER_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return vanilla intrinsic value. */
double umi_quant_option_pricer_intrinsic(double spot, double strike, UmiQuantOptionRight right);
/* Return non-negative model time value from a total option value. */
double umi_quant_option_pricer_time_value(double total_value, double spot, double strike, UmiQuantOptionRight right);

#ifdef __cplusplus
}
#endif

#endif
