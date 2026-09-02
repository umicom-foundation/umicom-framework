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
/*
 * Initialise trading order event log from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_order_event_log_init(UmiTradingOrderEventLog *log){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(log!=NULL){memset(log,0,sizeof *log);log->next_sequence=1U;}}
/*
 * Provide the trading order event log latest operation used by this module and its client
 * applications.
 */
const UmiTradingOrderEvent *umi_trading_order_event_log_latest(const UmiTradingOrderEventLog *log){return log!=NULL&&log->count>0U?&log->events[log->count-1U]:NULL;}
/*
 * Add trading order event log only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_trading_order_event_log_append(UmiTradingOrderEventLog *log,const UmiTradingOrderEvent *event){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(log==NULL||!umi_trading_order_event_valid(event))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(log->count>=UMI_TRADING_CORE_MAX_EVENTS)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(event->sequence!=log->next_sequence)return UMI_STATUS_INVALID_STATE;const UmiTradingOrderEvent *last=umi_trading_order_event_log_latest(log);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(last!=NULL&&event->event_time_ms<last->event_time_ms)return UMI_STATUS_INVALID_STATE;log->events[log->count++]=*event;log->next_sequence++;return UMI_STATUS_OK;}
