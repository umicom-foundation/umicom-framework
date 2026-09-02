/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/risk_limit.c
 *
 * PURPOSE:
 *   Validate risk-limit configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of risk limit. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/risk_limit.h"
/* Check that risk limit satisfies its contract before another service relies on it. */
int umi_risk_limit_valid(const UmiRiskLimit *l){return l!=NULL&&l->max_order_quantity>0.0&&l->max_order_notional>0.0&&l->max_position_quantity>0.0&&l->max_daily_loss>=0.0;}
