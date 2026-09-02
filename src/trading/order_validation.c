/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_validation.c
 *
 * PURPOSE:
 *   Apply structural and market-state checks before risk evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of order validation. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_validation.h"
#include "umicom/trading/order_request.h"
/*
 * Provide the order validate for market operation used by this module and its client
 * applications.
 */
UmiStatus umi_order_validate_for_market(const UmiOrderRequest *r,UmiMarketState state){UmiStatus s=umi_order_request_validate(r);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;return state==UMI_MARKET_OPEN?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;}
