/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_projection.h
 *
 * PURPOSE:
 *   Project a base value across a bounded stress path.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_PROJECTION_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_PROJECTION_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStressProjection { double values[UMI_PRU_MAX_PERIODS]; size_t count; } UmiPrudentialStressProjection;
/* Project a base value across a sequence of non-negative multipliers. */
UmiStatus umi_pru_stress_projection_calculate(UmiPrudentialStressProjection *result, double base_value, const double *multipliers, size_t count);

#ifdef __cplusplus
}
#endif

#endif
