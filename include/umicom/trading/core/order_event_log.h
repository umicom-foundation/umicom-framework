/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_event_log.h
 *
 * PURPOSE:
 *   Maintain a bounded monotonic lifecycle event log per trading service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_EVENT_LOG_H
#define UMICOM_TRADING_CORE_ORDER_EVENT_LOG_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/order_event.h"
typedef struct UmiTradingOrderEventLog { UmiTradingOrderEvent events[UMI_TRADING_CORE_MAX_EVENTS]; size_t count; uint64_t next_sequence; } UmiTradingOrderEventLog;
/* Initialise an empty order event log. */
void umi_trading_order_event_log_init(UmiTradingOrderEventLog *log);
/* Append an event while enforcing monotonically increasing sequence and time. */
UmiStatus umi_trading_order_event_log_append(UmiTradingOrderEventLog *log,const UmiTradingOrderEvent *event);
/* Return the latest event or NULL for an empty log. */
const UmiTradingOrderEvent *umi_trading_order_event_log_latest(const UmiTradingOrderEventLog *log);
#ifdef __cplusplus
}
#endif
#endif
