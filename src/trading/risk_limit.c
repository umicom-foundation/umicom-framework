/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/risk_limit.c
 *
 * PURPOSE:
 *   Validate risk-limit configuration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of risk limit. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/risk_limit.h"
int umi_risk_limit_valid(const UmiRiskLimit *l){return l!=NULL&&l->max_order_quantity>0.0&&l->max_order_notional>0.0&&l->max_position_quantity>0.0&&l->max_daily_loss>=0.0;}
