/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_identity.h
 *
 * PURPOSE:
 *   Provide stable client, broker and venue identifiers for one order lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_IDENTITY_H
#define UMICOM_TRADING_CORE_ORDER_IDENTITY_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the trading order identity data shared with callers of this public contract.
 */
typedef struct UmiTradingOrderIdentity { UmiFinancialId client_order_id; UmiFinancialId broker_order_id; UmiFinancialId venue_order_id; } UmiTradingOrderIdentity;
/* Initialise a client identity with optional downstream identifiers empty. */
UmiStatus umi_trading_order_identity_init(UmiTradingOrderIdentity *identity,const char *client_order_id);
/* Assign the broker identifier once routing has accepted the order. */
UmiStatus umi_trading_order_identity_set_broker(UmiTradingOrderIdentity *identity,const char *broker_order_id);
/* Assign the venue identifier once the venue acknowledges the order. */
UmiStatus umi_trading_order_identity_set_venue(UmiTradingOrderIdentity *identity,const char *venue_order_id);
#ifdef __cplusplus
}
#endif
#endif
