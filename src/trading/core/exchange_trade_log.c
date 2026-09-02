/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/exchange_trade_log.c
 *
 * PURPOSE:
 *   Maintain sequence-stable bounded exchange trade capture evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/exchange_trade_log.h"

#include <string.h>
/*
 * Initialise trading exchange trade log from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_exchange_trade_log_init(UmiTradingExchangeTradeLog *log){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(log!=NULL)memset(log,0,sizeof *log);}
/*
 * Find trading exchange trade log while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTradingTradeCapture *umi_trading_exchange_trade_log_find(const UmiTradingExchangeTradeLog *log,const UmiFinancialId *trade_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(log==NULL||trade_id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<log->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_equal(&log->trades[i].trade_id,trade_id))return &log->trades[i];return NULL;}
/*
 * Add trading exchange trade log only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_trading_exchange_trade_log_append(UmiTradingExchangeTradeLog *log,const UmiTradingTradeCapture *trade){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(log==NULL||!umi_trading_trade_capture_valid(trade))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_exchange_trade_log_find(log,&trade->trade_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(log->count>=UMI_TRADING_CORE_MAX_EVENTS)return UMI_STATUS_CAPACITY_EXCEEDED;log->trades[log->count++]=*trade;log->revision++;return UMI_STATUS_OK;}
