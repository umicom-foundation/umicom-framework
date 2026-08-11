/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/fee.c
 *
 * PURPOSE:
 *   Calculate simple quantity-based execution fees.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of fee. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/fee.h"
double umi_execution_fee(double quantity,double per_unit_fee,double minimum_fee){double fee=quantity>0.0&&per_unit_fee>0.0?quantity*per_unit_fee:0.0;return fee<minimum_fee?minimum_fee:fee;}
