/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/pnl_explain.h
 *
 * PURPOSE:
 *   Decompose P&L into market, carry and unexplained components.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_PNL_EXPLAIN_H
#define INCLUDE_UMICOM_FINANCE_QUANT_PNL_EXPLAIN_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantPnlExplain {
    double market_pnl;
    double carry_pnl;
    double total_pnl;
} UmiQuantPnlExplain;

/* Initialise and validate the pnl explain contract. */
UmiStatus umi_quant_pnl_explain_init(UmiQuantPnlExplain *record, double market_pnl, double carry_pnl, double total_pnl);

/* Return residual P&L not explained by market and carry components. */
double umi_quant_pnl_explain_unexplained(const UmiQuantPnlExplain *record);

#ifdef __cplusplus
}
#endif

#endif
