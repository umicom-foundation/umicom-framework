/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/survival_horizon.h
 *
 * PURPOSE:
 *   Calculate survival horizon from daily net flows and opening liquidity.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_SURVIVAL_HORIZON_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_SURVIVAL_HORIZON_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential survival horizon data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialSurvivalHorizon { double opening_liquidity; int32_t survived_days; double closing_liquidity; int exhausted; } UmiPrudentialSurvivalHorizon;
/* Calculate the first day on which cumulative net outflows exhaust opening liquidity. */
UmiStatus umi_pru_survival_horizon_calculate(UmiPrudentialSurvivalHorizon *result, double opening_liquidity, const double *daily_net_flows, size_t count);

#ifdef __cplusplus
}
#endif

#endif
