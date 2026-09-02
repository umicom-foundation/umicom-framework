/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_path.c
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

#include "umicom/finance/prudential/stress_path.h"

/* Add pru stress path only after its inputs and available capacity have been checked. */
UmiStatus umi_pru_stress_path_add(UmiPrudentialStressPath *path,double multiplier) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(path==NULL||!umi_pru_number_valid(multiplier)||multiplier<0.0)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(path->count>=UMI_PRU_MAX_PERIODS)return UMI_STATUS_CAPACITY_EXCEEDED; path->multipliers[path->count++]=multiplier; return UMI_STATUS_OK; }
