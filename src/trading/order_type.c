/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_type.c
 *
 * PURPOSE:
 *   Describe whether an order type requires limit or stop prices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of order type. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_type.h"
/*
 * Provide the order type requires limit operation used by this module and its client
 * applications.
 */
int umi_order_type_requires_limit(UmiOrderType type){return type==UMI_ORDER_LIMIT||type==UMI_ORDER_STOP_LIMIT;}
/*
 * Provide the order type requires stop operation used by this module and its client
 * applications.
 */
int umi_order_type_requires_stop(UmiOrderType type){return type==UMI_ORDER_STOP||type==UMI_ORDER_STOP_LIMIT;}
