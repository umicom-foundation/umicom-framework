/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/prudential_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate capital, liquidity and stress readiness evidence.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_SNAPSHOT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_SNAPSHOT_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential snapshot data shared with callers of this public contract.
 */
typedef struct UmiPrudentialSnapshot { uint64_t as_of_millis; double cet1_ratio; double total_capital_ratio; double lcr; double nsfr; size_t failed_stress_metrics; size_t breached_limits; } UmiPrudentialSnapshot;
/* Initialise an aggregate prudential snapshot with core capital and liquidity metrics. */
UmiStatus umi_pru_prudential_snapshot_init(UmiPrudentialSnapshot *snapshot, uint64_t as_of_millis, double cet1_ratio, double total_capital_ratio, double lcr, double nsfr, size_t failed_stress_metrics, size_t breached_limits);

#ifdef __cplusplus
}
#endif

#endif
