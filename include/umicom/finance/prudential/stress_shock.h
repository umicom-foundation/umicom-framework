/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_shock.h
 *
 * PURPOSE:
 *   Apply a percentage shock to a base prudential value.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_SHOCK_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_SHOCK_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStressShock { double base_value; double shock_rate; double stressed_value; } UmiPrudentialStressShock;
/* Apply a signed percentage shock to a finite base value. */
UmiStatus umi_pru_stress_shock_apply(UmiPrudentialStressShock *result, double base_value, double shock_rate);

#ifdef __cplusplus
}
#endif

#endif
