/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/cash_ledger.c
 *
 * PURPOSE:
 *   Maintain bounded integer cash balances by currency for brokerage settlement state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/cash_ledger.h"

#include <string.h>
/* Provide the same operation used by this module and its client applications. */
static bool same(const UmiCurrency *a,const UmiCurrency *b){return a!=NULL&&b!=NULL&&memcmp(a->code,b->code,sizeof a->code)==0;}
/*
 * Initialise trading cash ledger from caller-provided values so later operations receive a
 * known state.
 */
void umi_trading_cash_ledger_init(UmiTradingCashLedger *ledger){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ledger!=NULL)memset(ledger,0,sizeof *ledger);}
/*
 * Provide the trading cash ledger get operation used by this module and its client
 * applications.
 */
int64_t umi_trading_cash_ledger_get(const UmiTradingCashLedger *ledger,const UmiCurrency *currency){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ledger==NULL||currency==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<ledger->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(same(&ledger->items[i].currency,currency))return ledger->items[i].minor_units;return 0;}
/*
 * Perform trading cash ledger through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_trading_cash_ledger_apply(UmiTradingCashLedger *ledger,const UmiCurrency *currency,int64_t delta_minor){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ledger==NULL||currency==NULL||currency->code[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;size_t idx=ledger->count;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<ledger->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(same(&ledger->items[i].currency,currency)){idx=i;break;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(idx==ledger->count){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ledger->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;ledger->items[idx].currency=*currency;ledger->count++;}return umi_trading_core_add_i64(ledger->items[idx].minor_units,delta_minor,&ledger->items[idx].minor_units);}
