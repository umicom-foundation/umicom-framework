/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/stress_path.h
 *
 * PURPOSE:
 *   Store a bounded time path of stress multipliers.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_PATH_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_STRESS_PATH_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialStressPath { char path_id[UMI_PRU_ID_CAPACITY]; double multipliers[UMI_PRU_MAX_PERIODS]; size_t count; } UmiPrudentialStressPath;
/* Append a non-negative stress multiplier to a bounded time path. */
UmiStatus umi_pru_stress_path_add(UmiPrudentialStressPath *path, double multiplier);

#ifdef __cplusplus
}
#endif

#endif
