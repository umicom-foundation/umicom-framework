/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/pnl_attribution.h
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PNL_ATTRIBUTION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PNL_ATTRIBUTION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise pnl attribution data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterprisePnlAttribution { double market; double carry; double new_trades; double residual; double total; } UmiEnterprisePnlAttribution;
/* Build a reconciled P&L attribution whose residual closes to the actual total. */
UmiStatus umi_enterprise_pnl_attribution_calculate(UmiEnterprisePnlAttribution *attribution,double actual_total,double market,double carry,double new_trades);

#ifdef __cplusplus
}
#endif

#endif
