/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/total_rwa.h
 *
 * PURPOSE:
 *   Aggregate credit, market and operational risk-weighted assets.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_TOTAL_RWA_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_TOTAL_RWA_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialTotalRwa { double credit_rwa; double market_rwa; double operational_rwa; double total_rwa; } UmiPrudentialTotalRwa;
/* Aggregate non-negative credit, market and operational RWA components. */
UmiStatus umi_pru_total_rwa_calculate(UmiPrudentialTotalRwa *result, double credit_rwa, double market_rwa, double operational_rwa);

#ifdef __cplusplus
}
#endif

#endif
