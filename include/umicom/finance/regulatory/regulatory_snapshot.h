/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/regulatory_snapshot.h
 *
 * PURPOSE:
 *   Aggregate report, submission, capital and liquidity readiness evidence.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_SNAPSHOT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REGULATORY_SNAPSHOT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the regulatory snapshot data shared with callers of this public contract.
 */
typedef struct UmiRegulatorySnapshot { size_t reports_due; size_t reports_ready; size_t rejected_submissions; double capital_ratio; double liquidity_ratio; } UmiRegulatorySnapshot;
/* Initialise a regulatory snapshot with bounded counts and finite ratios. */
UmiStatus umi_reg_regulatory_snapshot_init(UmiRegulatorySnapshot *snapshot,size_t reports_due,size_t reports_ready,size_t rejected,double capital_ratio,double liquidity_ratio);
/* Return one when all due reports are ready and both regulatory ratios meet minima. */
int umi_reg_regulatory_snapshot_ready(const UmiRegulatorySnapshot *snapshot,double minimum_capital,double minimum_liquidity);

#ifdef __cplusplus
}
#endif

#endif
