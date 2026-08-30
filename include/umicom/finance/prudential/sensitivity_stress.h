/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/sensitivity_stress.h
 *
 * PURPOSE:
 *   Calculate stressed value from a sensitivity and market-factor shock.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_SENSITIVITY_STRESS_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_SENSITIVITY_STRESS_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialSensitivityStress { double base_value; double sensitivity; double factor_shock; double stressed_value; } UmiPrudentialSensitivityStress;
/* Apply a first-order sensitivity shock to a base prudential value. */
UmiStatus umi_pru_sensitivity_stress_calculate(UmiPrudentialSensitivityStress *result, double base_value, double sensitivity, double factor_shock);

#ifdef __cplusplus
}
#endif

#endif
