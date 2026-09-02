/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/pnl_explain.c
 *
 * PURPOSE:
 *   Decompose P&L into market, carry and unexplained components.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/pnl_explain.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_pnl_explain_init(UmiQuantPnlExplain *record, double market_pnl, double carry_pnl, double total_pnl)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(umi_quant_number_valid(market_pnl) && umi_quant_number_valid(carry_pnl) && umi_quant_number_valid(total_pnl))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->market_pnl = market_pnl;
    record->carry_pnl = carry_pnl;
    record->total_pnl = total_pnl;
    return UMI_STATUS_OK;
}

/* Return residual P&L not explained by market and carry components. */
double umi_quant_pnl_explain_unexplained(const UmiQuantPnlExplain *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->total_pnl - record->market_pnl - record->carry_pnl;
}
