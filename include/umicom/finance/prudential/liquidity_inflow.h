/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_inflow.h
 *
 * PURPOSE:
 *   Calculate stressed liquidity inflow after applying recognition rate.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_INFLOW_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_INFLOW_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLiquidityInflow { char item_id[UMI_PRU_ID_CAPACITY]; double base_amount; double factor; double stressed_amount; } UmiPrudentialLiquidityInflow;
/* Calculate a stressed liquidity or stable-funding amount from a bounded factor. */
UmiStatus umi_pru_liquidity_inflow_calculate(UmiPrudentialLiquidityInflow *result, const char *item_id, double base_amount, double factor);

#ifdef __cplusplus
}
#endif

#endif
