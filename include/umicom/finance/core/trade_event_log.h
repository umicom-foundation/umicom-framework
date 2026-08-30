/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/trade_event_log.h
 *
 * PURPOSE:
 *   Provide bounded trade event logs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_TRADE_EVENT_LOG_H
#define UMICOM_FINANCE_CORE_TRADE_EVENT_LOG_H

#include "umicom/finance/core/trade_event.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradeEventLog { UmiTradeEvent items[UMI_FINANCIAL_CORE_MAX_ITEMS]; size_t count; } UmiTradeEventLog;
/* Reset collection. */ void umi_trade_event_log_init(UmiTradeEventLog *c);
/* Append valid item. */ UmiStatus umi_trade_event_log_add(UmiTradeEventLog *c,const UmiTradeEvent *item);
/* Return item count. */ size_t umi_trade_event_log_count(const UmiTradeEventLog *c);
#ifdef __cplusplus
}
#endif

#endif
