/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_event.h
 *
 * PURPOSE:
 *   Capture sequence-ordered evidence for an order lifecycle transition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_EVENT_H
#define UMICOM_TRADING_CORE_ORDER_EVENT_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingOrderEvent { UmiFinancialId client_order_id; uint64_t sequence; int64_t event_time_ms; UmiTradingCoreOrderState state; } UmiTradingOrderEvent;
/* Initialise and validate capture sequence-ordered evidence for an order lifecycle transition. */
UmiStatus umi_trading_order_event_init(UmiTradingOrderEvent *value,const UmiFinancialId * client_order_id, uint64_t sequence, int64_t event_time_ms, UmiTradingCoreOrderState state);
/* Validate the invariant set for this trading record. */
bool umi_trading_order_event_valid(const UmiTradingOrderEvent *value);
#ifdef __cplusplus
}
#endif
#endif
