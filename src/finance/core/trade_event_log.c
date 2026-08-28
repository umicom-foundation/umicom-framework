/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/trade_event_log.c
 *
 * PURPOSE:
 *   Implement bounded trade event logs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/trade_event_log.h"

#include <string.h>
/* Reset collection. */ void umi_trade_event_log_init(UmiTradeEventLog *c){if(c!=NULL)memset(c,0,sizeof *c);}
/* Append valid item. */ UmiStatus umi_trade_event_log_add(UmiTradeEventLog *c,const UmiTradeEvent *item){if(c==NULL||item==NULL||!umi_trade_event_is_valid(item))return UMI_STATUS_INVALID_ARGUMENT;if(c->count>=UMI_FINANCIAL_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;c->items[c->count++]=*item;return UMI_STATUS_OK;}
/* Return item count. */ size_t umi_trade_event_log_count(const UmiTradeEventLog *c){return c!=NULL?c->count:0U;}
