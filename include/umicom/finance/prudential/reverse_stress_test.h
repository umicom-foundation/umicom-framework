/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/reverse_stress_test.h
 *
 * PURPOSE:
 *   Estimate the linear shock required to exhaust available headroom.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_REVERSE_STRESS_TEST_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_REVERSE_STRESS_TEST_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialReverseStressTest { double headroom; double exposure; double loss_per_unit_shock; double required_shock; } UmiPrudentialReverseStressTest;
/* Estimate the non-negative linear shock that would consume current prudential headroom. */
UmiStatus umi_pru_reverse_stress_test_calculate(UmiPrudentialReverseStressTest *result, double headroom, double exposure, double loss_per_unit_shock);

#ifdef __cplusplus
}
#endif

#endif
