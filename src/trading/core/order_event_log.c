/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_event_log.c
 *
 * PURPOSE:
 *   Maintain a bounded monotonic lifecycle event log per trading service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_event_log.h"

#include <string.h>
void umi_trading_order_event_log_init(UmiTradingOrderEventLog *log){if(log!=NULL){memset(log,0,sizeof *log);log->next_sequence=1U;}}
const UmiTradingOrderEvent *umi_trading_order_event_log_latest(const UmiTradingOrderEventLog *log){return log!=NULL&&log->count>0U?&log->events[log->count-1U]:NULL;}
UmiStatus umi_trading_order_event_log_append(UmiTradingOrderEventLog *log,const UmiTradingOrderEvent *event){if(log==NULL||!umi_trading_order_event_valid(event))return UMI_STATUS_INVALID_ARGUMENT;if(log->count>=UMI_TRADING_CORE_MAX_EVENTS)return UMI_STATUS_CAPACITY_EXCEEDED;if(event->sequence!=log->next_sequence)return UMI_STATUS_INVALID_STATE;const UmiTradingOrderEvent *last=umi_trading_order_event_log_latest(log);if(last!=NULL&&event->event_time_ms<last->event_time_ms)return UMI_STATUS_INVALID_STATE;log->events[log->count++]=*event;log->next_sequence++;return UMI_STATUS_OK;}
