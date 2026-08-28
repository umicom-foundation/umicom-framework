/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/cash_ledger.c
 *
 * PURPOSE:
 *   Maintain bounded integer cash balances by currency for brokerage settlement state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/cash_ledger.h"

#include <string.h>
static bool same(const UmiCurrency *a,const UmiCurrency *b){return a!=NULL&&b!=NULL&&memcmp(a->code,b->code,sizeof a->code)==0;}
void umi_trading_cash_ledger_init(UmiTradingCashLedger *ledger){if(ledger!=NULL)memset(ledger,0,sizeof *ledger);}
int64_t umi_trading_cash_ledger_get(const UmiTradingCashLedger *ledger,const UmiCurrency *currency){if(ledger==NULL||currency==NULL)return 0;for(size_t i=0;i<ledger->count;i++)if(same(&ledger->items[i].currency,currency))return ledger->items[i].minor_units;return 0;}
UmiStatus umi_trading_cash_ledger_apply(UmiTradingCashLedger *ledger,const UmiCurrency *currency,int64_t delta_minor){if(ledger==NULL||currency==NULL||currency->code[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;size_t idx=ledger->count;for(size_t i=0;i<ledger->count;i++)if(same(&ledger->items[i].currency,currency)){idx=i;break;}if(idx==ledger->count){if(ledger->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;ledger->items[idx].currency=*currency;ledger->count++;}return umi_trading_core_add_i64(ledger->items[idx].minor_units,delta_minor,&ledger->items[idx].minor_units);}
