/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/pnl_attribution.c
 *
 * PURPOSE:
 *   Decompose P&L into market, carry, new-trade and residual components.
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

#include "umicom/finance/enterprise/pnl_attribution.h"

UmiStatus umi_enterprise_pnl_attribution_calculate(UmiEnterprisePnlAttribution *a,double total,double market,double carry,double newtrades){if(a==NULL||!umi_quant_number_valid(total)||!umi_quant_number_valid(market)||!umi_quant_number_valid(carry)||!umi_quant_number_valid(newtrades))return UMI_STATUS_INVALID_ARGUMENT;a->market=market;a->carry=carry;a->new_trades=newtrades;a->total=total;a->residual=total-market-carry-newtrades;return UMI_STATUS_OK;}
