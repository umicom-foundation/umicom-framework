/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/liquidity_outflow.h
 *
 * PURPOSE:
 *   Calculate stressed liquidity outflow after applying run-off rate.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_OUTFLOW_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_LIQUIDITY_OUTFLOW_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialLiquidityOutflow { char item_id[UMI_PRU_ID_CAPACITY]; double base_amount; double factor; double stressed_amount; } UmiPrudentialLiquidityOutflow;
/* Calculate a stressed liquidity or stable-funding amount from a bounded factor. */
UmiStatus umi_pru_liquidity_outflow_calculate(UmiPrudentialLiquidityOutflow *result, const char *item_id, double base_amount, double factor);

#ifdef __cplusplus
}
#endif

#endif
