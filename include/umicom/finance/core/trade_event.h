/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/trade_event.h
 *
 * PURPOSE:
 *   Describe immutable trade lifecycle evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_TRADE_EVENT_H
#define UMICOM_FINANCE_CORE_TRADE_EVENT_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trade event data shared with callers of this public contract.
 */
typedef struct UmiTradeEvent { UmiFinancialId event_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; UmiFinancialDate effective_date; uint32_t state; bool active; } UmiTradeEvent;
/* Initialize the typed financial record. */ UmiStatus umi_trade_event_init(UmiTradeEvent *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state);
/* Validate the typed financial record. */ bool umi_trade_event_is_valid(const UmiTradeEvent *item);
#ifdef __cplusplus
}
#endif

#endif
